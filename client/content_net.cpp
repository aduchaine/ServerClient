// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W

#include "stdafx.h"

#include "client_packet.h"
#include "content_net.h"
#include "content_packet.h"

#include "content.h"
#include "gen_program.h"
#include "text_output.h"
#include "timers.h"


Timers ChatProbationTimer(CHAT_PROBATION, false);
Timers ContentActivityTimer(CONNECTION_TIMER, false);

bool remove_prmt = false;
bool key_prmt = false;

int content_update_loops = 0;
int content_stat = 0;
int edit_stat = 0;
unsigned int mail_list_counter = 0;

std::string prmt_to_remove = chat_user_prompt;

//std::vector<std::string> msgid; // not sure this is used


void ContentNet::InitializeContentLoop()
{
	if (Net.InitializeContentConnection() == true) {
		connected = true;
		Net.ClientPackets(CONTENT_2VAL, CONNECT, pm_acctID, random);
	}
	else {
		logged = false;
	}
}

void ContentNet::ContentUpdate()
{
	++content_update_loops;

	ClientPacket_header_struct CPh_struct; // new

	int data1_int;				// added new protocol
	int data2_int;				// added new protocol
	int data3_int;				// added for mail system
	int data4_int;				// added for mail system
	int data_cat;				// added for mail system

	std::string data1_string;
	std::string data2_string;	// added new protocol
	std::string data3_string;	// added new protocol
	std::string data4_string;	// added for mail system
	std::string message_string;
	std::string s_message_id;	// added for mail system - when changing mail_status
	std::string s_response_stat;// added for mail system - when changing mail_status
	std::string vec_name;		// added for mail system - send input name
	std::string vec_subject;	// added for mail system - send input subject
	std::string vec_body;		// added for mail system - send input body
	std::string vec_reply;		// added for mail system - send input reply body

	Cont.ProcessContent(menu_stat, content_stat);

	/// --- new protocol test --- ///
	int packet_size = recv(Net.ConnectSocket, Net.packet, 4, 0);
	if (packet_size == 0) { // this occurs when the client socket close was not done properly - server needs to shut down socket first
		printf("\nContent: Closing connection - server did not signal socket close - socket(%i)\n", Net.ConnectSocket);
		// here a second timer should be activated to recheck the connection by reinitializing it?	
		// send another packet now and get result - disconnect if no response
		ShutdownConnection(Net.ConnectSocket);
		WSACleanup();
		logged = false;
		connected = false;
		return;
	}
	if (packet_size < 0) { // if this happens, errant packet handling was not done properly
		/*	if (ContentActivityTimer.CheckTimer(CONNECTION_TIMER) == true) {
				printf("\nContent: Closing connection - server timeout - socket(%i)\n", Net.ConnectSocket);
				// here a second timer should be activated to recheck the connection by reinitializing it?
				// send another packet now and get result - disconnect if no response
				ShutdownConnection(Net.ConnectSocket);
				WSACleanup();
				logged = false;
				connected = false;
				} */
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
						GProg.RemoveCharsFromString(data3_string, "!");						
						data3_int = atoi(data3_string.c_str()); // 
						message_string = char_line2.substr(4, data_size - 4);
						if (data_size > 16) { // data_pos2
							data4_string = char_line2.substr(16, 12); // data_pos2
							GProg.RemoveCharsFromString(data4_string, "!");
							data4_int = atoi(data4_string.c_str());						
						}
					}

					// printf("\nsize:(%i) op_value:(%i) data3:(%s)\n", data_size, data2_int, GProg.CharOutput(data3_string)); // for heavy debugging

					k += data_size;
					switch (data2_int) {
					case VALIDATE_CREDS:
						//printf("\nVALIDATE_CREDS - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging					
						Net.ClientPackets(CONTENT_1VAL, VALIDATE_CREDS, pm_name);
						return;
					case CHAT_IN_SAY:
						//printf("\nCHAT_IN_SAY - case:(%i) input(%s) packet_size:(%i)\n", data2_int, GProg.CharOutput(message_string), packet_size); // for heavy debugging
						Cont.ChatIn(CHAT_IN_SAY, message_string);
						return;
					case CHAT_IN_TELL:
						//printf("\nCHAT_IN_TELL - case:(%i) input(%s) packet_size:(%i)\n", data2_int, GProg.CharOutput(message_string), packet_size); // for heavy debugging
						Cont.ChatIn(CHAT_IN_TELL, message_string);
						return;
					case CHAT_USER_JOIN:
						//printf("\nCHAT_USER_JOIN - case(%i) data3_string(%s) packet_size:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size); // for heavy debugging
						Cont.ChatIn(CHAT_USER_JOIN, data3_string);
						return;
					case CHAT_USER_LEAVE:
						//printf("\nCHAT_USER_LEAVE - case(%i) data3_string(%s) packet_size:(%i)", data2_int, GProg.CharOutput(data3_string), packet_size); // for heavy debugging
						Cont.ChatIn(CHAT_USER_LEAVE, data3_string);
						return;
					case INVALID_NAME_BADCHAR:
						//printf("\nINVALID_NAME_BADCHAR - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						edit_stat = data4_int;

						data_cat = CalculateMailDataCategory(data3_int);
						vec_name = Cont.MailVecData(data_cat, false);
						Text.ClientMessage(INVALID_NAME_BADCHAR, pm_name);						
						Cont.MailClearVec(data_cat);
						Net.ClientPackets(CONTENT_EMPTY, data3_int);
						return;
					case INVALID_NAME_LENGTH:
						//printf("\nINVALID_NAME_LENGTH - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						edit_stat = data4_int;						

						data_cat = CalculateMailDataCategory(data3_int);
						vec_name = Cont.MailVecData(data_cat, false);
						Text.ClientMessage(INVALID_NAME_LENGTH, pm_name);
						Cont.MailClearVec(data_cat);
						Net.ClientPackets(CONTENT_EMPTY, data3_int);
						return;
					case CONTENT_GOTO_MENU:
						//printf("\nCONTENT_GOTO_MENU - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(CONTENT_GOTO_MENU, pm_name);
						content_stat = CONTENT_GOTO_MENU;
						menu_stat = CONTENT_MENU;						
						lock_input = false;
						return;
					case INVALID_CREDS:			// signals a server error upon entry
						//printf("\nINVALID_CREDS - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(INVALID_CREDS, pm_name);
						Net.ClientPackets(CONTENT_EMPTY, INVALID_CREDS);
						return;
					case ACCT_LOCKED:			// will not start DB and file logs upon entry and will boot back to login::entry()
						//printf("\nACCT_LOCKED - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging			
						Text.ClientMessage(ACCT_LOCKED, pm_name);
						Net.ClientPackets(CONTENT_EMPTY, ACCT_LOCKED);
						return;
					case VALID_CREDS:
						//printf("\nVALID_CREDS - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging						
						Text.NonPacketTextMessage(ACCOUNT_LOGIN_COUNT, data3_string);
						Text.NonPacketTextMessage(ACCOUNT_AVG_TIME, data4_string);						
						StartConNetTimers(); // also located in contentmenu() - one of these may not be needed
						Text.ContentMessage(VALID_CREDS, pm_name);
						Net.ClientPackets(CONTENT_EMPTY, VALID_CREDS);
						return;
					case DB_ERROR:				// signals a server error (DB related) upon entry
						//printf("\nDB_ERROR - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(DB_ERROR, pm_name);
						Net.ClientPackets(CONTENT_EMPTY, DB_ERROR);
						return;
					case CONTENT_INVALID_INPUT:
						// printf("\nCONTENT_INVALID_INPUT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(CONTENT_INVALID_INPUT, pm_name);
						menu_stat = CONTENT_MENU; // this may not be needed
						lock_input = false;
						return;
					case LOGOUT_CONTENT_SERVER: // this should be changed
						//printf("\nLOGOUT_CONTENT_SERVER - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(LOGOUT_CONTENT_SERVER, pm_name);
						Text.ClientMessage(TO_LOGIN);
						Text.ClientMessage(QUIT);
						DisableConNetTimers();
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						logged = false;
						return;
					case FORCE_LOGOUT:			// this boots the client
						// printf("\nFORCE_LOGOUT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(FORCE_LOGOUT);
						Text.ClientMessage(TO_LOGIN);
						DisableConNetTimers();
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						logged = false;
						return;
					case SERVER_DISCONNECT:
						// printf("\nSERVER_DISCONNECT - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging				
						Text.ClientMessage(SERVER_DISCONNECT);
						Text.ClientMessage(QUIT);
						DisableConNetTimers();
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						logged = false;
						return;
					case CHECK_CLIENT_CONNECTION:			// a signal from the server to check connectivity - dummy packet
						//printf("\nCHECK_CLIENT_CONNECTION - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging			
						return;
					case CLIENT_DC_INACTIVE:			// a signal from the server to signal disconnection
						//printf("\nCLIENT_DC_INACTIVE - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(CLIENT_DC_INACTIVE, pm_name);
						Text.ClientMessage(TO_LOGIN, pm_name);
						DisableConNetTimers();
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						logged = false;
						return;
					case TO_LOGIN:				// this boots the client
						//printf("\nTO_LOGIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ClientMessage(QUIT, pm_name);
						DisableConNetTimers();
						lock_input = false;
						ShutdownConnection(Net.ConnectSocket);
						connected = false;
						logged = false;
						return;
					case CHAT_INSTRUCTIONS:
						//printf("\nCHAT_INSTRUCTIONS - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(CHAT_INSTRUCTIONS, pm_name);
						Text.ContentMessage(CONTENT_INVALID_INPUT, pm_name);
						lock_input = false;
						content_stat = CHAT_INSTRUCTIONS;
						menu_stat = CONTENT_MENU;						
						return;
					case CHAT_BEGIN:
						//printf("\nCHAT_BEGIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(CHAT_BEGIN, pm_name);
						key_prmt = true;
						remove_prmt = true;
						lock_input = false;	
						prmt_to_remove = chat_user_prompt;
						content_stat = CHAT_BEGIN;
						menu_stat = CHAT_ROOM;						
						return;
					case CHAT_MESS_VER:
						//printf("\nCHAT_MESS_VER - case:(%i) packet_size:(%i)\n", data2_int, packet_size); // for heavy debugging
						CHAT_USER_PROMPT;						
						lock_input = false;
						return;
					case CHAT_NO_LISTENERS:
						//printf("\nCHAT_NO_LISTENERS - case:(%i) packet_size:(%i)\n", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(CHAT_NO_LISTENERS, pm_name);
						lock_input = false;
						return;
					case CHAT_NO_LIST_RECIPIENT:
						//printf("\nCHAT_NO_LIST_RECIPIENT - case:(%i) packet_size:(%i)\n", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(CHAT_NO_LIST_RECIPIENT, pm_name);
						lock_input = false;
						return;
					case MESSAGES_EXCEEDED: // this condition should be put in client to reduce any continual messages of this type and load on the server with a final check done at server in case
						//printf("\nMESSAGES_EXCEEDED - case:(%i) packet_size:(%i)\n", data2_int, packet_size); // for heavy debugging
						Text.ContentMessage(MESSAGES_EXCEEDED, pm_name);
						Text.NonPacketTextMessage(CONTENT_INVALID_INPUT2, pm_name);
						lock_input = false;
						return;
					case MAIL_GOTO_MENU:
						//printf("\nMAIL_GOTO_MENU - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_GOTO_MENU, pm_name);
						menu_stat = MAIL_MENU;
						content_stat = MAIL_GOTO_MENU;
						lock_input = false;
						return;
					case MAIL_BOX:
						//printf("\nMAIL_BOX - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_BOX, pm_name);
						menu_stat = MAIL_MENU;
						content_stat = MAIL_BOX;
						lock_input = false;
						return;
					case MAIL_INSTRUCTIONS:
						//printf("\nMAIL_INSTRUCTIONS - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_INSTRUCTIONS, pm_name);
						Text.ContentMessage(CONTENT_INVALID_INPUT, pm_name);
						content_stat = MAIL_INSTRUCTIONS;
						lock_input = false;
						return;
					case MAIL_INBOX:
						//printf("\nMAIL_INBOX - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_INBOX, pm_name, data3_string, data4_string);
						content_stat = MAIL_INBOX;
						lock_input = false;
						return;
					case MAIL_OUTBOX:
						//printf("\nMAIL_OUTBOX - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_OUTBOX, pm_name, data3_string);
						content_stat = MAIL_OUTBOX;
						lock_input = false;
						return;					
					case MAIL_DELETE_INBOX:
						//printf("\nMAIL_DELETE_INBOX - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_DELETE_INBOX, pm_name, data3_string, data4_string);
						content_stat = MAIL_DELETE_INBOX;
						menu_stat = MAIL_DELETE_IN;
						lock_input = false;
						return;
					case MAIL_DELETE_OUTBOX:
						//printf("\nMAIL_DELETE_OUTBOX - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_DELETE_OUTBOX, pm_name, data3_string);
						content_stat = MAIL_DELETE_OUTBOX;
						menu_stat = MAIL_DELETE_OUT;
						lock_input = false;
						return;
					case MAIL_TO_SEND_SUCCESS:
						//printf("\nMAIL_TO_SEND_SUCCESS - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_TO_SEND_SUCCESS, pm_name);
						Net.ClientPackets(CONTENT_EMPTY, MAIL_TO_SEND_SUCCESS);
						return;
					case MAIL_TO_SEND_FAIL:
						//printf("\nMAIL_TO_SEND_FAIL - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_TO_SEND_FAIL, pm_name);
						Text.MailMessage(MAIL_GOTO_MENU, pm_name);
						content_stat = MAIL_GOTO_MENU;
						lock_input = false;
						return;
					case CONTENT_INVALID_INPUT3:
						//printf("\nCONTENT_INVALID_INPUT3 - case(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.ContentMessage(CONTENT_INVALID_INPUT, pm_name);
						lock_input = false;
						return;
					case MAIL_DELETE_READ:
						//printf("\nMAIL_DELETE_READ - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						menu_stat = MAIL_DELETE_IN;
						if (MailConfirmListBookends(MAIL_DELETE_READ, data_size, data3_int) == true) {
							return;
						}
						Cont.MailIn(MAIL_DELETE_READ, message_string);
						return;
					case MAIL_DELETE_UNREAD:
						//printf("\nMAIL_DELETE_UNREAD - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						menu_stat = MAIL_DELETE_IN;
						if (MailConfirmListBookends(MAIL_DELETE_UNREAD, data_size, data3_int) == true) {
							return;
						}
						Cont.MailIn(MAIL_DELETE_UNREAD, message_string);
						return;
					case MAIL_DELETE_SENT:
						//printf("\nMAIL_DELETE_SENT - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						menu_stat = MAIL_DELETE_OUT;
						if (MailConfirmListBookends(MAIL_DELETE_SENT, data_size, data3_int) == true) {
							return;
						}
						Cont.MailIn(MAIL_DELETE_SENT, message_string);
						return;
					case MAIL_DELETE_ALL_OPTIONS:
						//printf("\nMAIL_DELETE_ALL_OPTIONS - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_DELETE_ALL_OPTIONS, pm_name);
						content_stat = MAIL_DELETE_ALL_OPTIONS;
						lock_input = false;
						return;
					case MAIL_DELETE_ALL_READ:
						//printf("\nMAIL_DELETE_ALL_READ - case:(%i) input:(%s) data_size:(%i)\n", data2_int, GProg.CharOutput(data3_string), data_size); // for heavy debugging
						Text.MailMessage(MAIL_DELETE_ALL_READ, pm_name, data3_string);
						Net.ClientPackets(CONTENT_EMPTY, MAIL_GOTO_MENU);
						return;
					case MAIL_DELETE_ALL_UNREAD:
						//printf("\nMAIL_DELETE_ALL_UNREAD - case:(%i) input:(%s) data_size:(%i)\n", data2_int, GProg.CharOutput(data3_string), data_size); // for heavy debugging
						Text.MailMessage(MAIL_DELETE_ALL_UNREAD, pm_name, data3_string);
						Net.ClientPackets(CONTENT_EMPTY, MAIL_GOTO_MENU);
						return;
					case MAIL_DELETE_ALL_INBOX:
						//printf("\nMAIL_DELETE_ALL_INBOX - case:(%i) input:(%s) data_size:(%i)\n", data2_int, GProg.CharOutput(data3_string), data_size); // for heavy debugging
						Text.MailMessage(MAIL_DELETE_ALL_INBOX, pm_name, data3_string, data4_string);
						Net.ClientPackets(CONTENT_EMPTY, MAIL_GOTO_MENU);						
						return;
					case MAIL_DELETE_ALL_OUTBOX:
						//printf("\nMAIL_DELETE_ALL_OUTBOX - case:(%i) input:(%s) data_size:(%i)\n", data2_int, GProg.CharOutput(data3_string), data_size); // for heavy debugging
						Text.MailMessage(MAIL_DELETE_ALL_OUTBOX, pm_name, data3_string);
						Net.ClientPackets(CONTENT_EMPTY, MAIL_GOTO_MENU);
						return;
					case MAIL_INBOX_READ:
						//printf("\nMAIL_INBOX_READ - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						menu_stat = MAIL_READ_IN;
						if (MailConfirmListBookends(MAIL_INBOX_READ, data_size, data3_int) == true) {
							return;
						}
						Cont.MailIn(MAIL_INBOX_READ, message_string);
						return;
					case MAIL_INBOX_UNREAD:
						//printf("\nMAIL_INBOX_UNREAD - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						menu_stat = MAIL_READ_IN;
						if (MailConfirmListBookends(MAIL_INBOX_UNREAD, data_size, data3_int) == true) {
							return;
						}
						Cont.MailIn(MAIL_INBOX_UNREAD, message_string);
						return;
					case MAIL_OUTBOX_SENT:
						//printf("\nMAIL_OUTBOX_SENT - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						menu_stat = MAIL_READ_OUT;
						if (MailConfirmListBookends(MAIL_OUTBOX_SENT, data_size, data3_int) == true) {
							return;
						}
						Cont.MailIn(MAIL_OUTBOX_SENT, message_string);
						return;
					case MAIL_DELETE_FAIL:
						//printf("\nMAIL_DELETE_FAIL - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						if (data_size > 4) {
							Text.MailMessage(MAIL_DELETE_ALL_UNREAD, pm_name, data3_string);
						}
						Text.MailMessage(MAIL_DELETE_FAIL, pm_name);
						Text.MailMessage(MAIL_GOTO_MENU, pm_name);
						menu_stat = MAIL_MENU;
						content_stat = MAIL_GOTO_MENU;
						lock_input = false;
						return;
					case MAIL_LIST_OOR:
						//printf("\nMAIL_DELETE_INDV_SUCCESS - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_LIST_OOR, pm_name);
						mail_list_counter = data4_int;
						Net.ClientPackets(CONTENT_2VAL, data3_int, "newlist", data4_string);
						return;
					case CHAT_MENU:
						//printf("\nMAIL_DELETE_INDV_SUCCESS - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.ContentMessage(CHAT_MENU, pm_name);
						menu_stat = CONTENT_MENU;
						content_stat = CHAT_MENU;						
						lock_input = false;
						return;
					case MAIL_DELETE_INDV_SUCCESS:
						//printf("\nMAIL_DELETE_INDV_SUCCESS - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_DELETE_INDV_SUCCESS, pm_name);
						mail_list_counter = data4_int;
						Net.ClientPackets(CONTENT_2VAL, data3_int, "newlist", data4_string);
						return;
					case MAIL_READ_RECV_MESSAGE: // vector cleared because we may need the inc data for a reply
						//printf("\nMAIL_READ_RECV_MESSAGE - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Cont.MailClearVec(0);
						Cont.MailIn(MAIL_READ_RECV_MESSAGE, message_string);				
						return;					
					case MAIL_TO_SEND_RECIPIENT:
						//printf("\nMAIL_TO_SEND_RECIPIENT - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.NonPacketTextMessage(MAIL_SENDING, pm_name);
						Text.MailMessage(MAIL_TO_SEND_RECIPIENT, pm_name);
						edit_stat = 0;
						Cont.MailClearVec(0);						
						menu_stat = MAIL_SEND;
						content_stat = MAIL_TO_SEND_RECIPIENT;
						lock_input = false;
						return;
					case MAIL_TO_SEND_REPLY: // don't clear the vector here because we need the name/body
						//printf("\nMAIL_TO_SEND_REPLY - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging					
						vec_body = Cont.MailVecData(3, false);
						Text.NonPacketTextMessage(MAIL_REPLYING, pm_name);
						Text.MailMessage(MAIL_TO_SEND_REPLY, pm_name, vec_body);
						edit_stat = 0;						
						menu_stat = MAIL_REPLY;
						content_stat = MAIL_TO_SEND_REPLY;
						lock_input = false;
						return;
					case MAIL_TO_SEND_SUBJECT:
						//printf("\nMAIL_TO_SEND_SUBJECT - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.NonPacketTextMessage(MAIL_SENDING, pm_name);
						Text.MailMessage(MAIL_TO_SEND_SUBJECT, pm_name);
						content_stat = MAIL_TO_SEND_SUBJECT;
						lock_input = false;
						return;
					case MAIL_TO_SEND_BODY:
						//printf("\nMAIL_TO_SEND_BODY - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.NonPacketTextMessage(MAIL_SENDING, pm_name);
						Text.MailMessage(MAIL_TO_SEND_BODY, pm_name);
						content_stat = MAIL_TO_SEND_BODY;
						lock_input = false;
						return;
					case MAIL_TO_SEND_REVIEW:
						//printf("\nMAIL_TO_SEND_REVIEW - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_TO_SEND_REVIEW, pm_name);
						content_stat = MAIL_TO_SEND_REVIEW;
						lock_input = false;
						return;
					case MAIL_REVIEW_MESSAGE:
						//printf("\nMAIL_REVIEW_MESSAGE - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging						
						Cont.MailOutput(0, false, false, false, true);
						edit_stat = 0;
						content_stat = MAIL_REVIEW_MESSAGE;
						lock_input = false;
						return;
					case MAIL_REVIEW_REPLY:
						//printf("\nMAIL_REVIEW_REPLY - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging						
						Cont.MailOutput(0, false, false, true, true);
						edit_stat = 0;
						content_stat = MAIL_REVIEW_REPLY;
						lock_input = false;
						return;
					case MAIL_EDIT_RECIPIENT:
						//printf("\nMAIL_EDIT_RECIPIENT - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging						
						vec_name = Cont.MailVecData(1, false);
						Text.NonPacketTextMessage(MAIL_EDITING, pm_name);
						Text.MailMessage(MAIL_EDIT_RECIPIENT, pm_name, vec_name);
						content_stat = MAIL_EDIT_RECIPIENT;
						lock_input = false;
						return;
					case MAIL_EDIT_SUBJECT:
						//printf("\nMAIL_EDIT_SUBJECT - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging						
						vec_subject = Cont.MailVecData(2, false);
						GProg.RemoveCharsFromString(vec_subject, "\\");
						Text.NonPacketTextMessage(MAIL_EDITING, pm_name);
						Text.MailMessage(MAIL_EDIT_SUBJECT, pm_name, vec_subject);
						content_stat = MAIL_EDIT_SUBJECT;
						lock_input = false;
						return;
					case MAIL_EDIT_BODY:
						//printf("\nMAIL_EDIT_BODY - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging						
						vec_body = Cont.MailVecData(3, false);
						GProg.RemoveCharsFromString(vec_body, "\\");
						Text.NonPacketTextMessage(MAIL_EDITING, pm_name);
						Text.MailMessage(MAIL_EDIT_BODY, pm_name, vec_body);
						content_stat = MAIL_EDIT_BODY;
						lock_input = false;
						return;
					case MAIL_EDIT_REPLY:
						//printf("\nMAIL_EDIT_REPLY - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging					
						vec_body = Cont.MailVecData(3, false);
						vec_reply = Cont.MailVecData(4, false);
						GProg.RemoveCharsFromString(vec_body, "\\");
						Text.NonPacketTextMessage(MAIL_EDITING, pm_name);
						Text.MailMessage(MAIL_EDIT_REPLY, pm_name, vec_body, vec_reply);
						content_stat = MAIL_EDIT_REPLY;
						lock_input = false;
						return;
					case INVALID_NAME:
						//printf("\nINVALID_NAME - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						edit_stat = data4_int;

						data_cat = CalculateMailDataCategory(data3_int);
						vec_name = Cont.MailVecData(data_cat, false);
						Text.ClientMessage(INVALID_NAME, pm_name);						
						Cont.MailClearVec(data_cat);
						Net.ClientPackets(CONTENT_EMPTY, data3_int);
						return;
					case INVALID_INPUT_BADCHAR:
						//printf("\nINVALID_INPUT_BADCHAR - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging						
						edit_stat = data4_int; // don't think this is needed
						data_cat = CalculateMailDataCategory(data3_int);
						Cont.MailClearVec(data_cat);						
						Text.ClientMessage(INVALID_INPUT_BADCHAR, pm_name);
						Net.ClientPackets(CONTENT_EMPTY, data3_int);
						return;
					case INVALID_INPUT_LENGTH:
						//printf("\nINVALID_INPUT_LENGTH - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging						
						data_cat = CalculateMailDataCategory(data3_int);
						Cont.MailClearVec(data_cat);
						edit_stat = data4_int;						
						Text.ClientMessage(INVALID_INPUT_LENGTH, pm_name);
						Net.ClientPackets(CONTENT_EMPTY, data3_int);
						return;
					case MAIL_READ_FAIL:
						//printf("\nMAIL_READ_FAIL - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_READ_FAIL, pm_name);
						Text.MailMessage(MAIL_GOTO_MENU, pm_name);
						content_stat = MAIL_GOTO_MENU;
						lock_input = false;
						return;
					case MAIL_READ_SENT_MESSAGE:
						//printf("\nMAIL_READ_SENT_MESSAGE - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Cont.MailIn(MAIL_READ_SENT_MESSAGE, message_string);						
						return;
					case MAIL_NO_PREVIOUS_RECORDS:
						//printf("\nMAIL_READ_FAIL - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_NO_PREVIOUS_RECORDS, pm_name);
						mail_list_counter = 0;
						s_response_stat = std::to_string(0);
						Net.ClientPackets(CONTENT_2VAL, data3_int, "newlist", s_response_stat);
						return;
					case MAIL_NO_NEXT_RECORDS:
						//printf("\nMAIL_READ_FAIL - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_NO_NEXT_RECORDS, pm_name);
						mail_list_counter = data4_int;
						Net.ClientPackets(CONTENT_2VAL, data3_int, "newlist", data4_string);						
						return;
					case MAIL_MAILBOX_EMPTY:
						//printf("\nMAIL_MAILBOX_EMPTY - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging
						Text.MailMessage(MAIL_MAILBOX_EMPTY, pm_name);
						mail_list_counter = 0;
						Net.ClientPackets(CONTENT_EMPTY, MAIL_BOX);
						return;
					case MAIL_REVIEW_EDIT:
						//printf("\nMAIL_REVIEW_EDIT - case:(%i) data_size:(%i)\n", data2_int, data_size); // for heavy debugging						
						Cont.MailOutput(data3_int, false, false, false, true);
						edit_stat = data3_int;
						content_stat = MAIL_REVIEW_EDIT;
						lock_input = false;
						return;
					case MAIL_REVIEW_REPLY_EDIT:
						//printf("\nMAIL_REVIEW_REPLY_EDIT - case:(%i) data_size:(%i) edit_stat:(%i)\n", data2_int, data_size, data3_int); // for heavy debugging						
						Cont.MailOutput(data3_int, false, false, true, true); // data3_int = 8
						edit_stat = data3_int;
						content_stat = MAIL_REVIEW_REPLY_EDIT;
						lock_input = false;
						return;
					case CONTENT_1ST_LOGIN:			
						//printf("\nCONTENT_1ST_LOGIN - case(%i) packet_size:(%i)", data2_int, packet_size); // for heavy debugging						
						Text.NonPacketTextMessage(MAIL_LIST_HEAD, pm_name);
						Text.ContentMessage(CONTENT_1ST_LOGIN, pm_name);
						content_stat = CONTENT_1ST_LOGIN;
						menu_stat = CONTENT_MENU;
						lock_input = false;						
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

int ContentNet::CalculateMailDataCategory(int response_status_value)
{
	int data_category = -1;

	switch (response_status_value) {	
	case MAIL_TO_SEND_RECIPIENT:
		data_category = 1;
		break;
	case MAIL_TO_SEND_SUBJECT:
		data_category = 2;
		break;
	case MAIL_TO_SEND_BODY:
		data_category = 3;
		break;
	case MAIL_TO_SEND_REPLY:
		data_category = 4;
		break;
	case MAIL_EDIT_RECIPIENT:
		data_category = 5;
		break;
	case MAIL_EDIT_SUBJECT:
		data_category = 6;
		break;
	case MAIL_EDIT_BODY:
		data_category = 7;
		break;
	case MAIL_EDIT_REPLY:
		data_category = 8;
		break;
	}
	return data_category;
}

bool ContentNet::MailConfirmListBookends(int response_status, int size_data, int data_pos1)
{
	if (size_data > 16) {
		return false;
	}
	bool confirmed = false;

	if (data_pos1 == MAIL_LIST_HEAD) {
		Text.NonPacketTextMessage(MAIL_LIST_HEAD, pm_name);
		confirmed = true;
	}
	else if (data_pos1 == MORE_MAIL) {
		Text.NonPacketTextMessage(MORE_MAIL, pm_name);
		content_stat = response_status;
		lock_input = false;
		confirmed = true;
	}
	else if (data_pos1 == MAIL_NO_NEXT_RECORDS) {
		Text.MailMessage(MAIL_NO_NEXT_RECORDS, pm_name);
		confirmed = true;
	}
	else if (data_pos1 == MAIL_NO_PREVIOUS_RECORDS) {
		Text.MailMessage(MAIL_NO_PREVIOUS_RECORDS, pm_name);
		confirmed = true;
	}	
	return confirmed;
}

void ContentNet::DisableConNetTimers()
{
	ChatProbationTimer.DisableTimer();
}

void ContentNet::StartConNetTimers()
{
	ChatProbationTimer.StartTimer(CHAT_PROBATION);
}

void ContentNet::ProcessConNetTimers()
{
	if (ChatProbationTimer.CheckTimer(CHAT_PROBATION) == true) {
		Net.ClientPackets(CONTENT_EMPTY, CHAT_PROBATION_EXPIRE);
	}
}
