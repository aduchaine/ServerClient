/*
3/12/16

account.cpp
*/

#include "stdafx.h"

#include "login_net.h"
#include "login_packet.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"

int acct_ID;

std::string a_password;
std::string a_name;
// find out if variables/values passed through a function are data race safe


int Account::CheckAccountName(std::string p_name, int login_status)
{
	const char * pm_name = p_name.c_str();

	int login_stat;

	login_stat = GProg.IsValidInput(p_name, true, false, false, LOGIN_CHAR, NO_CHAR, 0, 0);

	if (login_stat == 0) {
		login_status = DoCheckLockout(pm_name, login_status);
	}	

	if (login_status == CHECK_NAME_OLD) {
		if (login_stat == 0) {
			if (DB.NameConflict(pm_name) == false) {
				login_status = INVALID_NAME_OLD;
			}
			else { // Login name qualifies
				File.LogRecord(Logs::Normal, Logs::Account, "Existing user login attempted on account: (%s)", pm_name);
				login_status = CHECK_PASSWORD_OLD;
			}
		}
		else {
			login_status = INVALID_NAME_OLD;
		}
	}
	else if (login_status == CHECK_NAME_NEW_VALIDITY) {
		login_stat = GProg.IsValidInput(p_name, true, false, true, LOGIN_CHAR, NO_CHAR, 4, 12);

		if (login_stat == 0) { // name is valid - CHECK_NAME_NEW_VALIDITY
			if (DB.NameConflict(pm_name) == true) {			// existing account holds this name
				login_status = INVALID_NAME_NEW_CONFLICT;
			}		
			else if (LogNet.FindReservedName(p_name) == true) { // name in use by another new account in registration
				login_status = INVALID_NAME_NEW_CONFLICT;
			}
			else { // Login name qualifies
				File.LogRecord(Logs::Normal, Logs::Account, "New account being created with account name: (%s)", pm_name);
				login_status = CHECK_PASSWORD_NEW_VALIDITY;
			}
		}
		else if (login_stat == 1) {
			login_status = INVALID_NAME_BADCHAR;
		}
		else {
			login_status = INVALID_NAME_LENGTH;
		}
	}
	else if (login_status == CHECK_NAME_PW_CHG) {		
		if (login_stat == 0) {
			if (DB.NameConflict(pm_name) == false) {
				login_status = INVALID_NAME_PW_CHG;
			}
			else { // Login name qualifies
				File.LogRecord(Logs::Normal, Logs::Account, "Password change attempted with user name: (%s).", pm_name);
				login_status = CHECK_PASSWORD_PW_CHG_LOGIN;
			}
		}
		else {
			login_status = INVALID_NAME_PW_CHG;
		}
	}
	else if (login_status == ADMIN_LOGIN) {	
		if (login_stat == 0) {
			if (DB.AdminNameConflict(pm_name) == false) {
				File.LogRecord(Logs::Normal, Logs::Admin, "Invalid admin name - input:(%s)", pm_name);
				login_status = ADMIN_INVALID_NAME;
			}
			else { // Admin account(name) checked out
				File.LogRecord(Logs::Normal, Logs::Admin, "Admin login attempted on account: (%s)", pm_name);
				login_status = ADMIN_PW_LOGIN;
			}
		}
		else {
			login_status = ADMIN_INVALID_NAME;
		}		
	}
	return login_status;
}

