// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W


#include "stdafx.h"

#include <map>

#include "content_net.h"
#include "content_packet.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"
#include "timers.h"


Timers ServerConnWaitTimer(REESTABLISH_CONN_TIMER, true);
Timers ChatMessExpireTimer(CHECK_MESS_EXPIRATION, true);
Timers ChatIdleTimer(IDLE_CHAT, false);

std::vector<unsigned int> socket_pos;
std::map<int, int> user_valid_map;
std::map<int, int> message_count_map;

bool connected = false; // extern
bool restart_conn = true; // new - extern - for controlled shutdown and restart attempts

int content_update_loops = 0;


void ContentNet::ReceiveContentPackets()
{
	++content_update_loops;

	SOCKET ClientSocket;
	ContentPacket_header_struct CPh_struct;

	//	// ---- much of this is being changed	
	int packet_size;			// packet size from recv call
	int timestamp;
	int response_stat;
	int acct_ID;				// needs to be different than "data3_int" - it comes form somewhere else and is part of validation
	int data1_int;				// packet size from header
	int data2_int;				// content op
	int data3_int;				// used for acct id
	int data4_int;				// used for pw code
	int l_count;				// user acct stats
	int avg_time;				// user acct stats

	std::string s_l_count;		// user acct stats
	std::string s_avg_time;		// user acct stats
	std::string s_response_status;
	std::string data1_string;
	std::string data2_string;	// used for pw code
	std::string data3_string;	// uses - acct_name/acct_ID
	std::string data4_string;	// used for pw code
	std::string name;			// name from DB
	std::string acct_name;		// name from memory from DB	
	std::string message_string;

	const char * pm_name;
	
	if (ServerConnWaitTimer.IsEnabled() == true) {
		if (ServerConnWaitTimer.CheckTimer(REESTABLISH_CONN_TIMER) == true) {
			File.LogRecord(Logs::Normal, Logs::NetError, "ERROR: Server connection required. Closing all connections and performing shutdown.");
			printf("\n\t     ---- No server connections! SHUTDOWN called ----\n");
			Net.CloseAllClientConnections();
			restart_conn = false;
			connected = false;
			return;
		}
	}

	/// --- new protocol test --- /// -- appears to work
	for (unsigned i = 0; i < socket_pos.size(); i++) {
		unsigned int c = socket_pos.at(i);
		ClientSocket = Net.client_sock[c];

		packet_size = recv(ClientSocket, Net.packet, 4, 0);
		if (packet_size == 0) {
			printf("\nContent: Closing connection - client did not signal socket close - socket-position(%i:%i)\n", ClientSocket, c);
			if (Net.LogoutByConnection(c, NONE, true, false, false) == false) {
				if (Net.LogoutByConnection(c, NONE, true, true, false) == false) {
					printf("\nContent: Closing connection - UNIDENTIFIED CONNECTION - socket-position(%i:%i)\n", ClientSocket, c);
					Net.LogoutByConnection(c, NONE, false, false, true); // should not be needed but may be a good safegaurd
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

		//	if (packet_size > 0) { printf("packet_size: %i\n", packet_size); } // for heavy debugging
		if (packet_size == 4) {
			unsigned int j = 0;
			while (j < (unsigned int)packet_size) {
				CPh_struct.deserialize(&(Net.packet[j])); // new
				
				std::string char_line = Net.packet; // was "const"
				data1_string = char_line.substr(0, 4);
				GProg.RemoveCharsFromString(data1_string, "!");
				data1_int = atoi(data1_string.c_str()); // packet size				
				
				int data_size = recv(ClientSocket, Net.packet, data1_int - 4, 0);				
				if (data_size < 0) {
					return;
				}
				
				//	if (data_size > 0) { printf("\npacket_size data_size: %i\n", data_size); } // for heavy debugging
				if (data_size == data1_int - 4) {
					
					// maybe put this above
					timestamp = static_cast<int>(time(0));
					if (Net.CheckActivityBySocket(ClientSocket, 0) == 0) {
						Net.AddToActivityList(ClientSocket, timestamp, 0);
					}
					else if (Net.CheckActivityBySocket(ClientSocket, 0) == 1) {
						Net.RemoveFromActivityList(ClientSocket);
						Net.AddToActivityList(ClientSocket, timestamp, 0);
					}

					unsigned int k = 0;
					while (k < (unsigned int)data_size) {
						CPh_struct.deserialize(&(Net.packet[k]));
						
						std::string char_line2 = Net.packet; // was "const"
						data2_string = char_line2.substr(0, 4);
						GProg.RemoveCharsFromString(data2_string, "!");
						data2_int = atoi(data2_string.c_str()); // op_value						
						
						if (data_size > 4) {
							data3_string = char_line2.substr(4, 12); // data_pos1
							GProg.RemoveCharsFromString(data3_string, "!");
							data3_int = atoi(data3_string.c_str()); // acctID usually
							name = Net.GetNameByAcctIDFromAcctInfo(data3_int);
							message_string = char_line2.substr(4, data_size - 4);
							if (data_size > 16) { // data_pos2
								data4_string = char_line2.substr(16, 12);
								data4_int = atoi(data4_string.c_str()); // acct code						
							}
						}

						acct_name = Net.GetNameFromAcctInfo(c);
						pm_name = name.c_str();

						//printf("\nRECV: size:(%i) op_value:(%i) data3:(%s) data4:(%s)", data_size, data2_int, GProg.CharOutput(data3_string), GProg.CharOutput(data4_string)); // for heavy debugging
						File.LogRecord(Logs::High, Logs::ContentPacket, "\n\t Begin Packet Process: size:(%i) op_value:(%i) data3:(%s)", data_size, data2_int, GProg.CharOutput(data3_string));

						k += data_size;
						switch (data2_int) {
						case DUAL_PROCEED_OLD:				// these are sent from login server to logout a dual login user
							File.LogRecord(Logs::High, Logs::ContentPacket, "DUAL_PROCEED_OLD - case:(%i) input1:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::Account, "Dual Login: logging out current account(%s)", pm_name);
							Net.ForceLogoutByAccount(data3_int, false);
							s_response_status = std::to_string(DUAL_PROCEED_OLD);
							SendServerPackets(CONTENT_2VAL, DUAL_LOGIN_RESPONSE, data3_string, s_response_status);
							return;
						case DUAL_PROCEED_PW_CHG:
							File.LogRecord(Logs::High, Logs::ContentPacket, "DUAL_PROCEED_PW_CHG - case:(%i) input1:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::Account, "Dual Login: logging out current account(%s)", pm_name);
							Net.ForceLogoutByAccount(data3_int, false);
							s_response_status = std::to_string(DUAL_PROCEED_PW_CHG);
							SendServerPackets(CONTENT_2VAL, DUAL_LOGIN_RESPONSE, data3_string, s_response_status);
							return;
						case DUAL_PROCEED_ADMIN: // admin true??
							File.LogRecord(Logs::High, Logs::ContentPacket, "DUAL_PROCEED_ADMIN - case:(%i) input1:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::Account, "Dual Login: Admin - logging out current admin account(%s)", pm_name);
							Net.ForceLogoutByAccount(data3_int, true);
							s_response_status = std::to_string(DUAL_PROCEED_ADMIN);
							SendServerPackets(CONTENT_2VAL, DUAL_LOGIN_RESPONSE, data3_string, s_response_status);
							return;
						case DUAL_PROCEED_ADMIN2:
							File.LogRecord(Logs::High, Logs::ContentPacket, "DUAL_PROCEED_ADMIN2 - case:(%i) input1:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::Account, "Dual Login: Admin - logging out user account association (%s)", pm_name);
							Net.ForceLogoutByAccount(data3_int, false);
							s_response_status = std::to_string(DUAL_PROCEED_ADMIN2);
							SendServerPackets(CONTENT_2VAL, DUAL_LOGIN_RESPONSE, data3_string, s_response_status);
							return;
						case DUAL_LOCK_OLD:
							File.LogRecord(Logs::High, Logs::ContentPacket, "DUAL_LOCK_OLD - case:(%i) input1:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::Account, "Dual Login: logging out current account(%s)", pm_name);
							Net.ForceLogoutByAccount(data3_int, false);
							s_response_status = std::to_string(DUAL_LOCK_OLD);
							SendServerPackets(CONTENT_2VAL, DUAL_LOGIN_RESPONSE, data3_string, s_response_status);
							return;
						case DUAL_LOCK_PW_CHG:
							File.LogRecord(Logs::High, Logs::ContentPacket, "DUAL_LOCK_PW_CHG - case:(%i) input1:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::Account, "Dual Login: logging out current account(%s)", pm_name);
							Net.ForceLogoutByAccount(data3_int, false);
							s_response_status = std::to_string(DUAL_LOCK_PW_CHG);
							SendServerPackets(CONTENT_2VAL, DUAL_LOGIN_RESPONSE, data3_string, s_response_status);
							return;
						case DUAL_LOCK_ADMIN: // admin true??
							File.LogRecord(Logs::High, Logs::ContentPacket, "DUAL_LOCK_ADMIN - case:(%i) input1:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::Account, "Dual Login: Admin - logging out current admin account(%s)", pm_name);
							Net.ForceLogoutByAccount(data3_int, true);
							s_response_status = std::to_string(DUAL_LOCK_ADMIN);
							SendServerPackets(CONTENT_2VAL, DUAL_LOGIN_RESPONSE, data3_string, s_response_status);
							return;
						case DUAL_LOCK_ADMIN2:
							File.LogRecord(Logs::High, Logs::ContentPacket, "DUAL_LOCK_ADMIN2 - case:(%i) input1:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::Account, "Dual Login: Admin - logging out user account association (%s)", pm_name);
							Net.ForceLogoutByAccount(data3_int, false);
							s_response_status = std::to_string(DUAL_LOCK_ADMIN2);
							SendServerPackets(CONTENT_2VAL, DUAL_LOGIN_RESPONSE, data3_string, s_response_status);
							return;
						case SERVERTALK3:						// from login server with acctID and code before client sends his copy
							File.LogRecord(Logs::High, Logs::ContentPacket, "SERVERTALK3 - case:(%i) acctID:(%i) code:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, data3_int, data4_int, packet_size, ClientSocket, c);
							user_valid_map[data3_int] = data4_int;
							return;
						case BAD_LOG:
							File.LogRecord(Logs::High, Logs::ContentPacket, "BAD_LOG - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.FindUserAccount();
							return;
						case CONNECT:						// from client with acctID and code
							File.LogRecord(Logs::High, Logs::ContentPacket, "CONNECT - case:(%i) acctID:(%i) code:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, data3_int, data4_int, packet_size, ClientSocket, c);
							response_stat = UserValidation(data3_int, data4_int);
							if (response_stat == VALIDATE_CREDS) {
								name = DB.GetAccountName(data3_int);
								Net.AddToAcctInfoList(c, data3_int, name);
							}
							ContentPackets(CONTENT_EMPTY, response_stat, ClientSocket, c);
							return;
						case VALIDATE_CREDS:				// this is a second name validation
							File.LogRecord(Logs::High, Logs::ContentPacket, "VALIDATE_CREDS - case:(%i) name:(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size, ClientSocket, c);
							acct_ID = Net.GetAcctIDFromAcctInfo(c);
							if (data3_string != acct_name) {
								data2_int = INVALID_CREDS;
							}
							response_stat = Acct.CheckAccountCreds(acct_ID, data3_string, data2_int);
							if (response_stat == VALID_CREDS) {
								Net.AddToContentUserList(c, ClientSocket, acct_ID, 0, 0, 0, VALID_CREDS);
								AddMessageCounters(c, timestamp, true);
								l_count = DB.GetLoginCount(acct_ID, false);
								avg_time = DB.GetAverageSessionTime(acct_ID, false);
								s_l_count = std::to_string(l_count);
								s_avg_time = std::to_string(avg_time);
								if (l_count == 1) {
									response_stat = CONTENT_1ST_LOGIN;
								}
							}
							RemoveValidation(acct_ID);
							ContentPackets(CONTENT_2VAL, response_stat, ClientSocket, c, s_l_count, s_avg_time);
							return;
						case CHAT_OUT_SAY:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CHAT_OUT_SAY - case:(%i) input(%s) data_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(message_string), data_size, ClientSocket, c);
							if (CheckMessageCounter(c, timestamp) == true) {
								ContentPackets(CONTENT_EMPTY, MESSAGES_EXCEEDED, ClientSocket, c);
								StartChatIdleTimer();
								return;
							}
							AddMessageCounters(c, timestamp, false);
							Net.ProcessChat(c, ClientSocket, CHAT_OUT_SAY, message_string);
							return;
						case CHAT_OUT_TELL:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CHAT_OUT_TELL - case:(%i) input(%s) packet_size:(%i) socket:(%i) position:(%i)", data2_int, GProg.CharOutput(message_string), packet_size, ClientSocket, c);
							if (CheckMessageCounter(c, timestamp) == true) {
								ContentPackets(CONTENT_EMPTY, MESSAGES_EXCEEDED, ClientSocket, c);
								StartChatIdleTimer();
								return;
							}
							AddMessageCounters(c, timestamp, false);
							Net.ProcessChat(c, ClientSocket, CHAT_OUT_TELL, message_string);
							return;
						case CONTENT_GOTO_MENU:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CONTENT_GOTO_MENU - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.RemoveUserFromList(c); // need to find a better way
							acct_ID = Net.GetAcctIDFromAcctInfo(c); // need to find a better way
							Net.AddToContentUserList(c, ClientSocket, acct_ID, 0, 0, 0, CONTENT_GOTO_MENU); // need to find a better way
							ContentPackets(CONTENT_EMPTY, CONTENT_GOTO_MENU, ClientSocket, c);
							return;
						case INVALID_CREDS:
							File.LogRecord(Logs::High, Logs::ContentPacket, "INVALID_CREDS - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, TO_LOGIN, false, false, false);
							return;
						case ACCT_LOCKED:
							File.LogRecord(Logs::High, Logs::ContentPacket, "ACCT_LOCKED - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, TO_LOGIN, false, false, false);
							return;
						case VALID_CREDS:
							File.LogRecord(Logs::High, Logs::ContentPacket, "VALID_CREDS - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, CONTENT_GOTO_MENU, ClientSocket, c);
							return;
						case DB_ERROR:
							File.LogRecord(Logs::High, Logs::ContentPacket, "DB_ERROR - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, TO_LOGIN, false, false, false);
							return;
						case CONTENT_INVALID_INPUT:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CONTENT_INVALID_INPUT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, CONTENT_INVALID_INPUT, ClientSocket, c);
							return;
						case LOGOUT_CONTENT_SERVER:
							File.LogRecord(Logs::High, Logs::ContentPacket, "LOGOUT - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, LOGOUT_CONTENT_SERVER, true, false, false);
							return;
						case SERVERTALK1:				// initial/test ping from login server
							File.LogRecord(Logs::High, Logs::ContentPacket, "SERVERTALK1 - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::General, "Login server connected on socket-position:(%i:%i)", ClientSocket, c);
							if (Net.CheckActivityBySocket(ClientSocket, 0) == 1) { // server socket exists
								Net.RemoveFromActivityList(ClientSocket);
								Net.AddToActivityList(ClientSocket, timestamp, 1); // delete previous activty and, give the connection server status
							}
							ContentPackets(CONTENT_EMPTY, SERVERTALK1, ClientSocket, c);
							Net.AddToContentUserList(c, ClientSocket, 0, 0, 0, 1, SERVERTALK1); // give the user server status - need to be able to use more cases other than VALID_CREDS
							Net.StartNetTimers();
							ConNet.StartConNetTimers();
							return;
						case SERVERTALK2:				// shutdown directive from login server
							File.LogRecord(Logs::High, Logs::ContentPacket, "SERVERTALK2 - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							File.LogRecord(Logs::Normal, Logs::General, "Login server connected for SHUTDOWN on socket-position:(%i:%i)", ClientSocket, c);
							Net.AddToContentUserList(c, ClientSocket, 0, 0, 0, 1, SERVERTALK2);
							Net.LogoutByConnection(c, SERVERTALK2, false, false, true);
							restart_conn = false;
							connected = false;
							return;
						case CHECK_SERVER_CONNECTION:	// a signal from other servers to check connectivity
							//printf("\nCHECK_SERVER_CONNECTION - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c); // for heavy debugging
							return;
						case TO_LOGIN:
							File.LogRecord(Logs::High, Logs::ContentPacket, "TO_LOGIN - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.LogoutByConnection(c, TO_LOGIN, true, false, false);
							return;
						case CHAT_INSTRUCTIONS:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CHAT_INSTRUCTIONS - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.RemoveUserFromList(c);														// need to find a better way
							acct_ID = Net.GetAcctIDFromAcctInfo(c);											// need to find a better way
							Net.AddToContentUserList(c, ClientSocket, acct_ID, 0, 0, 0, CHAT_INSTRUCTIONS); // need to find a better way
							ContentPackets(CONTENT_EMPTY, CHAT_INSTRUCTIONS, ClientSocket, c);
							return;
						case CHAT_BEGIN:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CHAT_BEGIN - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							Net.RemoveUserFromList(c);													// need to find a better way
							acct_ID = Net.GetAcctIDFromAcctInfo(c);										// need to find a better way
							Net.AddToContentUserList(c, ClientSocket, acct_ID, 1, 0, 0, CHAT_BEGIN);	// need to find a better way
							ContentPackets(CONTENT_EMPTY, CHAT_BEGIN, ClientSocket, c);
							return;
						case CHAT_PROBATION_EXPIRE:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CHAT_PROBATION_EXPIRE - case:(%i) packet_size:(%i) socket:(%i) position:(%i)", data2_int, packet_size, ClientSocket, c);
							RemoveChatProbation(c);
							return;
						case MAIL_GOTO_MENU:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_GOTO_MENU - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, MAIL_GOTO_MENU, ClientSocket, c);
							return;
						case MAIL_BOX:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_BOX - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, MAIL_BOX, ClientSocket, c);
							return;
						case MAIL_INSTRUCTIONS:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_INSTRUCTIONS - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, MAIL_INSTRUCTIONS, ClientSocket, c);
							return;
						case MAIL_INBOX:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_INBOX - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_INBOX, "");
							return;
						case MAIL_OUTBOX:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_OUTBOX - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_OUTBOX, "");
							return;
						case CONTENT_INVALID_INPUT3:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CONTENT_INVALID_INPUT3 - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, CONTENT_INVALID_INPUT3, ClientSocket, c);
							return;							
						case MAIL_DELETE_INBOX:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_INBOX - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_INBOX, "");
							return;
						case MAIL_DELETE_OUTBOX:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_OUTBOX - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_OUTBOX, "");
							return;
						case MAIL_DELETE_ALL_OPTIONS:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_ALL_OPTIONS - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, MAIL_DELETE_ALL_OPTIONS, ClientSocket, c);
							return;
						case MAIL_DELETE_ALL_READ:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_ALL_READ - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_ALL_READ, "");
							return;
						case MAIL_DELETE_ALL_UNREAD:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_ALL_UNREAD - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_ALL_UNREAD, "");
							return;
						case MAIL_DELETE_ALL_INBOX:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_ALL_INBOX - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_ALL_INBOX, "");
							return;
						case MAIL_DELETE_ALL_OUTBOX:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_ALL_OUTBOX - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_ALL_OUTBOX, "");
							return;
						case MAIL_DELETE_READ:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_READ - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_READ, message_string);
							return;
						case MAIL_DELETE_UNREAD:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_UNREAD - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_UNREAD, message_string);
							return;
						case MAIL_DELETE_SENT:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_DELETE_SENT - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_DELETE_SENT, message_string);
							return;						
						case MAIL_INBOX_READ:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_INBOX_READ - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_INBOX_READ, message_string);
							return;
						case MAIL_INBOX_UNREAD:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_INBOX_UNREAD - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_INBOX_UNREAD, message_string);
							return;
						case MAIL_OUTBOX_SENT:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_OUTBOX_SENT - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							Net.ProcessMail(c, ClientSocket, MAIL_OUTBOX_SENT, message_string);
							return;
						case MAIL_TO_SEND: // this is both the first and the last step in sending a mail	
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_TO_SEND - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_RECIPIENT, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_TO_SEND, message_string);
							return;							
						case MAIL_TO_REPLY: // this should be both the first and the last step in sending a reply
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_TO_REPLY - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_REPLY, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_TO_REPLY, message_string);
							return;
						case CHAT_MENU:
							File.LogRecord(Logs::High, Logs::ContentPacket, "CHAT_MENU - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, CHAT_MENU, ClientSocket, c);
							return;
						case MAIL_TO_SEND_RECIPIENT:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_TO_SEND_RECIPIENT - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_RECIPIENT, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_TO_SEND_RECIPIENT, message_string);
							return;
						case MAIL_TO_SEND_SUBJECT:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_TO_SEND_SUBJECT - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_SUBJECT, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_TO_SEND_SUBJECT, message_string);
							return;
						case MAIL_TO_SEND_BODY:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_TO_SEND_BODY - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_BODY, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_TO_SEND_BODY, message_string);
							return;
						case MAIL_TO_SEND_REPLY:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_TO_SEND_REPLY - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_REPLY, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_TO_SEND_REPLY, message_string);
							return;
						case MAIL_TO_SEND_REVIEW:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_TO_SEND_REVIEW - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_REVIEW, ClientSocket, c);
							return;
						case MAIL_REVIEW_MESSAGE:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_REVIEW_MESSAGE - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							// edit status = 0
							ContentPackets(CONTENT_EMPTY, MAIL_REVIEW_MESSAGE, ClientSocket, c);
							return;
						case MAIL_EDIT_RECIPIENT:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_EDIT_RECIPIENT - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_EDIT_RECIPIENT, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_EDIT_RECIPIENT, message_string);
							return;
						case MAIL_EDIT_SUBJECT:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_EDIT_SUBJECT - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_EDIT_SUBJECT, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_EDIT_SUBJECT, message_string);
							return;
						case MAIL_EDIT_BODY:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_EDIT_BODY - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_EDIT_BODY, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_EDIT_BODY, message_string);
							return;
						case MAIL_EDIT_REPLY:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_EDIT_REPLY - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							if (data_size == 4) {
								ContentPackets(CONTENT_EMPTY, MAIL_EDIT_REPLY, ClientSocket, c);
								return;
							}
							Net.ProcessMail(c, ClientSocket, MAIL_EDIT_REPLY, message_string);
							return;
						case MAIL_TO_SEND_SUCCESS:
							File.LogRecord(Logs::High, Logs::ContentPacket, "MAIL_TO_SEND_SUCCESS - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							ContentPackets(CONTENT_EMPTY, MAIL_GOTO_MENU, ClientSocket, c);
							return;

						case TEST_PACKETS:				// for heavy debugging
							printf("\nTEST_PACKETS - case-headsize-datasize:(%i-%i-%i) data3-data4:(%s>-<%s) socket-position:(%i:%i)\n",
								data2_int, packet_size, data_size, GProg.CharOutput(data3_string), GProg.CharOutput(data4_string), ClientSocket, c);							
							return;
						default:
							File.LogRecord(Logs::Normal, Logs::NetError, "ContentPacket ERROR Default: - case:(%i) data_size:(%i) socket:(%i) position:(%i)", data2_int, data_size, ClientSocket, c);
							printf("\nERROR Default - case-headsize-datasize:(%i-%i-%i) data3-data4:(%s>-<%s) socket-position:(%i:%i)\n",
								data2_int, packet_size, data_size, GProg.CharOutput(data3_string), GProg.CharOutput(data4_string), ClientSocket, c);
							return;
						}
					}
				}
			}
		}
	}
}

