/*
3/12/16

account.cpp
*/

#include "stdafx.h"

#include "client_packet.h"
#include "login_net.h"
#include "login_packet.h"

#include "login.h"
#include "gen_program.h"
#include "text_output.h"


std::string p_input;									// input passed from keylogging

std::string name = name.c_str();						// <name> is the input line from the user
std::string name2 = name2.c_str();						// <name2> is the second input line from the user for new accounts
std::string m_name = m_name.c_str();					// <m_name> is modified <name> (all lowercase) is assigned to the variable below after it checks out with server
extern std::string pm_name = pm_name.c_str();			// <pm_name> is permanent-modified name stored during client session - this will change if a different name is assigned

// <p_password> as a function variable is "passed password"
std::string password = password.c_str();				// <password> is the input line from the user
std::string password2 = password2.c_str();				// <password2> is the second input line from the user for new accounts and PW change options
extern std::string pu_password = pu_password.c_str();	// <pu_password> is permanent-user password stored during client sessiona and assigned from one of the above inputs, depending on circumstance

std::string dual = dual.c_str();						// <dual> is the input line from the user for the dual_login prompt


int Login::DirectLogin(int menu_status, int login_status, std::string p_prompt)
{
	std::string m_prompt = " ";

	m_prompt = GProg.CheckPromptInput(p_prompt);

	if (m_prompt == "old" && login_status != CHANGE_TO_ENTRY) {
		login_status = CHECK_NAME_OLD;
	}
	else if (m_prompt == "new" && login_status != CHANGE_TO_ENTRY) {
		login_status = CHECK_NAME_NEW_VALIDITY;
	}
	else if (m_prompt == "password" && login_status != CHANGE_TO_ENTRY) {
		login_status = CHECK_NAME_PW_CHG;
	}
	else if (m_prompt == "back" && login_status != CHANGE_TO_ENTRY && login_status != PROCEED) {
		if (login_status == CHECK_PASSWORD_OLD) {
			login_status = CHECK_NAME_OLD;
		}
		else if (login_status == CHECK_PASSWORD_NEW_VALIDITY) {
			login_status = CHECK_NAME_NEW_VALIDITY;
		}
		else if (login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
			login_status = CHECK_NAME_PW_CHG;
		}
		else if (login_status == CHECK_PASSWORD_PW_CHG_VALIDITY) {
			login_status = CHECK_NAME_PW_CHG;
		}
		else {
			login_status = CHANGE_TO_LOGINMENU;
		}
	}
	else if (m_prompt == "login" && login_status == CHANGE_TO_ENTRY) {
		login_status = CHECK_CLIENT_VERSION;
	}
	else if (m_prompt == "logout" && login_status == PROCEED) {
		login_status = LOGOUT_TO_LOGIN;
	}
	else if (m_prompt == "menu" && login_status == PROCEED) {
		login_status = PROCEED;
	}
	else if (m_prompt == "continue" && login_status == PROCEED) {
		login_status = CONTINUE;
	}
	else if (m_prompt == "proceed") {
		if (login_status == DUAL_LOG_OLD) {
			login_status = DUAL_PROCEED_OLD;
		}
		else if (login_status == DUAL_LOG_PW_CHG) {
			login_status = DUAL_PROCEED_PW_CHG;
		}
		else {
			login_status = NONE;
		}
	}
	else if (m_prompt == "lock") {
		if (login_status == DUAL_LOG_OLD) {
			login_status = DUAL_LOCK_OLD;
		}
		else if (login_status == DUAL_LOG_PW_CHG) {
			login_status = DUAL_LOCK_PW_CHG;
		}
		else {
			login_status = NONE;
		}
	}	
	else if (m_prompt == "test") { // for testing, remove/comment out to disable this option
		login_status = TEST_PACKETS;
	}
	else if (m_prompt == "quit") {
		login_status = QUIT;
	}
	else if (m_prompt == "#admin") {
		login_status = CHANGE_TO_ADMIN;
	}
	else {
		login_status = NONE;
	}
	return login_status;
}

