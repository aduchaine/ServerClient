/*
4/2/16

content_packet.h
*/

#ifndef CONTENT_PACKET_H
#define CONTENT_PACKET_H


#pragma once

#include "stdafx.h"


enum ContentPacketTypes {

	CONNECT = 23,
	VALIDATE_CREDS = 24,
	CHAT_IN_SAY = 26,
	CHAT_IN_TELL = 27,
	CHAT_OUT_SAY = 28,
	CHAT_OUT_TELL = 29,
	CHAT_USER_JOIN = 30,
	CHAT_USER_LEAVE = 31,

	CONTENT_GOTO_MENU = 157,
	INVALID_CREDS = 159,
	VALID_CREDS = 161,
	CONTENT_INVALID_INPUT = 163,
	LOGOUT_CONTENT_SERVER = 164,
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
	MAIL_LIST_OOR = 223,				// out of range list selection
	CHAT_MENU = 224,					// chat menu
	MAIL_DELETE_INDV_SUCCESS = 225,		// delete success message + data to send user back to revised list + position
	MAIL_READ_RECV_MESSAGE = 226,		// the actual mail message, for recv mail text format
	MAIL_TO_SEND_REPLY = 227,			// reply choice from MAIL_READ_RECV_MESSAGE
	MAIL_TO_SEND_RECIPIENT = 228,		// client input request/send
	MAIL_TO_SEND_SUBJECT = 229,			// client input request/send
	MAIL_TO_SEND_BODY = 230,			// client input request/send
	MAIL_TO_SEND_REVIEW = 231,			// client input request/send - client menu to make changes to the three required fields or confirm sending
	MAIL_REVIEW_MESSAGE = 232,			// client input request
	MAIL_EDIT_RECIPIENT = 233,			// client input request
	MAIL_EDIT_SUBJECT = 234,			// client input request
	MAIL_EDIT_BODY = 235,				// client input request
	MAIL_READ_FAIL = 239,				// used for DB/data unreadable  errors 
	MAIL_READ_SENT_MESSAGE = 240,		// the actual mail message, for sent mail text format
	MAIL_NO_NEXT_RECORDS = 243,
	MAIL_NO_PREVIOUS_RECORDS = 244,
	MAIL_MAILBOX_EMPTY = 245,			// server response if 0 messages in box, returns user to mailbox
	MAIL_REVIEW_EDIT = 246,				// client accepts edit change, also sends the mail
	CONTENT_1ST_LOGIN = 247,			// first login, welcome message to be expanded to other things
	MAIL_REVIEW_REPLY = 248,			// client accepts reply, also sends the mail
	MAIL_EDIT_REPLY = 249,				// edit body of reply
	MAIL_REVIEW_REPLY_EDIT = 250,
	MAIL_TO_REPLY = 251

};

#endif