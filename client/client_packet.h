/*
4/11/16

client_packet.h
*/

#ifndef CLIENT_PACKET_H
#define CLIENT_PACKET_H


#pragma once

#include "stdafx.h"


enum ClientPacketTypes {

	NONE = 0,

	TEST_PACKETS = 127,

	INVALID_NAME_BADCHAR = 131,
	INVALID_NAME_LENGTH = 134,
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
	TO_LOGIN = 192,	
	INVALID_NAME = 236,
	INVALID_INPUT_BADCHAR = 237,
	INVALID_INPUT_LENGTH = 238,
	CHECK_CLIENT_VERSION = 241,
	INVALID_CLIENT_VERSION = 242

};

struct ClientPacket_header_struct {

	unsigned char length[4];
	unsigned char response_status[4];

	void serialize(char * login_status_value) {
		memcpy(login_status_value, this, sizeof(length));
		memcpy(login_status_value + 4, this, sizeof(response_status));
	}
	void deserialize(char * login_status_value) {
		memcpy(this, login_status_value, sizeof(length));
		memcpy(this, login_status_value + 4, sizeof(response_status));
	}
};

#endif