// this is used to connect to the server by typing the correct menu prompt, the client remains disconnected otherwise
int Login::InitialLogin(int login_status)
{
	int result = -1;
	std::string p_menu = " ";

	std::getline(std::cin, p_menu);

	login_status = DirectLogin(0, login_status, p_menu);

	if (login_status == NONE) {
		login_status = INVALID_INPUT;
	}
	else if (login_status == QUIT) {
		login_status = NONE;
	}	
	else { // one of the other menu items which makes a request of the server
		result = Net.InitializeLoginConnection(); // was CHANGE_TO_LOGINMENU when result == true
	}

	if (result == 0) { // do quit
		login_status = NONE;
	}	
	return login_status;	
}

void Login::ProcessLogin(int menu_status, int login_status)
{
	GProg.ProcessUserInput(false, false);

	if (do_process == false) {
		return;
	}

	LoginMenu(menu_status, login_status);
	GoOn(menu_status, login_status);
	AdminMenu(menu_status, login_status);
	AdminLogin(menu_status, login_status);
	ExistingAccountLogin(menu_status, login_status);
	NewAccountLogin(menu_status, login_status);
	ChangePasswordLogin(menu_status, login_status);
}

void Login::LoginMenu(int menu_status, int login_status)
{
	if (menu_status != LOGIN_MENU) {
		return;
	}

	typing = false;
	do_process = false;
	
	login_status = DirectLogin(menu_status, login_status, p_input);
	if (login_status == NONE) {
		login_status = INVALID_INPUT;
	}
	Net.ClientPackets(LOGIN_EMPTY, login_status);
}

void Login::GoOn(int menu_status, int login_status)
{
	if (menu_status != PROCEED_MENU) {
		return;
	}

	typing = false;
	do_process = false;
	
	login_status = Log.DirectLogin(menu_status, login_status, p_input);
	if (login_status == NONE) {
		login_status = INVALID_INPUT2;
	}
	Net.ClientPackets(LOGIN_EMPTY, login_status);
}

void Login::ExistingAccountLogin(int menu_status, int login_status)
{
	if (menu_status != EXISTING_ACCOUNT) {
		return;
	}

	typing = false;
	do_process = false;

	if (login_status == PASSWORD_LOCKED || login_status == LOG_LOCKED || login_status == SECURITY_LOCKED) {
		login_status = CHANGE_TO_ENTRY;
	}

	if (login_status == CHECK_NAME_OLD) {
		name = p_input;

		login_status = DirectLogin(menu_status, login_status, name);
		if (login_status == NONE) {
			login_status = CHECK_NAME_OLD;
			m_name = GProg.CheckPromptInput(name);
			pm_name.assign(m_name);
			Net.ClientPackets(LOGIN_1VAL, CHECK_NAME_OLD, m_name);
			return;
		}
	}
	else if (login_status == CHECK_PASSWORD_OLD) {
		password = p_input;
		mask_input = false;

		login_status = DirectLogin(menu_status, login_status, password);
		if (login_status == NONE) {
			login_status = CHECK_PASSWORD_OLD;
			pu_password.assign(password);			
			Net.ClientPackets(LOGIN_1VAL, CHECK_PASSWORD_OLD, password);
			return;
		}	
	}
	else if (login_status == DUAL_LOG_OLD) {
		dual = p_input;

		login_status = DirectLogin(menu_status, login_status, dual);
		if (login_status == NONE) {
			login_status = INVALID_INPUT3;
		}
	}
	Net.ClientPackets(LOGIN_EMPTY, login_status);
}

