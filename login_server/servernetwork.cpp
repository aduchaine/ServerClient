// first try -> followed the directions from the link below exactly - the commented out sections are what I added - not, highly customized now
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W

#include "stdafx.h"

#include <algorithm>

#include "servernetwork.h"
#include "login_net.h"
#include "login_packet.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"
#include "timers.h"

extern int iResult = 0;

Timers ActivityTimer(CLIENT_ACTIVITY, true);
Timers ServerActivityTimer(SERVER_ACTIVITY, true);

std::vector<unsigned int>::iterator it_sock;


bool ServerNetwork::InitializeConnection()
{
	WSADATA wsaData; // create WSADATA object	

	int opt = TRUE; // or 1?

	MasterSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	
	struct addrinfo *result = NULL; // address info for the server to listen to
	struct addrinfo serv_addr;
	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		File.LogRecord(Logs::Normal, Logs::NetError, "WSAStartup failed with error: %d", iResult);
		connected = false;
		return connected;
	}

	ZeroMemory(&serv_addr, sizeof(serv_addr)); 	// set address information
	serv_addr.ai_family = AF_INET;			// AF_INET			AF_UNSPEC		PF_INET
	serv_addr.ai_socktype = SOCK_STREAM;	// SOCK_DGRAM		SOCK_STREAM
	serv_addr.ai_protocol = IPPROTO_TCP;	// IPPROTO_UDP		IPPROTO_TCP
	serv_addr.ai_flags = AI_PASSIVE;

	
	if (SERVER_CONNECTION == 0) {
		iResult = getaddrinfo("192.168.0.3", LOGIN_PORTII, &serv_addr, &result); // change the last number as needed, use ipconfig
	}
	else if (SERVER_CONNECTION == 1) {
		iResult = getaddrinfo("192.168.0.2", LOGIN_PORT, &serv_addr, &result); // change the last number as needed, use ipconfig
	}
	else if (SERVER_CONNECTION == 2) {
		iResult = getaddrinfo("127.0.0.1", LOGIN_PORTII, &serv_addr, &result);
	}

//	iResult = getaddrinfo(NULL, LOGIN_PORT, &serv_addr, &result);			// I think NULL = localhost
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		File.LogRecord(Logs::Normal, Logs::NetError, "getaddrinfo failed with error: %d", iResult);
		WSACleanup();
		connected = false;
		return connected;
	}

	MasterSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // Create a SOCKET for connecting to server
	if (MasterSocket == INVALID_SOCKET) {		
		printf("%s\n", WSAGetLastErrorMessage("socket FAILED with error :"));
		File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("socket FAILED with error :"));
		freeaddrinfo(result);
		WSACleanup();
		connected = false;
		return connected;
	}

	u_long iMode = 1; // Set the mode of the socket to be nonblocking
	iResult = ioctlsocket(MasterSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR) {		
		printf("%s\n", WSAGetLastErrorMessage("ioctlsocket FAILED with error :"));
		File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("ioctlsocket FAILED with error :"));
		closesocket(MasterSocket);
		WSACleanup();
		connected = false;
		return connected;
	}

	iResult = bind(MasterSocket, result->ai_addr, (int)result->ai_addrlen); // Setup the TCP listening socket
	if (iResult == SOCKET_ERROR) {
		printf("%s\n", WSAGetLastErrorMessage("bind FAILED with error :"));
		File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("bind FAILED with error :"));		
		freeaddrinfo(result);
		closesocket(MasterSocket);
		WSACleanup();
		connected = false;
		return connected;
	}

	//set master socket to allow multiple connections , this is just a good habit, it will work without this
	if (setsockopt(MasterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		connected = false;
		return connected;
	}

	freeaddrinfo(result); // no longer need address information

	iResult = listen(MasterSocket, SOMAXCONN); // start listening for new clients attempting to connect
	if (iResult == SOCKET_ERROR) {
		printf("%s\n", WSAGetLastErrorMessage("listen FAILED with error :"));
		File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("listen FAILED with error :"));		
		closesocket(MasterSocket);
		WSACleanup();
		connected = false;
		return connected;
	}
	DisableTimers();
	connected = true;
	return connected;
}

