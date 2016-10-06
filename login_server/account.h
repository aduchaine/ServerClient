/*
3/12/16

account.h
*/

#ifndef ACCOUNT_H
#define ACCOUNT_H


enum LoginFailTypes {

	NIL = 0,

	BAD_PASSWORD = 1,
	ADMIN_BAD_PASSWORD = 2,
	DUAL_LOGIN = 3,
	ADMIN_DUAL_LOGIN = 4,
	BAD_LOGOUT = 5,
	ADMIN_BAD_LOGOUT = 6

};

class Account
{

public:

	int CheckAccountName(std::string p_name, int login_status);
	int CheckAccountPassword(int socket_position, std::string p_password, int login_status);

	int DualLogin(const char * p_name, int p_acctID, int login_status);
	int LogSuccess(int socket_position, int login_status);

	int DoFailedLogin(int p_acctID, int login_status);
	int DoCheckLockout(const char * p_name, int login_status); // this should stay as p_name because it is prior to name approval, before saving acct info

	void LogFail(int p_acctID, int fail_type); // can't use "socket_position" because this will be called when there are no connected users associated with the acctID
	void UserLogout(int p_acctID, std::string p_name);
	void AdminLogFail(int p_acctID, int fail_type); // can't use "socket_position" because this will be called when there are no connected users associated with the acctID
	void AdminLogout(int p_acctID, std::string p_name);

};

extern Account Acct;

#endif