void Login::NewAccountLogin(int menu_status, int login_status)
{
	if (menu_status != NEW_ACCOUNT) {
		return;
	}

	typing = false;
	do_process = false;

	if (login_status == CHECK_NAME_NEW_VALIDITY) {		
		name = p_input;

		login_status = DirectLogin(menu_status, login_status, name);
		if (login_status == NONE) {
			login_stat = CHECK_NAME_NEW_VALIDITY2;
			Text.NonPacketTextMessage(CHECK_NAME_NEW_VALIDITY2, p_input);
			lock_input = false;
			return;
		}
	}	
	else if (login_status == CHECK_NAME_NEW_VALIDITY2) {				
		name2 = p_input;

		login_status = DirectLogin(menu_status, login_status, name2);
		if (login_status == NONE) {
			if (name2 != name) {
				login_status = INVALID_NAME_NEW_NOMATCH;
			}
			else if (GProg.IsValidInput(name2, false, false, true, NO_CHAR, NO_CHAR, 4, 12) == 2) {
				login_status = INVALID_NAME_LENGTH;
			}
			else {
				login_status = CHECK_NAME_NEW_VALIDITY;
				m_name = GProg.CheckPromptInput(name2);
				pm_name.assign(m_name);
				Net.ClientPackets(LOGIN_1VAL, CHECK_NAME_NEW_VALIDITY, m_name);
				return;
			}
		}
	}
	else if (login_status == CHECK_PASSWORD_NEW_VALIDITY) {
		password = p_input;
		mask_input = false;

		login_status = DirectLogin(menu_status, login_status, password);
		if (login_status == NONE) {
			login_stat = CHECK_PASSWORD_NEW_VALIDITY2;
			Text.NonPacketTextMessage(CHECK_PASSWORD_NEW_VALIDITY2, p_input);
			mask_input = true;
			lock_input = false;
			return;
		}
	}
	else if (login_status = CHECK_PASSWORD_NEW_VALIDITY2) {
		password2 = p_input;
		mask_input = false;

		login_status = DirectLogin(menu_status, login_status, password2);
		if (login_status == NONE) {
			if (password2 != password) {
				login_status = INVALID_PASSWORD_NEW_NOMATCH;
			}
			else if (GProg.IsValidInput(password2, false, false, true, NO_CHAR, NO_CHAR, 4, 12) == 2) {
				login_status = INVALID_PASSWORD_NEW_LENGTH;
			}
			else {
				login_status = CHECK_PASSWORD_NEW_VALIDITY;
				pu_password.assign(password2);
				Net.ClientPackets(LOGIN_1VAL, CHECK_PASSWORD_NEW_VALIDITY, password2);
				return;
			}
		}
	}
	Net.ClientPackets(LOGIN_EMPTY, login_status);
}

void Login::ChangePasswordLogin(int menu_status, int login_status)
{
	if (menu_status != CHNG_PASS_ACCOUNT) {
		return;
	}

	typing = false;
	do_process = false;

	if (login_status == PASSWORD_LOCKED) { // probably not needed
		login_status = CHANGE_TO_ENTRY;
	}

	if (login_status == CHECK_NAME_PW_CHG) {
		name = p_input;		

		login_status = DirectLogin(menu_status, login_status, name);
		if (login_status == NONE) {
			login_status = CHECK_NAME_PW_CHG;
			m_name = GProg.CheckPromptInput(name);
			pm_name.assign(m_name);
			Net.ClientPackets(LOGIN_1VAL, CHECK_NAME_PW_CHG, m_name);
			return;
		}
	}
	else if (login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
		password = p_input;
		mask_input = false;		

		login_status = DirectLogin(menu_status, login_status, password);
		if (login_status == NONE) {
			login_stat = CHECK_PASSWORD_PW_CHG_LOGIN2;
			Text.NonPacketTextMessage(CHECK_PASSWORD_PW_CHG_LOGIN2, p_input);
			mask_input = true;
			lock_input = false;
			return;
		}
	}
	else if (login_status == CHECK_PASSWORD_PW_CHG_LOGIN2) {
		password2 = p_input;
		mask_input = false;

		login_status = DirectLogin(menu_status, login_status, password2);
		if (login_status == NONE) {
			if (password2 != password) {
				login_status = INVALID_PASSWORD_PW_LOGIN_NOMATCH;
			}
			else {
				login_status = CHECK_PASSWORD_PW_CHG_LOGIN;
				Net.ClientPackets(LOGIN_1VAL, CHECK_PASSWORD_PW_CHG_LOGIN, password2);
				return;
			}
		}
	}
	else if (login_status == CHECK_PASSWORD_PW_CHG_VALIDITY) {
		password = p_input;
		mask_input = false;

		login_status = DirectLogin(menu_status, login_status, password);
		if (login_status == NONE) {
			login_stat = CHECK_PASSWORD_PW_CHG_VALIDITY2;
			Text.NonPacketTextMessage(CHECK_PASSWORD_PW_CHG_VALIDITY2, p_input);
			mask_input = true;
			lock_input = false;
			return;
		}
	}
	else if (login_status == CHECK_PASSWORD_PW_CHG_VALIDITY2) {
		password2 = p_input;
		mask_input = false;

		login_status = DirectLogin(menu_status, login_status, password2);
		if (login_status == NONE) {
			if (password2 != password) {
				login_status = INVALID_PASSWORD_PW_CHG_NOMATCH;
			}
			else if (GProg.IsValidInput(password2, false, false, true, NO_CHAR, NO_CHAR, 4, 12) == 2) {
				login_status = INVALID_PASSWORD_PW_CHG_LENGTH;
			}
			else {
				login_status = CHECK_PASSWORD_PW_CHG_RECORD;
				pu_password.assign(password2);
				Net.ClientPackets(LOGIN_1VAL, CHECK_PASSWORD_PW_CHG_RECORD, password2);
				return;
			}
		}
	}
	else if (login_status == DUAL_LOG_PW_CHG) {
		dual = p_input;

		login_status = DirectLogin(menu_status, login_status, dual);
		if (login_status == NONE) {
			login_status = INVALID_INPUT4;
		}
	}
	Net.ClientPackets(LOGIN_EMPTY, login_status);
}

