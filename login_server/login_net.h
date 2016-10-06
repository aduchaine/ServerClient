/*
3/31/16

login_net.h
*/

#ifndef LOGIN_NET_H
#define LOGIN_NET_H

#pragma once

#include "stdafx.h"

#include <vector>
#include <map>

#include "servernetwork.h"


enum ServerPacketCategory {

	LOGIN_EMPTY = 1,
	LOGIN_1VAL = 2,
	LOGIN_2VAL = 3,
	CONTENT_EMPTY = 4,
	CONTENT_NUMBER = 5,
	CONTENT_1VAL = 6,
	CONTENT_2VAL = 7,
	CONTENT_CHAT = 8,

};

class LoginNet
{

public:

	// ----- testing new tcp protocol with one packet function send rather than several
	void LoginPackets(int packet_type, int clienttalk_opvalue, int client_socket, int socket_position, std::string data_pos1 = "", std::string data_pos2 = "");
	void SendServerPacketErrorHandling(int servertalk_opvalue); // for server send errors
	void SendClientPacketErrorHandling(int clienttalk_opvalue, int client_socket, int socket_position); // for client send errors
	void DoClientDualLogin(int account_id, int clienttalk_opvalue);

	// packet receiving
	void ReceiveLoginPackets();
	void ReceiveContentCommPackets();

	// general/utility	
	bool FindReservedName(std::string chosen_name);
	std::string GetReservedName(int socket_position);
	void RemoveReservedName(int socket_position);
	void ReserveAccountID(int socket_position);
	int GetReservedAcctID(int socket_position);
	void RemoveReservedAcctID(int socket_position);

	void DoBadLogAccounts();
	void CompareAccountLock(int p_acctID, bool admin);
	void ProcessLogNetTimers();
	void DisableLogNetTimers(); // used at startup for timers which should not start right away
	void StartLogNetTimers();

	bool CheckClientVersion(std::string client_version);

	// test/unfinished - may not need, current protocol works fine
//	void LoginHeaderPackets(struct LoginPacket_header_struct, char buffer[8], unsigned int size_length, unsigned int clienttalk_opvalue);
//	void LoginHeaderPackets(unsigned int size_length, unsigned int clienttalk_opvalue);
	void DoTestPackets();

};

extern LoginNet LogNet;

extern std::map<int, int> new_acctID_map;
extern std::vector<int> content_acct_ids;
extern std::map<int, int> DBlogged_accts_map;
extern std::map<int, int> locked_accts_map;
extern std::vector<unsigned int> socket_pos;

#endif
