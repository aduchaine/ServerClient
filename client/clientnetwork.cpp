// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W


#include "stdafx.h"

#include "clientnetwork.h"
#include "content_packet.h"
#include "login_net.h"

#include "gen_program.h"


extern int iResult = 0;


bool ClientNetwork::InitializeLoginConnection()
{	
	WSADATA wsaData; // create WSADATA object	
	ConnectSocket = INVALID_SOCKET; // socket	
	struct addrinfo *result = NULL, *ptr = NULL, serv_addr; // holds address info for socket to connect to	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
	if (iResult != 0) {
		printf("\n\nWSAStartup failed with error: %d\n", iResult);
		return false;
	}
	// set address info
	ZeroMemory(&serv_addr, sizeof(serv_addr));
	serv_addr.ai_family = AF_INET; // AF_INET  AF_UNSPEC
	serv_addr.ai_socktype = SOCK_STREAM; // SOCK_DGRAM SOCK_STREAM
	serv_addr.ai_protocol = IPPROTO_TCP; // IPPROTO_UDP IPPROTO_TCP

	//resolve server address and port	
	if (SERVER_CONNECTION == 0) {
		iResult = getaddrinfo("192.168.0.3", LOGIN_PORTII, &serv_addr, &result);
	}
	else if (SERVER_CONNECTION == 1) {
		iResult = getaddrinfo("24.165.29.219", LOGIN_PORT, &serv_addr, &result);
	}
	else if (SERVER_CONNECTION == 2) {
		iResult = getaddrinfo("127.0.0.1", LOGIN_PORTII, &serv_addr, &result);
	}
	else if (SERVER_CONNECTION == 3) {
		iResult = getaddrinfo("24.165.29.219", LOGIN_PORTII, &serv_addr, &result);
	}

	if (iResult != 0) {
		printf("\n\ngetaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}	
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) { // Attempt to connect to an address until one succeeds		
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol); // Create a SOCKET for connecting to server
		if (ConnectSocket == INVALID_SOCKET) {
			printf("\n\n%s\n", WSAGetLastErrorMessage("socket FAILED with error :"));
			WSACleanup();
			return false;
		}		
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen); // Connect to server.
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			printf("\n\nUnable to connect to the login server.\n");
			return false;
		}
	}	
	freeaddrinfo(result); // no longer need address info for server	
	if (ConnectSocket == INVALID_SOCKET) { // if connection failed
		printf("\n\nUnable to connect to the login server.\n");
		WSACleanup();
		return false;
	}	
	u_long iMode = 1; // Set the mode of the socket to be nonblocking - only block client(0) if on short server timer
	iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR) {
		printf("\n\n%s\n", WSAGetLastErrorMessage("ioctlsocket FAILED with error :"));
		closesocket(ConnectSocket);
		WSACleanup();
		return false;
	}	
	char value = 1; //disable nagle
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
	return true; // everything went through
}

bool ClientNetwork::InitializeContentConnection()
{
	WSADATA wsaData;	
	ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, serv_addr;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("\n\nWSAStartup failed with error: %d\n", iResult);
		return false;
	}
	ZeroMemory(&serv_addr, sizeof(serv_addr));
	serv_addr.ai_family = AF_INET; // AF_INET  AF_UNSPEC
	serv_addr.ai_socktype = SOCK_STREAM; // SOCK_DGRAM SOCK_STREAM
	serv_addr.ai_protocol = IPPROTO_TCP; // IPPROTO_UDP IPPROTO_TCP TCP connection!!!	

	if (SERVER_CONNECTION == 0) {
		iResult = getaddrinfo("192.168.0.3", CONTENT_PORTII, &serv_addr, &result);
	}
	else if (SERVER_CONNECTION == 1) {
		iResult = getaddrinfo("24.165.29.219", CONTENT_PORT, &serv_addr, &result);
	}
	else if (SERVER_CONNECTION == 2) {
		iResult = getaddrinfo("127.0.0.1", CONTENT_PORTII, &serv_addr, &result);
	}
	else if (SERVER_CONNECTION == 3) {
		iResult = getaddrinfo("24.165.29.219", CONTENT_PORTII, &serv_addr, &result);
	}

	if (iResult != 0) {
		printf("\n\ngetaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}	
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {		
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("\n\n%s\n", WSAGetLastErrorMessage("socket FAILED with error :"));
			WSACleanup();
			return false;
		}		
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			printf("\n\nUnable to connect to the content server.\n");
			return false;
		}
	}	
	freeaddrinfo(result);	
	if (ConnectSocket == INVALID_SOCKET) {
		printf("\n\nUnable to connect to the content server.\n");
		WSACleanup();
		return false;
	}	
	u_long iMode = 1;
	iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR) {
		printf("\n\n%s\n", WSAGetLastErrorMessage("ioctlsocket FAILED with error :"));
		closesocket(ConnectSocket);
		WSACleanup();
		return false;
	}	
	char value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

	return true; // everything went through
}

