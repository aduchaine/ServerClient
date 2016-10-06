/*
4/11/16

all_packets.h
*/

// this file is for reference only

// --- >>> last update - 4/26/16

// packets where data is sent will be labelled 127 and less, for now
// packets with only an identifier should be labelled higher than 127
enum AllPacketTypes {

	NONE = 0,

	CHECK_NAME_OLD = 1,
	CHECK_NAME_NEW_VALIDITY = 2,
	CHECK_NAME_PW_CHG = 3,
	CHECK_PASSWORD_OLD = 4,
	CHECK_PASSWORD_NEW_VALIDITY = 5,						// this will return from client side if second PW matches the first
	CHECK_PASSWORD_PW_CHG_LOGIN = 6,
	CHECK_PASSWORD_PW_CHG_RECORD = 7,						// PW_CHG_RECORD will return from client side if second PW matches the first
	ADMIN_LOGIN = 8,
	ADMIN_PW_LOGIN = 9,
	DUAL_PROCEED_OLD = 10,									// does different things in client/server but, typically signals logout of one and login of most recent client entity
	DUAL_PROCEED_PW_CHG = 11,		                		// does different things in client/server but, typically signals logout of one and login of most recent client entity
	DUAL_PROCEED_ADMIN = 12,		                		// does different things in client/server but, typically signals logout of one and login of most recent client entity
	DUAL_PROCEED_ADMIN2 = 13,		                		// does different things in client/server but, typically signals logout of one and login of most recent client entity
	DUAL_LOCK_OLD = 14,				                		// does different things between all entities but, the result is logging out user locking account
	DUAL_LOCK_PW_CHG = 15,			                		// does different things between all entities but, the result is logging out user locking account
	DUAL_LOCK_ADMIN = 16,			                		// does different things between all entities but, the result is logging out admin and locking account
	DUAL_LOCK_ADMIN2 = 17,			                		// does different things between all entities but, the result is logging out user and locking account
	LOGIN_COMPLETE_OLD = 18,
	LOGIN_COMPLETE_NEW = 19,
	CONTINUE = 20,
	SERVERTALK3 = 21,										// login server user validation to content server with acctID and code
	BAD_LOG = 22,											// data packet sent from content server of logged-in accounts used for the purpose of locking "bad log" accounts
	CONNECT = 23,											// from client to content server with acctID and code
	VALIDATE_CREDS = 24,	                        		// this is a second validation of sorts
	DUAL_LOGIN_RESPONSE = 25,
	CHAT_IN_SAY = 26,										// content server chat message format - client recv - in is swapped for out when verifing to sender - the sender portions can likely be done on client
	CHAT_IN_TELL = 27,										// content server chat message format - client recv - in is swapped for out when verifing to sender
	CHAT_OUT_SAY = 28,										// content server chat message format - client send - out is swapped for in when sending to recipients 
	CHAT_OUT_TELL = 29,										// content server chat message format - client send - out is swapped for in when sending to recipients
	CHAT_USER_JOIN = 30,									// content server chat user join chatroom - notify client(s)
	CHAT_USER_LEAVE = 31,									// content server chat user leave chatroom - notify client(s)	

	TEST_PACKETS = 127,