// change the names of these variables to more general
void ContentNet::ContentPackets(int packet_type, int clienttalk_opvalue, int client_socket, int socket_position, std::string data_pos1, std::string data_pos2)
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
		GProg.AddCharsToString(s_ct_op, "!", 4);
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
		GProg.AddCharsToString(s_ct_op, "!", 4);
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
	case CONTENT_MAIL:
		size_length = 0x0008 + strlen(data_pos1.c_str());
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		strcpy(Net.packet + 8, data_pos1.c_str());
		break;
	}


	if (packet_type == CONTENT_NUMBER) { // for heavy debugging
		//printf("\nSEND: packet_type:(%i) size:(%i) s_op_value(%s) data_pos1(%s) data_pos2(%s)", packet_type, size_length, GProg.CharOutput(s_ct_op),
		//	sizeof(data_pos1) > 0 ? GProg.CharOutput(data_pos1) : "NULL", sizeof(data_pos2) > 0 ? GProg.CharOutput(data_pos2) : "NULL");
	}
	File.LogRecord(Logs::High, Logs::ContentPacket, "SEND: packet_type:(%i) size:(%i) s_op_value:(%s) data_pos1:(%s) data_pos2:(%s)", packet_type, size_length,
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
			SendClientPacketErrorHandling(clienttalk_opvalue, client_socket, socket_position);
		}
	}
}

