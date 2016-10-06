// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W

#ifndef SERVERNETWORK_H
#define SERVERNETWORK_H


#pragma once

#include "stdafx.h"

#include <list>

#include "ws-util.h"

#pragma comment (lib, "Ws2_32.lib")


#define RELEASE_VERSION "1.042"		// client must have the same value to connect

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

struct User_struct
{
	int client_id;			// "client_id" is the equivalent of "c" or "socket_position" (kindof like an entity_id)
	int socket;				// "socket" is "ClientSocket"
	int acct_id;			// "acct_id" is "account_id" from database
	int in_chat;			// "in_chat" is chat room switch (0 = no 1 = yes)
	int admin;				// "admin" is admin switch (0 = no 1 = yes)
	int server;				// "server" identifies server connections (0 = no 1 = yes)
};

// for validation during login - this memory will be written to when a user enters a correct login name
struct AccountInfo_struct
{
	int client_id;			// "client_id" is the equivalent of "c" or "socket_position" (like an entity_id)
	int acct_id;			// "acct_id" is account_id from the DB 
	std::string a_name;		// "a_name" is account name from the DB
	std::string a_password;	// "a_password" is password from the DB
};

struct SocketActivity_struct
{
	int socket;				// "socket" is "ClientSocket"
	int time_value;			// "time_value" is the inserted "static_cast<int>(time(0))" value
	int is_server;			// "is_server" identifies connections - 0 = client connection 1 = server connection
};

class ServerNetwork
{

public:

	SOCKET MasterSocket;
	SOCKET ClientSocket;

	bool InitializeConnection();
	void DirectIncomingConnections();
	void StartNetTimers();
	void DisableTimers();

	// new connectivity/activity function/struct
	void CheckClientActivity();
	int CheckActivityBySocket(int client_socket, int check_status);
	void AddToActivityList(int client_socket, int time_stamp, int server);
	void RemoveFromActivityList(int client_socket);	
	std::list<SocketActivity_struct> activity_list;	

	// account info from DB after name verification
	void AddToAcctInfoList(int socket_position, int acct_int, std::string p_name, std::string p_password = ""); // password not needed except for admin login when implemented
	void RemoveAcctFromList(int socket_position);
	int GetAcctIDFromAcctInfo(int socket_position);
	std::string GetPWFromAcctInfo(int socket_position);
	std::string GetNameFromAcctInfo(int socket_position);
	std::string GetNameByAcctIDFromAcctInfo(int acct_int);
	int GetAcctIDFromAcctInfo(std::string p_name);
	std::list<AccountInfo_struct> acct_info_list;

	// client info after successful cred verif
	void AddToContentUserList(int socket_position, int client_socket, int account_id, int inchat, int is_admin, int is_server, int response_status);
	void RemoveUserFromList(int socket_position);
	bool FindUserByAccount(int account_id);
	bool FindUserBySockPos(int socket_position);
	bool IsUserInChat(int socket_position);
	void FindUserAccount();
	std::list<User_struct> user_list;

	// client logouts/close connections
	void CloseClientConnection(int client_socket, int socket_position);
	void CloseAllClientConnections();
	bool LogoutByConnection(int socket_position, int logout_packets, bool logged_in, bool admin, bool server);
	bool ForceLogoutByAccount(int account_id, bool admin);
	
	// chat
	void ProcessChat(int socket_position, int client_socket, int response_status_value, std::string p_chat);
	bool SendChatNotificationPackets(int subject_sock_pos, int chat_packets, std::string p_message = " ");

	// mail system
	void ProcessMail(int socket_position, int client_socket, int response_status_value, std::string p_message);
	void HandleDeleteAll(int socket_position, int client_socket, int response_status_value);
	void HandleSendingInput(int socket_position, int client_socket, int response_status_value, std::string p_message);	
	void HandleLists(int socket_position, int client_socket, int response_status_value, std::string p_message, bool ascending);

	std::string FormatMailCount(int account_id, int response_status_value);
	int CalculateEditStatus(int response_status_value, int edit_status);
	int NoNextMessageCount(int new_ct, int message_ct);
	int PreviousMessageCount(int message_count);
	
	// various
	void ChangeServerTime();

	int client_sock[FD_SETSIZE];
	unsigned int max_clients = 1;
	char packet[MAX_PACKET_SIZE];

	void ShowTestOutput(int socket_position); // for heavy debugging/testing	

};

extern ServerNetwork Net;

extern bool connected;
extern bool restart_conn;

#endif