	CHECK_NAME_NEW_APPROVE = 129,
	INVALID_NAME_OLD = 130,
	INVALID_NAME_BADCHAR = 131,								// general usage, beyond login server	
	INVALID_NAME_PW_CHG = 132,
	INVALID_NAME_NEW_CONFLICT = 133,
	INVALID_NAME_LENGTH = 134,								// general usage, beyond login server
	CHECK_PASSWORD_PW_CHG_VALIDITY = 135,
	INVALID_PASSWORD_OLD = 136,
	INVALID_PASSWORD_NEW_BADCHAR = 137,
	INVALID_PASSWORD_PW_CHG_LOGIN = 138,
	INVALID_PASSWORD_PW_CHG_BADCHAR = 139,
	INVALID_PASSWORD_NEW_LENGTH = 140,
	INVALID_PASSWORD_PW_CHG_LENGTH = 141,
	INVALID_PASSWORD_NEW_NOMATCH = 142,						// NEW_NOMATCH will return from client side if second PW does not match the first
	INVALID_PASSWORD_PW_CHG_NOMATCH = 143,					// PW_CHG_NOMATCH will return from client side if second PW does not match the first
	INVALID_PASSWORD_PW_LOGIN_NOMATCH = 144,				// PW_LOGIN_NOMATCH will return from client side if second PW does not match the first
	CHANGE_TO_LOGINMENU = 145,
	CHANGE_TO_ENTRY = 146,									// default for ERROR, more ops needed for logging
	CHANGE_TO_ADMIN = 147,									// admin login if not logged in as user
	INVALID_INPUT = 148,									// invalid input from LoginMenu()
	INVALID_INPUT2 = 149,									// from GoOn() in client
	ACCOUNT_LOCK_BEGIN = 150,
	PASSWORD_LOCKED = 151,						// when a new op is made, put this down below next to the other "locked"
	ADMIN_INVALID_NAME = 152,
	ADMIN_INVALID_PW = 153,
	ADMIN_LOG_COMPLETE = 154,
	TO_ADMINMENU = 155,										// used for "back" in directadmin()
	ADMIN_INVALID_ENTRY = 156,
	CONTENT_GOTO_MENU = 157,								// initial content menu
	PROCEED = 158,
	INVALID_CREDS = 159,									// there is more than one possibility to get this return value, therefore, we can't call it DB_ERROR
	ACCT_LOCKED = 160,										// generic account packet for content server
	VALID_CREDS = 161,
	DB_ERROR = 162,
	CONTENT_INVALID_INPUT = 163,
	LOGOUT_CONTENT_SERVER = 164,
	SERVERTALK1 = 165,										// initial connect ping
	SERVERTALK2 = 166,										// shutdown directive
	DUAL_LOG_OLD = 167,										// signals or processes this login_status through responses
	DUAL_LOG_PW_CHG = 168,									// signals or processes this login_status through responses
	DUAL_LOG_ADMIN = 169,									// signals or processes this login_status through responses
	INVALID_INPUT3 = 170,									// unrecognized input for dual login prompt, old
	INVALID_INPUT4 = 171,									// unrecognized input for dual login prompt, pw change
	INVALID_INPUT5 = 172,									// unrecognized input for dual login prompt, admin
	ADMIN_USER_ACCT_CONFLICT = 173,							// dual login, account association, admin choose options
	USER_ADMIN_ACCT_CONFLICT = 174,							// dual login, account association, send user to entry menu
	FORCE_LOGOUT = 175,										// used mostly in dual login atm but has many other useful possibilities
	SERVER_DISCONNECT = 176,								// sent to all connected clients when a controlled server shutdown occurs
	CHECK_CLIENT_CONNECTION = 177,							// dummy packet to signal client connectivity
	CLIENT_DC_INACTIVE = 178,								// sent to an inactive client signalling disconnection
	LOGOUT_QUIT = 179,										// quit after logging in, only difference is the message on the client
	LOG_LOCKED = 180,										// improper user logout likely through a bug or abrupt server closure
	SECURITY_LOCKED = 181,									// possible security breach account lockout
	INVALID_NAME_NEW_NOMATCH = 182,							// NEW_NOMATCH will return from client side if second name does not match the first
	CHECK_SERVER_CONNECTION = 183							// server to server connection check, initiated by the accepting server
	LOGIN_SUCCESS_OLD = 184,
	LOGIN_SUCCESS_NEW = 185,
	LOGIN_SUCCESS_PW_CHG = 186,
	ADMIN_LOG_SUCCESS = 187,
	ADMIN_LOGOUT = 188,
	QUIT = 189,												// quit is handled differently depending on log status, if logged, more data is sent so put here
	LOGOUT_TO_LOGIN = 190,
	SHUTDOWN = 191,											// opcode from admin to shutdown all servers
	TO_LOGIN = 192,											
	CHAT_INSTRUCTIONS = 193,								// content: instructions on how to use the chat feature
	CHAT_BEGIN = 194,										// wip - this should "enter" a chatroom on content
	CHAT_MESS_RECV = 195,									// to allow client to continue with chat after sending a message
	CHAT_NO_LISTENERS = 196,								// not a vaild chat reipient, noone online
	CHAT_NO_LIST_RECIPIENT = 197,							// not a vaild chat reipient, target not online
	CHAT_PROBATION_EXPIRE = 198,							// packet from client signalling to widen number of allowable chat messages in a given timeframe
	MESSAGES_EXCEEDED = 199,								// when number of client messages are higher than the allowable amount in a given timeframe
	
