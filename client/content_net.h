// first try -> followed the directions from the link below exactly - the commented out sections are what I added
// courtesy of http://www.codeproject.com/Articles/412511/Simple-client-server-network-using-Cplusplus-and-W

#ifndef CONTENT_NET_H
#define CONTENT_NET_H


#pragma once

#include "stdafx.h"

#include "clientnetwork.h"


class ContentNet
{

public:

	void InitializeContentLoop();

	void ContentUpdate();

	bool MailConfirmListBookends(int response_status, int size_data, int data_pos1);
	int CalculateMailDataCategory(int response_status_value);

	void DisableConNetTimers();
	void StartConNetTimers();
	void ProcessConNetTimers();

};

extern ContentNet ConNet;

extern unsigned int mail_list_counter;

#endif