int Account::CheckAccountPassword(int socket_position, std::string p_password, int login_status)
{
	acct_ID = Net.GetAcctIDFromAcctInfo(socket_position);
	a_password = Net.GetPWFromAcctInfo(socket_position);
	a_name = Net.GetNameFromAcctInfo(socket_position);

	const char * pm_name = a_name.c_str();
	const char * pm_password = a_password.c_str();

	int login_stat;

	login_stat = GProg.IsValidInput(p_password, true, false, false, LOGIN_CHAR, NO_CHAR, 0, 0);

	if (login_status == CHECK_PASSWORD_OLD) {
		if (login_stat == 0) {
			if (a_password == p_password) {
				login_status = DualLogin(pm_name, acct_ID, CHECK_PASSWORD_OLD);

				if (login_status == CHECK_PASSWORD_OLD) {
					login_status = LOGIN_SUCCESS_OLD;
				}
			}
			else {
				login_status = DoFailedLogin(acct_ID, CHECK_PASSWORD_OLD);
			}
		}
		else {
			login_status = DoFailedLogin(acct_ID, CHECK_PASSWORD_OLD);
		}
	}
	else if (login_status == CHECK_PASSWORD_NEW_VALIDITY) {
		login_stat = GProg.IsValidInput(p_password, true, false, true, LOGIN_CHAR, NO_CHAR, 6, 12);

		if (login_stat == 0) { // CHECK_PASSWORD_NEW_VALIDITY		
			login_status = LOGIN_SUCCESS_NEW;
		}
		else if (login_stat == 1) {
			login_status = INVALID_PASSWORD_NEW_BADCHAR;
		}
		else {
			login_status = INVALID_PASSWORD_NEW_LENGTH;
		}
	}
	else if (login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
		if (login_stat == 0) {
			if (a_password == p_password) {
				login_status = DualLogin(pm_name, acct_ID, CHECK_PASSWORD_PW_CHG_LOGIN);
				if (login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
					login_status = CHECK_PASSWORD_PW_CHG_VALIDITY;
				}
			}
			else {
				login_status = DoFailedLogin(acct_ID, CHECK_PASSWORD_PW_CHG_LOGIN);
				File.LogRecord(Logs::Normal, Logs::Account, "Change Password - Invalid password for account:(%s) input:(%s)", pm_name, pm_password);
			}
		}
		else {
			login_status = DoFailedLogin(acct_ID, CHECK_PASSWORD_PW_CHG_LOGIN);
			File.LogRecord(Logs::Normal, Logs::Account, "Change Password - Invalid password for account:(%s) input:(%s)", pm_name, pm_password);
		}
	}
	else if (login_status == CHECK_PASSWORD_PW_CHG_RECORD) {
		login_stat = GProg.IsValidInput(p_password, true, false, true, LOGIN_CHAR, NO_CHAR, 6, 12);

		if (login_stat == 0) { // CHECK_PASSWORD_PW_CHG_RECORD
			login_status = LOGIN_SUCCESS_PW_CHG;
		}
		else if (login_stat == 1) {
			login_status = INVALID_PASSWORD_PW_CHG_BADCHAR;
		}
		else {
			login_status = INVALID_PASSWORD_PW_CHG_LENGTH;
		}
	}
	else if (login_status == ADMIN_PW_LOGIN) {
		if (login_stat == 0) {
			if (a_password == p_password) {
				login_status = DualLogin(pm_name, acct_ID, ADMIN_PW_LOGIN);
				if (login_status == ADMIN_PW_LOGIN) {
					login_status = ADMIN_LOG_SUCCESS;
				}
			}
			else {
				login_status = DoFailedLogin(acct_ID, ADMIN_PW_LOGIN);
				File.LogRecord(Logs::Normal, Logs::Admin, "Admin Login - Invalid password for account:(%s) input:(%s)", pm_name, pm_password);
			}
		}
		else {
			login_status = DoFailedLogin(acct_ID, ADMIN_PW_LOGIN);
			File.LogRecord(Logs::Normal, Logs::Admin, "Admin Login - Invalid password for account:(%s) input:(%s)", pm_name, pm_password);
		}
	}
	return login_status;
}

int Account::DualLogin(const char * p_name, int p_acctID, int login_status)
{	
	if (DB.CheckAccountIsLogged(p_acctID, true) == true) { // admin accounts - check if admin is logged in, normal users cannot boot an admin
		if (login_status == ADMIN_PW_LOGIN) {
			File.LogRecord(Logs::Normal, Logs::Account, "Dual Login detected on admin account(%s)", p_name);
			File.AdminRecord(p_name, "Dual Login detected");
			login_status = DUAL_LOG_ADMIN;
		}
		else {
			File.LogRecord(Logs::Normal, Logs::Account, "Dual Login detected on admin account(%s) - admin account association", p_name);
			File.UserRecord(p_name, "Dual Login detected: admin account association");
			login_status = USER_ADMIN_ACCT_CONFLICT; // dual login, account association, send user to entry menu
		}		
		return login_status;
	}	
	if (DB.CheckAccountIsLogged(p_acctID, false) == true) { // user accounts - check if logged in content server
		if (login_status == CHECK_PASSWORD_OLD || login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
			File.LogRecord(Logs::Normal, Logs::Account, "Dual Login detected on account(%s)", p_name);
			File.UserRecord(p_name, "Dual Login detected");
			if (login_status == CHECK_PASSWORD_OLD) {
				login_status = DUAL_LOG_OLD; // account is logged in content server
			}
			else {
				login_status = DUAL_LOG_PW_CHG; // account is logged in content server
			}			
		}
		else {
			File.LogRecord(Logs::Normal, Logs::Account, "Dual Login detected on account(%s) - user account association", p_name);
			File.AdminRecord(p_name, "Dual Login detected: user account association");
			login_status = ADMIN_USER_ACCT_CONFLICT; // dual login, content server account association, admin choose options			
		}
		return login_status;
	}	
	if (Net.FindUserByAccount(p_acctID) == true) { // user accounts - check if logged in login server
		if (login_status == CHECK_PASSWORD_OLD || login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
			File.LogRecord(Logs::Normal, Logs::Account, "Dual Login detected on account(%s)", p_name);
			File.UserRecord(p_name, "Dual Login detected");
			if (login_status == CHECK_PASSWORD_OLD) {
				login_status = DUAL_LOG_OLD; // account is logged in login server
			}
			else {
				login_status = DUAL_LOG_PW_CHG; // account is logged in login server
			}			
		}
		else {
			File.LogRecord(Logs::Normal, Logs::Account, "Dual Login detected on account(%s) - user account association", p_name);
			File.AdminRecord(p_name, "Dual Login detected: user account association");
			login_status = ADMIN_USER_ACCT_CONFLICT; // dual login, login server account association, admin choose options			
		}
		return login_status;
	}
	return login_status;
}