	MAIL_GOTO_MENU = 200,									// acessing mail options, sends unread mail count to client
	MAIL_BOX = 201,											// text/packet done
	MAIL_INSTRUCTIONS = 202,								// text/packet done
	MAIL_INBOX = 203,										// text/packet done
	MAIL_OUTBOX = 204,										// text/packet done
	MAIL_DELETE_INBOX = 205,								// text/packet done
	MAIL_DELETE_OUTBOX = 206,								// text/packet done					// 
	MAIL_TO_SEND_SUCCESS = 207,								// client verification packets
	MAIL_TO_SEND_FAIL = 208,								// client verification packets
	CONTENT_INVALID_INPUT3 = 209,							// text/packet done
	MAIL_TO_SEND = 210,										// initial send mail packet from client signalling a request for send mail format
	MAIL_DELETE_READ = 211,									// delete individual mail messages of this category
	MAIL_DELETE_UNREAD = 212,								// delete individual mail messages of this category
	MAIL_DELETE_SENT = 213,									// delete individual mail messages of this category
	MAIL_DELETE_ALL_OPTIONS = 214,							// shows inbox delete all options for inbox
	MAIL_DELETE_ALL_READ = 215,								// delete all mail messages of this category
	MAIL_DELETE_ALL_UNREAD = 216,							// delete all mail messages of this category
	MAIL_DELETE_ALL_INBOX = 217,							// delete all mail messages of this category
	MAIL_DELETE_ALL_OUTBOX = 218,							// delete all mail messages of this category
	MAIL_INBOX_READ = 219,									
	MAIL_INBOX_UNREAD = 220,
	MAIL_OUTBOX_SENT = 221,									
	MAIL_DELETE_FAIL = 222,
	MAIL_LIST_OOR = 223,									// out of range list selection
	CHAT_MENU = 224,										// chat menu
	MAIL_DELETE_INDV_SUCCESS = 225,							// delete success message + data to send user back to revised list + position
	MAIL_READ_RECV_MESSAGE = 226,							// the actual mail message, for recv mail text format
	MAIL_TO_SEND_REPLY = 227,								// reply choice from MAIL_READ_RECV_MESSAGE
	MAIL_TO_SEND_RECIPIENT = 228,							// client input request/send
	MAIL_TO_SEND_SUBJECT = 229,								// client input request/send
	MAIL_TO_SEND_BODY = 230,								// client input request/send
	MAIL_TO_SEND_REVIEW = 231,								// client input request/send - client menu to make changes to the three required fields or confirm sending
	MAIL_REVIEW_MESSAGE = 232,								// client input request
	MAIL_EDIT_RECIPIENT = 233,								// client input request
	MAIL_EDIT_SUBJECT = 234,								// client input request
	MAIL_EDIT_BODY = 235,									// client input request
	INVALID_NAME = 236,										// the message is general but the packet is not - invalid name packet
	INVALID_INPUT_BADCHAR = 237,							// the message is general but the packet is not - invalid input with bad chars
	INVALID_INPUT_LENGTH = 238,								// the message is general but the packet is not - invalid input with bad length
	MAIL_READ_FAIL = 239,									// used for DB/data unreadable  errors 
	MAIL_READ_SENT_MESSAGE = 240,							// the actual mail message, for sent mail text format
	CHECK_CLIENT_VERSION = 241,								// intial ping to server, returns to client accept or version out of date
	INVALID_CLIENT_VERSION = 242							// server response, will drop connection
	MAIL_NO_NEXT_RECORDS = 243,								// no more records in this direction - using "next" will not return any records
	MAIL_NO_PREVIOUS_RECORDS = 244,							// no more records in this direction - using "previous" will not return any records
	MAIL_MAILBOX_EMPTY = 245,								// server response if 0 messages in box, returns user to mailbox
	MAIL_REVIEW_EDIT = 246,									// client accepts edit change, also sends the mail
	CONTENT_1ST_LOGIN = 247									// first login, welcome message to be expanded to other things
	MAIL_REVIEW_REPLY = 248,								// client accepts reply, also sends the mail
	MAIL_EDIT_REPLY = 249									// edit body of reply
	MAIL_REVIEW_REPLY_EDIT = 250,
	MAIL_TO_REPLY = 251

};