bool ServerNetwork::InitializeContentConnection()
{
	WSADATA wsaData;
	ContentSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, serv_addr;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("\n\nWSAStartup failed with error: %d\n", iResult);
		return false;
	}
	ZeroMemory(&serv_addr, sizeof(serv_addr));
	serv_addr.ai_family = AF_INET;			// AF_INET			AF_UNSPEC
	serv_addr.ai_socktype = SOCK_STREAM;	// SOCK_DGRAM		SOCK_STREAM
	serv_addr.ai_protocol = IPPROTO_TCP;	// IPPROTO_UDP		IPPROTO_TCP	

	if (SERVER_CONNECTION == 0) {
		iResult = getaddrinfo("192.168.0.3", CONTENT_PORTII, &serv_addr, &result); // change the last number as needed, use ipconfig
	}
	else if (SERVER_CONNECTION == 1) {
		iResult = getaddrinfo("192.168.0.2", CONTENT_PORT, &serv_addr, &result); // change the last number as needed, use ipconfig
	}
	else if (SERVER_CONNECTION == 2) {
		iResult = getaddrinfo("127.0.0.1", CONTENT_PORTII, &serv_addr, &result);
	}

//	iResult = getaddrinfo("24.165.29.219", CONTENT_PORT, &serv_addr, &result);	// use this address as client if remote - change as the server IP changes

	if (iResult != 0) {
		printf("\n\ngetaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ContentSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ContentSocket == INVALID_SOCKET) {
			printf("\n\n%s\n", WSAGetLastErrorMessage("socket FAILED with error :"));
			WSACleanup();
			return false;
		}
		iResult = connect(ContentSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ContentSocket);
			ContentSocket = INVALID_SOCKET;
			printf("\n\nSOCKET ERROR: Unable to connect to the content server.\n");
			return false;
		}
	}
	freeaddrinfo(result);
	if (ContentSocket == INVALID_SOCKET) { // is this the same as above?
		printf("\n\nINVALID SOCKET: Unable to connect to content server.\n");
		WSACleanup();
		return false;
	}
	u_long iMode = 1;
	iResult = ioctlsocket(ContentSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR) {
		printf("\n\n%s\n", WSAGetLastErrorMessage("ioctlsocket FAILED with error :"));
		closesocket(ContentSocket);
		WSACleanup();
		return false;
	}
	char value = 1;
	setsockopt(ContentSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));	

	return true;
}