// this may not be needed - not true, it finds the server socket
void ContentNet::SendServerPackets(int packet_type, int status_value, std::string data_pos1, std::string data_pos2)
{	
	std::list<User_struct>::iterator it;		

	for (it = Net.user_list.begin(); it != Net.user_list.end(); ++it) {
		if (it->server == 1) {
			int server_socket = it->socket;
			int socket_position = it->client_id;

			switch (packet_type) {
			case CONTENT_NUMBER:
				File.LogRecord(Logs::High, Logs::ContentPacket, "SPC_CONTENT_NUMBER - case:(%i) input1:(%s) socket:(%i) position:(%i)", status_value, GProg.CharOutput(data_pos1), server_socket, socket_position);
				//printf("\n Send ServerPackets data_pos1(%s) server_opvalue-socket-position(%i:%i:%i)", GProg.CharOutput(data_pos1), status_value, server_socket, socket_position); // for heavy debugging
				ContentPackets(CONTENT_NUMBER, status_value, server_socket, socket_position, data_pos1);
				break;
			case CONTENT_EMPTY:
				File.LogRecord(Logs::High, Logs::ContentPacket, "SPC_CONTENT_EMPTY - case:(%i) socket:(%i) position:(%i)", status_value, server_socket, socket_position);
				//printf("\n Send ServerPackets server_opvalue-socket-position(%i:%i:%i)", status_value, server_socket, socket_position); // for heavy debugging
				ContentPackets(CONTENT_EMPTY, status_value, server_socket, socket_position);
				break;
			case CONTENT_2VAL:
				File.LogRecord(Logs::High, Logs::ContentPacket, "SPC_CONTENT_2VAL - case:(%i) input1:(%s) input2:(%s) socket:(%i) position:(%i)", status_value, GProg.CharOutput(data_pos1), GProg.CharOutput(data_pos2), server_socket, socket_position);
				//printf("\n Send ServerPackets data1-data2(%s:%s) server_opvalue-socket-position(%i:%i:%i)", GProg.CharOutput(data_pos1), GProg.CharOutput(data_pos2), status_value, server_socket, socket_position); // for heavy debugging
				ContentPackets(CONTENT_2VAL, status_value, server_socket, socket_position, data_pos1, data_pos2);
				break;
			}			
		}
	}
}