// change the names of these variables to more general
void ClientNetwork::ClientPackets(int packet_type, int clienttalk_opvalue, std::string data_pos1, std::string data_pos2)
{
	unsigned int size_length;

	std::string s_len;
	std::string s_ct_op;

	s_ct_op = std::to_string(clienttalk_opvalue);

	switch (packet_type) {
	case LOGIN_EMPTY:
		size_length = 0x0008;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
	//	GProg.AddCharsToString(s_ct_op, "!", 4); // this likely isn't needed
		memcpy(packet, &s_len, 4);
		memcpy(packet + 4, &s_ct_op, 4);
		break;
	case LOGIN_1VAL:
		size_length = 0x0014;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		memcpy(packet, &s_len, 4);
		memcpy(packet + 4, &s_ct_op, 4);
		memcpy(packet + 8, &data_pos1, 12);
		break;
	case LOGIN_2VAL:
		size_length = 0x0020;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		GProg.AddCharsToString(data_pos1, "!", 12);
		memcpy(packet, &s_len, 4);
		memcpy(packet + 4, &s_ct_op, 4);
		memcpy(packet + 8, &data_pos1, 12);
		memcpy(packet + 20, &data_pos2, 12);
		break;
	case CONTENT_EMPTY:
		size_length = 0x0008;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
	//	GProg.AddCharsToString(s_ct_op, "!", 4); // this likely isn't needed
		memcpy(packet, &s_len, 4);
		memcpy(packet + 4, &s_ct_op, 4);
		break;
	case CONTENT_NUMBER:
		size_length = 0x0008 + strlen(data_pos1.c_str());
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		strcpy(Net.packet + 8, data_pos1.c_str());
		break;
	case CONTENT_1VAL:
		size_length = 0x0014;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		memcpy(packet, &s_len, 4);
		memcpy(packet + 4, &s_ct_op, 4);
		memcpy(packet + 8, &data_pos1, 12);
		break;
	case CONTENT_2VAL:
		size_length = 0x0020;
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		GProg.AddCharsToString(data_pos1, "!", 12);
		memcpy(packet, &s_len, 4);
		memcpy(packet + 4, &s_ct_op, 4);
		memcpy(packet + 8, &data_pos1, 12);
		memcpy(packet + 20, &data_pos2, 12);
		break;
	case CONTENT_CHAT:		
		if (clienttalk_opvalue == CHAT_OUT_TELL) {
			size_length = 0x0014 + strlen(data_pos2.c_str());
			s_len = std::to_string(size_length);
			GProg.AddCharsToString(s_len, "!", 4);
			GProg.AddCharsToString(s_ct_op, "!", 4);
			memcpy(packet, &s_len, 4);
			memcpy(packet + 4, &s_ct_op, 4);
			GProg.AddCharsToString(data_pos1, "!", 12);
			memcpy(packet + 8, &data_pos1, 12);
			strcpy(packet + 20, data_pos2.c_str());
		}
		else {
			size_length = 0x0008 + strlen(data_pos1.c_str());
			s_len = std::to_string(size_length);
			GProg.AddCharsToString(s_len, "!", 4);
			GProg.AddCharsToString(s_ct_op, "!", 4);
			memcpy(packet, &s_len, 4);
			memcpy(packet + 4, &s_ct_op, 4);
			strcpy(packet + 8, data_pos1.c_str());
		}
		break;
	case CONTENT_MAIL:
		size_length = 0x0008 + strlen(data_pos1.c_str());
		s_len = std::to_string(size_length);
		GProg.AddCharsToString(s_len, "!", 4);
		GProg.AddCharsToString(s_ct_op, "!", 4);
		memcpy(Net.packet, &s_len, 4);
		memcpy(Net.packet + 4, &s_ct_op, 4);
		strcpy(Net.packet + 8, data_pos1.c_str());
		break;
	}

	//printf("\nClientPackets: packet_type:(%i) size:(%i) s_op_value(%s) data_pos1(%s) data_pos2(%s)", packet_type, size_length, GProg.CharOutput(s_ct_op),
	//	sizeof(data_pos1) > 0 ? GProg.CharOutput(data_pos1) : "NULL", sizeof(data_pos2) > 0 ? GProg.CharOutput(data_pos2) : "NULL"); // for heavy debugging

	iResult = send(ConnectSocket, packet, size_length, 0);
	int sock_error = WSAGetLastError();
	if (iResult == SOCKET_ERROR) {
		LogNet.SendPacketErrorHandling(clienttalk_opvalue);
	}
}

void ClientNetwork::DoTestPackets()
{
	std::string s1 = "hello";
	std::string s2 = "dude";
	std::string i1 = "12345";
	std::string chat1 = "Adrian";
	std::string chat2 = "hey dude what is up";

	//	ClientPackets(LOGIN_EMPTY, 127);
	//	ClientPackets(LOGIN_1VAL, 127, s1);
	//	ClientPackets(LOGIN_2VAL, 127, s1, s2);
	//	ClientPackets(CONTENT_EMPTY, 127);
	//	ClientPackets(CONTENT_NUMBER, 127, i1);
	//	ClientPackets(CONTENT_1VAL, 127, s1);
	//	ClientPackets(CONTENT_2VAL, 127, s1, s2);
	//	ClientPackets(CONTENT_CHAT, CHAT_OUT_TELL, chat1, chat2);
	//	ClientPackets(CONTENT_CHAT, 127, chat2);
}