int Account::DoCheckLockout(const char * p_name, int login_status)
{
	int locked = DB.CheckAcctLockout(p_name);

	if (locked == BAD_PASSWORD || locked == ADMIN_BAD_PASSWORD) {
		login_status = PASSWORD_LOCKED;
	}
	else if (locked == DUAL_LOGIN || locked == ADMIN_DUAL_LOGIN) {
		login_status = SECURITY_LOCKED;
	}
	else if (locked == BAD_LOGOUT || locked == ADMIN_BAD_LOGOUT) {
		login_status = LOG_LOCKED;
	}
	return login_status;	
}

int Account::DoFailedLogin(int p_acctID, int login_status)
{
	if (login_status == ADMIN_PW_LOGIN) {
		DB.UpdateFailedLoginCount(p_acctID, false, true);
	}
	else {
		DB.UpdateFailedLoginCount(p_acctID, false, false);
	}

	if (DB.CheckFailedLoginCount(p_acctID, login_status) >= 3) {
		if (login_status == ADMIN_PW_LOGIN){
			AdminLogFail(p_acctID, ADMIN_BAD_PASSWORD);
		}
		else {
			LogFail(p_acctID, BAD_PASSWORD);
		}
		login_status = ACCOUNT_LOCK_BEGIN;
	}
	else if (login_status == ADMIN_PW_LOGIN) {
		login_status = ADMIN_INVALID_PW;
	}
	else if (login_status == CHECK_PASSWORD_PW_CHG_LOGIN) {
		login_status = INVALID_PASSWORD_PW_CHG_LOGIN;
	}
	else {
		login_status = INVALID_PASSWORD_OLD;
	}
	return login_status;
}

int Account::LogSuccess(int socket_position, int login_status)
{
	acct_ID = Net.GetAcctIDFromAcctInfo(socket_position);
	a_password = Net.GetPWFromAcctInfo(socket_position);
	a_name = Net.GetNameFromAcctInfo(socket_position);

	const char * pm_name = a_name.c_str();
	const char * pm_password = a_password.c_str();

	if (login_status == LOGIN_SUCCESS_OLD) {
		DB.UpdateFailedLoginCount(acct_ID, true, false);
		DB.UpdateLoginCount(acct_ID, false);
		File.StartUserLogs(pm_name, true, true);
		login_status = LOGIN_COMPLETE_OLD;
	}
	if (login_status == LOGIN_SUCCESS_NEW) {
		DB.UpdateFailedLoginCount(acct_ID, true, false);
		if (DB.NewUserRecord(acct_ID, pm_name, pm_password) == true) {
			DB.UpdateLoginCount(acct_ID, false);
			File.LogRecord(Logs::Normal, Logs::Account, "New account registration - NAME: %s PW: %s", pm_name, pm_password);
			File.StartUserLogs(pm_name, false, true);
			File.UserRecord(pm_name, "Account Creation");
			File.UserRecord(pm_name, "Login Name: %s", pm_name);
			File.UserRecord(pm_name, "Password: %s", pm_password);
			File.UserRecord(pm_name, "Initial login");
			login_status = LOGIN_COMPLETE_NEW;
		}
		else {
			login_status = DB_ERROR;
		}
	}
	if (login_status == LOGIN_SUCCESS_PW_CHG) {
		DB.UpdateFailedLoginCount(acct_ID, true, false);
		if (DB.UserPWChange(acct_ID, pm_password) == true) {
			DB.UpdateLoginCount(acct_ID, false);
			File.LogRecord(Logs::Normal, Logs::Account, "Account password change - NAME: %s PW: %s", pm_name, pm_password);
			File.StartUserLogs(pm_name, true, true);
			File.UserRecord(pm_name, "Account password change");
			File.UserRecord(pm_name, "Password: %s", pm_password);
			login_status = LOGIN_COMPLETE_OLD;
		}
		else {
			login_status = DB_ERROR;
		}
	}
	if (login_status == ADMIN_LOG_SUCCESS) {
		DB.UpdateFailedLoginCount(acct_ID, true, true);
		if (DB.CheckAdminRecord(acct_ID) == false) { // checks to see if admin account has been created
			if (DB.NewAdminRecord(acct_ID, pm_name, pm_password) == true) {
				DB.UpdateLoginCount(acct_ID, true);
				File.LogRecord(Logs::Normal, Logs::Account, "New Admin Registration - NAME: %s PW: %s", pm_name, pm_password);
				File.AdminRecord(pm_name, "Admin Account Creation");
				File.AdminRecord(pm_name, "Login Name: %s", pm_name);
				File.AdminRecord(pm_name, "Password: %s", pm_password);
				File.AdminRecord(pm_name, "Initial login");
				if (DB.DBAdminLogin(acct_ID) == true) {
					login_status = ADMIN_LOG_COMPLETE;
				}
				else {
					login_status = DB_ERROR;
				}
			}
			else {
				login_status = DB_ERROR;
			}
		}
		else if (DB.DBAdminLogin(acct_ID) == true) {
			DB.UpdateLoginCount(acct_ID, true);
			File.StartAdminLogs(pm_name, true, true);
			login_status = ADMIN_LOG_COMPLETE;
		}
		else {
			login_status = DB_ERROR;
		}
	}
	return login_status;
}

