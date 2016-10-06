// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W


#include "stdafx.h"

#include <algorithm>

#include "servernetwork.h"
#include "content_packet.h"
#include "content_net.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"
#include "timers.h"


extern int iResult = 0;

Timers ActivityTimer(CLIENT_ACTIVITY, true);
Timers ServerTimeTimer(SERVER_TIME_UPDATE, true);

std::vector<int> acct_ids;
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

	ZeroMemory(&serv_addr, sizeof(serv_addr));	// set address information
	serv_addr.ai_family = AF_INET;				// AF_INET	AF_UNSPEC	PF_INET
	serv_addr.ai_socktype = SOCK_STREAM;		// SOCK_DGRAM	SOCK_STREAM
	serv_addr.ai_protocol = IPPROTO_TCP;		// IPPROTO_UDP	IPPROTO_TCP
	serv_addr.ai_flags = AI_PASSIVE;

	if (SERVER_CONNECTION == 0) {
		iResult = getaddrinfo("192.168.0.3", CONTENT_PORTII, &serv_addr, &result); // change the last number as needed, use ipconfig
	}
	else if (SERVER_CONNECTION == 1) {
		iResult = getaddrinfo("192.168.0.2", CONTENT_PORT, &serv_addr, &result); // change the last number as needed, use ipconfig
	}
	else if (SERVER_CONNECTION == 2) {
		iResult = getaddrinfo("127.0.0.1", CONTENT_PORTII, &serv_addr, &result);
	}

//	iResult = getaddrinfo(NULL, CONTENT_PORT, &serv_addr, &result);				// I think NULL = localhost
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

