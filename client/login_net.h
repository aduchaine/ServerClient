/*
3/31/16

login_net.h
*/

#ifndef LOGIN_NET_H
#define LOGIN_NET_H


#pragma once

#include "stdafx.h"

#include "clientnetwork.h"


class LoginNet
{

public:

	bool InitializeLoginLoop();

	void LoginUpdate();

	void SendPacketErrorHandling(int clienttalk_opvalue);

	void DisableLogNetTimers();

};

extern LoginNet LogNet;

#endif