void ContentNet::SendClientPacketErrorHandling(int status_value, int client_socket, int socket_position)
{
	int sock_error = WSAGetLastError();
	int server;

	server = Net.CheckActivityBySocket(client_socket, 0);

	if (sock_error != WSAECONNRESET) {
		printf("%s", WSAGetLastErrorMessage("\nContentPackets: Send packet FAILED with error :"));
		File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("Send packet FAILED with error :"));
	}
	if (server == 2) { // if server packet doesn't go through, close the server connection and wait for reconnect timer		
		Net.LogoutByConnection(socket_position, NONE, false, false, true);
		if (ServerConnWaitTimer.IsEnabled() == false) {
			ServerConnWaitTimer.StartTimer(REESTABLISH_CONN_TIMER);
			std::cout << " --- Enabling RestartConnectionTimer" << std::endl; // for heavy debugging			
			return;
		}				
	}
	if (Net.LogoutByConnection(socket_position, NONE, true, false, false) == false) {
		if (Net.LogoutByConnection(socket_position, NONE, true, true, false) == false) {
			Net.LogoutByConnection(socket_position, NONE, false, false, true); // likely not needed because it's handled above
		}
	}
}

int ContentNet::UserValidation(int acct_id, int validation_code)
{
	int response_stat;

	if (FindValidation(acct_id, validation_code) == true) {
		response_stat = VALIDATE_CREDS;
	}
	else {
		response_stat = INVALID_CREDS;
	}
	return response_stat;
}