void ServerNetwork::DirectIncomingConnections()
{
	fd_set socket_set;									// set of socket descriptors
	unsigned int max_socket;							// max socket descriptors

	struct timeval tv;									// this is for the select() function
	tv.tv_sec = 0;
	tv.tv_usec = 50;

	sockaddr_in clientIP;
	int IPaddrsize = sizeof(clientIP);

	FD_ZERO(&socket_set);								//clear the socket set
	FD_SET(MasterSocket, &socket_set);					//add master socket to set
	max_socket = MasterSocket;

	// this may be a little off	based on curent usage - it shouldn't iterate unless something is present
	for (unsigned int c = 1; c < max_clients; c++) {	// add child sockets to set - was c = 0
		ClientSocket = client_sock[c];					// socket descriptor
		if (ClientSocket > 0) {							// if valid socket descriptor then add to read list
			FD_SET(ClientSocket, &socket_set);
		}
		if (ClientSocket > max_socket) {				// highest file descriptor number, need it for the select function
			max_socket = ClientSocket;
		}
	}

	iResult = select(max_socket + 1, &socket_set, NULL, NULL, &tv); // wait for an activity on one of the sockets

	if ((iResult < 0) && (errno != EINTR)) {
		printf("%s\n", WSAGetLastErrorMessage("select FAILED with error :"));
		File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("select FAILED with error :"));
		WSACleanup();
		connected = false;
	}

	if (FD_ISSET(MasterSocket, &socket_set)) {			// If something happened on the master socket , then its an incoming connection
		if ((ClientSocket = accept(MasterSocket, (struct sockaddr *)&clientIP, (socklen_t*)&IPaddrsize)) < 0) {
			printf("%s\n", WSAGetLastErrorMessage("accept FAILED with error :"));
			File.LogRecord(Logs::Normal, Logs::NetError, "%s", WSAGetLastErrorMessage("accept FAILED with error :"));
			WSACleanup();
			connected = false;
		}

		char value = 1; // uncertain what this does - I think this is Nagle disabling
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		// <max_clients> and <socket_pos.size()> must be equal to work - this is where it happens 
		++max_clients;
		for (unsigned int c = 1; c < max_clients; c++) {							// add new socket to array of sockets // testing c = 1 - seems to work
			it_sock = std::find(socket_pos.begin(), socket_pos.end(), c);			// find an open position 
			if (it_sock != socket_pos.end()) {										// not open, found "c" in the vector
			//	printf("\n socket_position in use"); // for heavy debugging
				continue;
			}
			else {																	// position is empty
				socket_pos.push_back(c);											// add position(c) to occupied positions
				client_sock[c] = ClientSocket;										// give a socket descriptor

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

		//std::cout << "\n\t\t    -----CheckClientActivity CheckTimer-----" << std::endl; // for heavy debugging
		
		for (unsigned i = 0; i < socket_pos.size(); i++) {
			unsigned int c = socket_pos.at(i);
			ClientSocket = client_sock[c];
			int short_check = CheckActivityBySocket(ClientSocket, 1);
			int long_check = CheckActivityBySocket(ClientSocket, 2);

			if (ClientSocket > 0) {
				if (long_check == 4) {
					ConNet.ContentPackets(CONTENT_EMPTY, CLIENT_DC_INACTIVE, ClientSocket, c);
					if (LogoutByConnection(c, NONE, true, false, false) == false) {
						LogoutByConnection(c, NONE, true, true, false);
					}
				}
				else if (short_check == 3) {
					ConNet.ContentPackets(CONTENT_EMPTY, CHECK_CLIENT_CONNECTION, ClientSocket, c);
				}
				else if (short_check == 2) { // with more servers can expand the check values and corresponding timers	
					ConNet.DisableConnWaitTimer();
					ConNet.ContentPackets(CONTENT_EMPTY, CHECK_SERVER_CONNECTION, ClientSocket, c);
				}
			}
		}
	}
}

// check_status with this function is somewhat counter-intuitive
int ServerNetwork::CheckActivityBySocket(int client_socket, int check_status)
{
	/*
	check_status = 0		// basic check (if exists)
	check_status = 1		// check INACTIVE_CHECK
	check_status = 2		// check INACTIVE_CHECK X 2

	activity_type = 0		// did not check
	activity_type = 1		// checked as client (default if socket)
	activity_type = 2		// checked as server
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
				if (it->time_value + (INACTIVE_CHECK / 500) < timestamp) {
					activity_type = 4;
				}
			}
			else {
				activity_type = 1;
			}
		}
	}
	return activity_type;
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
	//	printf("\nAddToActivityList - socket-time-server:(%i:%i:%i)", new_activity.socket, new_activity.time_value, new_activity.is_server); // for heavy debugging
}

void ServerNetwork::RemoveFromActivityList(int client_socket)
{
	if (client_socket < 1) {
		return;
	}
	for (std::list<SocketActivity_struct>::iterator it = activity_list.begin(); it != activity_list.end(); ++it) {
		if (it->socket == client_socket) {
			//	printf("\nRemoveFromActivityList - socket-time-server:(%i:%i:%i)", it->socket, it->time_value, it->is_server); // for heavy debugging
			it = activity_list.erase(it);
			break;
		}
	}
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
	//printf("\n\t AccountInfo client_id-acctID-name-pw(%i:%i:%s:%s)", acct_info.client_id, acct_info.acct_id, GProg.CharOutput(acct_info.a_name), GProg.CharOutput(acct_info.a_password)); // for heavy debugging
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

int ServerNetwork::GetAcctIDFromAcctInfo(std::string p_name)
{
	int acctid = 0;

	for (std::list<AccountInfo_struct>::iterator it = acct_info_list.begin(); it != acct_info_list.end(); ++it) {
		if (it->a_name == p_name) {
			acctid = it->acct_id;
			break;
		}		
	}
	return acctid;
}

// this function should be placed immediately after a successful login
void ServerNetwork::AddToContentUserList(int socket_position, int client_socket, int account_id, int inchat, int is_admin, int is_server, int response_status)
{
	if (socket_position < 0) {
		return;
	}

	User_struct user_acct;
	new User_struct;

	user_acct.client_id = socket_position;
	user_acct.socket = client_socket;
	user_acct.acct_id = account_id;
	user_acct.in_chat = inchat;
	user_acct.admin = is_admin;
	user_acct.server = is_server;
	user_list.push_back(user_acct);

	if (inchat == 1) {
		SendChatNotificationPackets(socket_position, CHAT_USER_JOIN);
	}	
//	printf("\n AddToContentUserList:(%i-%i-%i-%i-%i-%i)\n", user_acct.client_id, user_acct.socket, user_acct.acct_id, user_acct.in_chat, user_acct.admin, user_acct.server); // for heavy debugging
}

void ServerNetwork::RemoveUserFromList(int socket_position)
{
	if (socket_position < 0) {
		return;
	}	

	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->client_id == socket_position) {
			if (IsUserInChat(socket_position) == true) {
				SendChatNotificationPackets(socket_position, CHAT_USER_LEAVE);
			}
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

bool ServerNetwork::IsUserInChat(int socket_position)
{
	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->client_id == socket_position) {
			if (it->in_chat == 1) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
}

// this could be changed and used for many things possibly with an enumerated status_type as the passed variable
// this is used to send acctIDs to login server and will be called at specific timer intervals - vector is cleared after send to prepare for the next cycle
void ServerNetwork::FindUserAccount()
{
	std::string s_acct_id; // single user acctID
	std::string s_acct_ids; // all user acctIDs

	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->acct_id > 0) {
			acct_ids.push_back(it->acct_id);
			//printf("\n FindUserAccount1 acct_ids.size:(%i) vec_acctID(%i)", acct_ids.size(), it->acct_id); // for heavy debugging
		}
	}
	if (acct_ids.size() > 0) {		
		for (unsigned int i = 0; i < acct_ids.size(); i++) {
			//printf("\n FindUserAccount2 acct_ids.size:(%i) vec_acctID(%i)", acct_ids.size(), acct_ids.at(i)); // for heavy debugging
			s_acct_id = std::to_string(acct_ids.at(i)); // possibly use append here and add the ! char
			GProg.AddCharsToString(s_acct_id, "!", 4);
			s_acct_ids.append(s_acct_id);			
		}
		ConNet.SendServerPackets(CONTENT_NUMBER, BAD_LOG, s_acct_ids);

	}
	else { // no accounts connected so should send an empty packet saying so
		ConNet.SendServerPackets(CONTENT_EMPTY, BAD_LOG);
	}
	acct_ids.clear();
}

// closes a single connection
void ServerNetwork::CloseClientConnection(int client_socket, int socket_position)
{
	client_socket =  client_sock[socket_position];
	RemoveUserFromList(socket_position);
	RemoveFromActivityList(client_socket);
	RemoveAcctFromList(socket_position);
	ConNet.RemoveUserFromChatFootprint(socket_position);
	if (ShutdownConnection(client_socket) != true) {
		printf("\n\t----ShutdownConnection failure on socket-position:(%i:%i)----\n\n", client_socket, socket_position);
		File.LogRecord(Logs::Normal, Logs::NetError, "ShutdownConnection failure on socket-position:(%i:%i)", client_socket, socket_position);
	}
	else {
		printf("\n\t    ----Removed client socket-position:(%i:%i)----\n", client_socket, socket_position);
		File.LogRecord(Logs::High, Logs::ContentPacket, "Removed client connection on socket-position:(%i:%i)", client_socket, socket_position);
	}

	--max_clients;
	client_sock[socket_position] = 0;
	socket_pos.erase(std::remove(socket_pos.begin(), socket_pos.end(), socket_position), socket_pos.end());
	//printf("\n clients_connected-vecsize -> (%i-%i)\n", max_clients - 1, socket_pos.size()); // for heavy debugging
}

void ServerNetwork::CloseAllClientConnections()
{
	for (it_sock = socket_pos.begin(); it_sock != socket_pos.end(); ++it_sock) {
		while (socket_pos.size() > 0) {
			ClientSocket = client_sock[*it_sock];

			//printf("\n CloseAllClientConnections: vecsize-socket-position(%i:%i:%i)\n", socket_pos.size(), ClientSocket, *it_sock); // for heavy debugging
			//printf("\n\t CloseAllClientConnections socket-position:(%i:%i)\n", ClientSocket, *it_sock); // for heavy debugging

			if (ClientSocket > 0 && *it_sock > 0) { // was c >= 0
				if (LogoutByConnection(*it_sock, SERVER_DISCONNECT, true, false, false) == false) {
					if (Net.LogoutByConnection(*it_sock, NONE, true, true, false) == false) {
						Net.LogoutByConnection(*it_sock, NONE, false, false, true);
					}
				}
			}
		}
	}
}

// used if client loses the connection, client makes a choice to "quit" or a controlled server shutdown
bool ServerNetwork::LogoutByConnection(int socket_position, int logout_packets, bool logged_in, bool admin, bool server)
{
	bool success = false;
	std::string name;

	if (admin == true) {
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->client_id == socket_position) {
				if (it->acct_id > 0 && it->admin == 1) {
					if (logout_packets != NONE) {
						ConNet.ContentPackets(CONTENT_EMPTY, logout_packets, it->socket, it->client_id);
					}					
					name = DB.GetAccountName(it->acct_id);
			//		Acct.AdminLogout(name);
					CloseClientConnection(it->socket, socket_position);
					success = true;
					return success;
				}
			}
		}
	}
	else if (server == true) {		
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->client_id == socket_position) {
				if (it->acct_id == 0 && it->server == 1) {
					if (logout_packets != NONE) {
						ConNet.ContentPackets(CONTENT_EMPTY, logout_packets, it->socket, socket_position);
					}
					File.LogRecord(Logs::Normal, Logs::General, "Closing server flagged connection - socket-position:(%i:%i)", it->socket, socket_position);
					CloseClientConnection(it->socket, socket_position);
					success = true;
					return success;
				}
			}
		}
	}
	else if (logged_in == false) { // this shouldn't happen		
		ClientSocket = client_sock[socket_position];
		if (logout_packets != NONE) {
			ConNet.ContentPackets(CONTENT_EMPTY, logout_packets, ClientSocket, socket_position);
		}
		File.LogRecord(Logs::Normal, Logs::Error, "ERROR: Client was not logged in - Closing connection - socket-position(%i:%i)", ClientSocket, socket_position);
		printf("\nERROR: Client was not logged in - Closing connection - socket-position(%i:%i)\n", ClientSocket, socket_position);
		CloseClientConnection(ClientSocket, socket_position);
		success = true;
		return success;
	}
	else {
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->client_id == socket_position) {
				if (it->acct_id > 0 && it->admin == 0) {
					if (logout_packets != NONE) {
						ConNet.ContentPackets(CONTENT_EMPTY, logout_packets, it->socket, it->client_id);
					}
					name = GetNameFromAcctInfo(socket_position); // assure there are no issues using this variable after closing the connection
					Acct.UserLogout(it->acct_id, name);
					CloseClientConnection(it->socket, socket_position);
					success = true;
					return success;
				}
			}
		}
	}
	return success;
}

// used if client is logged in and is forcefully removed
bool ServerNetwork::ForceLogoutByAccount(int account_id, bool admin)
{
	bool success = false;
	std::string name;

	if (admin == true) {
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->acct_id == account_id) {
				if (it->socket > 0 && it->admin == 1) {
					ConNet.ContentPackets(CONTENT_EMPTY, FORCE_LOGOUT, it->socket, it->client_id);
					name = GetNameFromAcctInfo(it->client_id);
			//		Acct.AdminLogout(GProg.CharOutput(name));
					CloseClientConnection(it->socket, it->client_id);
					success = true;
					break;
				}
				File.LogRecord(Logs::Normal, Logs::Error, "ERROR: admin account_id(%i) exists in memory with no valid socket/admin flag", account_id);
			}
		}
	}
	else {
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->acct_id == account_id) {
				if (it->socket > 0 && it->admin == 0) {
					ConNet.ContentPackets(CONTENT_EMPTY, FORCE_LOGOUT, it->socket, it->client_id);
					name = GetNameFromAcctInfo(it->client_id); // assure there are no issues using this variable after closing the connection
					Acct.UserLogout(it->acct_id, name);
					CloseClientConnection(it->socket, it->client_id);
					success = true;
					break;
				}
				File.LogRecord(Logs::Normal, Logs::Error, "ERROR: user account_id(%i) exists in memory with no valid socket", account_id);
			}
		}
	}
	return success;
}

// the names are swapped around from the user to send to(if /t) to the user sending so the recipient(s) can identify who said what
void ServerNetwork::ProcessChat(int socket_position, int client_socket, int response_status_value, std::string p_chat)
{
	SOCKET ClientSocket;
	bool success = false;
	std::string s_sender;
	std::string caps_s_sender;
	std::string rec_name;
	std::string caps_rec_name;
	std::string message;
	int acctid;
	int sock_pos;	

	if (response_status_value == CHAT_OUT_TELL) {
		rec_name = p_chat.substr(0, 12);
		GProg.RemoveCharsFromString(rec_name, "!");
		acctid = GetAcctIDFromAcctInfo(rec_name);
		s_sender = GetNameFromAcctInfo(socket_position);
		caps_rec_name = rec_name;
		caps_s_sender = s_sender;
		caps_rec_name[0] = toupper(caps_rec_name[0]);
		caps_s_sender[0] = toupper(caps_s_sender[0]);

		message = p_chat.substr(12, strlen(GProg.CharOutput(p_chat)) - 12);
		File.ChatRecord("[%s TELLS %s] %s", GProg.CharOutput(caps_s_sender), GProg.CharOutput(caps_rec_name), GProg.CharOutput(message));
		// printf("\n ProcessCHAT_OUT_TELL-> s_sender:(%s) rec_name:(%s) acctid:(%i)\n", GProg.CharOutput(s_sender), GProg.CharOutput(rec_name), acctid); // for heavy debugging

		if (acctid == 0) {
			File.ChatRecord("%s is not connected.", GProg.CharOutput(caps_rec_name));
			ConNet.ContentPackets(CONTENT_EMPTY, CHAT_NO_LIST_RECIPIENT, client_socket, socket_position);
		}
		else {
			for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
				if (it->acct_id == acctid && it->in_chat == 1) {
					ClientSocket = it->socket;
					sock_pos = it->client_id;

					ConNet.ContentPackets(CONTENT_CHAT, CHAT_IN_TELL, ClientSocket, sock_pos, s_sender, message);
					ConNet.ContentPackets(CONTENT_EMPTY, CHAT_MESS_VER, client_socket, socket_position);
					success = true;
					// printf("\n  ProcessCHAT_TELL back to sender-> rec_name:(%s) message(%s)\n", GProg.CharOutput(rec_name), GProg.CharOutput(message)); // for heavy debugging
					break;
				}
			}
			if (success == false) {
				File.ChatRecord("%s is not connected.", GProg.CharOutput(caps_rec_name));
				ConNet.ContentPackets(CONTENT_EMPTY, CHAT_NO_LIST_RECIPIENT, client_socket, socket_position);
			}
		}
	}
	else if (response_status_value == CHAT_OUT_SAY) {

		if (SendChatNotificationPackets(socket_position, CHAT_IN_SAY, p_chat) == true) {
			ConNet.ContentPackets(CONTENT_EMPTY, CHAT_MESS_VER, client_socket, socket_position);
		}
		else  {
			File.ChatRecord("There is noone else in chat.");
			ConNet.ContentPackets(CONTENT_EMPTY, CHAT_NO_LISTENERS, client_socket, socket_position);
		}
		//printf("\n  Process CHAT_SAY backtosender-> message(%s)\n",  GProg.CharOutput(p_chat)); // for heavy debugging
	}
}

// need to assure proper characters when putting messages in the DB
void ServerNetwork::ProcessMail(int socket_position, int client_socket, int response_status_value, std::string p_message)
{
	std::string s_sender;
	std::string rec_name;
	std::string subject;
	std::string message;
	std::string s_unread_ct;
	std::string s_read_ct;
	std::string s_sent_ct;
	std::string s_record_status; 
	std::string s_message_id;

	int message_count = 0;
	int acctid;
	int sender_acctid;
	
	switch (response_status_value) {		
	case MAIL_TO_SEND: // this is the main mail send process
		sender_acctid = GetAcctIDFromAcctInfo(socket_position);
		s_sender = GetNameFromAcctInfo(socket_position);
		rec_name = p_message.substr(0, 12);
		GProg.RemoveCharsFromString(rec_name, "!");
		subject = p_message.substr(12, 25);
		GProg.RemoveCharsFromString(subject, "!");
		message = p_message.substr(37, strlen(GProg.CharOutput(p_message)) - 37);
		acctid = DB.GetAccountID(rec_name.c_str());
		
		if (DB.NewMailRecord(sender_acctid, s_sender.c_str(), rec_name.c_str(), acctid, subject.c_str(), message.c_str()) == true) {
			ConNet.ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_SUCCESS, client_socket, socket_position);
		}
		else {
			ConNet.ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_FAIL, client_socket, socket_position);
		}
		break;
	case MAIL_TO_REPLY: // eventual the threadID will be sent back with the message to add it to a thread by ID
		sender_acctid = GetAcctIDFromAcctInfo(socket_position);
		s_sender = GetNameFromAcctInfo(socket_position);
		rec_name = p_message.substr(0, 12);
		GProg.RemoveCharsFromString(rec_name, "!");
		subject = p_message.substr(12, 25);
		GProg.RemoveCharsFromString(subject, "!");
		message = p_message.substr(37, strlen(GProg.CharOutput(p_message)) - 37);
		acctid = DB.GetAccountID(rec_name.c_str());
		
		if (DB.NewMailRecord(sender_acctid, s_sender.c_str(), rec_name.c_str(), acctid, subject.c_str(), message.c_str()) == true) {
			ConNet.ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_SUCCESS, client_socket, socket_position);
		}
		else {
			ConNet.ContentPackets(CONTENT_EMPTY, MAIL_TO_SEND_FAIL, client_socket, socket_position);
		}
		break;
	case MAIL_INBOX:
		acctid = GetAcctIDFromAcctInfo(socket_position);
		s_unread_ct = FormatMailCount(acctid, MAIL_INBOX_UNREAD);
		s_read_ct = FormatMailCount(acctid, MAIL_INBOX_READ);
		ConNet.ContentPackets(CONTENT_2VAL, MAIL_INBOX, client_socket, socket_position, s_unread_ct, s_read_ct);
		break;
	case MAIL_OUTBOX:
		acctid = GetAcctIDFromAcctInfo(socket_position);
		s_sent_ct = FormatMailCount(acctid, MAIL_OUTBOX_SENT);
		ConNet.ContentPackets(CONTENT_1VAL, MAIL_OUTBOX, client_socket, socket_position, s_sent_ct);
		break;
	case MAIL_DELETE_INBOX:
		acctid = GetAcctIDFromAcctInfo(socket_position);
		s_unread_ct = FormatMailCount(acctid, MAIL_DELETE_UNREAD);
		s_read_ct = FormatMailCount(acctid, MAIL_DELETE_READ);
		ConNet.ContentPackets(CONTENT_2VAL, MAIL_DELETE_INBOX, client_socket, socket_position, s_unread_ct, s_read_ct);
		break;
	case MAIL_DELETE_OUTBOX:
		acctid = GetAcctIDFromAcctInfo(socket_position);
		s_sent_ct = FormatMailCount(acctid, MAIL_DELETE_SENT);
		ConNet.ContentPackets(CONTENT_1VAL, MAIL_DELETE_OUTBOX, client_socket, socket_position, s_sent_ct);
		break;
	case MAIL_DELETE_ALL_READ:
		HandleDeleteAll(socket_position, client_socket, MAIL_DELETE_ALL_READ);
		break;
	case MAIL_DELETE_ALL_UNREAD:
		HandleDeleteAll(socket_position, client_socket, MAIL_DELETE_ALL_UNREAD);
		break;
	case MAIL_DELETE_ALL_INBOX:
		HandleDeleteAll(socket_position, client_socket, MAIL_DELETE_ALL_INBOX);
		break;
	case MAIL_DELETE_ALL_OUTBOX:
		HandleDeleteAll(socket_position, client_socket, MAIL_DELETE_ALL_OUTBOX);
		break;
	case MAIL_DELETE_READ:
		HandleLists(socket_position, client_socket, MAIL_DELETE_READ, p_message, true);
		break;		
	case MAIL_DELETE_UNREAD:
		HandleLists(socket_position, client_socket, MAIL_DELETE_UNREAD, p_message, true);
		break;
	case MAIL_DELETE_SENT:
		HandleLists(socket_position, client_socket, MAIL_DELETE_SENT, p_message, true);
		break;
	case MAIL_INBOX_READ:
		HandleLists(socket_position, client_socket, MAIL_INBOX_READ, p_message, false);
		break;
	case MAIL_INBOX_UNREAD:
		HandleLists(socket_position, client_socket, MAIL_INBOX_UNREAD, p_message, false);
		break;
	case MAIL_OUTBOX_SENT:
		HandleLists(socket_position, client_socket, MAIL_OUTBOX_SENT, p_message, false);
		break;
		
	case MAIL_TO_SEND_REPLY:
		HandleSendingInput(socket_position, client_socket, MAIL_TO_SEND_REPLY, p_message);
		break;
	case MAIL_TO_SEND_RECIPIENT:
		HandleSendingInput(socket_position, client_socket, MAIL_TO_SEND_RECIPIENT, p_message);
		break;
	case MAIL_TO_SEND_SUBJECT:
		HandleSendingInput(socket_position, client_socket, MAIL_TO_SEND_SUBJECT, p_message);
		break;
	case MAIL_TO_SEND_BODY:
		HandleSendingInput(socket_position, client_socket, MAIL_TO_SEND_BODY, p_message);
		break;	
	case MAIL_EDIT_RECIPIENT:
		HandleSendingInput(socket_position, client_socket, MAIL_EDIT_RECIPIENT, p_message);
		break;
	case MAIL_EDIT_SUBJECT:
		HandleSendingInput(socket_position, client_socket, MAIL_EDIT_SUBJECT, p_message);
		break;
	case MAIL_EDIT_BODY:
		HandleSendingInput(socket_position, client_socket, MAIL_EDIT_BODY, p_message);
		break;
	case MAIL_EDIT_REPLY:
		HandleSendingInput(socket_position, client_socket, MAIL_EDIT_REPLY, p_message);
		break;

	default:
		File.LogRecord(Logs::Normal, Logs::NetError, "ProcessMail ERROR Default: - case:(%i) p_message:(%s) socket:(%i) position:(%i)", response_status_value, p_message.c_str(), client_socket, socket_position);
		printf("\nERROR ProcessMail Default - case:(%i) p_message:(%s) socket:(%i) position:(%i)", response_status_value, p_message.c_str(), client_socket, socket_position);
	}
}

void ServerNetwork::HandleDeleteAll(int socket_position, int client_socket, int response_status_value)
{
	std::string s_delete_ct1;
	std::string s_delete_ct2;

	int delete_count1 = 0;
	int delete_count2;
	int acctid;

	acctid = GetAcctIDFromAcctInfo(socket_position);

	if (response_status_value == MAIL_DELETE_ALL_INBOX) {		
		delete_count2 = DB.GetMailCount(acctid, MAIL_DELETE_ALL_READ);
	}
	delete_count1 = DB.SetMailMessageStatus(response_status_value, 0, acctid);

	if (delete_count1 >= 0) { // try to account for read and unread with MAIL_DELETE_ALL_INBOX - means either sending a string back, doing it twice or saving previous values
		if (response_status_value == MAIL_DELETE_ALL_INBOX) {
			delete_count1 = delete_count1 - delete_count2;
			s_delete_ct1 = std::to_string(delete_count1); // MAIL_DELETE_ALL_UNREAD
			s_delete_ct2 = std::to_string(delete_count2); // MAIL_DELETE_ALL_READ
			ConNet.ContentPackets(CONTENT_2VAL, MAIL_DELETE_ALL_INBOX, client_socket, socket_position, s_delete_ct1, s_delete_ct2);
		}
		else {
			s_delete_ct1 = std::to_string(delete_count1);
			ConNet.ContentPackets(CONTENT_1VAL, response_status_value, client_socket, socket_position, s_delete_ct1);
		}		
	}
	else {
		ConNet.ContentPackets(CONTENT_EMPTY, MAIL_DELETE_FAIL, client_socket, socket_position);
	}
}

void ServerNetwork::HandleSendingInput(int socket_position, int client_socket, int response_status_value, std::string p_message)
{
	std::string::size_type sz;

	std::string s_send_status;
	std::string s_edit_status;
	std::string data1_string;
	std::string data2_string;

	const char * char_set;

	int input_status;	
	int min_len;
	int max_len;
	int char_response = INVALID_INPUT_BADCHAR;
	int length_response = INVALID_INPUT_LENGTH;
	int valid_response = MAIL_REVIEW_EDIT;
	int edit_status = 0;
	int acct_id = 1;

	data1_string = p_message.substr(0, 2);
	GProg.RemoveCharsFromString(data1_string, "!");
	edit_status = stoi(data1_string, &sz);	
	edit_status = CalculateEditStatus(response_status_value, edit_status);	
	s_edit_status = std::to_string(edit_status);

	data2_string = p_message.substr(2, strlen(p_message.c_str()) - 2);

	s_send_status = std::to_string(response_status_value);

	if (response_status_value == MAIL_TO_SEND_RECIPIENT || response_status_value == MAIL_EDIT_RECIPIENT) {
		if (response_status_value == MAIL_TO_SEND_RECIPIENT) {
			valid_response = MAIL_TO_SEND_SUBJECT;
		}		
		char_set = LOGIN_CHAR;
		min_len = 4;
		max_len = 12;
		char_response = INVALID_NAME_BADCHAR;
		length_response = INVALID_NAME_LENGTH;
		acct_id = DB.GetAccountID(data2_string.c_str());
	}
	if (response_status_value == MAIL_TO_SEND_SUBJECT || response_status_value == MAIL_EDIT_SUBJECT) {
		if (response_status_value == MAIL_TO_SEND_SUBJECT) {
			valid_response = MAIL_TO_SEND_BODY;
		}		
		char_set = DB_CHARS;
		min_len = 2;
		max_len = 25;
	}
	if (response_status_value == MAIL_TO_SEND_BODY || response_status_value == MAIL_EDIT_BODY || response_status_value == MAIL_TO_SEND_REPLY || response_status_value == MAIL_EDIT_REPLY) {
		if (response_status_value == MAIL_TO_SEND_BODY) {
			valid_response = MAIL_REVIEW_MESSAGE;
		}
		else if (response_status_value == MAIL_TO_SEND_REPLY) {
			valid_response = MAIL_REVIEW_REPLY;
		}
		else if (response_status_value == MAIL_EDIT_REPLY) {
			valid_response = MAIL_REVIEW_REPLY_EDIT;
		}
		char_set = DB_CHARS;
		min_len = 12;
		max_len = 2048;
	}
	
	input_status = GProg.IsValidInput(data2_string, false, false, true, char_set, NO_CHAR, min_len, max_len); // test the char stuff after "CalculateEditStatus()" is done

	if (input_status == 1) {
		ConNet.ContentPackets(CONTENT_2VAL, char_response, client_socket, socket_position, s_send_status, s_edit_status);
	}
	else if (input_status == 2) {
		ConNet.ContentPackets(CONTENT_2VAL, length_response, client_socket, socket_position, s_send_status, s_edit_status);
	}
	else if (acct_id == 0) {
		ConNet.ContentPackets(CONTENT_2VAL, INVALID_NAME, client_socket, socket_position, s_send_status, s_edit_status);
	}
	else {
		ConNet.ContentPackets(CONTENT_1VAL, valid_response, client_socket, socket_position, s_edit_status);
	}
}

// can use bitmask flags here
int ServerNetwork::CalculateEditStatus(int response_status_value, int edit_status)
{
/*	if (edit_status > 8) {
		return edit_status;
	}

	int i, iter;

	for (i = 1, iter = 1; i < 16; i <<= 1, iter++) { // the numeric value "16" represents the max bitmask range
		if (edit_status & i) {

			printf("Mail Flag: %d set\n", iter);

			if (iter == READ) {
				edit_status = edit_status + 1;
				printf("Mail Flag -> READ: %d set\n", iter);
			}
			if (iter == DELETE_SENDER) {
				edit_status = edit_status + 2;
				printf("Mail Flag -> DELETE_SENDER: %d set\n", iter);
			}
			if (iter == DELETE_RECV) {
				edit_status = edit_status + 4;
				printf("Mail Flag -> DELETE_RECV: %d set\n", iter);
			}
			if (iter == 4) {
				edit_status = 8;
				printf("Mail Flag -> ELSE: %d set\n", iter);
			}
		}
	} */

	if (response_status_value == MAIL_EDIT_RECIPIENT) {	
		if (edit_status == 0 || edit_status == 2 || edit_status == 4 || edit_status == 6) {
			edit_status = edit_status + 1;
		}		
	}
	else if (response_status_value == MAIL_EDIT_SUBJECT) {
		if (edit_status == 0 || edit_status == 1 || edit_status == 4 || edit_status == 5) {
			edit_status = edit_status + 2;
		}
	}
	else if (response_status_value == MAIL_EDIT_BODY) {
		if (edit_status <= 3) {
			edit_status = edit_status + 4;
		}
	}
	else if (response_status_value == MAIL_EDIT_REPLY) {
		if (edit_status == 0) {
			edit_status = 8;
		}
	}
	return edit_status;
}

void ServerNetwork::HandleLists(int socket_position, int client_socket, int response_status_value, std::string p_message, bool ascending)
{
	std::string::size_type sz;

	bool d_sent = false;
	bool in_read = false;

	std::string p_mail;
	std::string message;
	std::string s_message_ct;
	std::string s_record_status;
	std::string s_message_id;

	int message_id;
	int message_count = 0;
	int new_count = 0;
	int counter;
	int acctid;

	acctid = GetAcctIDFromAcctInfo(socket_position);
	
	if (strlen(p_message.c_str()) > 0) {
		message = p_message.substr(0, 12);
		GProg.RemoveCharsFromString(message, "!");

		s_message_ct = p_message.substr(12, strlen(p_message.c_str()) - 12);
		message_count = stoi(s_message_ct, &sz);

		if (message == "previous") {		
			if (message_count > 20) {
				new_count = PreviousMessageCount(message_count);
				s_message_ct = std::to_string(new_count);
				//printf("\n previous - message_count-new_count:(%i-%i)\n", message_count, new_count); // for heavy debugging

				ConNet.ContentPackets(CONTENT_1VAL, response_status_value, client_socket, socket_position, s_message_ct);
				DB.GetMailLists(socket_position, client_socket, response_status_value, acctid, new_count, ascending);
			}			
			else {
				s_record_status = std::to_string(response_status_value);
				ConNet.ContentPackets(CONTENT_1VAL, MAIL_NO_PREVIOUS_RECORDS, client_socket, socket_position, s_record_status);
				return;
			}
		}
		else if (message == "next") {
			new_count = DB.GetMailLists(socket_position, client_socket, response_status_value, acctid, message_count, ascending);			
			//printf("\n next - message_count-new_count:(%i-%i)\n", message_count, new_count); // for heavy debugging

			counter = NoNextMessageCount(new_count, message_count);
			if (counter < message_count) {
				s_record_status = std::to_string(response_status_value);
				s_message_ct = std::to_string(counter);
				ConNet.ContentPackets(CONTENT_2VAL, MAIL_NO_NEXT_RECORDS, client_socket, socket_position, s_record_status, s_message_ct);				
				return;
			}			
			if (new_count - message_count < 21) {
				s_record_status = std::to_string(MAIL_NO_NEXT_RECORDS);
				ConNet.ContentPackets(CONTENT_1VAL, response_status_value, client_socket, socket_position, s_record_status);
			}			
		}
		else if (message == "newlist") {
			new_count = DB.GetMailLists(socket_position, client_socket, response_status_value, acctid, message_count, ascending);		
			//printf("\n newlist - message_count-new_count:(%i-%i)\n", message_count, new_count); // for heavy debugging

			if (new_count == 0) {
				ConNet.ContentPackets(CONTENT_EMPTY, MAIL_MAILBOX_EMPTY, client_socket, socket_position);
				return;
			}
			if (new_count - message_count < 21) {
				s_record_status = std::to_string(MAIL_NO_NEXT_RECORDS);
				ConNet.ContentPackets(CONTENT_1VAL, response_status_value, client_socket, socket_position, s_record_status);
			}
		}
		else { // this handles user # selection
			int offset = stoi(message, &sz); // offset (# chosen)
			int offset_ct = message_count % 20;	// # of records in the set

			if (offset_ct == 0) {
				offset_ct = 20;
			}
			new_count = message_count - offset_ct + offset;
			s_message_ct = std::to_string(message_count - offset_ct);
			s_record_status = std::to_string(response_status_value);
			//printf("\n HandleLists - count-offset_ct-offset-msg_position:(%i-%i-%i-%i)\n", message_count, offset_ct, offset, new_count); // for heavy debugging

			if (response_status_value == MAIL_DELETE_READ || response_status_value == MAIL_DELETE_UNREAD || response_status_value == MAIL_DELETE_SENT) {							
				message_id = DB.GetMailID(acctid, response_status_value, new_count, true);				

				if (DB.SetMailMessageStatus(response_status_value, message_id, 0) == 1) {
					ConNet.ContentPackets(CONTENT_2VAL, MAIL_DELETE_INDV_SUCCESS, client_socket, socket_position, s_record_status, s_message_ct);
				}
				else if (offset > offset_ct) {					
					ConNet.ContentPackets(CONTENT_2VAL, MAIL_LIST_OOR, client_socket, socket_position, s_record_status, s_message_ct);
				}
				else {
					ConNet.ContentPackets(CONTENT_EMPTY, MAIL_DELETE_FAIL, client_socket, socket_position);
				}
				return;
			}
			else if (response_status_value == MAIL_INBOX_READ || response_status_value == MAIL_INBOX_UNREAD || response_status_value == MAIL_OUTBOX_SENT) {			
				if (response_status_value == MAIL_OUTBOX_SENT) {
					d_sent = true;
				}
				if (response_status_value == MAIL_INBOX_READ) {
					in_read = true;
				}
				message_id = DB.GetMailID(acctid, response_status_value, new_count, false);
				
				if (message_id != 0) {
					p_mail = DB.ReadMailRecord(message_id, d_sent);

					if (d_sent == false && in_read == false) {
						DB.SetMailMessageStatus(MAIL_INBOX_UNREAD, message_id, 0);
					}
					if (response_status_value == MAIL_OUTBOX_SENT) {
						ConNet.ContentPackets(CONTENT_MAIL, MAIL_READ_SENT_MESSAGE, client_socket, socket_position, p_mail);
					}
					else {
						ConNet.ContentPackets(CONTENT_MAIL, MAIL_READ_RECV_MESSAGE, client_socket, socket_position, p_mail);
					}
				}
				else if (message_count > 0 && message_id == 0) {
					ConNet.ContentPackets(CONTENT_2VAL, MAIL_LIST_OOR, client_socket, socket_position, s_record_status, s_message_ct);
					return;
				}
				else {
					ConNet.ContentPackets(CONTENT_EMPTY, MAIL_READ_FAIL , client_socket, socket_position);
					return;
				}				
			}			
		}
	}
	else if (strlen(p_message.c_str()) == 0) {
		new_count = DB.GetMailLists(socket_position, client_socket, response_status_value, acctid, 0, ascending);
	
		if (new_count == 0) {
			ConNet.ContentPackets(CONTENT_EMPTY, MAIL_MAILBOX_EMPTY, client_socket, socket_position);
			return;
		}
		if (new_count != 21) {
			s_record_status = std::to_string(MAIL_NO_NEXT_RECORDS);
			ConNet.ContentPackets(CONTENT_1VAL, response_status_value, client_socket, socket_position, s_record_status);
		}
	}
	s_record_status = std::to_string(MORE_MAIL);
	ConNet.ContentPackets(CONTENT_1VAL, response_status_value, client_socket, socket_position, s_record_status);
}

// change this to return "int" and change conditions which use this currently, calculate the previous message_ct before sending the current list and return that
int ServerNetwork::NoNextMessageCount(int new_ct, int message_ct)
{
	int new_counter = 0;
	int rem_mess_count = message_ct % 20;

	if (new_ct == message_ct && new_ct % 20 == 0) {
		new_counter = message_ct - 20;		
	}
	else if (new_ct % 20 != 0 && new_ct - message_ct < 20) {
		new_counter = message_ct - rem_mess_count;		
	}
	else {
		new_counter = message_ct;
	}
	return new_counter;
}

// for "previous" so far - no purpose to alter for "next" client seems to handle this fine
int ServerNetwork::PreviousMessageCount(int message_counter)
{
	int new_counter;
	int rem_mess_count = message_counter % 20;

	if (message_counter <= 20) {		// "previous" should be disabled
		new_counter = 0;
	}
	else if (rem_mess_count != 0) {		// remainder of > 0 means end of records
		new_counter = message_counter - (rem_mess_count) - 20;		
	}
	else {								// remainder of 0 means more records available
		new_counter = message_counter - 40;		
	}
	return new_counter;
}

std::string ServerNetwork::FormatMailCount(int account_id, int response_status_value)
{
	int message_count = 0;
	std::string s_message_ct;
	
	message_count = DB.GetMailCount(account_id, response_status_value);

	std::string s_count = std::to_string(message_count);

	if (message_count == 0) { // no mail of this category
		s_message_ct = "-(0)-";
	}
	else if (strlen(s_count.c_str()) == 1) {
		s_message_ct = "-(" + s_count + ")-";
	}
	else if (strlen(s_count.c_str()) == 2) {
		s_message_ct = "(" + s_count + ")-";
	}
	else if (strlen(s_count.c_str()) == 3) {
		s_message_ct = "(" + s_count + ")";
	}
	else { // more than 999 mails // if (strlen(s_count.c_str()) > 3)
		s_message_ct = "(999)";
	}
	return s_message_ct;
}

bool ServerNetwork::SendChatNotificationPackets(int subject_sock_pos, int chat_packets, std::string p_message)
{
	SOCKET ClientSocket;
	bool success = false;
	std::string s_sender;
	std::string caps_s_sender;
	int sock_pos;	

	s_sender = GetNameFromAcctInfo(subject_sock_pos);	
	caps_s_sender = s_sender;
	caps_s_sender[0] = toupper(caps_s_sender[0]);

	if (chat_packets == CHAT_IN_SAY) {
		File.ChatRecord("[%s SAYS] %s", GProg.CharOutput(caps_s_sender), GProg.CharOutput(p_message));

		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->acct_id > 0 && it->server == 0 && it->in_chat == 1) {
				ClientSocket = it->socket;
				sock_pos = it->client_id;

				if (sock_pos != subject_sock_pos) {					
					ConNet.ContentPackets(CONTENT_CHAT, CHAT_IN_SAY, ClientSocket, sock_pos, s_sender, p_message);
					success = true;
					//printf("\n SendChatNotificationPackets-> s_sender:(%s) message(%s)\n", GProg.CharOutput(s_sender), GProg.CharOutput(p_message)); // for heavy debugging
				}
			}
		}
		return success;
	}
	if (chat_packets == CHAT_USER_JOIN || chat_packets == CHAT_USER_LEAVE) {
		if (chat_packets == CHAT_USER_JOIN) {
			File.ChatRecord("%s joined chat.", GProg.CharOutput(caps_s_sender));
		}
		else if (chat_packets == CHAT_USER_LEAVE) {
			File.ChatRecord("%s left the room.", GProg.CharOutput(caps_s_sender));
		}		
		for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
			if (it->acct_id > 0 && it->server == 0 && it->in_chat == 1) {
				ClientSocket = it->socket;
				sock_pos = it->client_id;

				if (sock_pos != subject_sock_pos) {
					ConNet.ContentPackets(CONTENT_1VAL, chat_packets, ClientSocket, sock_pos, s_sender);
					// printf("\n SendChatNotificationPackets-> s_sender:(%s)\n", GProg.CharOutput(s_sender)); // for heavy debugging
				}
			}
		}
	}
	return success;
}