void ServerNetwork::DirectIncomingConnections()
{
	fd_set socket_set;									// set of socket descriptors
	unsigned int max_socket;							// max socket descriptors

	struct timeval tv;									// this is for the select() function
	tv.tv_sec = 0;
	tv.tv_usec = 50;

	sockaddr_in clientIP;
	int IPaddrsize = sizeof(clientIP);

	FD_ZERO(&socket_set);								//	clear the socket set	
	FD_SET(MasterSocket, &socket_set);					//	add master socket to set = listen socket
	max_socket = MasterSocket;
	
	// this may be a little off	based on curent usage - it won't do anything here unless something is present
	for (unsigned int c = 1; c < max_clients; c++) {	//	add child sockets to set (c) - was c = 0
		ClientSocket = client_sock[c];					//	socket descriptor (ClientSocket)	
		if (ClientSocket > 0) {							//	if valid socket descriptor then add to read list
			FD_SET(ClientSocket, &socket_set);
		}
		if (ClientSocket > max_socket) {				//	highest file descriptor number, need it for the select function (unsure what this is exactly)
			max_socket = ClientSocket;
		}		
	}

	iResult = select(max_socket + 1, &socket_set, NULL, NULL, &tv); // wait for an activity on one of the sockets

	if ((iResult < 0) && (errno != EINTR)) {
		printf("%s\n", WSAGetLastErrorMessage("select FAILED with error :"));
		File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("select FAILED with error :"));
		WSACleanup();
		connected = false; // if select fails the whole thing won't work so this seems ok
	}

	if (FD_ISSET(MasterSocket, &socket_set)) { // If something happened on the master socket , then its an incoming connection
		if ((ClientSocket = accept(MasterSocket, (struct sockaddr *)&clientIP, (socklen_t*)&IPaddrsize)) < 0) {
			printf("%s\n", WSAGetLastErrorMessage("accept FAILED with error :"));
			File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("accept FAILED with error :"));
			WSACleanup();
			connected = false; // probably don't need to shutdown the server if it can't accept a client unless I am misunderstanding this
		}		

		char value = 1; // uncertain what this does - I think this is Nagle disabling
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));		
		
		// <max_clients> and <socket_pos.size()> must be equal to work - this is where it happens 
		++max_clients;																// increment up to allow the next connection	
		for (unsigned int c = 1; c < max_clients; c++) {							// add new socket to array of sockets // was c = 0
			it_sock = std::find(socket_pos.begin(), socket_pos.end(), c);			// find an open position 
			if (it_sock != socket_pos.end()) {										// not open, found "c" in the vector
			//	printf("\n socket_position in use"); // for heavy debugging	
				continue;
			}
			else {																	// position is empty
				socket_pos.push_back(c);											// add position(c) to occupied positions
				client_sock[c] = ClientSocket;										// assign give the socket descriptor chosen in select

				//printf("\n clients_connected-socket_position-vecsize -> (%i-%i-%i)\n", max_clients - 1, c, socket_pos.size()); // for heavy debugging			
				std::cout << "    New connection from " << inet_ntoa(clientIP.sin_addr) << ":" << ntohs(clientIP.sin_port)
					<< " on socket-position:(" << ClientSocket << ":" << c << ")" << std::endl;
				File.LogRecord(Logs::Normal, Logs::LoginPacket, "New connection from %s:%i on socket-position:(%i:%i)", inet_ntoa(clientIP.sin_addr), ntohs(clientIP.sin_port), ClientSocket, c);
				break;
			}
		}
	}
}

void ServerNetwork::CheckClientActivity()
{
	SOCKET ClientSocket;

	if (ActivityTimer.CheckTimer(CLIENT_ACTIVITY) == true) {

		//std::cout << "\n\t\t    -----ActivityTimer.CheckTimer-----" << std::endl; // for heavy debugging

		for (unsigned i = 0; i < socket_pos.size(); i++) {
			unsigned int c = socket_pos.at(i);
			ClientSocket = client_sock[c];
			int short_check = CheckActivityBySocket(ClientSocket, 1);
			int long_check = CheckActivityBySocket(ClientSocket, 2);

			if (ClientSocket > 0) {
				if (short_check == 1) { // this will send a packet each time CLIENT_ACTIVITY checks until long_check == true					
					LogNet.LoginPackets(LOGIN_EMPTY, CHECK_CLIENT_CONNECTION, ClientSocket, c);
				}
				else if (long_check == 4) {					
					if (LogoutByConnection(c, CLIENT_DC_INACTIVE, true, false) == false) {
						if (LogoutByConnection(c, CLIENT_DC_INACTIVE, true, true) == false) {
							LogoutByConnection(c, CLIENT_DC_INACTIVE, false, false);
						}
					}
				}
				else if (short_check == 3) { // this will send a packet each time CLIENT_ACTIVITY checks until long_check == true
					LogNet.LoginPackets(LOGIN_EMPTY, CHECK_CLIENT_CONNECTION, ClientSocket, c);
				}
			}
		}
	}
}

void ServerNetwork::CheckServerActivity(int socket)
{
	if (ServerActivityTimer.CheckTimer(SERVER_ACTIVITY) == true) {
		//std::cout << "\n\t\t    -----ServerActivityTimer.CheckTimer-----" << std::endl; // for heavy debugging
		int type = CheckActivityBySocket(socket, 0); // returns 2 if content server and it will not check the INACTIVE_CHECK timer value
		if (socket > 0) {
			if (type == 2) {
				LogNet.LoginPackets(CONTENT_EMPTY, CHECK_SERVER_CONNECTION, Net.ContentSocket, 0);
			}
		}
	}
}

