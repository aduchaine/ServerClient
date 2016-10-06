/*
3/13/16

database.h
*/

#ifndef DATABASE_H
#define DATABASE_H


class Database
{

public:

	// user/account
	bool NewUserRecord(int p_acctID, const char * p_name, const char * p_password);
	bool UserPWChange(int p_acctID, const char * p_password);	

	// global account lockout - associated with a timer
	void UpdateLockoutStatus(); // used to update	
	void ResetLockedAcct();	// used at startup and for errant lockouts
	void CheckAllAcctLockout();
	void CheckBadLogAccounts(); // checks all accounts for logout descrepancies

	// individual account lockout - when user does certain things
	void UpdateAcctLockout(const char * p_name);
	int CheckAcctLockout(const char * p_name);
	void UpdateFailedLoginCount(int p_acctID, bool reset, bool admin);
	int CheckFailedLoginCount(int p_acctID, int login_status);
	void IntitalAcctLockout(int p_acctID, const char * p_name, int lockout_type);
	bool CheckAccountIsLogged(int p_acctID, bool admin); // checks for logout descrepancies when a client attempts to log in

	// admin/account
	bool DBAdminLogin(int p_acctID);
	bool DBAdminLogout(int p_acctID);
	void AdminLoginTime(int p_acctID);
	bool NewAdminRecord(int p_acctID, const char * p_name, const char * p_password);	

	// general
	void CheckDatabaseTables();
	bool CheckTable(const char * p_table); // was <std::string>
	void ConnectDB();
	void DisconnectDB();

	// utility
	bool NameConflict(const char * p_name);	
	bool CheckAdminRecord(int p_acctID);  // checks to see if admin account has been created
	bool AdminNameConflict(const char * p_name);
	int GetAutoIncrementValue(const char * p_table);	
	void UpdateLoginCount(int p_acctID, bool admin);
	int GetLoginCount(int p_acctID, bool admin);
	void UpdateAverageSessionTime(int p_acctID, bool admin);
	int GetAverageSessionTime(int p_acctID, bool admin);
	void AccountInfo(int socket_position, std::string p_name, bool admin);
	std::string GetAccountName(int account_id);
	int GetAccountID(const char * p_name);

	// create tables
	void CreateTimeTable();
	void CreateAccountTable();
	void CreateAdminAccountTable();
	void CreateAccountLockoutTable();	

};

extern Database DB;

#endif