void ServerNetwork::ChangeServerTime()
{
	if (ServerTimeTimer.CheckTimer(SERVER_TIME_UPDATE) == false) {
		return;
	}
	DB.UpdateServerTime(false);
}

void ServerNetwork::DisableTimers()
{
	ActivityTimer.DisableTimer();
	ServerTimeTimer.DisableTimer();

	ConNet.DisableConNetTimers();
}

void ServerNetwork::StartNetTimers()
{
	if (ActivityTimer.IsEnabled() == false) {
		ActivityTimer.StartTimer(CLIENT_ACTIVITY);
	}
	if (ServerTimeTimer.IsEnabled() == false) {
		ServerTimeTimer.StartTimer(SERVER_TIME_UPDATE);
		std::cout << "\n\t\t\t-----Start Net Timers-----\n" << std::endl;
	}	
}

// for heavy debugging/testing
void ServerNetwork::ShowTestOutput(int socket_position)
{
	for (std::list<User_struct>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (it->client_id == socket_position) {
			printf("\nUser - entity:(%i) socket:(%i) AcctID:(%i) in_chat:(%i)",	it->client_id, it->socket, it->acct_id, it->in_chat);
			printf("\nUser - admin:(%i) server:(%i) list_size:(%i)", it->admin, it->server, int(user_list.size()));
			break;
		}
	}
}