void ServerNetwork::AddToActivityList(int client_socket, int time_stamp, int server)
{
	if (client_socket < 1) {
		return;
	}
	SocketActivity_struct new_activity;
	new SocketActivity_struct;

	new_activity.socket = client_socket;
	new_activity.time_value = time_stamp;
	new_activity.is_server = server;
	activity_list.push_back(new_activity);
}

void ServerNetwork::RemoveFromActivityList(int client_socket)
{
	if (client_socket < 1) {
		return;
	}
	for (std::list<SocketActivity_struct>::iterator it = activity_list.begin(); it != activity_list.end(); ++it) {
		if (it->socket == client_socket) {			
			it = activity_list.erase(it);			
			break;
		}
	}
}

int ServerNetwork::CheckActivityBySocket(int client_socket, int check_status)
{
	/*
	check_status = 0		// basic check -> return what and if exists
	check_status = 1		// check INACTIVE_CHECK
	check_status = 2		// check INACTIVE_CHECK X 2

	activity_type = 0		// did not check
	activity_type = 1		// checked as client (default if socket checks)
	activity_type = 2		// checked as content server (can create more cases for more servers, etc)
	activity_type = 3		// checked as INACTIVE_CHECK
	activity_type = 4		// checked as INACTIVE_CHECK X 2
	*/

	int activity_type = 0;
	int timestamp = static_cast<int>(time(0));

	for (std::list<SocketActivity_struct>::iterator it = activity_list.begin(); it != activity_list.end(); ++it) {
		if (it->socket == client_socket) {			
			if (it->is_server == 1) {
				activity_type = 2;
				break;
			}			
			if (check_status == 1) {
				if (it->time_value + (INACTIVE_CHECK / 1000) < timestamp) {
					activity_type = 3;					
				}
			}
			else if (check_status == 2) {
				if (it->time_value + (INACTIVE_CHECK / 500) < timestamp) { // double the time
					activity_type = 4;					
				}
			}
			else {
				activity_type = 1;
			}
		}
	}	
	return activity_type; // activity_type = 1 means no activity because it's returning a value ??
}

void ServerNetwork::AddToAcctInfoList(int socket_position, int acct_int, std::string p_name, std::string acct_pw)
{
	if (socket_position < 0) {
		return;
	}
	AccountInfo_struct acct_info;
	new AccountInfo_struct;

	acct_info.client_id = socket_position;
	acct_info.acct_id = acct_int;
	acct_info.a_name = p_name;
	acct_info.a_password = acct_pw;
	acct_info_list.push_back(acct_info);
}

void ServerNetwork::RemoveAcctFromList(int socket_position)
{
	if (socket_position < 0) {
		return;
	}
	for (std::list<AccountInfo_struct>::iterator it = acct_info_list.begin(); it != acct_info_list.end(); ++it) {
		if (it->client_id == socket_position) {
			it = acct_info_list.erase(it);
			break;
		}
	}
}

std::string ServerNetwork::GetPWFromAcctInfo(int socket_position)
{
	std::string acctpw = "";

	for (std::list<AccountInfo_struct>::iterator it = acct_info_list.begin(); it != acct_info_list.end(); ++it) {
		if (it->client_id == socket_position) {
			acctpw = it->a_password;
			break;
		}
	}
	return acctpw;
}

std::string ServerNetwork::GetNameFromAcctInfo(int socket_position)
{
	std::string acctname = "";

	for (std::list<AccountInfo_struct>::iterator it = acct_info_list.begin(); it != acct_info_list.end(); ++it) {
		if (it->client_id == socket_position) {
			acctname = it->a_name;
			break;
		}
	}
	return acctname;
}

std::string ServerNetwork::GetNameByAcctIDFromAcctInfo(int acct_int)
{
	std::string acctname = "";

	for (std::list<AccountInfo_struct>::iterator it = acct_info_list.begin(); it != acct_info_list.end(); ++it) {
		if (it->acct_id == acct_int) {
			acctname = it->a_name;
			break;
		}
	}
	return acctname;
}