bool ContentNet::FindValidation(int acct_id, int validation_num)
{
	std::map<int, int>::iterator it1;

	it1 = user_valid_map.find(acct_id);
	if (it1 != user_valid_map.end()) {
		if (it1->second == validation_num) {
			//printf("\n it1->first(%i) acct_id(%i) - it1->second(%i) validation_num(%i)", it1->first, acct_id, it1->second, validation_num); // for heavy debugging			
			return true;
		}
	}
	return false;
}

void ContentNet::RemoveValidation(int acct_id)
{
	if (acct_id < 0) {
		return;
	}
	std::map<int, int>::iterator it1;

	it1 = user_valid_map.find(acct_id);
	if (it1 != user_valid_map.end()) {
		//std::cout << "\n RemoveValidation acct_id = " << acct_id << std::endl; // test
		user_valid_map.erase(it1);
	}
}

void ContentNet::ProcessMessageCounters()
{
	if (mess_time_list.size() <= 0) {
		return;
	}

	int timestamp = static_cast<int>(time(0));
	int sock_pos = 0;	

	for (std::list<ChatMessageTime_struct>::iterator it1 = mess_time_list.begin(); it1 != mess_time_list.end(); ++it1) {
		if (timestamp >= it1->time_value + MESSAGE_EXPIRATION && it1->is_probation == 0) {
			sock_pos = it1->socket_pos;
			DoUserMessCounter(sock_pos, -1, true, false);
			mess_time_list.erase(it1);
		}
	}
}