enum NonPacketActions {

	CHECK_NAME_NEW_VALIDITY2 = 501,							// client 2nd input handled internally
	CHECK_PASSWORD_NEW_VALIDITY2 = 502,						// client 2nd input handled internally
	CHECK_PASSWORD_PW_CHG_LOGIN2 = 503,						// client 2nd input handled internally
	CHECK_PASSWORD_PW_CHG_VALIDITY2 = 504,					// client 2nd input handled internally
	CONTENT_INVALID_INPUT2 = 505,							// invalid client chat command input 
	CHAT_SELF_TALK = 506,									// not a vaild chat reipient, tell to self
	CHAT_INCHAT = 507,										// client issuing command "/chat" while already in chat
	CHAT_NO_MESSAGE = 508,									// message size 0 - no message
	CHAT_COMMAND = 509,										// shows client command input
	CHAT_TOO_LONG = 510,									// response if client chat message exceeds max length									
	MORE_MAIL = 511,
	CONFIRM_DELETE_ALL_OUTBOX = 512,
	CONFIRM_DELETE_ALL_READ = 513,
	CONFIRM_DELETE_ALL_UNREAD = 514,
	CONFIRM_DELETE_ALL_INBOX = 515,
	MAIL_SENDING = 516,										// mail sending header
	MAIL_EDITING = 517,										// mail editing header
	MAIL_LIST_HEAD = 518,									// list header signal to client - text formatting action
	MAIL_ACCEPT_EDIT = 519,									// singals overwriting of vectors during mail editing + message
	ACCOUNT_LOGIN_COUNT = 520,								// shows the user # of logins
	ACCOUNT_AVG_TIME = 521,									// shows the user average login time
	MAIL_REPLYING = 522										// mail reply header

};

// below updated 4/26/16

// login server
enum LoginPacketTypes {

	NONE = 0,

	CHECK_NAME_OLD = 1,
	CHECK_NAME_NEW_VALIDITY = 2,
	CHECK_NAME_PW_CHG = 3,
	CHECK_PASSWORD_OLD = 4,
	CHECK_PASSWORD_NEW_VALIDITY = 5,
	CHECK_PASSWORD_PW_CHG_LOGIN = 6,
	CHECK_PASSWORD_PW_CHG_RECORD = 7,
	ADMIN_LOGIN = 8,
	ADMIN_PW_LOGIN = 9,
	DUAL_PROCEED_OLD = 10,
	DUAL_PROCEED_PW_CHG = 11,
	DUAL_PROCEED_ADMIN = 12,
	DUAL_PROCEED_ADMIN2 = 13,
	DUAL_LOCK_OLD = 14,
	DUAL_LOCK_PW_CHG = 15,
	DUAL_LOCK_ADMIN = 16,
	DUAL_LOCK_ADMIN2 = 17,
	LOGIN_COMPLETE_OLD = 18,
	LOGIN_COMPLETE_NEW = 19,
	CONTINUE = 20,
	SERVERTALK3 = 21,
	BAD_LOG = 22,
	TEST_PACKETS = 127,