int ServerNetwork::GetAcctIDFromAcctInfo(int socket_position)
{
	int acctid = 0;

	for (std::list<AccountInfo_struct>::iterator it = acct_info_list.begin(); it != acct_info_list.end(); ++it) {
		if (it->client_id == socket_position) {
			acctid = it->acct_id;
			break;
		}
	}
	return acctid;
}

void ServerNetwork::AddToLoginUserList(int socket_position, int client_socket, int account_id, int is_admin, int is_server)
{
	if (socket_position < 0) {
		return;
	}
	User_struct user_acct;
	new User_struct;

	user_acct.client_id = socket_position;
	user_acct.socket = client_socket;
	user_acct.acct_id = account_id;
	user_acct.admin = is_admin;
	user_acct.server = is_server;
	user_list.push_back(user_acct);
}

void ServerNetwork::RemoveUserFromList(int socket_position)
{
	if (socket_position < 0) {
		return;
	}
	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->client_id == socket_position) {
			it = user_list.erase(it);
			break;
		}
	}
}

bool ServerNetwork::FindUserByAccount(int account_id)
{
	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->acct_id == account_id) {
			return true;
		}
	}
	return false;
}

bool ServerNetwork::FindUserBySockPos(int socket_position)
{
	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->client_id == socket_position) {
			return true;
		}
	}
	return false;
}

// this is used to add logged-in admin acctIDs to the acctIDs the content server sent over
void ServerNetwork::AddToLoggedAccounts()
{
	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->admin > 0) {
			content_acct_ids.push_back(it->acct_id);
			//printf("\n AddToLoggedAccounts admin acctID:(%i)\n", it->acct_id); // for heavy debugging
		}
	}
}

// closes a single connection
void ServerNetwork::CloseClientConnection(int client_socket, int socket_position)
{
	client_socket = client_sock[socket_position];
	RemoveUserFromList(socket_position);
	LogNet.RemoveReservedName(socket_position);
	RemoveFromActivityList(client_socket);
	RemoveAcctFromList(socket_position);
	if (ShutdownConnection(client_socket) != true) {
		printf("\n\t----ShutdownConnection failure on socket-position:(%i:%i)----\n\n", client_socket, socket_position); // for heavy debugging
		File.LogRecord(Logs::Normal, Logs::NetError, "ShutdownConnection failure on socket-position:(%i:%i)", client_socket, socket_position);
	}
	else {
		printf("\n\t    ----Removed client socket-position:(%i:%i)----\n\n", client_socket, socket_position); // for heavy debugging
		File.LogRecord(Logs::High, Logs::LoginPacket, "Removed client connection on socket-position:(%i:%i)", client_socket, socket_position);
	}

	client_sock[socket_position] = 0;
	socket_pos.erase(std::remove(socket_pos.begin(), socket_pos.end(), socket_position), socket_pos.end());
	--max_clients;
//	printf("\n clients_connected-vecsize -> (%i-%i)\n", max_clients - 1, socket_pos.size()); // for heavy debugging
}

void ServerNetwork::CloseAllClientConnections()
{
	for (it_sock = socket_pos.begin(); it_sock != socket_pos.end(); ++it_sock) {
		while (socket_pos.size() > 0) {
			ClientSocket = client_sock[*it_sock];

			//	printf("\n CloseAllClientConnections: vecsize-socket_position(%i:%i)\n", socket_pos.size(), *it_sock); // for heavy debugging
			//	printf("\n\t CloseAllClientConnections socket-position:(%i:%i)\n", ClientSocket, *it_sock); // for heavy debugging

			if (ClientSocket > 0 && *it_sock > 0) {
				if (LogoutByConnection(*it_sock, SERVER_DISCONNECT, true, true) == false) { // try admin first, then normal user
					if (LogoutByConnection(*it_sock, SERVER_DISCONNECT, true, false) == false) {
						LogoutByConnection(*it_sock, SERVER_DISCONNECT, false, false);
					}
				}
			}
		}
	}
}

