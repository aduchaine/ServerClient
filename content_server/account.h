/*
3/12/16

account.h
*/

#ifndef ACCOUNT_H
#define ACCOUNT_H


#pragma once

#include "stdafx.h"


class Account
{

public:

	// user
	int CheckAccountCreds(int p_acctID, std::string p_name, int response_status);
	void UserLogout(int p_acctID, std::string p_name);

	// admin - not implemented yet - since all users will be logged in on content, there is no need to combine admin login with <CheckAccountCreds> and the functions do different things
/*	int CheckAdminName(std::string p_name, int response_status);
	int CheckAdminPassword(std::string p_name, std::string p_password, int response_status);
	int AdminLogSuccess(std::string p_name, int response_status);
	void AdminLogFail(std::string p_name);
	void AdminLogout(std::string p_name); */

};

extern Account Acct;

#endif
