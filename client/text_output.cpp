/*
3/30/16

text_output.cpp
*/

#include "stdafx.h"

#include "client_packet.h"
#include "content_packet.h"
#include "login_packet.h"

#include "text_output.h"
#include "gen_program.h"


using namespace std::chrono;


void TextOutput::ClientMessage(int client_status, std::string p_name)
{
	if (client_status == INVALID_INPUT) {
		std::cout << "\n\nType your selection from the menu above and press ENTER to continue." << std::endl;	
	}
	if (client_status == QUIT) {
		std::cout << "\n\tGoodbye" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));//2
	}
	if (client_status == TO_LOGIN) {
		std::cout << "\n\n\tLogout successful.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));//3
	}
	if (client_status == SERVER_DISCONNECT) {
		std::cout << "\n\n   The connection to the server has been lost." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (client_status == CLIENT_DC_INACTIVE) {
		std::cout << "\n    You have been disconnected from the server for inactivity." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\nRecords will reflect your logout as necessary." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (client_status == FORCE_LOGOUT) {
		std::cout << "\n    You have been logged out." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\nPlease contact the administrator." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (client_status == DB_ERROR) {
		std::cout << "\n\t\t>>>---DATABASE ERROR---<<<\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "    Please contact the administrator.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (client_status == ACCT_LOCKED) {
		std::cout << "\nThe account is locked.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    Please try again later.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (client_status == INVALID_NAME_BADCHAR) {
		std::cout << "\nName (" << p_name << ") invalid." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "\n\tPlease use numbers and/or letters and no spaces.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (client_status == INVALID_NAME_LENGTH) {
		std::cout << "\nName invalid: name should be between 4 and 12 characters in length.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (client_status == INVALID_NAME) {
		std::cout << "\nInvalid name <" << p_name << ">, check the spelling and try again.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (client_status == INVALID_INPUT_BADCHAR) {
		std::cout << "Invalid characters in the input.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (client_status == INVALID_INPUT_LENGTH) {
		std::cout << "Invalid length of input.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (client_status == INVALID_INPUT2) {
		std::cout << "\n\nType \"menu\" or a menu selection and press ENTER to continue.\n" << std::endl;		
	}
}

void TextOutput::LoginMessage(int login_status, std::string p_name)
{
	p_name[0] = toupper(p_name[0]);

	if (login_status == CHECK_NAME_OLD) {
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------new--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-----password-----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t  To change your selection type from above and press ENTER\n" << std::endl;
		std::cout << "  ...Existing Accounts" << std::endl;
		std::cout << "\tEnter your login name:" << std::endl;
	}
	if (login_status == CHECK_NAME_NEW_VALIDITY) {
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------old--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------new--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t  To change your selection type from above and press ENTER\n" << std::endl;
		std::cout << "  ...New accounts" << std::endl;
		std::cout << "\tPlease choose a login name:" << std::endl;
	}
	if (login_status == CHECK_NAME_PW_CHG) {	
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------old--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------new--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t  To change your selection type from above and press ENTER\n" << std::endl;
		std::cout << "  ...Changing Your Password" << std::endl;
		std::cout << "\tEnter your login name:" << std::endl;
	}
	if (login_status == CHECK_NAME_NEW_APPROVE) {
		std::cout << "\n\tLogin name <" << p_name << "> is approved.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}	
	if (login_status == INVALID_NAME_NEW_CONFLICT) {
		std::cout << "\nName (" << p_name << ") exists, please choose a different login name.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}	
	if (login_status == CHECK_PASSWORD_OLD || login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------old--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------new--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t  To change your selection type from above and press ENTER\n" << std::endl;
		if (login_status == CHECK_PASSWORD_OLD) {
			std::cout << "  ...Existing Accounts" << std::endl;
			std::cout << "\tEnter your password:" << std::endl;
		}
		if (login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
			std::cout << "  ...Changing Your Password" << std::endl;
			std::cout << "\tEnter your current password:" << std::endl;
		}
	}
	if (login_status == CHECK_PASSWORD_NEW_VALIDITY) {
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------old--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------new--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t  To change your selection type from above and press ENTER\n" << std::endl;
		std::cout << "       Your password must be between 6 and 12 characters in length..." << std::endl;
		std::cout << "\t\t  and not contain any special characters.\n" << std::endl;
		std::cout << "  ...New accounts" << std::endl;
		std::cout << "\tCreate your password:" << std::endl;
	}
	if (login_status == CHECK_PASSWORD_PW_CHG_VALIDITY) {
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------old--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------new--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t  To change your selection type from above and press ENTER\n" << std::endl;
		std::cout << "       Your password must be between 6 and 12 characters in length..." << std::endl;
		std::cout << "\t\t  and not contain any special characters.\n" << std::endl;
		std::cout << "  ...Changing Your Password" << std::endl;
		std::cout << "\tEnter your new password:" << std::endl;
	}
	if (login_status == INVALID_PASSWORD_OLD || login_status == INVALID_PASSWORD_PW_CHG_LOGIN || login_status == ADMIN_INVALID_PW) {
		std::cout << "\n\nPassword invalid." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (login_status == INVALID_PASSWORD_NEW_BADCHAR || login_status == INVALID_PASSWORD_PW_CHG_BADCHAR) {
		std::cout << "\n\nPassword invalid: please use numbers and/or letters and no spaces.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (login_status == INVALID_PASSWORD_NEW_LENGTH || login_status == INVALID_PASSWORD_PW_CHG_LENGTH) {
		std::cout << "\n\nPassword invalid: password must be between 6 and 12 characters in length.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (login_status == INVALID_PASSWORD_NEW_NOMATCH || login_status == INVALID_PASSWORD_PW_CHG_NOMATCH 
		|| login_status == INVALID_PASSWORD_PW_LOGIN_NOMATCH || login_status == INVALID_NAME_NEW_NOMATCH) {
		std::cout << "\n\nYour first input does not match your second input." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\n\tPlease try again." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (login_status == LOGIN_SUCCESS_NEW) {
		std::cout << "\n\n\tRegistration successful." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (login_status == LOGIN_SUCCESS_PW_CHG) {
		std::cout << "\n\n\tPassword changed successfully.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (login_status == LOGIN_COMPLETE_OLD) {
		std::cout << "\n\nThank you.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\tLogin successful.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "Welcome back " << p_name << ".\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (login_status == LOGIN_COMPLETE_NEW) {
		std::cout << "\n\nThank you.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\tLogin successful.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "Welcome " << p_name << ".\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (login_status == CHANGE_TO_LOGINMENU) {
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------new--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------old--------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-----password-----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}
	if (login_status == CHANGE_TO_ENTRY) {
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------login------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}
	if (login_status == ACCOUNT_LOCK_BEGIN) {
		std::cout << "\n\n     You have three attempts at logging in with an incorrect password.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "\t     You are prevented from logging in for " << LOCKOUT_TIMER / 1000 << " seconds." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}	
	if (login_status == PROCEED) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-----continue-----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------logout------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}	
	if (login_status == ADMIN_LOGIN) {
		std::cout << "\n\n  ...Admin login" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "\tEnter your login name:\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (login_status == ADMIN_PW_LOGIN) {
		std::cout << "\n\n  ...Admin login" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "\tEnter your password:\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (login_status == ADMIN_INVALID_NAME) {
		std::cout << "\n\nNo admin account exists with that name." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}	
	if (login_status == ADMIN_LOG_SUCCESS) {
		std::cout << "\n\n\tLogin successful.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (login_status == TO_ADMINMENU) {
		std::cout << "\n\n";
		std::cout << "\t\t Admin: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-----shutdown-----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------logout------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}
	if (login_status == ADMIN_LOGOUT) {
		std::cout << "\n\tLogging out as administrator.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "\n\t\tReturning to the start menu.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (login_status == ADMIN_INVALID_ENTRY) {
		std::cout << "\nInvalid entry.\n" << std::endl;
	}
	if (login_status == SHUTDOWN) {
		std::cout << "\n   ...server shutdown" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (login_status == DUAL_LOG_OLD || login_status == DUAL_LOG_PW_CHG || login_status == DUAL_LOG_ADMIN) {
		std::cout << "\n\n\tYour account is currently logged in." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\nYou can proceed with login, lock the account, go back to the menu or quit." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(4));
	}
	if (login_status == ADMIN_USER_ACCT_CONFLICT) {
		std::cout << "\n\n\tThe user account associated with this account is currently logged in." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\nYou can proceed with login, lock the account, go back to the menu or quit." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(4));
	}
	if (login_status == DUAL_LOG_OLD || login_status == DUAL_LOG_PW_CHG || login_status == DUAL_LOG_ADMIN || login_status == ADMIN_USER_ACCT_CONFLICT) {
		std::cout << "\n\n";
		std::cout << "\t\t\t ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------proceed-----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------lock-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------quit-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}
	if (login_status == DUAL_LOCK_OLD || login_status == DUAL_LOCK_PW_CHG || login_status == DUAL_LOCK_ADMIN || login_status == DUAL_LOCK_ADMIN2) {
		std::cout << "\n\n   ...removing active session and locking the account." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\nYou will be notified by Telegram to reset your password." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (login_status == USER_ADMIN_ACCT_CONFLICT) {
		std::cout << "\n\n\tUnable to login your account at this time." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\nPlease contact an administrator if the problem persists." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(4));
	}
	if (login_status == PASSWORD_LOCKED) {
		std::cout << "\nThe account is locked due to failed password attempts.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    Please try again later.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (login_status == LOG_LOCKED) {
		std::cout << "\n\nYour account was not logged out properly" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "\tafter your last session and has been locked.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    Pending administrator review, it will unlocked shortly.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "\t    We apologize for the inconvenience.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (login_status == SECURITY_LOCKED) {
		std::cout << "\nPossible security breach in your account.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    The account has been locked and you must contact an administrator.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (login_status == INVALID_CLIENT_VERSION) {
		std::cout << "\nYour client version is out of date.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    Please download the latest version.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
}

void TextOutput::ContentMessage(int content_status, std::string p_name)
{
	p_name[0] = toupper(p_name[0]);

	if (content_status == INVALID_CREDS) {
		std::cout << "\n\t\t\t>>>---SERVER ERROR---<<<\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "    Please contact the administrator.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == VALID_CREDS) {
		std::cout << "\n\nWelcome to the Content Server.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    Your session has begun.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (content_status == CONTENT_GOTO_MENU) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------chat-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;		
		std::cout << "\t\t>>>>>>>>>>>>-------mail-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------logout------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}
	if (content_status == CONTENT_INVALID_INPUT) {
		std::cout << "Type \"menu\" or a menu option and press ENTER to continue." << std::endl;
	}
	if (content_status == LOGOUT_CONTENT_SERVER) {
		std::cout << "\n\t\t..." << p_name << "...\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "...logging out.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (content_status == CHAT_MENU) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-------MENU-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>----begin chat----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>---instructions---<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;		
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}
	if (content_status == CHAT_INSTRUCTIONS) {
		std::cout << "\n\n      ----To type your message or any of the commands press ENTER----" << std::endl;
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 21);
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "You can use any of the menu commands or \"menu\" at any time\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "  Using a command will exit \"chat\" mode.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "  To use the commands type \"/\" plus the command." << std::endl;		
		std::this_thread::sleep_until(system_clock::now() + seconds(3));		
		std::cout << "    Usage:<\'ENTER\'/menu\'ENTER\'(will return to the menu)>\n\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));		
		std::cout << "There are two modes of chat.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "  You can chat to all by pressing ENTER and typing your message." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    Usage:<\'ENTER\'Richard eats babies of between 4-6 months!\'ENTER\'>" << std::endl;
		std::cout << "    Output:<Soandso says: Richard eats babies of between 4-6 months!>\n" << std::endl;				
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "  To send a private message, use \"@\" followed by the name." << std::endl;		
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    Usage:<\'ENTER\'@adrian You are a gentleman and a scholar.\'ENTER\'>" << std::endl;
		std::cout << "    Output:<Soandso tells you: You are a gentleman and a scholar.>\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "  Capital letters are not necessary.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == CHAT_BEGIN) {
		std::cout << "\nFor the menu options, type \"menu\" and press ENTER.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\n\t" << p_name << "...\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "...you have entered an old-school chatroom.\n\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		CHAT_USER_PROMPT;
	}
	if (content_status == CHAT_NO_LISTENERS) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "  -- There is nobody to chat with.\n" << std::endl;
		CHAT_USER_PROMPT;
	}
	if (content_status == CHAT_NO_LIST_RECIPIENT) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "  -- The recipient of your message is not connected.\n" << std::endl;
		CHAT_USER_PROMPT;
	}
	if (content_status == CHAT_USER_JOIN) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "\t\t       --- " << p_name << " has joined chat ---\n" << std::endl;
	}
	if (content_status == CHAT_USER_LEAVE) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "\t\t      --- " << p_name << " has left the room ---\n" << std::endl;
	}
	if (content_status == MESSAGES_EXCEEDED) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "  -- You have exceeded the number of allowed messages in a given timeframe.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "\tPlease wait to send another message.\n\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (content_status == CONTENT_1ST_LOGIN) {
		std::cout << "\n\n	  This is your first login to the Content Server.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\tThank you for joining.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "There are currently two main functions performed by this program." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "  There is a chatroom in the old-school sense..." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "\tand message sending similar to e-mail.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "The chatroom is a place to have live text conversions." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    It is reminiscent of the early days of the internet.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "Message sending has typical e-mail functions" << std::endl;
		std::cout << "\t  which have been around about as long.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "If you want to make the screen larger," << std::endl;		
		std::cout << "  it will elongate vertically by maximizing the window or using the mouse." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    You may also use the mouse to click/drag the scroll bar on the right.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    Please note: this is the only functionality for the mouse.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "The commands \"logout\" and \"quit\" will always work to logout your account." << std::endl;		
		std::cout << "  \"menu\" and \"back\" will work in most cases to go back to a previous menu." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    While in the chatroom, you must use \"/\" before typing any command.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "There are instructions specific to each function" << std::endl;	
		std::cout << "    which can be referenced while using that function..\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    That is all!\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(4));
		std::cout << "\t\t--- Press ENTER to continue ---" << std::endl;
	}
}

void TextOutput::MailMessage(int content_status, std::string p_name, std::string p_message1, std::string p_message2, std::string p_message3, std::string p_message4)
{
	p_name[0] = toupper(p_name[0]);

	if (content_status == MAIL_GOTO_MENU) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-----MAIL MENU----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------mailbox-----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------compose-----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>---instructions---<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;	
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}
	if (content_status == MAIL_BOX) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-----MAIL MENU----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;		
		std::cout << "\t\t>>>>>>>>>>>>------inbox-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------outbox------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------back-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	}
	// unfinished
	if (content_status == MAIL_INSTRUCTIONS) {
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 23);
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "Type \"back\" or \"menu\" at any time to bring you to the general mail menu.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "The mailbox holds all sent and received mail messages.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "  When inside your mailbox, you can choose to read or delete your messages." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    Both options will show a list of your messages ordered numerically." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));		
		std::cout << "    The newest messages are listed first when choosing to read." << std::endl;
		std::cout << "    The oldest messages are listed first when choosing to delete." << std::endl;
		std::cout << "      To select a specific message type the number and press ENTER.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    When accessing sent messages, \"U\" signifies the mail has not been opened" << std::endl;
		std::cout << "\tand \"R\" means the receiver of your mail has opened the message.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "To send a message to someone use the \"compose\" option.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "  You will be prompted for a name, subject and message body." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    Capital letters are not necessary when typing the name." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));				
		std::cout << "    Pressing ENTER will accpet any previous input typed." << std::endl;
		std::cout << "\tUse spaces and just keep typing.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "Editing the same field more than once" << std::endl;
		std::cout << "\twithout accepting will erase the previous edit.\n" << std::endl;
	}
	if (content_status == MAIL_INBOX) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-----MAIL MENU----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------unread------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------" << p_message1 << "------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------read-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------" << p_message2 << "------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------delete------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "The most recent 20 messages will appear from the category chosen," << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    or you can choose to delete inbox messages." << std::endl;
	}
	if (content_status == MAIL_OUTBOX) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-----MAIL MENU----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;		
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------sent-------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------" << p_message1 << "------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------delete------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;		
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "The most recent 20 messages will appear from the sent mailbox," << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    or you can choose to delete outbox messages" << std::endl;
	}
	if (content_status == MAIL_DELETE_INBOX) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-----MAIL MENU----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>---delete unread--<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------" << p_message1 << "------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>----delete read---<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------" << p_message2 << "------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>--delete options--<<<<<<<<<<<<\n" << std::endl;		
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
		std::cout << "  ...Deleting inbox messages\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "The oldest 20 messages will appear from the category chosen," << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    \"Delete options\" will show delete inbox options." << std::endl;
	}
	if (content_status == MAIL_DELETE_OUTBOX) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-----MAIL MENU----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>----delete sent---<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-------" << p_message1 << "------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-delete all outbox<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;	
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
		std::cout << "  ...Deleting outbox messages\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "The oldest 20 messages will appear from sent messages," << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    or you can choose to delete all outbox messages" << std::endl;
	}
	if (content_status == MAIL_DELETE_ALL_OPTIONS) {
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-----MAIL MENU----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-delete all unread<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>--delete all read-<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-delete all inbox-<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
		std::cout << "  ...Deleting inbox messages\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    To exit this menu type \"back\"." << std::endl;
	}
	if (content_status == MAIL_DELETE_READ || content_status == MAIL_DELETE_UNREAD || content_status == MAIL_INBOX_READ || content_status == MAIL_INBOX_UNREAD) {
		std::cout << "\n" << p_message1 << ") [" << p_message2 << "] from:" << p_message3 << " subject:[" << p_message4 << "]";
	
	}
	if (content_status == MAIL_DELETE_SENT || content_status == MAIL_OUTBOX_SENT) {
		if (p_name == "1" || p_name == "5") {
			std::cout << "\n" << p_message1 << ") R [" << p_message2 << "] to:" << p_message3 << " subject:[" << p_message4 << "]";
		}
		else {
			std::cout << "\n" << p_message1 << ") U [" << p_message2 << "] to:" << p_message3 << " subject:[" << p_message4 << "]";
		}	
	}
	if (content_status == MAIL_DELETE_INDV_SUCCESS) {
		std::cout << "\n    Your message was successfully deleted.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == MAIL_DELETE_ALL_READ || content_status == MAIL_DELETE_ALL_UNREAD || content_status == MAIL_DELETE_ALL_OUTBOX) {
		std::cout << "\n -- " << p_message1 << " messages were successfully deleted.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == MAIL_DELETE_ALL_INBOX) {
		std::cout << "\n -- " << p_message1 << " unread and " << p_message2 << " read messages were successfully deleted.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == MAIL_DELETE_FAIL) {
		std::cout << "\n    Your message(s) FAILED to be deleted.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "\nPlease contact the administrator." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (content_status == MAIL_LIST_OOR) {
		std::cout << "\n    Your selection was out of range.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "\nPlease type a number from 1 to 20 shown in the list." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (content_status == MAIL_READ_FAIL) {
		std::cout << "\n    Your message(s) could not be accessed.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "\nPlease contact the administrator." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (content_status == MAIL_READ_RECV_MESSAGE || content_status == MAIL_READ_SENT_MESSAGE) {
		p_message2[0] = toupper(p_message2[0]);
		std::cout << "\n\n";
		std::cout << "\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 21);
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    Receipt: " << p_message1 << std::endl;
		if (content_status == MAIL_READ_RECV_MESSAGE) {
			std::cout << "    From: " << p_message2 << std::endl;
		}
		else {
			std::cout << "    To: " << p_message2 << std::endl;
		}
		std::cout << "    Subject: " << p_message3 << "\n" << std::endl;
		std::cout << "\t    --- Begin message\n" << std::endl;
		std::cout << p_message4 << std::endl;
		std::cout << "\n\t    --- End message" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		if (content_status == MAIL_READ_RECV_MESSAGE) {
			std::cout << "\nType \"reply\" to compose a reply to the person who sent you this message.";
		}
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (content_status == MAIL_TO_SEND_RECIPIENT) {
		std::cout << "Enter the name of the message recipient:\n" << std::endl;	
	}
	if (content_status == MAIL_TO_SEND_SUBJECT) {
		std::cout << "    The subject line can be up to 25 characters in length...\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "Enter the subject line:\n" << std::endl;		
	}
	if (content_status == MAIL_TO_SEND_BODY) {
		std::cout << "Compose your message:\n" << std::endl;
	}
	if (content_status == MAIL_TO_SEND_REVIEW) {		
		std::cout << "\n\n\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\t\t>>>>>>>>>>>>-----MAIL MENU----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>-----edit name----<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>----edit subject--<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>----edit message--<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------------------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t>>>>>>>>>>>>------review------<<<<<<<<<<<<\n" << std::endl;
		std::cout << "\t\t    Type your selection and press ENTER\n" << std::endl;
		std::cout << "  ...Editing your message\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "To abort sending type \"back\", to check your message type \"review\"," << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    or select \"accept\" to save previous edits and review your updated message." << std::endl;
	}
	if (content_status == MAIL_REVIEW_MESSAGE) {
		p_message1[0] = toupper(p_message1[0]);		
		std::cout << "\n\n\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 21);
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    To: " << p_message1 << std::endl;
		std::cout << "    Subject: " << p_message2 << "\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\t    --- Begin message\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << p_message3 << std::endl;
		std::cout << "\n\t    --- End message\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "To send your message type \"send\"," << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    or to edit your message type \"edit\" to go to the editing menu." << std::endl;
	}
	if (content_status == MAIL_EDIT_RECIPIENT) {
		std::cout << "    To: " << p_message1 << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "\nType the name of the message recipient and press ENTER:\n" << std::endl;
	}	
	if (content_status == MAIL_EDIT_SUBJECT) {
		std::cout << "    Subject: " << p_message1 << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "\n    The subject line can be up to 25 characters in length...\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "Type the subject line and press ENTER:\n" << std::endl;
	}
	if (content_status == MAIL_EDIT_BODY) {
		std::cout << "\t    --- Begin current message\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << p_message1 << std::endl;
		std::cout << "\n\t    --- End current message\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "Type your message and press ENTER:\n" << std::endl;
	}
	if (content_status == MAIL_TO_SEND_SUCCESS) {
		std::cout << "\n    Your message was sent successfully.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == MAIL_TO_SEND_FAIL) {
		std::cout << "\n    Your message FAILED to be sent.\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "\nPlease contact the administrator." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (content_status == MAIL_NO_NEXT_RECORDS) {
		std::cout << "\n    You have reached the end of the mail records.";
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == MAIL_NO_PREVIOUS_RECORDS) {
		std::cout << "\n    You cannot go before the beginning.";
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == MAIL_MAILBOX_EMPTY) {
		std::cout << "\n    There are no messages in this mailbox.";
		std::this_thread::sleep_until(system_clock::now() + seconds(3));
	}
	if (content_status == MAIL_REVIEW_EDIT) {
		p_message1[0] = toupper(p_message1[0]);
		std::cout << "\n\n\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 21);
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    To: " << p_message1 << std::endl;
		std::cout << "    Subject: " << p_message2 << "\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\t    --- Begin message\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << p_message3 << std::endl;
		std::cout << "\n\t    --- End message\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "To accept your changes and send your message type \"send\"," << std::endl;		
		std::cout << "\tor to revert and view the previous message type \"revert\".\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "To make further edits type \"edit\" to go back to the editing menu," << std::endl;
		std::cout << "\tor to accept your edit and view your message type \"accept\".\n" << std::endl;		
	}
	if (content_status == MAIL_TO_SEND_REPLY) {		
		std::cout << "\t    --- Begin message\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));		
		std::cout << p_message1 << std::endl;
		std::cout << "\t    --- End message\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "Type your reply and press ENTER:\n" << std::endl;
	}
	if (content_status == MAIL_REVIEW_REPLY) { // == MAIL_REVIEW_MESSAGE
		p_message1[0] = toupper(p_message1[0]);
		std::cout << "\n\n\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 21);
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    To: " << p_message1 << std::endl;
		std::cout << "    Subject: " << p_message2 << "\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));		
		std::cout << p_message3 << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\t    --- Begin current reply\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));		
		std::cout << p_message4 << std::endl;
		std::cout << "\n\t    --- End current reply\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "To send your reply type \"send\"," << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "    or to edit your reply message type \"edit\"." << std::endl;
	}
	if (content_status == MAIL_EDIT_REPLY) {		
		std::cout << p_message1 << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\n\t    --- Begin current reply\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << p_message2 << std::endl;
		std::cout << "\n\t    --- End current reply\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "Type your message and press ENTER:\n" << std::endl;
	}
	if (content_status == MAIL_REVIEW_REPLY_EDIT) {
		p_message1[0] = toupper(p_message1[0]);
		std::cout << "\n\n\t\t User: " << p_name << " - ";
		GProg.DisplayTime();
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 21);
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
		std::cout << "    To: " << p_message1 << std::endl;
		std::cout << "    Subject: " << p_message2 << "\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));		
		std::cout << p_message3 << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\n\t    --- Begin current reply\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(1));		
		std::cout << p_message4 << std::endl;
		std::cout << "\n\t    --- End current reply\n" << std::endl;
		//	std::this_thread::sleep_until(system_clock::now() + seconds(3));
		std::cout << "To accept your changes and send your message type \"send\"," << std::endl;
		std::cout << "\tor to revert and view the previous message type \"revert\".\n" << std::endl;		
	}
}