// used if client loses the connection, client makes a choice to "quit" or a controlled server shutdown
bool ServerNetwork::LogoutByConnection(int socket_position, int send_logout_packets, bool logged_in, bool admin)
{
	bool success = false;
	std::string name;

//	printf("\n LogoutByConnection: position(%i)\n", socket_position); // for heavy debugging

	if (admin == true) {
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->client_id == socket_position) {				
				if (it->acct_id > 0 && it->admin == 1) {					
					if (send_logout_packets != NONE) {
						LogNet.LoginPackets(LOGIN_EMPTY, send_logout_packets, it->socket, it->client_id);
					}
					name = GetNameFromAcctInfo(socket_position);
					Acct.AdminLogout(it->acct_id, name);
					CloseClientConnection(it->socket, socket_position);				
					success = true;
					return success;
				}
				// chage this
				File.LogRecord(Logs::Normal, Logs::Error, "ERROR: admin socket_position(%i) exists in memory with no valid account_id/admin flag", socket_position);
			}			
		}
	}
	else if (logged_in == false) {
		ClientSocket =  client_sock[socket_position];
		if (send_logout_packets != NONE) {
			LogNet.LoginPackets(LOGIN_EMPTY, send_logout_packets, ClientSocket, socket_position);
		}
		//printf("\nLogoutByConnection: Closing connection - socket-position(%i:%i)\n", ClientSocket, socket_position); // for heavy debugging
		CloseClientConnection(ClientSocket, socket_position);
		success = true;
		return success;
	}
	else {
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->client_id == socket_position) {				
				if (it->acct_id > 0 && it->admin == 0) {
					if (send_logout_packets != NONE) {
						LogNet.LoginPackets(LOGIN_EMPTY, send_logout_packets, it->socket, it->client_id);
					}
					name = GetNameFromAcctInfo(socket_position);
					Acct.UserLogout(it->acct_id, name);
					CloseClientConnection(it->socket, socket_position);					
					success = true;
					return success;
				}
				File.LogRecord(Logs::Normal, Logs::Error, "ERROR: user socket_position(%i) exists in memory with no valid account_id", socket_position);
			}			
		}
	}	
	return success;
}

// used if client is logged in and another user logs the same account
bool ServerNetwork::ForceLogoutByAccount(int account_id, bool admin)
{
	bool success = false;
	std::string name;

	if (admin == true) {
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->acct_id == account_id) {
				if (it->socket > 0 && it->admin == 1) {
					LogNet.LoginPackets(LOGIN_EMPTY, FORCE_LOGOUT, it->socket, it->client_id);
					name = GetNameFromAcctInfo(it->client_id);
					Acct.AdminLogout(account_id, name);
					CloseClientConnection(it->socket, it->client_id);					
					success = true;
					return success;
				}
				File.LogRecord(Logs::Normal, Logs::Error, "ERROR: admin account_id(%i) exists in memory with no valid socket/admin flag", account_id);
			}			
		}
	}
	else {		
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->acct_id == account_id) {
				if (it->socket > 0 && it->admin == 0) {
					LogNet.LoginPackets(LOGIN_EMPTY, FORCE_LOGOUT, it->socket, it->client_id);
					name = GetNameFromAcctInfo(it->client_id);
					Acct.UserLogout(account_id, name);
					CloseClientConnection(it->socket, it->client_id);					
					success = true;
					return success;
				}
				File.LogRecord(Logs::Normal, Logs::Error, "ERROR: user account_id(%i) exists in memory with no valid socket", account_id);
			}			
		}
	}
	return success;
}

void ServerNetwork::DisableTimers()
{
	ActivityTimer.DisableTimer();
//	ServerActivityTimer.DisableTimer();

	LogNet.DisableLogNetTimers();
}

void ServerNetwork::StartNetTimers()
{
	ActivityTimer.StartTimer(CLIENT_ACTIVITY);
	ServerActivityTimer.StartTimer(SERVER_ACTIVITY);
	std::cout << "\t\t\t-----Start Net Timers-----\n" << std::endl; // for heavy debugging
}

// for heavy debugging/testing
void ServerNetwork::ShowTestOutput(int socket_position)
{
	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->client_id == socket_position) {
			printf("\nstruct_User - entity:(%i) socket:(%i) AcctID:(%i) list_size:(%i)", it->client_id, it->socket, it->acct_id, int(user_list.size()));
			break;
		}
	}
}
