// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W

#ifndef LOGIN_PACKET_H
#define LOGIN_PACKET_H


#pragma once

#include "stdafx.h"


enum NonPacketActions {

	CHECK_NAME_NEW_VALIDITY2 = 501,
	CHECK_PASSWORD_NEW_VALIDITY2 = 502,
	CHECK_PASSWORD_PW_CHG_LOGIN2 = 503,
	CHECK_PASSWORD_PW_CHG_VALIDITY2 = 504,
	CONTENT_INVALID_INPUT2 = 505,
	CHAT_SELF_TALK = 506,
	CHAT_INCHAT = 507,
	CHAT_NO_MESSAGE = 508,
	CHAT_COMMAND = 509,
	CHAT_TOO_LONG = 510,
	MORE_MAIL = 511,
	CONFIRM_DELETE_ALL_OUTBOX = 512,
	CONFIRM_DELETE_ALL_READ = 513,
	CONFIRM_DELETE_ALL_UNREAD = 514,
	CONFIRM_DELETE_ALL_INBOX = 515,
	MAIL_SENDING = 516,
	MAIL_EDITING = 517,	
	MAIL_LIST_HEAD = 518,
	MAIL_ACCEPT_EDIT = 519,
	ACCOUNT_LOGIN_COUNT = 520,
	ACCOUNT_AVG_TIME = 521,
	MAIL_REPLYING = 522

};

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
	DUAL_LOGIN_RESPONSE = 25,
	TEST_PACKETS = 127,

	CHECK_NAME_NEW_APPROVE = 129,
	INVALID_NAME_OLD = 130,
	INVALID_NAME_BADCHAR = 131,
	INVALID_NAME_PW_CHG = 132,
	INVALID_NAME_NEW_CONFLICT = 133,
	INVALID_NAME_LENGTH = 134,
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
	SHUTDOWN = 191,
	CHECK_CLIENT_VERSION = 241,
	INVALID_CLIENT_VERSION = 242

};

struct LoginPacket_header_struct {	

	unsigned char length[4];
	unsigned char login_status[4];

	void serialize(char * login_status_value) {
		memcpy(login_status_value, this, sizeof(length));
		memcpy(login_status_value + 4, this, sizeof(login_status));
	}
	void deserialize(char * login_status_value) {
		memcpy(this, login_status_value, sizeof(length));
		memcpy(this, login_status_value + 4, sizeof(login_status));
	}
};

#endif