void TextOutput::NonPacketTextMessage(int client_status, std::string p_message)
{
	if (client_status == CHECK_NAME_NEW_VALIDITY2) {
		std::cout << "\n\t\tPlease re-enter your user name to continue.\n" << std::endl;
	}
	if (client_status == CHECK_PASSWORD_NEW_VALIDITY2 || client_status == CHECK_PASSWORD_PW_CHG_LOGIN2 || client_status == CHECK_PASSWORD_PW_CHG_VALIDITY2) {
		std::cout << "\n\t\tPlease re-enter your password to continue.\n" << std::endl;
	}
	if (client_status == CONTENT_INVALID_INPUT2) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "Press ENTER to type your message. Use \"/instructions\" or \"/menu\" if needed.\n" << std::endl;
		CHAT_USER_PROMPT;
	}
	if (client_status == CHAT_SELF_TALK) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "  -- Do you enjoy talking to yourself?\n" << std::endl;
		CHAT_USER_PROMPT;
	}
	if (client_status == CHAT_INCHAT) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "  -- You cannot join what you are already a part of.\n" << std::endl;
		CHAT_USER_PROMPT;
	}
	if (client_status == CHAT_NO_MESSAGE) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "  -- Your message was blank.\n" << std::endl;
		CHAT_USER_PROMPT;
	}
	if (client_status == CHAT_COMMAND) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << p_message << "\n" << std::endl;
	}
	if (client_status == CHAT_TOO_LONG) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "  -- Your message was too long. Part of it was cut off.\n" << std::endl;
	}
	if (client_status == MORE_MAIL) {
		std::cout << "\n\n    Type a number and press ENTER to select a mail record," << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\tor type \"next\" or \"previous\" for messages not listed." << std::endl;
	}	
	if (client_status == CONFIRM_DELETE_ALL_OUTBOX || client_status == CONFIRM_DELETE_ALL_READ || client_status == CONFIRM_DELETE_ALL_UNREAD || client_status == CONFIRM_DELETE_ALL_INBOX) {
		std::cout << "\n    Deleting records is permanent. Are you certian you wish to proceed?\n" << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
		std::cout << "\nPlease type \"yes\" to confirm and proceed with deletion. Tyoe \"menu\" to abort and go back to the mail menu.\n" << std::endl;
	}
	if (client_status == MAIL_SENDING) {
		std::cout << "  ...Sending a message\n" << std::endl;
	//	std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (client_status == MAIL_EDITING) {
		std::cout << "\n  ...Editing your message\n" << std::endl;
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 21);
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
	if (client_status == MAIL_LIST_HEAD) { // this is prompted by a packet but not a packet type itself
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 24);
	}	
	if (client_status == ACCOUNT_LOGIN_COUNT) {
		std::cout << "\n\n	You have logged in " << p_message << " times." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (client_status == ACCOUNT_AVG_TIME) {
		std::cout << "\n    The average time logged in is " << p_message << " seconds." << std::endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(2));
	}
	if (client_status == MAIL_REPLYING) {
		std::cout << "  ...Replying to a message\n" << std::endl;
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 21);
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}
}

void TextOutput::ChatMessage(int content_status, std::string p_name, std::string p_message)
{
	p_name[0] = toupper(p_name[0]);

	if (content_status == CHAT_IN_SAY) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << p_name << " says: " << p_message << "\n" << std::endl;
	}
	if (content_status == CHAT_IN_TELL) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << p_name << " tells you: " << p_message << "\n" << std::endl;
	}
	if (content_status == CHAT_OUT_SAY) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "You say: " << p_message << "\n" << std::endl;
	}
	if (content_status == CHAT_OUT_TELL) {
		GProg.GotoConsoleCoords(0, GProg.GetConsoleYpos() - 1);
		std::cout << "You tell " << p_name << ": " << p_message << "\n" << std::endl;
	}
}