int Login::DirectAdmin(int admin_status, std::string p_prompt)
{
	std::string m_prompt = " ";

	m_prompt = GProg.CheckPromptInput(p_prompt);
	if (admin_status == ADMIN_LOG_SUCCESS) {
		if (m_prompt == "shutdown") {
			admin_status = SHUTDOWN;
		}
		else if (m_prompt == "quit" || m_prompt == "logout") {
			admin_status = ADMIN_LOGOUT;
		}
		else if (m_prompt == "back" || m_prompt == "menu") {
			admin_status = TO_ADMINMENU;
		}
		else if (m_prompt == "log settings") {
			admin_status = TO_ADMINMENU; // placeholder
		}
		else {
			admin_status = NONE;
		}
	}
	else if (m_prompt == "back" || m_prompt == "quit") {
		admin_status = CHANGE_TO_ENTRY;
	}
	else if (m_prompt == "proceed") {
		if (admin_status == DUAL_LOG_ADMIN) {
			admin_status = DUAL_PROCEED_ADMIN; // admin 2nd account is logged in
		}
		else {
			admin_status = DUAL_PROCEED_ADMIN2; // user 2nd account is logged in
		}
	}
	else if (m_prompt == "lock" && admin_status == DUAL_LOG_ADMIN) {
		admin_status = DUAL_LOCK_ADMIN;
	}
	else {
		admin_status = NONE;
	}
	return admin_status;
}

void Login::AdminMenu(int menu_status, int login_status)
{
	if (menu_status != ADMIN_MENU) {
		return;
	}

	typing = false;
	do_process = false;

	login_status = DirectAdmin(login_status, p_input);
	if (login_status == NONE) {
		login_status = ADMIN_INVALID_ENTRY;
	}
	Net.ClientPackets(LOGIN_EMPTY, login_status);
}

void Login::AdminLogin(int menu_status, int admin_status)
{
	if (menu_status != ADMIN_ACCOUNT) {
		return;
	}

	typing = false;
	do_process = false;

	if (admin_status == ADMIN_LOGIN) {
		name = p_input;
		mask_input = false;

		admin_status = DirectAdmin(admin_status, name);
		if (admin_status == NONE) {
			admin_status = ADMIN_LOGIN;
			m_name = GProg.CheckPromptInput(name);
			pm_name.assign(m_name);
			Net.ClientPackets(LOGIN_1VAL, ADMIN_LOGIN, m_name);
			return;
		}
	}
	else if (admin_status == ADMIN_PW_LOGIN) { // verify credentials	
		password = p_input;
		mask_input = false;

		admin_status = DirectAdmin(admin_status, password);
		if (admin_status == NONE) {
			admin_status = ADMIN_PW_LOGIN;
			pu_password.assign(password);
			Net.ClientPackets(LOGIN_1VAL, ADMIN_PW_LOGIN, password);
			return;
		}
	}
	else if (admin_status == DUAL_LOG_ADMIN) {
		dual = p_input;

		admin_status = DirectAdmin(admin_status, dual);
		if (admin_status == NONE) {
			admin_status = INVALID_INPUT5;
		}
	}
	else if (admin_status == ADMIN_USER_ACCT_CONFLICT) {
		dual = p_input;

		admin_status = DirectAdmin(admin_status, dual);
		if (admin_status == NONE) {
			admin_status = INVALID_INPUT5;
		}
	}
	Net.ClientPackets(LOGIN_EMPTY, admin_status);
}
