/*
3/31/16

login_net.cpp
*/

#include "stdafx.h"

#include "login_net.h"
#include "login_packet.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"
#include "timers.h"


std::mutex mutexcl;

Timers BadLogWaitTimer(BAD_LOG_WAIT, false);
Timers BadLogTimer(BAD_LOG_TIMER, true);

std::map<int, int> new_acctID_map;
std::map<int, std::string> reserve_name_map;
std::vector<int> content_acct_ids;
std::map<int, int> DBlogged_accts_map;
std::map<int, int> locked_accts_map;
std::vector<unsigned int> socket_pos;

bool connected = false;
bool restart_conn = true; // extern - for controlled shutdown and restart attempts

int login_update_loops = 0;


void LoginNet::ReceiveLoginPackets()
{
	++login_update_loops;

	SOCKET ClientSocket;
	LoginPacket_header_struct LPh_struct; // new

	int packet_size;
	int timestamp;
	int login_stat;
	int acct_ID;
	int data1_int; // added new protocol
	int data2_int; // added new protocol
	int data3_int; // added new protocol

	std::string data1_string;
	std::string data2_string; // added new protocol
	std::string data3_string; // added new protocol
	std::string s_acctid;
	std::string s_random;
	std::string acct_name;

	const char * pm_name;

	/// --- new protocol test --- /// -- appears to work content side
	for (unsigned i = 0; i < socket_pos.size(); i++) {
		unsigned int c = socket_pos.at(i);
		ClientSocket = Net.client_sock[c];

		packet_size = recv(ClientSocket, Net.packet, 4, 0);
		if (packet_size == 0) {
			printf("\nERROR Login: Closing connection - client did not signal socket close - socket-position(%i:%i)\n", ClientSocket, c);
			if (Net.LogoutByConnection(c, NONE, true, false) == false) {
				if (Net.LogoutByConnection(c, NONE, true, true) == false) {
					printf("\nLogin: Closing connection - UNIDENTIFIED CONNECTION - socket-position(%i:%i)\n", ClientSocket, c);
					Net.LogoutByConnection(c, NONE, false, false); // should not be needed but may be a good safegaurd
				}
			}
			continue;
		}
		if (packet_size < 0) {
			Net.CheckClientActivity();
			continue;
		}

		if (packet_size != 4) { // probably not needed
			return;
		}

		//	if (packet_size > 0) { printf("\nServerComm packet_size: %i\n", packet_size); } // for heavy debugging
		if (packet_size == 4) {
			unsigned int j = 0;
			while (j < (unsigned int)packet_size) {
				LPh_struct.deserialize(&(Net.packet[j])); // new
				
				std::string char_line = Net.packet; // was "const"
				data1_string = char_line.substr(0, 4);
				GProg.RemoveCharsFromString(data1_string, "!");
				data1_int = atoi(data1_string.c_str()); // packet size				
				
				int data_size = recv(ClientSocket, Net.packet, data1_int - 4, 0);
				
				if (data_size < 0) {
					return;
				}

				///-- -- // -- probably need to separate this into it's own function or at least within it's own loop
				//	if (data_size > 0) { printf("\nServerComm data_size: %i\n", data_size); } // for heavy debugging
				if (data_size == data1_int - 4) {
					
					// if we got here packet_size > 0 so, change the activty_list timestamp
					timestamp = static_cast<int>(time(0));
					if (Net.CheckActivityBySocket(ClientSocket, 0) == 0) {
						Net.AddToActivityList(ClientSocket, timestamp, 0);
					}
					else if (Net.CheckActivityBySocket(ClientSocket, 0) == 1) { // client socket exists
						Net.RemoveFromActivityList(ClientSocket);
						Net.AddToActivityList(ClientSocket, timestamp, 0);
					}

					unsigned int k = 0;
					while (k < (unsigned int)data_size) {
						LPh_struct.deserialize(&(Net.packet[k])); // new
						
						std::string char_line2 = Net.packet; // was "const"
						data2_string = char_line2.substr(0, 4);
						GProg.RemoveCharsFromString(data2_string, "!");
						data2_int = atoi(data2_string.c_str()); // op_value						
						
						s_random = GProg.RandomInttoString();

						if (data_size > 4) {
							data3_string = char_line2.substr(4, 12); // data_pos1
							data3_int = atoi(data3_string.c_str()); // acctID usually				
						}
						
						//printf("\nRECV: size:(%i) op_value:(%i) data3:(%s)", data_size, data2_int, GProg.CharOutput(data3_string)); // for heavy debugging
						File.LogRecord(Logs::High, Logs::LoginPacket, "RECV: size:(%i) op_value:(%i) data3:(%s)\n", data1_int, data2_int, GProg.CharOutput(data3_string));

						k += data_size;
						switch (data2_int) {
						case CHECK_NAME_OLD:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHECK_NAME_OLD - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							if (data_size <= 4) {
								RemoveReservedName(c);
								Net.RemoveAcctFromList(c);
								LoginPackets(LOGIN_EMPTY, CHECK_NAME_OLD, ClientSocket, c);
								return;
							}
							else {
								login_stat = Acct.CheckAccountName(data3_string, data2_int);
								if (login_stat == CHECK_PASSWORD_OLD) {
									DB.AccountInfo(c, data3_string, false);
								}
								LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
								return;
							}
						case CHECK_NAME_NEW_VALIDITY:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHECK_NAME_NEW_VALIDITY - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							if (data_size <= 4) {
								RemoveReservedName(c);
								Net.RemoveAcctFromList(c);
								LoginPackets(LOGIN_EMPTY, CHECK_NAME_NEW_VALIDITY, ClientSocket, c);
								return;
							}
							else {
								mutexcl.lock();
								login_stat = Acct.CheckAccountName(data3_string, data2_int);
								if (login_stat == CHECK_PASSWORD_NEW_VALIDITY) {
									reserve_name_map[c] = data3_string;
								}
								mutexcl.unlock();
								LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
								return;
							}
						case CHECK_NAME_PW_CHG:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHECK_NAME_PW_CHG - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							if (data_size <= 4) {
								RemoveReservedName(c);
								Net.RemoveAcctFromList(c);
								LoginPackets(LOGIN_EMPTY, CHECK_NAME_PW_CHG, ClientSocket, c);
								return;
							}
							else {
								login_stat = Acct.CheckAccountName(data3_string, data2_int);
								if (login_stat == CHECK_PASSWORD_PW_CHG_LOGIN) {
									DB.AccountInfo(c, data3_string, false);
									reserve_name_map[c] = data3_string;
								}
								LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
								return;
							}
						case CHECK_PASSWORD_OLD:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHECK_PASSWORD_OLD - case:(%i) PW:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							login_stat = Acct.CheckAccountPassword(c, data3_string, data2_int);
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case CHECK_PASSWORD_NEW_VALIDITY:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHECK_PASSWORD_NEW_VALIDITY - case:(%i) PW:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							login_stat = Acct.CheckAccountPassword(c, data3_string, data2_int);
							if (login_stat == LOGIN_SUCCESS_NEW) {
								mutexcl.lock();
								ReserveAccountID(c);
								mutexcl.unlock();
								acct_ID = GetReservedAcctID(c);
								acct_name = GetReservedName(c);
								Net.AddToAcctInfoList(c, acct_ID, acct_name, data3_string);
								RemoveReservedName(c);
							}
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case CHECK_PASSWORD_PW_CHG_LOGIN:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHECK_PASSWORD_PW_CHG_LOGIN - case:(%i) PW:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							login_stat = Acct.CheckAccountPassword(c, data3_string, data2_int);
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case CHECK_PASSWORD_PW_CHG_RECORD:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHECK_PASSWORD_PW_CHG_RECORD - case:(%i) PW:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							login_stat = Acct.CheckAccountPassword(c, data3_string, data2_int);
							if (login_stat == LOGIN_SUCCESS_PW_CHG) {
								Net.RemoveAcctFromList(c);
								acct_name = GetReservedName(c);
								pm_name = acct_name.c_str(); // eventually changing the main function data type
								acct_ID = DB.GetAccountID(pm_name); // this is required
								Net.AddToAcctInfoList(c, acct_ID, acct_name, data3_string);
								RemoveReservedName(c);
							}
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case ADMIN_LOGIN:
							File.LogRecord(Logs::High, Logs::LoginPacket, "ADMIN_LOGIN - case:(%i) input:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							login_stat = Acct.CheckAccountName(data3_string, data2_int);
							if (login_stat == ADMIN_PW_LOGIN) {
								acct_ID = DB.GetAccountID(GProg.CharOutput(data3_string));
								if (DB.CheckAdminRecord(acct_ID) == false) {
									DB.AccountInfo(c, data3_string, false);
								}
								else {
									DB.AccountInfo(c, data3_string, true);
								}
							}
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case ADMIN_PW_LOGIN:
							File.LogRecord(Logs::High, Logs::LoginPacket, "ADMIN_PW_LOGIN - case:(%i) PW:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							login_stat = Acct.CheckAccountPassword(c, data3_string, data2_int);
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case DUAL_PROCEED_OLD:
							File.LogRecord(Logs::High, Logs::LoginPacket, "DUAL_PROCEED_OLD - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);						// need to get acctID to use in next step					
							if (Net.ForceLogoutByAccount(acct_ID, false) == false) {	// here we disconnect the other client logged in and do a logout procedure
								s_acctid = std::to_string(acct_ID);
								LoginPackets(CONTENT_1VAL, DUAL_PROCEED_OLD, Net.ContentSocket, 0, s_acctid); // if we can't logout the current user here, send to content to logout
								return;
							}															// this is done and account.cpp is done for these cases
							LoginPackets(LOGIN_EMPTY, LOGIN_SUCCESS_OLD, ClientSocket, c);
							return;
						case DUAL_PROCEED_PW_CHG:
							File.LogRecord(Logs::High, Logs::LoginPacket, "DUAL_PROCEED_PW_CHG - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							if (Net.ForceLogoutByAccount(acct_ID, false) == false) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(CONTENT_1VAL, DUAL_PROCEED_PW_CHG, Net.ContentSocket, 0, s_acctid);
								return;
							}
							LoginPackets(LOGIN_EMPTY, CHECK_PASSWORD_PW_CHG_VALIDITY, ClientSocket, c);
							return;
						case DUAL_PROCEED_ADMIN:
							File.LogRecord(Logs::High, Logs::LoginPacket, "DUAL_PROCEED_ADMIN - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							if (Net.ForceLogoutByAccount(acct_ID, true) == false) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(CONTENT_1VAL, DUAL_PROCEED_ADMIN, Net.ContentSocket, 0, s_acctid);
								return;
							}
							LoginPackets(LOGIN_EMPTY, ADMIN_LOG_SUCCESS, ClientSocket, c);
							return;
						case DUAL_PROCEED_ADMIN2:
							File.LogRecord(Logs::High, Logs::LoginPacket, "DUAL_PROCEED_ADMIN2 - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							if (Net.ForceLogoutByAccount(acct_ID, false) == false) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(CONTENT_1VAL, DUAL_PROCEED_ADMIN2, Net.ContentSocket, 0, s_acctid);
								return;
							}
							LoginPackets(LOGIN_EMPTY, ADMIN_LOG_SUCCESS, ClientSocket, c);
							return;
						case DUAL_LOCK_OLD:			// to lock we'll the same thing as "proceed" and add-> lock the account, send the current client to entry and close the connection
							File.LogRecord(Logs::High, Logs::LoginPacket, "DUAL_LOCK_OLD - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							if (Net.ForceLogoutByAccount(acct_ID, false) == false) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(CONTENT_1VAL, DUAL_LOCK_OLD, Net.ContentSocket, 0, s_acctid);
								return;
							}
							Acct.LogFail(c, DUAL_LOGIN);
							Net.LogoutByConnection(c, DUAL_LOCK_OLD, false, false);
							return;
						case DUAL_LOCK_PW_CHG:
							File.LogRecord(Logs::High, Logs::LoginPacket, "DUAL_LOCK_PW_CHG - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							if (Net.ForceLogoutByAccount(acct_ID, false) == false) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(CONTENT_1VAL, DUAL_LOCK_PW_CHG, Net.ContentSocket, 0, s_acctid);
								return;
							}
							Acct.LogFail(c, DUAL_LOGIN);
							Net.LogoutByConnection(c, DUAL_LOCK_PW_CHG, false, false);
							return;
						case DUAL_LOCK_ADMIN:
							File.LogRecord(Logs::High, Logs::LoginPacket, "DUAL_LOCK_ADMIN - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							if (Net.ForceLogoutByAccount(acct_ID, true) == false) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(CONTENT_1VAL, DUAL_LOCK_ADMIN, Net.ContentSocket, 0, s_acctid);
								return;
							}
							Acct.AdminLogFail(c, ADMIN_DUAL_LOGIN);
							Net.LogoutByConnection(c, DUAL_LOCK_ADMIN, false, false);
							return;
						case DUAL_LOCK_ADMIN2:
							File.LogRecord(Logs::High, Logs::LoginPacket, "DUAL_LOCK_ADMIN2 - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							if (Net.ForceLogoutByAccount(acct_ID, false) == false) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(CONTENT_1VAL, DUAL_LOCK_ADMIN2, Net.ContentSocket, 0, s_acctid);
								return;
							}
							Acct.AdminLogFail(c, ADMIN_DUAL_LOGIN);
							Net.LogoutByConnection(c, DUAL_LOCK_ADMIN2, false, false);
							return;
						case LOGIN_COMPLETE_OLD:
							File.LogRecord(Logs::High, Logs::LoginPacket, "LOGIN_COMPLETE_OLD - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, PROCEED, ClientSocket, c);
							return;
						case LOGIN_COMPLETE_NEW:
							File.LogRecord(Logs::High, Logs::LoginPacket, "LOGIN_COMPLETE_NEW - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, PROCEED, ClientSocket, c);
							return;
						case CONTINUE:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CONTINUE - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							s_acctid = std::to_string(acct_ID);
							LoginPackets(CONTENT_2VAL, SERVERTALK3, Net.ContentSocket, 0, s_acctid, s_random);
							LoginPackets(LOGIN_1VAL, CONTINUE, ClientSocket, c, s_random);
							Net.LogoutByConnection(c, NONE, true, false);
							return;
						case INVALID_NAME_LENGTH:
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_NAME_LENGTH - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_NAME_LENGTH, ClientSocket, c);
							return;
						case INVALID_PASSWORD_NEW_LENGTH:
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_PASSWORD_NEW_LENGTH - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_PASSWORD_NEW_LENGTH, ClientSocket, c);
							return;
						case INVALID_PASSWORD_PW_CHG_LENGTH:
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_PASSWORD_PW_CHG_LENGTH - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_PASSWORD_PW_CHG_LENGTH, ClientSocket, c);
							return;
						case INVALID_PASSWORD_NEW_NOMATCH:
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_PASSWORD_NEW_NOMATCH - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_PASSWORD_NEW_NOMATCH, ClientSocket, c);
							return;
						case INVALID_PASSWORD_PW_CHG_NOMATCH:
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_PASSWORD_PW_CHG_NOMATCH - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_PASSWORD_PW_CHG_NOMATCH, ClientSocket, c);
							return;
						case INVALID_PASSWORD_PW_LOGIN_NOMATCH:
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_PASSWORD_PW_LOGIN_NOMATCH - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_PASSWORD_PW_LOGIN_NOMATCH, ClientSocket, c);
							return;
						case CHANGE_TO_LOGINMENU:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHANGE_TO_LOGINMENU - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							RemoveReservedName(c);
							Net.RemoveAcctFromList(c);
							LoginPackets(LOGIN_EMPTY, CHANGE_TO_LOGINMENU, ClientSocket, c);
							return;
						case CHANGE_TO_ENTRY:						// default for ERROR, more ops needed for logging
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHANGE_TO_ENTRY - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, CHANGE_TO_ENTRY, false, false); // assure user is not logged in
							return;
						case CHANGE_TO_ADMIN:						// admin login if not logged in as user
							File.LogRecord(Logs::High, Logs::LoginPacket, "ADMIN_LOGIN - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							if (Net.FindUserBySockPos(c) == true) {
								acct_ID = Net.GetAcctIDFromAcctInfo(c);
								acct_name = Net.GetNameFromAcctInfo(c);
								Acct.UserLogout(acct_ID, acct_name);
								Net.RemoveAcctFromList(c);
								Net.RemoveUserFromList(c);
								LoginPackets(LOGIN_EMPTY, ADMIN_LOGIN, ClientSocket, c);
								return;
							}
							RemoveReservedName(c);
							Net.RemoveAcctFromList(c);
							LoginPackets(LOGIN_EMPTY, ADMIN_LOGIN, ClientSocket, c);
							return;
						case INVALID_INPUT:							// invalid input from LoginMenu()
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_INPUT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_INPUT, ClientSocket, c);
							return;
						case INVALID_INPUT2:						// invalid input from GoOn() in client
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_INPUT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_INPUT2, ClientSocket, c);
							return;
						case ACCOUNT_LOCK_BEGIN:
							File.LogRecord(Logs::High, Logs::LoginPacket, "ACCOUNT_LOCK_BEGIN - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, CHANGE_TO_ENTRY, false, false);
							return;
						case PASSWORD_LOCKED:
							File.LogRecord(Logs::High, Logs::LoginPacket, "PASSWORD_LOCKED - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, CHANGE_TO_ENTRY, false, false);
							return;
						case ADMIN_LOG_COMPLETE:
							File.LogRecord(Logs::High, Logs::LoginPacket, "ADMIN_LOG_COMPLETE - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, ADMIN_MENU, ClientSocket, c);
							return;
						case ADMIN_MENU:							// used for "back" in directadmin()
							File.LogRecord(Logs::High, Logs::LoginPacket, "ADMIN_MENU - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, ADMIN_MENU, ClientSocket, c);
							return;
						case ADMIN_INVALID_ENTRY:
							File.LogRecord(Logs::High, Logs::LoginPacket, "ADMIN_INVALID_ENTRY - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, ADMIN_INVALID_ENTRY, ClientSocket, c);
							return;
						case PROCEED:
							File.LogRecord(Logs::High, Logs::LoginPacket, "PROCEED - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, PROCEED, ClientSocket, c);
						case DB_ERROR:
							File.LogRecord(Logs::High, Logs::ContentPacket, "DB_ERROR - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, CHANGE_TO_ENTRY, false, false);
							return;
						case INVALID_INPUT3:						// invalid input from ExistingAccountLogin() in client for dual_login
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_INPUT3 - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_INPUT3, ClientSocket, c);
							return;
						case INVALID_INPUT4:						// invalid input from ChangePasswordLogin() in client for dual_login
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_INPUT4 - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_INPUT4, ClientSocket, c);
							return;
						case INVALID_INPUT5:						// invalid input from AdminLogin() in client for dual_login
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_INPUT5 - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_INPUT5, ClientSocket, c);
							return;
						case USER_ADMIN_ACCT_CONFLICT:				// dual login, account association, send user to entry menu - not logged in
							File.LogRecord(Logs::High, Logs::LoginPacket, "USER_ADMIN_ACCT_CONFLICT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, CHANGE_TO_ENTRY, false, false);
							return;
						case SERVER_DISCONNECT:						// default for ERROR, more ops needed for logging - more ops in but not complete
							File.LogRecord(Logs::High, Logs::LoginPacket, "SERVER_DISCONNECT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, SERVER_DISCONNECT, ClientSocket, c);
							return;
						case CHECK_CLIENT_CONNECTION:				// this needs work - if called it should bring up the memory of location in processes which is far from done
							//printf("\nCHECK_CLIENT_CONNECTION - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", LP_types, packet_size, ClientSocket, c);
							return;
						case LOG_LOCKED:							// improper logout locked
							File.LogRecord(Logs::High, Logs::LoginPacket, "LOG_LOCKED - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, CHANGE_TO_ENTRY, false, false);
							return;
						case SECURITY_LOCKED:						// dual login locked
							File.LogRecord(Logs::High, Logs::LoginPacket, "SECURITY_LOCKED - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, CHANGE_TO_ENTRY, false, false);
							return;
						case INVALID_NAME_NEW_NOMATCH:
							File.LogRecord(Logs::High, Logs::LoginPacket, "INVALID_NAME_NEW_NOMATCH - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							LoginPackets(LOGIN_EMPTY, INVALID_NAME_NEW_NOMATCH, ClientSocket, c);
							return;
						case LOGIN_SUCCESS_OLD:
							File.LogRecord(Logs::High, Logs::LoginPacket, "LOGIN_SUCCESS_OLD - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							Net.AddToLoginUserList(c, ClientSocket, acct_ID, 0, 0);
							login_stat = Acct.LogSuccess(c, LOGIN_SUCCESS_OLD);
							if (login_stat == LOGIN_COMPLETE_OLD) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(LOGIN_1VAL, LOGIN_COMPLETE_OLD, ClientSocket, c, s_acctid);
								return;
							}
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case LOGIN_SUCCESS_NEW:
							File.LogRecord(Logs::High, Logs::LoginPacket, "LOGIN_SUCCESS_NEW - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = GetReservedAcctID(c);
							Net.AddToLoginUserList(c, ClientSocket, acct_ID, 0, 0);
							login_stat = Acct.LogSuccess(c, LOGIN_SUCCESS_NEW);
							if (login_stat == LOGIN_COMPLETE_NEW) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(LOGIN_1VAL, LOGIN_COMPLETE_NEW, ClientSocket, c, s_acctid);
								RemoveReservedAcctID(c);
								return;
							}
							RemoveReservedAcctID(c);
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case LOGIN_SUCCESS_PW_CHG:
							File.LogRecord(Logs::High, Logs::LoginPacket, "LOGIN_SUCCESS_PW_CHG - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							Net.AddToLoginUserList(c, ClientSocket, acct_ID, 0, 0);
							login_stat = Acct.LogSuccess(c, LOGIN_SUCCESS_PW_CHG);
							if (login_stat == LOGIN_COMPLETE_OLD) {
								s_acctid = std::to_string(acct_ID);
								LoginPackets(LOGIN_1VAL, LOGIN_COMPLETE_OLD, ClientSocket, c, s_acctid);
								return;
							}
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case ADMIN_LOG_SUCCESS:						// admin logging into login server
							File.LogRecord(Logs::High, Logs::LoginPacket, "ADMIN_LOG_SUCCESS - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							Net.AddToLoginUserList(c, ClientSocket, acct_ID, 1, 0);
							login_stat = Acct.LogSuccess(c, ADMIN_LOG_SUCCESS);
							LoginPackets(LOGIN_EMPTY, login_stat, ClientSocket, c);
							return;
						case ADMIN_LOGOUT:
							File.LogRecord(Logs::High, Logs::LoginPacket, "ADMIN_LOGOUT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, ADMIN_LOGOUT, true, true);
							return;
						case QUIT:									// quit before adn after logging in
							File.LogRecord(Logs::High, Logs::LoginPacket, "QUIT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							if (Net.FindUserBySockPos(c) == true) {
								Net.LogoutByConnection(c, LOGOUT_QUIT, true, false);
								return;
							}
							Net.LogoutByConnection(c, QUIT, false, false);
							return;
						case LOGOUT_TO_LOGIN:
							File.LogRecord(Logs::High, Logs::LoginPacket, "LOGOUT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, LOGOUT_TO_LOGIN, true, false);
							return;
						case SHUTDOWN:
							File.LogRecord(Logs::High, Logs::LoginPacket, "SHUTDOWN - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							acct_name = Net.GetNameFromAcctInfo(c);
							pm_name = acct_name.c_str(); // eventually changing the main function data type
							File.LogRecord(Logs::Normal, Logs::General, "Server shutdown called by Admin:(%s)", pm_name);
							File.AdminRecord(pm_name, "Server shutdown called.");
							if (Net.InitializeContentConnection() == false) { // creates a connection regardless of whether one exists
								Net.LogoutByConnection(c, SHUTDOWN, true, true);
								ShutdownConnection(Net.ContentSocket);
								restart_conn = false;
								connected = false;
								return;
							}
							LoginPackets(CONTENT_EMPTY, SERVERTALK2, Net.ContentSocket, 0);
							Net.LogoutByConnection(c, SHUTDOWN, true, true);
							return;
						case CHECK_CLIENT_VERSION:
							File.LogRecord(Logs::High, Logs::LoginPacket, "CHECK_CLIENT_VERSION - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (CheckClientVersion(data3_string) == false) {
								LoginPackets(LOGIN_EMPTY, INVALID_CLIENT_VERSION, ClientSocket, c);
								Net.LogoutByConnection(c, NONE, false, false);
								return;
							}							
							RemoveReservedName(c);
							Net.RemoveAcctFromList(c);
							LoginPackets(LOGIN_EMPTY, CHANGE_TO_LOGINMENU, ClientSocket, c);
							return;							

						case TEST_PACKETS:							// for heavy debugging
							printf("\nTEST_PACKETS - case:(%i) input:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							return;							
						default:
							File.LogRecord(Logs::Normal, Logs::NetError, "LoginPacket ERROR LPS_Default - case:(%i) input:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							printf("\nERROR Default - case:(%i) input:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							return;
						}
					}
				}
			}
		}
	}
}

void LoginNet::ReceiveContentCommPackets()
{
	LoginPacket_header_struct LPh_struct;

	//	// ---- much of this is being changed - login variables
	int timestamp = static_cast<int>(time(0));
	int data1_int;
	int data2_int;
	int data3_int;				// used for acct id from content server
	int data4_int;				// used for pw code

	std::string data1_string;
	std::string data2_string;	// used for pw code
	std::string data3_string;	// used for acct_ID's from content server
	std::string data4_string;	// used for pw code
	
	/// --- new protocol test --- /// -- appears to work
	int packet_size = recv(Net.ContentSocket, Net.packet, 4, 0);
	if (packet_size == 0) {
		printf("\nServerComm: Closing connection - content server did not signal socket close - socket (%i)\n", Net.ContentSocket);
		ShutdownConnection(Net.ContentSocket);
		WSACleanup();
		return;
	}

	if (packet_size < 0) {
		Net.CheckServerActivity(Net.ContentSocket); // sends a dummy packet to be sent back verifying a connection
		return;
	}

	if (packet_size != 4) { // probably not needed
		return;
	}

	//	if (packet_size > 0) { printf("\nServerComm packet_size: %i\n", packet_size); } // for heavy debugging
	if (packet_size == 4) {
		unsigned int j = 0;
		while (j < (unsigned int)packet_size) {
			LPh_struct.deserialize(&(Net.packet[j])); // new
			
			std::string char_line = Net.packet; // was "const"
			data1_string = char_line.substr(0, 4);
			GProg.RemoveCharsFromString(data1_string, "!");
			data1_int = atoi(data1_string.c_str()); // packet size				
			
			int data_size = recv(Net.ContentSocket, Net.packet, data1_int - 4, 0);			
			if (data_size < 0) {
				return;
			}
			
			///-- -- // -- probably need to separate this into it's own function or at least within it's own loop
			//	if (data_size > 0) { printf("\nServerComm data_size: %i\n", data_size); } // for heavy debugging
			if (data_size == data1_int - 4) {
				unsigned int k = 0;
				while (k < (unsigned int)data_size) {
					LPh_struct.deserialize(&(Net.packet[k])); // new
					
					std::string char_line2 = Net.packet; // was "const"
					data2_string = char_line2.substr(0, 4);
					GProg.RemoveCharsFromString(data2_string, "!");
					data2_int = atoi(data2_string.c_str()); // op_value
					
					if (data_size > 4) {
						data3_string = char_line2.substr(4, 12); // data_pos1
						GProg.RemoveCharsFromString(data3_string, "!");
						data3_int = atoi(data3_string.c_str()); // acctID usually
						if (data_size > 16) {
							data4_string = char_line2.substr(16, 12); // data_pos2
							data4_int = atoi(data4_string.c_str()); // acct code
						}
					}

					//printf("\nRECV: size:(%i) op_value:(%i) int3:(%i) data3:(%s) data4:(%s)", data_size, data2_int, data3_int, GProg.CharOutput(data3_string), GProg.CharOutput(data4_string));
					File.LogRecord(Logs::High, Logs::LoginPacket, "size:(%i) op_value:(%i) int3:(%i) data3:(%s) data4:(%s)\n", data_size, data2_int, data3_int, GProg.CharOutput(data3_string), GProg.CharOutput(data4_string));

					k += data_size;
					switch (data2_int) {
					case BAD_LOG:
						if (data_size == 4) {			// empty packet from content server to signal <DoBadLogAccounts> - no response necessary
							File.LogRecord(Logs::High, Logs::LoginPacket, "ServerComm BAD_LOG - case:(%i) data_size:(%i) ContentSocket:(%i)", data2_int, data_size, Net.ContentSocket);
							Net.AddToLoggedAccounts();	// adds admin account ids logged in login server
							DoBadLogAccounts();
							return;
						}
						else {							// packet with acctIDs from content server to signal <BadLogWaitTimer> then <DoBadLogAccounts> when timer expires							
							int it_num_ids = (data_size - 4) / 4; // new
							int num_ids = (data_size - 4) / 4; // new
							int data_pos;

							for (it_num_ids = 0; it_num_ids < num_ids; it_num_ids++) {
								data_pos = (it_num_ids * 4) + 4;

								std::string num_string = char_line2.substr(data_pos, 4);
								GProg.RemoveCharsFromString(num_string, "!");
								int num_int = atoi(num_string.c_str());
								content_acct_ids.push_back(num_int);
								//	printf("\n OPcode-acctID-it_num_ids-data_pos (%i-%i-%i-%i)\n", data2_int, num_int, it_num_ids, data_pos); // for heavy debugging
							}
							if (content_acct_ids.size() == num_ids) { // won't work with admin connected on login - need to id the count of admin logged in or add admin later
								File.LogRecord(Logs::High, Logs::LoginPacket, "ServerComm BAD_LOG - case:(%i) data_size:(%i) ContentSocket:(%i)", data2_int, data_size, Net.ContentSocket);
								if (BadLogWaitTimer.IsEnabled() == false) {
									BadLogWaitTimer.StartTimer(BAD_LOG_WAIT);
								}
							}
							return;
						}
					case SERVERTALK1:				// intial ping to content server - a response starts everything
						File.LogRecord(Logs::High, Logs::LoginPacket, "ServerComm SERVERTALK1 - case:(%i) packet_size:(%i) ContentSocket:(%i)", data2_int, packet_size, Net.ContentSocket);
						Net.AddToActivityList(Net.ContentSocket, timestamp, 1); // give the connection server status
						Net.StartNetTimers(); // this could go somewhere else where contingent on server connections(beginservercomm())
						StartLogNetTimers();
						return;
					case SERVERTALK2:				// shutdown directive responce from content server - this packet will only be sent if prompted by admin
						File.LogRecord(Logs::High, Logs::LoginPacket, "ServerComm SERVERTALK2 shutdown directive - case:(%i) packet_size:(%i) ContentSocket:(%i)", data2_int, packet_size, Net.ContentSocket);
						ShutdownConnection(Net.ContentSocket);
						restart_conn = false;
						connected = false;
						return;						
					case CHECK_SERVER_CONNECTION:	// a signal from other servers to check connectivity - no response necessary
						//printf("\nServerComm CHECK_SERVER_CONNECTION - case:(%i) packet_size:(%i) ContentSocket:(%i)", LP_types, packet_size, Net.ContentSocket); // for heavy debugging				
						return;						
					case DUAL_LOGIN_RESPONSE:		// response from content server if other account is logged in content - verifies packet recv and hopefully a force logout
						File.LogRecord(Logs::High, Logs::LoginPacket, "ServerComm DUAL_LOGIN_RESPONSE - case:(%i) data1:(%s) data2:(%s) data_size:(%i) ContentSocket:(%i)",	data2_int, data_size, Net.ContentSocket);
						DoClientDualLogin(data3_int, data4_int);
						return;						

					case TEST_PACKETS:				// for heavy debugging
						printf("\nServerComm TEST_PACKETS - case:(%i) data_size:(%i) ContentSocket:(%i)", data2_int, data_size, Net.ContentSocket);
						return;						
					default:
						File.LogRecord(Logs::Normal, Logs::NetError, "ServerComm ERROR Default: - case:(%i) data_size:(%i) ContentSocket:(%i)", data2_int, data_size, Net.ContentSocket);
						printf("\nServerComm ERROR Default - case:(%i) data_size:(%i) ContentSocket:(%i)", data2_int, data_size, Net.ContentSocket);
						return;						
					}
				}
			}
		}
	}
}

// change the names of these variables to more general
void LoginNet::LoginPackets(int packet_type, int clienttalk_opvalue, int client_socket, int socket_position, std::string data_pos1, std::string data_pos2)
{
	unsigned int size_length;

	std::string s_len;
	std::string s_ct_op;

	s_ct_op = std::to_string(clienttalk_opvalue);

	switch (packet_type) {
	case LOGIN_EMPTY:
		size_length = 0x0008;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4); // this likely isn't needed
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		break;
	case LOGIN_1VAL:
		size_length = 0x0014;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		memcpy(Net.packet + 8, &data_pos1, 12);
		break;
	case LOGIN_2VAL:
		size_length = 0x0020;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		GProg.AddCharsToString(data_pos1, "!", 12);
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		memcpy(Net.packet + 8, &data_pos1, 12);
		memcpy(Net.packet + 20, &data_pos2, 12);
		break;
	case CONTENT_EMPTY:
		size_length = 0x0008;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4); // this likely isn't needed
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		break;
	case CONTENT_NUMBER:
		size_length = 0x0008 + strlen(data_pos1.c_str());
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);		
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);		
		strcpy(Net.packet + 8, data_pos1.c_str());
		break;
	case CONTENT_1VAL:
		size_length = 0x0014;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		memcpy(Net.packet + 8, &data_pos1, 12);
		break;
	case CONTENT_2VAL:
		size_length = 0x0020;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		GProg.AddCharsToString(data_pos1, "!", 12);
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		memcpy(Net.packet + 8, &data_pos1, 12);
		memcpy(Net.packet + 20, &data_pos2, 12);
		break;
	case CONTENT_CHAT:
		size_length = 0x0014 + strlen(data_pos2.c_str());
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		GProg.AddCharsToString(data_pos1, "!", 12);
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		memcpy(Net.packet + 8, &data_pos1, 12);
		strcpy(Net.packet + 20, data_pos2.c_str());
		break;
	}

	//printf("\nSEND: packet_type:(%i) size:(%i) s_op_value(%s) data_pos1(%s) data_pos2(%s)", packet_type, size_length, GProg.CharOutput(s_ct_op),
	//	sizeof(data_pos1) > 0 ? GProg.CharOutput(data_pos1) : "NULL", sizeof(data_pos2) > 0 ? GProg.CharOutput(data_pos2) : "NULL"); // for heavy debugging
	File.LogRecord(Logs::High, Logs::LoginPacket, "SEND: packet_type:(%i) size:(%i) s_op_value:(%s) data_pos1:(%s) data_pos2:(%s)\n", packet_type, size_length,
		GProg.CharOutput(s_ct_op), sizeof(data_pos1) > 0 ? GProg.CharOutput(data_pos1) : "NULL", sizeof(data_pos2) > 0 ? GProg.CharOutput(data_pos2) : "NULL");

	if (packet_type == LOGIN_EMPTY || packet_type == LOGIN_1VAL || packet_type == LOGIN_2VAL) {
		iResult = send(client_socket, Net.packet, size_length, 0);
		int sock_error = WSAGetLastError();
		if (iResult == SOCKET_ERROR) {
			SendClientPacketErrorHandling(clienttalk_opvalue, client_socket, socket_position);
		}
	}
	else {
		iResult = send(client_socket, Net.packet, size_length, 0);
		int sock_error = WSAGetLastError();
		if (iResult == SOCKET_ERROR) {
			SendServerPacketErrorHandling(clienttalk_opvalue);
		}
	}
}

void LoginNet::DoTestPackets()
{
	std::string s1 = "hello";
	std::string s2 = "dude";
	std::string i1 = "12345";
	std::string chat1 = "Adrian";
	std::string chat2 = "hey dude what is up you dickhead jerkoff";

	//	LoginPackets(LOGIN_EMPTY, 127, Net.ContentSocket, 1);
	//	LoginPackets(LOGIN_1VAL, 127, Net.ContentSocket, 1, s1);
	//	LoginPackets(LOGIN_2VAL, 127, Net.ContentSocket, 1, s1, s2);
	//	LoginPackets(CONTENT_EMPTY, 127, Net.ContentSocket, 1);
	//	LoginPackets(CONTENT_NUMBER, 127, Net.ContentSocket, 1, i1);
	//	LoginPackets(CONTENT_1VAL, 127, Net.ContentSocket, 1, s1);
	//	LoginPackets(CONTENT_2VAL, 127, Net.ContentSocket, 1, s1, s2);
	//	LoginPackets(CONTENT_CHAT, 29, Net.ContentSocket, 1, chat1, chat2);
	//	LoginPackets(CONTENT_CHAT, 127, Net.ContentSocket, 1, chat2);
}

void LoginNet::SendServerPacketErrorHandling(int servertalk_opvalue)
{
	int sock_error = WSAGetLastError();

	if (sock_error != WSAECONNRESET) {
		printf("\n connected-restart_conn:(%i:%i)\n", connected, restart_conn);
		printf("%s - servertalk_opvalue:(%i)\n", WSAGetLastErrorMessage("\nContentPackets: Send packet FAILED with error :"), servertalk_opvalue);
		File.LogRecord(Logs::Normal, Logs::NetError, "%s - servertalk_opvalue:(%i)", WSAGetLastErrorMessage("Send packet FAILED with error :"), servertalk_opvalue);
		//ShutdownConnection(Net.ContentSocket); // uncertain if a shutdown should occur, probably
	}
	if (servertalk_opvalue == CHECK_SERVER_CONNECTION) {
		File.LogRecord(Logs::Normal, Logs::NetError, "SOCKET ERROR: status-socket:(%i:%i) Server connection required. Closing all connections and performing shutdown.", servertalk_opvalue, Net.ContentSocket);
		printf("\nNo server connection! SHUTDOWN called - status-socket:(%i:%i)\n", servertalk_opvalue, Net.ContentSocket);
		ShutdownConnection(Net.ContentSocket);
		Net.CloseAllClientConnections();
		restart_conn = true;
		return;
	}
}

void LoginNet::SendClientPacketErrorHandling(int clienttalk_opvalue, int client_socket, int socket_position)
{
	int sock_error = WSAGetLastError();
	int server;

	server = Net.CheckActivityBySocket(client_socket, 0);

	if (sock_error != WSAECONNRESET) {
		printf("%s\n", WSAGetLastErrorMessage("\nContentPackets: Send packet FAILED with error :"));
		File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("Send packet FAILED with error :"));
	}
	if (server == 2) { // currently unused, no servers connected			
		File.LogRecord(Logs::Normal, Logs::NetError, "SOCKET ERROR: status-socket-position:(%i:%i:%i) Server connection required. Closing all connections and performing shutdown.", clienttalk_opvalue, client_socket, socket_position);
		printf("\nNo server connection! SHUTDOWN called - status-socket-position:(%i:%i:%i)\n", clienttalk_opvalue, client_socket, socket_position);
		Net.CloseAllClientConnections();
		restart_conn = true;
		return;
	}
	if (Net.LogoutByConnection(socket_position, NONE, true, false) == false) {
		if (Net.LogoutByConnection(socket_position, NONE, true, true) == false) {
			Net.LogoutByConnection(socket_position, NONE, false, false); // likely not needed because it's implied above
		}
	}
}

void LoginNet::DoClientDualLogin(int account_id, int clienttalk_opvalue)
{
	std::list<AccountInfo_struct>::iterator it;

	int client_socket;
	int socket_position;

	for (it = Net.acct_info_list.begin(); it != Net.acct_info_list.end(); ++it) {
		if (it->acct_id == account_id) {
			socket_position = it->client_id;
			client_socket = Net.client_sock[socket_position];
			//printf("\n\t DoClientDualLogin acctid-socket-position(%i:%i:%i)", account_id, client_socket, socket_position); // for heavy debugging
			break;
		}
	}

	if (client_socket <= 0 || socket_position <= 0) {
		printf("\n\t ERROR DoClientDualLogin acctid-socket-position(%i:%i:%i)", account_id, client_socket, socket_position); // for heavy debugging
		return;
	}

	switch (clienttalk_opvalue) {
	case DUAL_PROCEED_OLD:
		LoginPackets(LOGIN_EMPTY, LOGIN_SUCCESS_OLD, client_socket, socket_position);
		break;
	case DUAL_PROCEED_PW_CHG:
		LoginPackets(LOGIN_EMPTY, CHECK_PASSWORD_PW_CHG_VALIDITY, client_socket, socket_position);
		break;
	case DUAL_PROCEED_ADMIN:
		LoginPackets(LOGIN_EMPTY, ADMIN_LOG_SUCCESS, client_socket, socket_position);
		break;
	case DUAL_PROCEED_ADMIN2:
		LoginPackets(LOGIN_EMPTY, ADMIN_LOG_SUCCESS, client_socket, socket_position);
		break;
	case DUAL_LOCK_OLD:
		Acct.LogFail(socket_position, DUAL_LOGIN);
		Net.LogoutByConnection(socket_position, DUAL_LOCK_OLD, false, false);
		break;
	case DUAL_LOCK_PW_CHG:
		Acct.LogFail(socket_position, DUAL_LOGIN);
		Net.LogoutByConnection(socket_position, DUAL_LOCK_PW_CHG, false, false);
		break;
	case DUAL_LOCK_ADMIN:
		Acct.AdminLogFail(socket_position, ADMIN_DUAL_LOGIN);
		Net.LogoutByConnection(socket_position, DUAL_LOCK_ADMIN, false, false);
		break;
	case DUAL_LOCK_ADMIN2:
		Acct.AdminLogFail(socket_position, ADMIN_DUAL_LOGIN);
		Net.LogoutByConnection(socket_position, DUAL_LOCK_ADMIN2, false, false);
		break;
	}
}

void LoginNet::DoBadLogAccounts()
{
	std::map<int, int>::iterator it1;

	for (it1 = DBlogged_accts_map.begin(); it1 != DBlogged_accts_map.end(); ++it1) {
		//printf("\n DoBadLogAccounts admin?:(%i) acctID:(%i)\n", it1->second, it1->first); // for heavy debugging
		if (it1->second == 1) {
			CompareAccountLock(it1->first, true);
		}
		else {
			CompareAccountLock(it1->first, false);
		}		
	}
	content_acct_ids.clear();
	DBlogged_accts_map.clear();
	locked_accts_map.clear();
}

void LoginNet::CompareAccountLock(int p_acctID, bool admin)
{
	std::map<int, int>::iterator it1;
	std::vector<int>::iterator it2;
	
	it1 = locked_accts_map.find(p_acctID);
	it2 = std::find(content_acct_ids.begin(), content_acct_ids.end(), p_acctID);

	if (it1 != locked_accts_map.end()) { // locked accounts -> don't re-lock it
		return;
	}
	if (it2 != content_acct_ids.end()) { // logged accounts -> don't lock it
		return;
	}
	if (it1 == locked_accts_map.end()) { // not locked/logged because it's not in the "locked_accts_map" map -> lock it
		if (admin == true) {
			Acct.AdminLogFail(p_acctID, ADMIN_BAD_LOGOUT);
		}
		else {
			Acct.LogFail(p_acctID, BAD_LOGOUT);
		}
		return;
	} // don't think this condition will ever check
	if (it2 == content_acct_ids.end()) { // not logged because it's not in the "content_acct_ids" vector
		if (admin == true) {
			printf("\n not logged in admin -> locking");
			Acct.AdminLogFail(p_acctID, ADMIN_BAD_LOGOUT);
		}
		else {
			printf("\n not logged in user -> locking");
			Acct.LogFail(p_acctID, BAD_LOGOUT);
		}
		return;
	}
}

void LoginNet::ReserveAccountID(int socket_position)
{	
	std::map<int, int>::reverse_iterator rit;

	const char * p_table = "account";

	int open_acctID;
	
	if (new_acctID_map.empty() == false) {
		for (rit = new_acctID_map.rbegin(); rit != new_acctID_map.rend(); ++rit) {
			new_acctID_map[rit->first + 1] = socket_position;
			break;
		}
	}
	else {
		open_acctID = DB.GetAutoIncrementValue(p_table);
		new_acctID_map[open_acctID] = socket_position;
	}	
}

int LoginNet::GetReservedAcctID(int socket_position)
{
	std::map<int, int>::iterator it1;

	int res_acctID;

	for (it1 = new_acctID_map.begin(); it1 != new_acctID_map.end(); ++it1) {
		if (it1->second == socket_position) {
			res_acctID = it1->first;			
			break;
		}		
	}
	return res_acctID;
}

void LoginNet::RemoveReservedAcctID(int socket_position)
{
	if (socket_position < 0) {
		return;
	}
	std::map<int, int>::iterator it1;

	for (it1 = new_acctID_map.begin(); it1 != new_acctID_map.end(); ++it1) {
		if (it1->second == socket_position) {
			//std::cout << "\t\t RemoveReservedAcctID = " << it1->first << std::endl; // test
			new_acctID_map.erase(it1);
			break;
		}
	}
}

std::string LoginNet::GetReservedName(int socket_position)
{	
	std::map<int, std::string>::iterator it1;
	std::string res_name = "";

	it1 = reserve_name_map.find(socket_position);
	if (it1 != reserve_name_map.end()) {		
		res_name = it1->second;		
		return res_name;
	}
	return res_name;
}

bool LoginNet::FindReservedName(std::string chosen_name)
{
	std::map<int, std::string>::iterator it1;

	for (it1 = reserve_name_map.begin(); it1 != reserve_name_map.end(); ++it1) {
		if (it1->second == chosen_name) {			
			return true;
		}
	}
	return false;
}

void LoginNet::RemoveReservedName(int socket_position)
{
	if (socket_position < 0) {
		return;
	}
	std::map<int, std::string>::iterator it1;	

	it1 = reserve_name_map.find(socket_position);
	if (it1 != reserve_name_map.end()) {
		reserve_name_map.erase(it1);
	}
}

void LoginNet::DisableLogNetTimers()
{
	BadLogTimer.DisableTimer();
	BadLogWaitTimer.DisableTimer();
}

void LoginNet::StartLogNetTimers()
{
	BadLogTimer.StartTimer(BAD_LOG_TIMER);
	std::cout << "\t\t       -----Start LogNet Timers-----\n" << std::endl;
}

void LoginNet::ProcessLogNetTimers()
{
	if (connected == false) {
		return;
	}

	if (BadLogTimer.CheckTimer(BAD_LOG_TIMER) == true) { // put in an EOF condition in the packet eventually - this needs to go somewhere else
		LoginPackets(CONTENT_EMPTY, BAD_LOG, Net.ContentSocket, 0);
		DB.CheckBadLogAccounts();
	}
	if (BadLogWaitTimer.CheckTimer(BAD_LOG_WAIT) == true) { // put in an EOF condition in the packet eventually - this needs to go somewhere else
		Net.AddToLoggedAccounts();
		DoBadLogAccounts();
	}
}

bool LoginNet::CheckClientVersion(std::string client_version)
{
	bool version_valid = false;	

	if (client_version == RELEASE_VERSION) {
		version_valid = true;
	}
	return version_valid;
}
