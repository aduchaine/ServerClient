/*
3/31/16

login_net.cpp
*/

#include "stdafx.h"

#include <string>

#include "client_packet.h"
#include "login_net.h"
#include "login_packet.h"

#include "gen_program.h"
#include "login.h"
#include "text_output.h"
#include "timers.h"


bool done = false;
bool connected = false;
bool logged = false;

int login_update_loops = 0;
int login_stat = 0;

// need to work on these variables' location
std::string pm_acctID = "";	// <pm_acctID> is permanent-modified(not really modified) account ID stored during client session - this will change if a different login occurs
std::string random = "";

Timers LoginActivityTimer(CONNECTION_TIMER, false);
int check_connection_packets = 0;


bool LoginNet::InitializeLoginLoop()
{
	int status;	
	Text.LoginMessage(CHANGE_TO_ENTRY);
	while (connected == false) {
		status = Log.InitialLogin(CHANGE_TO_ENTRY);

		if (status == NONE) {
			done = true;
			break;
		}
		else if (status == INVALID_INPUT) {
			Text.ClientMessage(INVALID_INPUT);
			continue;
		}
		else {			
			Net.ClientPackets(LOGIN_1VAL, status, RELEASE_VERSION);
			connected = true;
		}
	}
	return connected;
}

void LoginNet::LoginUpdate()
{
	++login_update_loops;

	ClientPacket_header_struct CPh_struct; // new

	int data1_int; // added new protocol
	int data2_int; // added new protocol

	std::string data1_string;
	std::string data2_string; // added new protocol
	std::string data3_string; // added new protocol

	Log.ProcessLogin(menu_stat, login_stat); // new

	int packet_size = recv(Net.ConnectSocket, Net.packet, 4, 0);
	if (packet_size == 0) { // this occurs when the client socket close was not done properly - server needs to shut down socket first
		printf("\nLogin: Closing connection - server did not signal socket close - socket(%i)\n", Net.ConnectSocket);
		// here a second timer should be activated to recheck the connection by reinitializing it?	
		// send another packet now and get result - disconnect if no response
		ShutdownConnection(Net.ConnectSocket);
		WSACleanup();
		connected = false;
		return;
	}
	if (packet_size < 0) {
		if (check_connection_packets == 1 && LoginActivityTimer.CheckTimer(CONNECTION_TIMER) == true) {
			//std::cout << "\n\t   -----LoginActivityTimer.Check Timer -> send connection packet-----" << std::endl; // for heavy debugging
			LoginActivityTimer.DisableTimer();
			Net.ClientPackets(LOGIN_EMPTY, CHECK_CLIENT_CONNECTION);
		}
		if (check_connection_packets == 2 && LoginActivityTimer.CheckTimer(CONNECTION_TIMER) == true) {
			//std::cout << "\n\t-----LoginActivityTimer.Check Timer -> send final connection packet-----" << std::endl; // for heavy debugging
			LoginActivityTimer.DisableTimer();
			Net.ClientPackets(LOGIN_EMPTY, CHECK_CLIENT_CONNECTION);
		}
		return;
	}

	if (packet_size != 4) { // probably not needed
		return;
	}

	//	if (packet_size > 0) { printf("packet_size: %i\n", packet_size); } // for heavy debugging
	if (packet_size == 4) {
		unsigned int j = 0;
		while (j < (unsigned int)packet_size) {
			CPh_struct.deserialize(&(Net.packet[j])); // new
			
			std::string char_line = Net.packet; // was "const"
			data1_string = char_line.substr(0, 4);
			GProg.RemoveCharsFromString(data1_string, "!");
			data1_int = atoi(data1_string.c_str()); // packet size				
			
			int data_size = recv(Net.ConnectSocket, Net.packet, data1_int - 4, 0);
			
			if (data_size < 0) {
				return;
			}

			//	if (data_size > 0) { printf("\npacket_size data_size: %i\n", data_size); } // for heavy debugging
			if (data_size == data1_int - 4) {
				unsigned int k = 0;
				while (k < (unsigned int)data_size) {
					CPh_struct.deserialize(&(Net.packet[k])); // new

					std::string char_line2 = Net.packet; // was "const"
					data2_string = char_line2.substr(0, 4);
					GProg.RemoveCharsFromString(data2_string, "!");
					data2_int = atoi(data2_string.c_str()); // op_value
					
					if (data_size > 4) {
						data3_string = char_line2.substr(4, 12); // data_pos1
					}					

					//printf("\nRECV: size:(%i) op_value:(%i) data3:(%s)\n", data_size, data2_int, GProg.CharOutput(data3_string)); // for heavy debugging

					k += data_size;
					switch (data2_int) {
					case CHECK_NAME_OLD:
						// printf("\nCHECK_NAME_OLD - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(CHECK_NAME_OLD);
						menu_stat = EXISTING_ACCOUNT;
						login_stat = CHECK_NAME_OLD;
						lock_input = false;
						return;
					case CHECK_NAME_NEW_VALIDITY:
						//printf("\nCHECK_NAME_NEW_VALIDITY - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(CHECK_NAME_NEW_VALIDITY);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_NAME_NEW_VALIDITY;
						lock_input = false;
						return;
					case CHECK_NAME_PW_CHG:
						// printf("\nCHECK_NAME_PW_CHG - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(CHECK_NAME_PW_CHG);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = CHECK_NAME_PW_CHG;
						lock_input = false;
						return;
					case CHECK_PASSWORD_OLD: // rename - use "input"
						//printf("\nCHECK_PASSWORD_OLD - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(CHECK_PASSWORD_OLD, pm_name);
						menu_stat = EXISTING_ACCOUNT;
						login_stat = CHECK_PASSWORD_OLD;
						lock_input = false;
						mask_input = true;
						return;
					case CHECK_PASSWORD_NEW_VALIDITY:
						//printf("\nCHECK_PASSWORD_NEW_VALIDITY - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(CHECK_NAME_NEW_APPROVE, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_NEW_VALIDITY, pm_name);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_PASSWORD_NEW_VALIDITY;
						lock_input = false;
						mask_input = true;
						return;
					case CHECK_PASSWORD_PW_CHG_LOGIN:
						// printf("\nCHECK_PASSWORD_PW_CHG_LOGIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(CHECK_PASSWORD_PW_CHG_LOGIN, pm_name);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = CHECK_PASSWORD_PW_CHG_LOGIN;
						lock_input = false;
						mask_input = true;
						return;
					case ADMIN_LOGIN:
						// printf("\nADMIN_LOGIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(ADMIN_LOGIN);
						menu_stat = ADMIN_ACCOUNT;
						login_stat = ADMIN_LOGIN;
						lock_input = false;
						mask_input = true;
						return;
					case ADMIN_PW_LOGIN:
						// printf("\nADMIN_PW_LOGIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(ADMIN_PW_LOGIN);
						menu_stat = ADMIN_ACCOUNT;
						login_stat = ADMIN_PW_LOGIN;
						lock_input = false;
						mask_input = true;
						return;
					case DUAL_LOCK_OLD:
						//printf("\nDUAL_LOCK_OLD - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(DUAL_LOCK_OLD);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case DUAL_LOCK_PW_CHG:
						//printf("\nDUAL_LOCK_PW_CHG - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(DUAL_LOCK_PW_CHG);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case DUAL_LOCK_ADMIN:
						//printf("\nDUAL_LOCK_ADMIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(DUAL_LOCK_ADMIN);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case LOGIN_COMPLETE_OLD:
						//printf("\nLOGIN_COMPLETE_OLD - case(%i) pm_acctID(%s) packet_size:(%i)", data2_int, GProg.CharOutput(char_string), packet_size); // for heavy debugging
						pm_acctID = data3_string;
						Text.LoginMessage(LOGIN_COMPLETE_OLD, pm_name);
						Net.ClientPackets(LOGIN_EMPTY, LOGIN_COMPLETE_OLD);
						return;
					case LOGIN_COMPLETE_NEW:
						//printf("\nLOGIN_COMPLETE_NEW - case(%i) pm_acctID(%s) packet_size:(%i)", data2_int, GProg.CharOutput(char_string), packet_size); // for heavy debugging
						pm_acctID = data3_string;
						Text.LoginMessage(LOGIN_COMPLETE_NEW, pm_name);
						Net.ClientPackets(LOGIN_EMPTY, LOGIN_COMPLETE_NEW);
						return;
					case CONTINUE:
						//printf("\nCONTINUE - case(%i) char_string(%s) packet_size:(%i)", data2_int, GProg.CharOutput(char_string), packet_size); // for heavy debugging
						random = data3_string;
						menu_stat = 0;
						login_stat = 0;
						c_key.clear();
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						logged = true;
						return;
					case CHECK_NAME_NEW_APPROVE: // this one isn't necessary
						//printf("\nCHECK_NAME_NEW_CONFLICT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(CHECK_NAME_NEW_APPROVE, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_NEW_VALIDITY);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_NAME_NEW_APPROVE;
						lock_input = false;
						return;
					case INVALID_NAME_OLD:
						// printf("\nINVALID_NAME_OLD - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(INVALID_NAME, pm_name);
						Text.LoginMessage(CHECK_NAME_OLD);
						menu_stat = EXISTING_ACCOUNT;
						login_stat = CHECK_NAME_OLD;
						lock_input = false;
						return;
					case INVALID_NAME_BADCHAR:
						// printf("\nINVALID_NAME_BADCHAR - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(INVALID_NAME_BADCHAR, pm_name);
						Text.LoginMessage(CHECK_NAME_NEW_VALIDITY);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_NAME_NEW_VALIDITY;
						lock_input = false;
						return;
					case INVALID_NAME_PW_CHG:
						// printf("\nINVALID_NAME_PW_CHG - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(INVALID_NAME, pm_name);
						Text.LoginMessage(CHECK_NAME_PW_CHG);
						menu_stat = EXISTING_ACCOUNT;
						login_stat = CHECK_NAME_PW_CHG;
						lock_input = false;
						return;
					case INVALID_NAME_NEW_CONFLICT:
						// printf("\nINVALID_NAME_NEW_CONFLICT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_NAME_NEW_CONFLICT, pm_name);
						Text.LoginMessage(CHECK_NAME_NEW_VALIDITY);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_NAME_NEW_VALIDITY;
						lock_input = false;
						return;
					case INVALID_NAME_LENGTH:
						// printf("\nINVALID_NAME_LENGTH - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(INVALID_NAME_LENGTH);
						Text.LoginMessage(CHECK_NAME_NEW_VALIDITY);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_NAME_NEW_VALIDITY;
						lock_input = false;
						return;
					case CHECK_PASSWORD_PW_CHG_VALIDITY:
						// printf("\nCHECK_PASSWORD_PW_CHG_VALIDITY - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(CHECK_PASSWORD_PW_CHG_VALIDITY, pm_name);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = CHECK_PASSWORD_PW_CHG_VALIDITY;
						lock_input = false;
						mask_input = true;
						return;
					case INVALID_PASSWORD_OLD:
						// printf("\nINVALID_PASSWORD_OLD - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_OLD, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_OLD, pm_name);
						menu_stat = EXISTING_ACCOUNT;
						login_stat = CHECK_PASSWORD_OLD;
						lock_input = false;
						mask_input = true;
						return;
					case INVALID_PASSWORD_NEW_BADCHAR:
						// printf("\nINVALID_PASSWORD_NEW_BADCHAR - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_NEW_BADCHAR, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_NEW_VALIDITY, pm_name);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_PASSWORD_NEW_VALIDITY;
						lock_input = false;
						mask_input = true;
						return;
					case INVALID_PASSWORD_PW_CHG_LOGIN:
						// printf("\nINVALID_PASSWORD_PW_CHG_LOGIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_PW_CHG_LOGIN, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_PW_CHG_LOGIN, pm_name);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = CHECK_PASSWORD_PW_CHG_LOGIN;
						lock_input = false;
						mask_input = true;
						return;
					case INVALID_PASSWORD_PW_CHG_BADCHAR:
						// printf("\nINVALID_PASSWORD_PW_CHG_BADCHAR - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_PW_CHG_BADCHAR, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_PW_CHG_VALIDITY, pm_name);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = CHECK_PASSWORD_PW_CHG_VALIDITY;
						lock_input = false;
						mask_input = true;
						return;
					case INVALID_PASSWORD_NEW_LENGTH:
						// printf("\nINVALID_PASSWORD_NEW_LENGTH - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_NEW_LENGTH, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_NEW_VALIDITY, pm_name);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_PASSWORD_NEW_VALIDITY;
						lock_input = false;
						mask_input = true;
						return;
					case INVALID_PASSWORD_PW_CHG_LENGTH:
						// printf("\nINVALID_PASSWORD_PW_CHG_LENGTH - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_PW_CHG_LENGTH, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_PW_CHG_VALIDITY, pm_name);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = CHECK_PASSWORD_PW_CHG_VALIDITY;
						lock_input = false;
						mask_input = true;
						return;
					case INVALID_PASSWORD_NEW_NOMATCH:
						// printf("\nINVALID_PASSWORD_NEW_NOMATCH - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_NEW_NOMATCH, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_NEW_VALIDITY, pm_name);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_PASSWORD_NEW_VALIDITY;
						lock_input = false;
						return;
					case INVALID_PASSWORD_PW_CHG_NOMATCH:
						// printf("\nINVALID_PASSWORD_PW_CHG_NOMATCH - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_PW_CHG_NOMATCH, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_PW_CHG_VALIDITY, pm_name);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = CHECK_PASSWORD_PW_CHG_VALIDITY;
						lock_input = false;
						mask_input = true;
						return;
					case INVALID_PASSWORD_PW_LOGIN_NOMATCH:
						// printf("\nINVALID_PASSWORD_PW_LOGIN_NOMATCH - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_PASSWORD_PW_LOGIN_NOMATCH, pm_name);
						Text.LoginMessage(CHECK_PASSWORD_PW_CHG_LOGIN, pm_name);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = CHECK_PASSWORD_PW_CHG_LOGIN;
						lock_input = false;
						mask_input = true;
						return;
					case CHANGE_TO_LOGINMENU:
						// printf("\nCHANGE_LOGIN_TO_MENU - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(CHANGE_TO_LOGINMENU);
						menu_stat = LOGIN_MENU;
						login_stat = CHANGE_TO_LOGINMENU;
						lock_input = false;
						return;
					case CHANGE_TO_ENTRY:
						// printf("\nCHANGE_LOGIN_TO_ENTER - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case INVALID_INPUT:
						// printf("\nINVALID_INPUT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(INVALID_INPUT);
						menu_stat = LOGIN_MENU;
						login_stat = CHANGE_TO_LOGINMENU;
						lock_input = false;
						return;
					case INVALID_INPUT2:				// invalid input from GoOn()
						// printf("\nINVALID_INPUT2 - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.ClientMessage(INVALID_INPUT2);
						menu_stat = PROCEED_MENU;
						login_stat = PROCEED;
						lock_input = false;
						return;
					case ACCOUNT_LOCK_BEGIN:
						// printf("\nACCOUNT_LOCK_BEGIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(ACCOUNT_LOCK_BEGIN);
						Net.ClientPackets(LOGIN_EMPTY, ACCOUNT_LOCK_BEGIN);
						return;
					case PASSWORD_LOCKED:
						// printf("\nPASSWORD_LOCKED - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(PASSWORD_LOCKED);
						Net.ClientPackets(LOGIN_EMPTY, PASSWORD_LOCKED);
						return;
					case ADMIN_INVALID_NAME:
						// printf("\nADMIN_INVALID_NAME - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(ADMIN_INVALID_NAME);
						Text.LoginMessage(ADMIN_LOGIN);
						menu_stat = ADMIN_ACCOUNT;
						login_stat = ADMIN_LOGIN;
						lock_input = false;
						mask_input = true;
						return;
					case ADMIN_INVALID_PW:
						// printf("\nADMIN_INVALID_PW - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(ADMIN_INVALID_PW);
						Text.LoginMessage(ADMIN_PW_LOGIN);
						menu_stat = ADMIN_ACCOUNT;
						login_stat = ADMIN_PW_LOGIN;
						lock_input = false;
						mask_input = true;
						return;
					case ADMIN_LOG_COMPLETE:
						// printf("\nADMIN_LOG_COMPLETE - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(ADMIN_LOG_SUCCESS);
						Net.ClientPackets(LOGIN_EMPTY, ADMIN_LOG_COMPLETE);
						return;
					case TO_ADMINMENU:
						// printf("\nTO_ADMINMENU - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(TO_ADMINMENU, pm_name);
						menu_stat = ADMIN_MENU;
						login_stat = ADMIN_LOG_SUCCESS;			// should be changed to something else for clarity
						lock_input = false;
						return;
					case ADMIN_INVALID_ENTRY:
						// printf("\nADMIN_INVALID_ENTRY - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(ADMIN_INVALID_ENTRY);
						menu_stat = ADMIN_MENU;
						login_stat = ADMIN_LOG_SUCCESS;			// should be changed to something else for clarity
						lock_input = false;
						return;
					case PROCEED:
						// printf("\nPROCEED - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(PROCEED, pm_name);
						menu_stat = PROCEED_MENU;
						login_stat = PROCEED;
						lock_input = false;
						return;
					case DB_ERROR:				// signals a server error (DB related) upon entry
						//printf("\nDB_ERROR - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(DB_ERROR);
						Net.ClientPackets(LOGIN_EMPTY, DB_ERROR);
						//	SendLoginPackets(DB_ERROR);
						return;
					case DUAL_LOG_OLD:
						//printf("\nDUAL_LOG_OLD - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(DUAL_LOG_OLD);
						menu_stat = EXISTING_ACCOUNT;
						login_stat = DUAL_LOG_OLD;
						lock_input = false;
						return;
					case DUAL_LOG_PW_CHG:
						//printf("\nDUAL_LOG_PW_CHG - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(DUAL_LOG_PW_CHG);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = DUAL_LOG_PW_CHG;
						lock_input = false;
						return;
					case DUAL_LOG_ADMIN:
						//printf("\nDUAL_LOG_ADMIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(DUAL_LOG_ADMIN);
						menu_stat = ADMIN_ACCOUNT;
						login_stat = DUAL_LOG_ADMIN;
						lock_input = false;
						return;
					case INVALID_INPUT3:			// invalid input for DUAL_LOG_OLD
						//printf("\nINVALID_INPUT3 - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(INVALID_INPUT);
						menu_stat = EXISTING_ACCOUNT;
						login_stat = DUAL_LOG_OLD;
						lock_input = false;
						return;
					case INVALID_INPUT4:			// invalid input for DUAL_LOG_PW_CHG
						//printf("\nINVALID_INPUT4 - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(INVALID_INPUT);
						menu_stat = CHNG_PASS_ACCOUNT;
						login_stat = DUAL_LOG_PW_CHG;
						lock_input = false;
						return;
					case INVALID_INPUT5:			// invalid input for DUAL_LOG_ADMIN
						//printf("\nINVALID_INPUT5 - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(INVALID_INPUT);
						menu_stat = ADMIN_ACCOUNT;
						login_stat = DUAL_LOG_ADMIN;
						lock_input = false;
						return;
					case ADMIN_USER_ACCT_CONFLICT:
						// printf("\nADMIN_USER_ACCT_CONFLICT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(ADMIN_USER_ACCT_CONFLICT);
						menu_stat = ADMIN_ACCOUNT;
						login_stat = ADMIN_USER_ACCT_CONFLICT;
						lock_input = false;
						return;
					case USER_ADMIN_ACCT_CONFLICT:
						// printf("\nUSER_ADMIN_ACCT_CONFLICT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(USER_ADMIN_ACCT_CONFLICT);
						Net.ClientPackets(LOGIN_EMPTY, USER_ADMIN_ACCT_CONFLICT);
						return;
					case FORCE_LOGOUT:
						// printf("\nFORCE_LOGOUT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(FORCE_LOGOUT);
						Text.ClientMessage(TO_LOGIN); // different name
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case SERVER_DISCONNECT:
						//printf("\nSERVER_DISCONNECT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.ClientMessage(SERVER_DISCONNECT);
						Text.ClientMessage(QUIT);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case CHECK_CLIENT_CONNECTION:		// a signal from the server to check connectivity - dummy packet
						//printf("\nCHECK_CLIENT_CONNECTION - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						return;
					case CLIENT_DC_INACTIVE:			// a signal from the server to signal disconnection
						//printf("\nCLIENT_DC_INACTIVE - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(CLIENT_DC_INACTIVE);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case LOGOUT_QUIT:
						// printf("\nLOGOUT_QUIT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(TO_LOGIN);
						Text.ClientMessage(QUIT);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						done = true;
						return;
					case LOG_LOCKED:
						// printf("\nLOG_LOCKED - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(LOG_LOCKED);
						Net.ClientPackets(LOGIN_EMPTY, LOG_LOCKED);
						lock_input = false;
						return;
					case SECURITY_LOCKED:
						// printf("\nSECURITY_LOCKED - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(SECURITY_LOCKED);
						Net.ClientPackets(LOGIN_EMPTY, SECURITY_LOCKED);
						return;
					case INVALID_NAME_NEW_NOMATCH:
						//printf("\nINVALID_NAME_NEW_NOMATCH - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(INVALID_NAME_NEW_NOMATCH);
						Text.LoginMessage(CHECK_NAME_NEW_VALIDITY);
						menu_stat = NEW_ACCOUNT;
						login_stat = CHECK_NAME_NEW_VALIDITY;
						lock_input = false;
						return;
					case LOGIN_SUCCESS_OLD:			// possibly put a sleep(250) here
						//printf("\nLOGIN_SUCCESS_OLD - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						login_stat = LOGIN_SUCCESS_OLD;
						Net.ClientPackets(LOGIN_EMPTY, LOGIN_SUCCESS_OLD);
						return;
					case LOGIN_SUCCESS_NEW:			// possibly put a sleep(250) here
						// printf("\nLOGIN_SUCCESS_NEW - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(LOGIN_SUCCESS_NEW);
						login_stat = LOGIN_SUCCESS_NEW;
						Net.ClientPackets(LOGIN_EMPTY, LOGIN_SUCCESS_NEW);
						return;
					case LOGIN_SUCCESS_PW_CHG:		// possibly put a sleep(250) here
						// printf("\nLOGIN_SUCCESS_PW_CHG - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(LOGIN_SUCCESS_PW_CHG);
						login_stat = LOGIN_SUCCESS_PW_CHG;
						Net.ClientPackets(LOGIN_EMPTY, LOGIN_SUCCESS_PW_CHG);
						return;
					case ADMIN_LOG_SUCCESS:
						// printf("\nADMIN_LOG_SUCCESS - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Net.ClientPackets(LOGIN_EMPTY, ADMIN_LOG_SUCCESS);
						return;
					case ADMIN_LOGOUT:
						// printf("\nADMIN_LOGOUT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.LoginMessage(ADMIN_LOGOUT);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case QUIT:
						// printf("\nQUIT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.ClientMessage(QUIT);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						done = true;
						return;
					case LOGOUT_TO_LOGIN:
						// printf("\nLOGOUT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(TO_LOGIN);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						return;
					case SHUTDOWN:
						// printf("\nSHUTDOWN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.ClientMessage(TO_LOGIN);
						Text.LoginMessage(SHUTDOWN);
						Text.ClientMessage(QUIT);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						done = true;
						return;
					case INVALID_CLIENT_VERSION:
						// printf("\nINVALID_CLIENT_VERSION - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.LoginMessage(INVALID_CLIENT_VERSION);
						menu_stat = 0;
						login_stat = 0;
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						done = true;
						return;
						

					case TEST_PACKETS:
						printf("\nTEST_PACKETS - case(%i) data3_string(%s) data_size:(%i)", data2_int, GProg.CharOutput(data3_string), data_size);
						return;
					default:
						printf("\nERROR Default: case(%i) data3_string(%s) data_size: (%i)\n", data2_int, GProg.CharOutput(data3_string), data_size);
						return;
					}
				}
			}
		}
	}
}

void LoginNet::SendPacketErrorHandling(int clienttalk_opvalue)
{
	int sock_error = WSAGetLastError();

	++check_connection_packets;

	if (sock_error != WSAECONNRESET) {
		printf("%s\n", WSAGetLastErrorMessage("Send packet FAILED with error :"));
		return;
	}
	else if (check_connection_packets == 1) {
		printf("\nNo response from server1.");
		LoginActivityTimer.StartTimer(CONNECTION_TIMER);
		return;
	}
	else if (check_connection_packets == 2) {
		printf("\nNo response from server2.");
		LoginActivityTimer.StartTimer(CONNECTION_TIMER);		
		return;
	}
	else { // aborted server connection, results in closed socket
		printf("\nClosing connection - server unresponsive\n");
		ShutdownConnection(Net.ConnectSocket);
		WSACleanup();
		connected = false;
		return;
	}
}

void LoginNet::DisableLogNetTimers()
{
	LoginActivityTimer.DisableTimer();	
}