	CHECK_NAME_NEW_APPROVE = 129,
	INVALID_NAME_OLD = 130,
	INVALID_NAME_NEW_BADCHAR = 131,
	INVALID_NAME_PW_CHG = 132,
	INVALID_NAME_NEW_CONFLICT = 133,
	INVALID_NAME_NEW_LENGTH = 134,
	CHECK_PASSWORD_PW_CHG_VALIDITY = 135,
	INVALID_PASSWORD_OLD = 136,
	INVALID_PASSWORD_NEW_BADCHAR = 137,
	INVALID_PASSWORD_PW_CHG_LOGIN = 138,
	INVALID_PASSWORD_PW_CHG_BADCHAR = 139,
	INVALID_PASSWORD_NEW_LENGTH = 140,
	INVALID_PASSWORD_PW_CHG_LENGTH = 141,
	INVALID_PASSWORD_NEW_NOMATCH = 142,
	INVALID_PASSWORD_PW_CHG_NOMATCH = 143,
	INVALID_PASSWORD_PW_LOGIN_NOMATCH = 144,
	CHANGE_TO_LOGINMENU = 145,
	CHANGE_TO_ENTRY = 146,
	CHANGE_TO_ADMIN = 147,
	INVALID_INPUT = 148,
	INVALID_INPUT2 = 149,
	ACCOUNT_LOCK_BEGIN = 150,
	PASSWORD_LOCKED = 151,
	ADMIN_INVALID_NAME = 152,
	ADMIN_INVALID_PW = 153,
	ADMIN_LOG_COMPLETE = 154,
	ADMIN_MENU = 155,
	ADMIN_INVALID_ENTRY = 156,
	PROCEED = 158,
	DB_ERROR = 162,
	SERVERTALK1 = 165,
	SERVERTALK2 = 166,
	DUAL_LOG_OLD = 167,
	DUAL_LOG_PW_CHG = 168,
	DUAL_LOG_ADMIN = 169,
	INVALID_INPUT3 = 170,
	INVALID_INPUT4 = 171,
	INVALID_INPUT5 = 172,
	ADMIN_USER_ACCT_CONFLICT = 173,
	USER_ADMIN_ACCT_CONFLICT = 174,
	FORCE_LOGOUT = 175,
	SERVER_DISCONNECT = 176,
	CHECK_CLIENT_CONNECTION = 177,
	CLIENT_DC_INACTIVE = 178,
	LOGOUT_QUIT = 179,
	LOG_LOCKED = 180,
	SECURITY_LOCKED = 181,
	INVALID_NAME_NEW_NOMATCH = 182,
	CHECK_SERVER_CONNECTION = 183,
	LOGIN_SUCCESS_OLD = 184,
	LOGIN_SUCCESS_NEW = 185,
	LOGIN_SUCCESS_PW_CHG = 186,
	ADMIN_LOG_SUCCESS = 187,
	ADMIN_LOGOUT = 188,
	QUIT = 189,
	LOGOUT_TO_LOGIN = 190,
	SHUTDOWN = 191									

