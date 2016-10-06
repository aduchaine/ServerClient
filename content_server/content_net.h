// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W

#ifndef CONTENT_NET_H
#define CONTENT_NET_H


#pragma once

#include "stdafx.h"

#include <vector>

#include "servernetwork.h"


struct ChatMessageTime_struct
{
	int socket_pos;			// "socket_pos" is "socket_position" or client id
	int time_value;			// "time_value" is the inserted "static_cast<int>(time(0))" value
	int is_probation;		// "is_probation" identifies the entry - 1 = probation entry, 0 = not probation
};

class ContentNet
{

public:

	void ReceiveContentPackets();

	void ContentPackets(int packet_type, int clienttalk_opvalue, int client_socket, int socket_position, std::string data_pos1 = "", std::string data_pos2 = ""); // new
	void SendServerPackets(int packet_type, int status_value, std::string data_pos1 = "", std::string data_pos2 = "");
	void SendClientPacketErrorHandling(int status_value, int client_socket, int socket_position); // for sending errors but could probably be expanded to recv

	int UserValidation(int acct_id, int validation_code);
	bool FindValidation(int acct_id, int validation_num);
	void RemoveValidation(int acct_id);

	// new test - message counters - try to reduce some of these or combine them
	void ProcessMessageCounters();
	void DoUserMessCounter(int socket_position, int counter, bool found, bool remove);
	void RemoveChatProbation(int socket_position);
	void AddMessageCounters(int socket_position, int current_time, bool login);
	void RemoveUserFromChatFootprint(int socket_position);
	bool FindUserChatFootprint(int socket_position);
	bool CheckMessageCounter(int socket_position, int current_time);
	std::list<ChatMessageTime_struct> mess_time_list;

	void DisableConNetTimers(); // disables all ConNet timers
	void DisableConnWaitTimer(); // disables only the ServerConnWaitTimer
	void StartConNetTimers();
	void StartChatIdleTimer();
	void ProcessConNetTimers();	
	bool IsChatTimerEnabled();

};

extern ContentNet ConNet;

extern std::vector<unsigned int> socket_pos;

#endif