void Account::LogFail(int p_acctID, int fail_type)
{	
	std::string a_name;

	a_name = DB.GetAccountName(p_acctID);

	const char * pm_name = a_name.c_str();

	if (fail_type == BAD_PASSWORD) {
		File.LogRecord(Logs::Normal, Logs::Account, "Locked account(%s): three unsuccessful password attempts", pm_name);
		File.UserRecord(pm_name, "Locked account: three unsuccessful password attempts");
		DB.IntitalAcctLockout(p_acctID, pm_name, fail_type);
	}
	if (fail_type == DUAL_LOGIN) {
		File.LogRecord(Logs::Normal, Logs::Account, "Locked account(%s): possible compromised account", pm_name);
		File.UserRecord(pm_name, "Locked account: possible compromised account");
		DB.IntitalAcctLockout(p_acctID, pm_name, fail_type);
	}
	if (fail_type == BAD_LOGOUT) {
		File.LogRecord(Logs::Normal, Logs::Account, "Locked account(%s): improper logout", pm_name);
		File.UserRecord(pm_name, "Locked account: improper logout");
		DB.IntitalAcctLockout(p_acctID, pm_name, fail_type);
	}
}

void Account::AdminLogFail(int p_acctID, int fail_type)
{	
	std::string a_name;
	
	a_name = DB.GetAccountName(p_acctID);

	const char * pm_name = a_name.c_str();

	if (fail_type == ADMIN_BAD_PASSWORD) {
		File.LogRecord(Logs::Normal, Logs::Account, "Locked Admin account(%s): three unsuccessful password attempts", pm_name);
		File.AdminRecord(pm_name, "Locked Admin account: three unsuccessful password attempts");
		DB.IntitalAcctLockout(p_acctID, pm_name, fail_type);
	}
	if (fail_type == ADMIN_DUAL_LOGIN) {
		File.LogRecord(Logs::Normal, Logs::Account, "Locked Admin account(%s): possible compromised account", pm_name);
		File.AdminRecord(pm_name, "Locked Admin account: possible compromised account");
		DB.IntitalAcctLockout(p_acctID, pm_name, fail_type);
	}
	if (fail_type == ADMIN_BAD_LOGOUT) {
		File.LogRecord(Logs::Normal, Logs::Account, "Locked Admin account(%s): improper logout", pm_name);
		File.AdminRecord(pm_name, "Locked account: improper logout");
		DB.IntitalAcctLockout(p_acctID, pm_name, fail_type);
	}
}

void Account::UserLogout(int p_acctID, std::string p_name)
{
	File.CloseUserLogs(GProg.CharOutput(p_name), true, true);
}

void Account::AdminLogout(int p_acctID, std::string p_name)
{
	if (DB.DBAdminLogout(p_acctID) == false) {
		File.LogRecord(Logs::Normal, Logs::Account, "Database error: Failed to record user(%s) logout time.", GProg.CharOutput(p_name));
	}
	DB.UpdateAverageSessionTime(p_acctID, true);
	File.CloseAdminLogs(GProg.CharOutput(p_name), true, true);
}
