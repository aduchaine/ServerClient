// content_server->cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "content_net.h"
#include "content_packet.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"
#include "timers.h"

Account Acct;
Database DB;
GenProgram GProg;
MakeFile File;

ContentNet ConNet;
ServerNetwork Net;

void TryConnectContent();
void ContentLoop();

int connect_tries = 0;
int content_loops = 0;

Timers ServerConnectionTimer(RESTART_CONN_TIMER, true);


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "BEGIN Content Server main-- " << std::endl;

	ServerConnectionTimer.StartTimer(RESTART_CONN_TIMER);

	GProg.Enter();

	while (restart_conn == true) {

		TryConnectContent();

		while (connected == true) {
			++content_loops;

			ContentLoop();

			/*
			if (content_loops == 500) {
				//std::cout << "FindUserAccount1 " << std::endl;
				Net.FindUserAccount();
			}
			*/

			Sleep(50);
		}		
	}

	Net.CloseAllClientConnections(); // not needed

	GProg.Exit();

	std::cout << "\n\t\t    -----PUSH ENTER TO END SERVER-----" << std::endl;
	char end[256];
	fgets(end, 255, stdin);

	return 0;
}

void ContentLoop()
{
	ConNet.ProcessConNetTimers();
	Net.DirectIncomingConnections();
	ConNet.ReceiveContentPackets();
}

void TryConnectContent()
{
	if (connect_tries >= 5) {
		restart_conn = false;
		return;
	}
	if (ServerConnectionTimer.IsEnabled() == false) {
		ServerConnectionTimer.StartTimer(RESTART_CONN_TIMER);
		//std::cout << " ServerConnectionTimer was disabled, enabling " << std::endl; // for heavy debugging
	}
	if (ServerConnectionTimer.CheckTimer(RESTART_CONN_TIMER) == true) {
		if (Net.InitializeConnection() == false) {
			++connect_tries;
			//std::cout << " ServerConnectionTimer - connection failed - tries = " << connect_tries << std::endl; // for heavy debugging
		}
		else {
			connect_tries = 0;
			ServerConnectionTimer.DisableTimer();			
			//std::cout << " ServerConnectionTimer - connection success " << std::endl; // for heavy debugging
		}
	}
}
