// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W

#ifndef CLIENTNETWORK_H
#define CLIENTNETWORK_H


#pragma once

#include "stdafx.h"

#include "ws-util.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define RELEASE_VERSION "1.042"		// this should match up with the servers

#define DEFAULT_BUFLEN 512
#define LOGIN_PORT "6881"
#define CONTENT_PORT "6882"
#define LOGIN_PORTII "6883"			// this is the testing port
#define CONTENT_PORTII "6884"		// this is the testing port

#define MAX_PACKET_SIZE 1000000

#define SERVER_CONNECTION 1			// 0 = local network/laptop, 1 = internet/desktop, 2 = localhost/laptop, 3 = internet/laptop(atm other client settings if pointed properly won't matter too much)


enum ServerPacketCategory {

	LOGIN_EMPTY = 1,
	LOGIN_1VAL = 2,
	LOGIN_2VAL = 3,
	CONTENT_EMPTY = 4,
	CONTENT_NUMBER = 5,
	CONTENT_1VAL = 6,
	CONTENT_2VAL = 7,
	CONTENT_CHAT = 8,
	CONTENT_MAIL = 9
};

enum MenuStatus {

	LOGIN_MENU = 1,
	PROCEED_MENU = 2,
	ADMIN_MENU = 3,
	ADMIN_ACCOUNT = 4,
	EXISTING_ACCOUNT = 5,
	NEW_ACCOUNT = 6,
	CHNG_PASS_ACCOUNT = 7,
	CONTENT_MENU = 8,
	CHAT_ROOM = 9,
	MAIL_MENU = 10,	
	MAIL_SEND = 11,
	MAIL_REPLY = 12,
	MAIL_READ_IN = 13,
	MAIL_READ_OUT = 14,
	MAIL_DELETE_IN = 15,
	MAIL_DELETE_OUT = 16

};

class ClientNetwork
{

public:

	bool InitializeLoginConnection();
	
	bool InitializeContentConnection();
	
	void ClientPackets(int packet_type, int clienttalk_opvalue, std::string data_pos1 = "", std::string data_pos2 = ""); // new - handles sending of all packets

	SOCKET ConnectSocket;
	
	char packet[MAX_PACKET_SIZE];	

	void DoTestPackets(); // for testing

};

extern ClientNetwork Net;

extern bool done;
extern bool connected;
extern bool logged;

extern bool mask_input; // new
extern int menu_stat; // new

#endif
