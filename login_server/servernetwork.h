// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W

#ifndef SERVERNETWORK_H
#define SERVERNETWORK_H


#pragma once

#include "stdafx.h"

#include <list>
#include <map>

#include "ws-util.h"

#pragma comment (lib, "Ws2_32.lib")


#define RELEASE_VERSION "1.042"		// client must have the same value to connect

#define DEFAULT_BUFLEN 512
#define LOGIN_PORT "6881"
#define CONTENT_PORT "6882"
#define LOGIN_PORTII "6883"			// this is the testing port
#define CONTENT_PORTII "6884"		// this is the testing port

#define MAX_PACKET_SIZE 1000000

#define SERVER_CONNECTION 1			// 0 = local network/laptop, 1 = internet/desktop, 2 = localhost/laptop, 3 = internet/laptop (atm other client settings if pointed properly won't matter too much)


// logged in accounts
struct User_struct
{
	int client_id;			// "client_id" is the equivalent of "c" or "socket_position" (like an entity_id)
	int socket;				// "socket" is "ClientSocket" from below
	int acct_id;			// "acct_id" is "account_id" from database
	int admin;				// "admin" is admin switch (0 = no, 1 = yes)
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

// used anytime a client connects to the server - for client inactive feature
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
	SOCKET ContentSocket;

	bool InitializeConnection();
	bool InitializeContentConnection();
	void DirectIncomingConnections();
	void StartNetTimers();
	void DisableTimers();

	// new connectivity/activity function/struct
	void CheckClientActivity();
	void CheckServerActivity(int socket);
	void AddToActivityList(int client_socket, int time_stamp, int server);
	void RemoveFromActivityList(int client_socket); // used on shutdown and controlled logout/closeclientconnection	
	int CheckActivityBySocket(int client_socket, int check_status); // check_status(0 = if exist, 1 = timer, 2 = timerX2)
	std::list<SocketActivity_struct> activity_list;

	// account info from DB after name verification
	void AddToAcctInfoList(int socket_position, int acct_int, std::string p_name, std::string acct_pw);
	void RemoveAcctFromList(int socket_position);
	int GetAcctIDFromAcctInfo(int socket_position);
	std::string GetPWFromAcctInfo(int socket_position);
	std::string GetNameFromAcctInfo(int socket_position);
	std::string GetNameByAcctIDFromAcctInfo(int acct_int);
	std::list<AccountInfo_struct> acct_info_list;

	// client info after successful login
	void AddToLoginUserList(int socket_position, int client_socket, int account_id, int is_admin, int is_server);
	void RemoveUserFromList(int socket_position);
	bool FindUserByAccount(int account_id);
	bool FindUserBySockPos(int socket_position);
	void AddToLoggedAccounts();
	std::list<User_struct> user_list;

	// does most of the work for logging out clients
	void CloseClientConnection(int client_socket, int socket_position);
	void CloseAllClientConnections();
	bool LogoutByConnection(int socket_position, int send_logout_packets, bool logged_in, bool admin);
	bool ForceLogoutByAccount(int account_id, bool admin);

	int client_sock[FD_SETSIZE]; // was [31] - the max number of sockets(socket_position) -> FD_SETSIZE = 64 - can change to something else
	unsigned int max_clients = 1; // was 31 - this will be incremented and is bound by FD_SETSIZE
	char packet[MAX_PACKET_SIZE];	

	void ShowTestOutput(int socket_position); // for heavy debugging/testing

};

extern ServerNetwork Net;

extern bool connected;
extern bool restart_conn; // new for controlled shutdown and restart attempts

#endif
