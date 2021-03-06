/*
4/2/16

content_packet.h
*/

#ifndef CONTENT_PACKET_H
#define CONTENT_PACKET_H


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
	DUAL_LOGIN_RESPONSE = 25,
	CHAT_IN_SAY = 26,
	CHAT_IN_TELL = 27,
	CHAT_OUT_SAY = 28,
	CHAT_OUT_TELL = 29,
	CHAT_USER_JOIN = 30,
	CHAT_USER_LEAVE = 31,
	
	TEST_PACKETS = 127,

	INVALID_NAME_BADCHAR = 131,
	INVALID_NAME_LENGTH = 134,
	CONTENT_GOTO_MENU = 157,
	INVALID_CREDS = 159,
	ACCT_LOCKED = 160,
	VALID_CREDS = 161,
	DB_ERROR = 162,
	CONTENT_INVALID_INPUT = 163,
	LOGOUT_CONTENT_SERVER = 164,
	SERVERTALK1 = 165,
	SERVERTALK2 = 166,
	FORCE_LOGOUT = 175,
	SERVER_DISCONNECT = 176,
	CHECK_CLIENT_CONNECTION = 177,
	CLIENT_DC_INACTIVE = 178,
	CHECK_SERVER_CONNECTION = 183,
	SHUTDOWN = 191,
	TO_LOGIN = 192,
	CHAT_INSTRUCTIONS = 193,
	CHAT_BEGIN = 194,
	CHAT_MESS_VER = 195,
	CHAT_NO_LISTENERS = 196,
	CHAT_NO_LIST_RECIPIENT = 197,
	CHAT_PROBATION_EXPIRE = 198,
	MESSAGES_EXCEEDED = 199,

	MAIL_GOTO_MENU = 200,				
	MAIL_BOX = 201,						
	MAIL_INSTRUCTIONS = 202,			
	MAIL_INBOX = 203,					
	MAIL_OUTBOX = 204,					
	MAIL_DELETE_INBOX = 205,			
	MAIL_DELETE_OUTBOX = 206,			
	MAIL_TO_SEND_SUCCESS = 207,			
	MAIL_TO_SEND_FAIL = 208,			
	CONTENT_INVALID_INPUT3 = 209,		
	MAIL_TO_SEND = 210,					
	MAIL_DELETE_READ = 211,
	MAIL_DELETE_UNREAD = 212,
	MAIL_DELETE_SENT = 213,
	MAIL_DELETE_ALL_OPTIONS = 214,		
	MAIL_DELETE_ALL_READ = 215,			
	MAIL_DELETE_ALL_UNREAD = 216,		
	MAIL_DELETE_ALL_INBOX = 217,		
	MAIL_DELETE_ALL_OUTBOX = 218,		
	MAIL_INBOX_READ = 219,				
	MAIL_INBOX_UNREAD = 220,
	MAIL_OUTBOX_SENT = 221,				
	MAIL_DELETE_FAIL = 222,
	MAIL_LIST_OOR = 223,
	CHAT_MENU = 224,
	MAIL_DELETE_INDV_SUCCESS = 225,
	MAIL_READ_RECV_MESSAGE = 226,		
	MAIL_TO_SEND_REPLY = 227,
	MAIL_TO_SEND_RECIPIENT = 228,
	MAIL_TO_SEND_SUBJECT = 229,
	MAIL_TO_SEND_BODY = 230,
	MAIL_TO_SEND_REVIEW = 231,
	MAIL_REVIEW_MESSAGE = 232,
	MAIL_EDIT_RECIPIENT = 233,
	MAIL_EDIT_SUBJECT = 234,
	MAIL_EDIT_BODY = 235,
	INVALID_NAME = 236,
	INVALID_INPUT_BADCHAR = 237,
	INVALID_INPUT_LENGTH = 238,
	MAIL_READ_FAIL = 239,
	MAIL_READ_SENT_MESSAGE = 240,
	CHECK_CLIENT_VERSION = 241,
	INVALID_CLIENT_VERSION = 242,
	MAIL_NO_NEXT_RECORDS = 243,
	MAIL_NO_PREVIOUS_RECORDS = 244,
	MAIL_MAILBOX_EMPTY = 245,
	MAIL_REVIEW_EDIT = 246,
	CONTENT_1ST_LOGIN = 247,
	MAIL_REVIEW_REPLY = 248,
	MAIL_EDIT_REPLY = 249,
	MAIL_REVIEW_REPLY_EDIT = 250,
	MAIL_TO_REPLY = 251

};

struct ContentPacket_header_struct {

	unsigned char length[4];
	unsigned char response_status[4];

	void serialize(char * response_status_value) {
		memcpy(response_status_value, this, sizeof(length));
		memcpy(response_status_value + 4, this, sizeof(response_status));
	}
	void deserialize(char * response_status_value) {
		memcpy(this, response_status_value, sizeof(length));
		memcpy(this, response_status_value + 4, sizeof(response_status));		
	}
};

#endif
