/*
3/13/16

database.h
*/

#ifndef DATABASE_H
#define DATABASE_H


#define DBUNREADRECV_PARAM get_unreadrecv_param
#define DBSENT_PARAM get_sent_param
#define DBREADRECV_PARAM get_readrecv_param
#define DBALLRECV_PARAM get_allrecv_param

#define READ 1
#define DELETE_SENDER 2
#define DELETE_RECV 4

/*
bit 1 = read/unread
bit 2 = delete sender no/yes
bit 3 = delete recv no/yes

status = 0 unread
status = 1 read
status = 2 deleted sender/unread
status = 3 deleted sender/read
status = 4 deleted recv/unread
status = 5 deleted recv/read
status = 6 deleted recv/deleted sender/unread	- archived - eventually make a separate mail archive table
status = 7 deleted recv/deleted sender/read		- archived
*/

class Database
{

public:

	// general
	void CheckDatabaseTables();
	bool CheckTable(const char * p_table);
	void ConnectDB();
	void DisconnectDB();

	// server
	void UpdateServerTime(bool login);
	bool ServerLogout();

	// user/account
	bool DBUserLogin(int p_acctID);	
	bool DBUserLogout(int p_acctID);
	void UserLoginTime(int p_acctID);
	bool CheckAcctLockout(int p_acctID); // this should never occur but is here for error handling, mainly
	
	// mail - assure the "sent" variable is not needed
	bool NewMailRecord(int ps_acctID, const char * p_sender, const char * p_recipient, int pr_acctID, const char * p_subject, const char * p_body);
	int GetMailID(int p_acctID, int response_status_value, unsigned int counter_pos, bool ascending);
	std::string ReadMailRecord(int message_id, bool sent);
	int GetMailCount(int p_acctID, int response_status_value);
	int GetMailLists(int socket_position, int client_socket, int response_status_value, int p_acctID, unsigned int counter_pos, bool ascending);	

	int SetMailMessageStatus(int response_status_value, int message_id, int acctID);
	const char * GetMailStatusParam(int response_status_value);
	int GetMessageStatChgVal(int response_status_value);
	void AddToMailArchive(int response_status_value, int acctID);
	void NewMailArchive(int message_id);

	// utility
	void UpdateLoginCount(int p_acctID, bool admin);
	int GetLoginCount(int p_acctID, bool admin);
	void UpdateAverageSessionTime(int p_acctID, bool admin);
	int GetAverageSessionTime(int p_acctID, bool admin);
	std::string GetAccountName(int account_id);
	int GetAccountID(const char * p_name);

	// create tables	
	void CreateTimeTable();
	void CreateMailTable();
	void CreateMailArchiveTable();
	void CreateAccountTable();
	void CreateAdminAccountTable();
	void CreateAccountLockoutTable();

};

extern Database DB;

#endif