void ContentNet::DoUserMessCounter(int socket_position, int counter, bool found, bool remove)
{
	std::map<int, int>::iterator it2;	

	if (found == false) {
		message_count_map[socket_position] = counter;
		//printf("\n DoUserMessCounter not found - sock_pos-count:(%i-%i)", socket_position, counter); // for heavy debugging
	}	
	else {
		it2 = message_count_map.find(socket_position);
		if (it2 != message_count_map.end()) {
			if (remove == true) {
				//printf("\n DoUserMessCounter  remove - sock_pos-count:(%i-%i)", socket_position, counter); // for heavy debugging
				message_count_map.erase(it2);
			}
			else {
				counter = it2->second + counter;
				//printf("\n DoUserMessCounter found - sock_pos-count:(%i-%i)", socket_position, counter); // for heavy debugging
				if (counter < 0) {
					counter = 0;
				}
				message_count_map.erase(it2);
				if (counter != 0) {
					message_count_map[socket_position] = counter;
				}
			}
		}
	}
}

void ContentNet::RemoveChatProbation(int socket_position)
{
	int timestamp = static_cast<int>(time(0));

	std::list<ChatMessageTime_struct>::iterator it1;

	//printf("\n RemoveChatProbation - sock_pos-size:(%i-%i)", socket_position, mess_time_list.size()); // for heavy debugging

	for (it1 = mess_time_list.begin(); it1 != mess_time_list.end(); ++it1) {
		if (it1->socket_pos == socket_position && it1->is_probation == 1) {
			DoUserMessCounter(socket_position, -5, true, false);
			mess_time_list.erase(it1);
			break;
		}
	}
}