	// old
/*	CHECK_NAME_OLD = 1,
	CHECK_NAME_NEW_VALIDITY = 2,
	CHECK_NAME_PW_CHG = 3,
	CHECK_NAME_NEW_APPROVE = 4,
	INVALID_NAME_OLD = 11,
	INVALID_NAME_NEW_BADCHAR = 12,
	INVALID_NAME_PW_CHG = 13,
	INVALID_NAME_NEW_CONFLICT = 14,
	INVALID_NAME_NEW_LENGTH = 15,
	CHECK_PASSWORD_OLD = 21,
	CHECK_PASSWORD_NEW_VALIDITY = 22,
	CHECK_PASSWORD_PW_CHG_LOGIN = 23,
	CHECK_PASSWORD_PW_CHG_VALIDITY = 24,
	CHECK_PASSWORD_NEW_RECORD = 25,				
	CHECK_PASSWORD_PW_CHG_RECORD = 27,			
	INVALID_PASSWORD_OLD = 31,
	INVALID_PASSWORD_NEW_BADCHAR = 32,
	INVALID_PASSWORD_PW_CHG_LOGIN = 33,
	INVALID_PASSWORD_PW_CHG_BADCHAR = 34,
	INVALID_PASSWORD_NEW_LENGTH = 35,
	INVALID_PASSWORD_PW_CHG_LENGTH = 36,
	INVALID_PASSWORD_NEW_NOMATCH = 37,			
	INVALID_PASSWORD_PW_CHG_NOMATCH = 38,		
	INVALID_PASSWORD_PW_LOGIN_NOMATCH = 39,		
	LOGIN_SUCCESS_OLD = 41,
	LOGIN_SUCCESS_NEW = 42,
	LOGIN_SUCCESS_PW_CHG = 43,
	LOGIN_COMPLETE_OLD = 44,
	LOGIN_COMPLETE_NEW = 45,
	CHANGE_TO_LOGINMENU = 51,
	CHANGE_LOGIN_TO_OLD = 52,
	CHANGE_LOGIN_TO_NEW = 53,
	CHANGE_LOGIN_TO_PW_CHG = 54,
	CHANGE_TO_ENTRY = 55,
	CHANGE_TO_ADMIN = 56,
	CHANGE_TO_ADMIN_LOG = 57,
	INVALID_INPUT = 61,
	QUIT = 63,
	ACCOUNT_LOCK_BEGIN = 64,
	ACCOUNT_LOCKED = 65,
	PROCEED = 66,
	LOGOUT = 67,
	CONTINUE = 68,
	INVALID_INPUT2 = 69,						
	ADMIN_LOGIN = 71,
	ADMIN_INVALID_NAME = 72,
	ADMIN_PW_LOGIN = 73,
	ADMIN_INVALID_PW = 74,
	ADMIN_LOG_SUCCESS = 75,
	ADMIN_LOG_COMPLETE = 76,
	ADMIN_MENU = 77,
	ADMIN_LOGOUT = 78,
	ADMIN_INVALID_ENTRY = 79,
	SHUTDOWN = 81,								
	SERVERTALK1 = 201,							
	SERVERTALK2 = 202,							
	SERVERTALK3 = 203							
	*/
};

// content server
enum ContentPacketTypes {

	NONE = 0,

	DUAL_PROCEED_OLD = 10,
	DUAL_PROCEED_PW_CHG = 11,
	DUAL_PROCEED_ADMIN = 12,
	DUAL_PROCEED_ADMIN2 = 13,
	DUAL_LOCK_OLD = 14,
	DUAL_LOCK_PW_CHG = 15,
	DUAL_LOCK_ADMIN = 16,
	DUAL_LOCK_ADMIN2 = 17,
	SERVERTALK3 = 21,
	BAD_LOG = 22,
	CONNECT = 23,
	VALIDATE_CREDS = 24,
	TEST_PACKETS = 127,

	START_CONTENT = 157,
	INVALID_CREDS = 159,
	ACCT_LOCKED = 160,
	VALID_CREDS = 161,
	DB_ERROR = 162,
	SAY_WHAT = 163,
	LOGOUT_CONTENT_SERVER = 164,
	SERVERTALK1 = 165,
	SERVERTALK2 = 166,
	FORCE_LOGOUT = 175,
	SERVER_DISCONNECT = 176,
	CHECK_CLIENT_CONNECTION = 177,
	CLIENT_DC_INACTIVE = 178,
	LOGOUT_QUIT = 179,										// not used here yet
	CHECK_SERVER_CONNECTION = 183,
	SHUTDOWN = 191,
	TO_LOGIN = 192

	// old
/*	SHUTDOWN = 81,						
	VALIDATE_CREDS = 101,
	CONNECT = 102,						
	INVALID_CREDS = 103,				
	ACCT_LOCKED = 104,
	VALID_CREDS = 105,
	DB_ERROR = 106,
	START_CONTENT = 111,
	SAY_WHAT = 112,
	LOGOUT = 113,
	TO_LOGIN = 114,
	SERVERTALK1 = 201,					
	SERVERTALK2 = 202,					
	SERVERTALK3 = 203					
	*/
};

