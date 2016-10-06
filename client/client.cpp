// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "content_net.h"
#include "login_net.h"

#include "login_packet.h"

#include "content.h"
#include "gen_program.h"
#include "login.h"
#include "text_output.h"

#include "conio.h"

GenProgram GProg;
Login Log;
Content Cont;
TextOutput Text;

LoginNet LogNet;
ContentNet ConNet;
ClientNetwork Net;

void LoginLoop();
void ContentLoop();

int login_loops = 0;
int content_loops = 0;


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "BEGIN main-- " << std::endl;

	std::cout << "\n\t\t   -----PUSH ENTER TO START CLIENT-----" << std::endl;
	char start[256];
	fgets(start, 255, stdin);

	LogNet.DisableLogNetTimers();
	ConNet.DisableConNetTimers();
	
	while (done == false) {

		LogNet.InitializeLoginLoop();		

		while (connected == true) {
			LoginLoop();	
		}

		if (logged == true) {
			Sleep(500);
			ConNet.InitializeContentLoop();
		}

		while (logged == true) {
			ContentLoop();
		}	
	}
	ShutdownConnection(Net.ConnectSocket); // probably not necessary

	std::cout << "\n\t\t     -----PUSH ENTER TO END CLIENT-----" << std::endl;
	char end[256];
	fgets(end, 255, stdin);

	return 0;
}

void LoginLoop()
{
	++login_loops;

	LogNet.LoginUpdate();
}

void ContentLoop()
{
	++content_loops;

	ConNet.ProcessConNetTimers();
	ConNet.ContentUpdate();

	Sleep(50);
}

//		testing for new packets
/*bool InitiateInput2(char c) // for testing
{
	if (_kbhit()) {
		c = _getch();
		return true;
	}
	return false;
} */

/*	char key_press = ' ';
	Net.InitializeContentConnection();
	while (done == false) { // test

		if (InitiateInput2(key_press) == true) {
			Net.DoTestPackets();
		}
	} */

/*
#include "ad_icon.h" 

CLIENT_AD_ICON ICON ad_icon.ico
*/