void ContentNet::AddMessageCounters(int socket_position, int current_time, bool login)
{
	if (socket_position < 0) {
		return;
	}

	int probation = 0;

	if (FindUserChatFootprint(socket_position) == true) {
		DoUserMessCounter(socket_position, 1, true, false);
	}
	else if (login == true) {
		DoUserMessCounter(socket_position, 5, false, false);
		probation = 1;
	}
	else {
		DoUserMessCounter(socket_position, 1, false, false);
	}

	ChatMessageTime_struct chat_mess_time;
	new ChatMessageTime_struct;

	chat_mess_time.socket_pos = socket_position;
	chat_mess_time.time_value = current_time;
	chat_mess_time.is_probation = probation;
	mess_time_list.push_back(chat_mess_time);

	//printf("\n AddMessageCounters - sock_pos-time-probation:(%i-%i-%i)", socket_position, current_time, probation); // for heavy debugging
}

bool ContentNet::FindUserChatFootprint(int socket_position)
{
	for (std::list<ChatMessageTime_struct>::iterator it1 = mess_time_list.begin(); it1 != mess_time_list.end(); ++it1) {
		if (it1->socket_pos == socket_position) {
			return true;
		}
	}
	return false;
}

void ContentNet::RemoveUserFromChatFootprint(int socket_position)
{
	if (socket_position < 0) {
		return;
	}

	for (std::list<ChatMessageTime_struct>::iterator it1 = mess_time_list.begin(); it1 != mess_time_list.end(); ++it1) {
		if (it1->socket_pos == socket_position) {
			mess_time_list.erase(it1);
		}
	}	
	DoUserMessCounter(socket_position, 0, true, true);
}