// client
enum ClientPacketTypes {

	NONE = 0,

	TEST_PACKETS = 127,

	INVALID_INPUT = 148,
	ACCT_LOCKED = 160,
	DB_ERROR = 162,
	FORCE_LOGOUT = 175,
	SERVER_DISCONNECT = 176,
	CHECK_CLIENT_CONNECTION = 177,
	CLIENT_DC_INACTIVE = 178,
	LOGOUT_QUIT = 179,
	QUIT = 189,
	LOGOUT_TO_LOGIN = 190,
	SHUTDOWN = 191,
	TO_LOGIN = 192

};

// client
enum LoginPacketTypes {

	CHECK_NAME_OLD = 1,
	CHECK_NAME_NEW_VALIDITY = 2,
	CHECK_NAME_PW_CHG = 3,
	CHECK_PASSWORD_OLD = 4,
	CHECK_PASSWORD_NEW_VALIDITY = 5,
	CHECK_PASSWORD_PW_CHG_LOGIN = 6,
	CHECK_PASSWORD_PW_CHG_RECORD = 7,
	ADMIN_LOGIN = 8,
	ADMIN_PW_LOGIN = 9,
	LOGIN_COMPLETE_OLD = 18,
	LOGIN_COMPLETE_NEW = 19,
	CONTINUE = 20,
	DUAL_PROCEED_OLD = 10,
	DUAL_PROCEED_PW_CHG = 11,
	DUAL_PROCEED_ADMIN = 12,
	DUAL_PROCEED_ADMIN2 = 13,
	DUAL_LOCK_OLD = 14,
	DUAL_LOCK_PW_CHG = 15,
	DUAL_LOCK_ADMIN = 16,
	DUAL_LOCK_ADMIN2 = 17,

	CHECK_NAME_NEW_APPROVE = 129,
	INVALID_NAME_OLD = 130,
	INVALID_NAME_NEW_BADCHAR = 131,
	INVALID_NAME_PW_CHG = 132,
	INVALID_NAME_NEW_CONFLICT = 133,
	INVALID_NAME_NEW_LENGTH = 134,
	CHECK_PASSWORD_PW_CHG_VALIDITY = 135,
	INVALID_PASSWORD_OLD = 136,
	INVALID_PASSWORD_NEW_BADCHAR = 137,
	INVALID_PASSWORD_PW_CHG_LOGIN = 138,
	INVALID_PASSWORD_PW_CHG_BADCHAR = 139,
	INVALID_PASSWORD_NEW_LENGTH = 140,
	INVALID_PASSWORD_PW_CHG_LENGTH = 141,
	INVALID_PASSWORD_NEW_NOMATCH = 142,
	INVALID_PASSWORD_PW_CHG_NOMATCH = 143,
	INVALID_PASSWORD_PW_LOGIN_NOMATCH = 144,
	CHANGE_TO_LOGINMENU = 145,
	CHANGE_TO_ENTRY = 146,
	CHANGE_TO_ADMIN = 147,
	INVALID_INPUT2 = 149,
	ACCOUNT_LOCK_BEGIN = 150,
	PASSWORD_LOCKED = 151,
	ADMIN_INVALID_NAME = 152,
	ADMIN_INVALID_PW = 153,
	ADMIN_LOG_COMPLETE = 154,
	ADMIN_MENU = 155,
	ADMIN_INVALID_ENTRY = 156,
	PROCEED = 158,
	DUAL_LOG_OLD = 167,
	DUAL_LOG_PW_CHG = 168,
	DUAL_LOG_ADMIN = 169,
	INVALID_INPUT3 = 170,
	INVALID_INPUT4 = 171,
	INVALID_INPUT5 = 172,
	ADMIN_USER_ACCT_CONFLICT = 173,
	USER_ADMIN_ACCT_CONFLICT = 174,
	LOG_LOCKED = 180,
	SECURITY_LOCKED = 181,
	INVALID_NAME_NEW_NOMATCH = 182,
	LOGIN_SUCCESS_OLD = 184,
	LOGIN_SUCCESS_NEW = 185,
	LOGIN_SUCCESS_PW_CHG = 186,
	ADMIN_LOG_SUCCESS = 187,
	ADMIN_LOGOUT = 188

