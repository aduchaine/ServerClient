/*
3/12/16

account.cpp
*/

#include "stdafx.h"

#include "content_packet.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"


// non_existent accounts should be impossible
int Account::CheckAccountCreds(int p_acctID, std::string p_name, int response_status)
{
	const char * pm_name = p_name.c_str(); // eventually changing the main function data type

	if (response_status == INVALID_CREDS) {
		File.LogRecord(Logs::Normal, Logs::Error, "ERROR: Invalid name - input:(%s)", pm_name);
		return INVALID_CREDS;
	}
	if (DB.CheckAcctLockout(p_acctID) == true) {
		response_status = ACCT_LOCKED;
	}
	if (response_status == VALIDATE_CREDS) {
		if (DB.DBUserLogin(p_acctID) == true) {
			DB.UpdateLoginCount(p_acctID, false);
			File.StartUserLogs(pm_name, true, true);
			response_status = VALID_CREDS;
		}
		else { // DB error
			response_status = DB_ERROR;
		}
	}
	return response_status;	
}

void Account::UserLogout(int p_acctID, std::string p_name)
{
	if (DB.DBUserLogout(p_acctID) == false) {
		File.LogRecord(Logs::Normal, Logs::Error, "Database Error: Failed to record user(%s) logout time.", GProg.CharOutput(p_name));
	}
	DB.UpdateAverageSessionTime(p_acctID, false);
	File.CloseUserLogs(GProg.CharOutput(p_name), true, true);
}