bool ContentNet::CheckMessageCounter(int socket_position, int current_time)
{
	bool exceeds = false;
	std::map<int, int>::iterator it1;
	
	it1 = message_count_map.find(socket_position);
	if (it1->second >= MAX_MESSAGES) {
		exceeds = true;
	}	
	return exceeds;
}

void ContentNet::DisableConNetTimers()
{
	if (ServerConnWaitTimer.IsEnabled() == true) {
		ServerConnWaitTimer.DisableTimer();
	}
	if (ChatMessExpireTimer.IsEnabled() == true) {
		ChatMessExpireTimer.DisableTimer();
	}	
}

void ContentNet::DisableConnWaitTimer()
{
	if (ServerConnWaitTimer.IsEnabled() == true) {
		ServerConnWaitTimer.DisableTimer();
	}	
}

void ContentNet::StartConNetTimers()
{
	if (ChatMessExpireTimer.IsEnabled() == false) {
		ChatMessExpireTimer.StartTimer(CHECK_MESS_EXPIRATION);		
	}		
	std::cout << "\t\t       -----Start ConNet Timers-----\n" << std::endl;
}

void ContentNet::StartChatIdleTimer()
{
	ChatIdleTimer.StartTimer(CHECK_MESS_EXPIRATION);
}

void ContentNet::ProcessConNetTimers()
{
	if (connected == false) {
		return;
	}
	Net.ChangeServerTime();
	if (ChatMessExpireTimer.CheckTimer(CHECK_MESS_EXPIRATION) == true) {
		ProcessMessageCounters();
	}	
}

bool ContentNet::IsChatTimerEnabled()
{
	if (ChatIdleTimer.IsEnabled() == false) {
		return false;
	}
	else if (ChatIdleTimer.CheckTimer(IDLE_CHAT) == true) {
		return false;
	}
	else {		
		return true;
	}
}