	// old
/*	CHECK_NAME_OLD = 1,
	CHECK_NAME_NEW_VALIDITY = 2,
	CHECK_NAME_PW_CHG = 3,
	CHECK_NAME_NEW_APPROVE = 4,
	INVALID_NAME_OLD = 11,
	INVALID_NAME_NEW_BADCHAR = 12,
	INVALID_NAME_PW_CHG = 13,
	INVALID_NAME_NEW_CONFLICT = 14,
	INVALID_NAME_NEW_LENGTH = 15,
	CHECK_PASSWORD_OLD = 21,
	CHECK_PASSWORD_NEW_VALIDITY = 22,
	CHECK_PASSWORD_PW_CHG_LOGIN = 23,
	CHECK_PASSWORD_PW_CHG_VALIDITY = 24,
	CHECK_PASSWORD_NEW_RECORD = 25,				
	CHECK_PASSWORD_PW_CHG_RECORD = 27,			
	INVALID_PASSWORD_OLD = 31,
	INVALID_PASSWORD_NEW_BADCHAR = 32,
	INVALID_PASSWORD_PW_CHG_LOGIN = 33,
	INVALID_PASSWORD_PW_CHG_BADCHAR = 34,
	INVALID_PASSWORD_NEW_LENGTH = 35,
	INVALID_PASSWORD_PW_CHG_LENGTH = 36,
	INVALID_PASSWORD_NEW_NOMATCH = 37,			
	INVALID_PASSWORD_PW_CHG_NOMATCH = 38,		
	INVALID_PASSWORD_PW_LOGIN_NOMATCH = 39,		
	LOGIN_SUCCESS_OLD = 41,
	LOGIN_SUCCESS_NEW = 42,
	LOGIN_SUCCESS_PW_CHG = 43,
	LOGIN_COMPLETE_OLD = 44,
	LOGIN_COMPLETE_NEW = 45,
	CHANGE_TO_LOGINMENU = 51,
	CHANGE_TO_ENTRY = 55,
	CHANGE_TO_ADMIN = 56,
	CHANGE_TO_ADMIN_LOG = 57,
	INVALID_INPUT = 61,
	QUIT = 63,
	ACCOUNT_LOCK_BEGIN = 64,
	ACCOUNT_LOCKED = 65,
	PROCEED = 66,
	LOGOUT = 67,
	CONTINUE = 68,
	INVALID_INPUT2 = 69,						
	ADMIN_LOGIN = 71,
	ADMIN_INVALID_NAME = 72,
	ADMIN_PW_LOGIN = 73,
	ADMIN_INVALID_PW = 74,
	ADMIN_LOG_SUCCESS = 75,
	ADMIN_LOG_COMPLETE = 76,
	ADMIN_MENU = 77,
	ADMIN_LOGOUT = 78,
	ADMIN_INVALID_ENTRY = 79,
	SHUTDOWN = 81,								
	*/
};

// client
enum ContentPacketTypes {

	CONNECT = 23,
	VALIDATE_CREDS = 24,

	START_CONTENT = 157,
	INVALID_CREDS = 159,
	VALID_CREDS = 161,
	SAY_WHAT = 163,
	LOGOUT_CONTENT_SERVER = 164

	// old
/*	SHUTDOWN = 81,		
	VALIDATE_CREDS = 101,
	CONNECT = 102,		
	INVALID_CREDS = 103,	
	ACCT_LOCKED = 104,
	VALID_CREDS = 105,
	DB_ERROR = 106,
	START_CONTENT = 111,
	SAY_WHAT = 112,
	LOGOUT = 113,
	TO_LOGIN = 114
	*/
};
