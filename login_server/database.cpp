/*
3/12/16

database.cpp
*/

#include "stdafx.h"

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "login_net.h"
#include "login_packet.h"
#include "servernetwork.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"


sql::Driver * driver;			// Create a connection 
sql::Connection * con;			// Connect to/open the database
sql::Statement * stmt;			// execute a simple query
sql::ResultSet * results;		// output results
sql::PreparedStatement * pstmt;	// execute a more complex query

const std::string host = "tcp://127.0.0.1:3306";
const std::string uname = "root";
const std::string dbpass = "bmoney";
const std::string dbname = "novice";

const char * mc_dbname = dbname.c_str();

std::string query = "";

//	<e.getErrorCode(), e.getSQLState()> print out funny values for std::string, don't know how to fix this - below these work when using <cout>
	// use this in Enter() to test <DB.CreateAccountLockoutTable();> // good for testing sql exceptions
	// int code = e.getErrorCode(); // test
	// std::string state(e.getSQLState()); // test
	// std::cout << "\ncode: " << code << std::endl; // test
	// std::cout << "\nstate: " << state << std::endl; // test
//	add a return to something or make what happens dependant on the <SQLException> - find something similar to "ws_util.cpp"
//	adding a crash log similar to eqemu would be helpful although, proactive debugging does a better job

// saving/recording the server time may need to be more accurate than in seconds
// we may end up losing or gaining time depending interger truncation, using milliseconds(X1000) may be necessary
// if the time table is empty but exists login does not enter any values -- need to fix 
// server time/login time should be stored in a constant variable upon startup (if that's how it's done)
// having a const/static int <server_time> on startup could help to keep track of time besides updating the DB on a timer

	
bool Database::NewUserRecord(int p_acctID, const char * p_name, const char * p_password)
{
	bool success = false;
	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("INSERT INTO `%s`.`account` (`account_id`, `account_name`, `password`, `creation_date`) VALUES(%i, '%s', '%s', %i)"
			, mc_dbname, p_acctID, p_name, p_password, static_cast<int>(time(0)));
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
		success = true;
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return success;
}

bool Database::UserPWChange(int p_acctID, const char * p_password)
{
	bool success = false;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("UPDATE `%s`.`account` SET `password` = '%s' WHERE `account_id` = %i"
			, mc_dbname, p_password, p_acctID);
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
		success = true;
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return success;
}

void Database::UpdateFailedLoginCount(int p_acctID, bool reset, bool admin)
{
	try {
		ConnectDB();

		if (admin == true) {
			if (reset == false) {
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`admin_account` SET `invalid_pw_loops` = `invalid_pw_loops` + 1 WHERE `account_id` = %i", mc_dbname, p_acctID);
				stmt->execute(query);

				delete stmt;
			}
			else {
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`admin_account` SET `invalid_pw_loops` = 0 WHERE `account_id` = %i", mc_dbname, p_acctID);
				stmt->execute(query);

				delete stmt;
			}
		}
		else if (admin == false) {
			if (reset == false) {
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`account` SET `invalid_pw_loops` = `invalid_pw_loops` + 1 WHERE `account_id` = %i", mc_dbname, p_acctID);
				stmt->execute(query);

				delete stmt;
			}
			else {
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`account` SET `invalid_pw_loops` = 0 WHERE `account_id` = %i", mc_dbname, p_acctID);
				stmt->execute(query);

				delete stmt;
			}
		}		
		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
}

int Database::CheckFailedLoginCount(int p_acctID, int login_status)
{
	int f_count;

	bool admin = (login_status == ADMIN_PW_LOGIN);

	try {
		ConnectDB();

		if (admin == true) {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT `invalid_pw_loops` FROM `%s`.`admin_account` WHERE `account_id` = %i", mc_dbname, p_acctID);
			results = stmt->executeQuery(query);

			while (results->next()) {
				f_count = results->getInt("invalid_pw_loops");
				break;
			}
			delete results;
			delete stmt;
		}
		else {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT `invalid_pw_loops` FROM `%s`.`account` WHERE `account_id` = %i", mc_dbname, p_acctID);
			results = stmt->executeQuery(query);

			while (results->next()) {
				f_count = results->getInt("invalid_pw_loops");
				break;
			}
			delete results;
			delete stmt;
		}				

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return f_count;
}

void Database::IntitalAcctLockout(int p_acctID, const char * p_name, int lockout_type)
{	
	int timer_time;	
	int current_time = static_cast<int>(time(0));	

	if (lockout_type == BAD_PASSWORD || lockout_type == ADMIN_BAD_PASSWORD) {
		timer_time = LOCKOUT_TIMER / 1000;
	}
	else if (lockout_type == DUAL_LOGIN || lockout_type == ADMIN_DUAL_LOGIN) {
		timer_time = COMPROMISED_ACCOUNT / 1000;
	}
	else if (lockout_type == BAD_LOGOUT || lockout_type == ADMIN_BAD_LOGOUT) {
		timer_time = IMPROPER_LOGOUT / 1000;
	}

	try {
		ConnectDB();

		if (lockout_type % 2 == 1) { // user accounts
			stmt = con->createStatement();
			query = File.StringFormat("INSERT INTO `%s`.`account_lockout` (`account_id`, `account_name`, `lockout_date`, `lockout_type`, `lockout_duration`, `lockout`) VALUES(%i, '%s', %i, %i, %i, 1)"
				, mc_dbname, p_acctID, p_name, current_time, lockout_type, timer_time);
			stmt->execute(query);
			delete stmt;

			stmt = con->createStatement();
			query = File.StringFormat("UPDATE `%s`.`account` SET `lockout` = 1, `invalid_pw_loops` = 0 WHERE `account_id` = %i", mc_dbname, p_acctID);
			stmt->execute(query);
			delete stmt;
		}
		else { // admin accounts
			stmt = con->createStatement();
			query = File.StringFormat("INSERT INTO `%s`.`account_lockout` (`account_id`, `account_name`, `lockout_date`, `lockout_type`, `lockout_duration`, `lockout`) VALUES(%i, '%s', %i, %i, %i, 1)"
				, mc_dbname, p_acctID, p_name, current_time, lockout_type, timer_time);
			stmt->execute(query);
			delete stmt;

			stmt = con->createStatement();
			query = File.StringFormat("UPDATE `%s`.`admin_account` SET `lockout` = 1, `invalid_pw_loops` = 0 WHERE `account_id` = %i", mc_dbname, p_acctID);
			stmt->execute(query);
			delete stmt;
		}

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}	
}

void Database::UpdateAcctLockout(const char * p_name)
{
	bool account_lockout = false;

	int timer_time;
	int lockout_time;
	int current_time = static_cast<int>(time(0));

	try {
		ConnectDB();
		
		stmt = con->createStatement();
		query = File.StringFormat("SELECT `lockout_date`, `lockout_duration` FROM `%s`.`account_lockout` WHERE `account_name` = '%s' AND `lockout` = 1", mc_dbname, p_name);
		results = stmt->executeQuery(query);
		delete stmt;

		while (results->next()) {
			lockout_time = results->getInt("lockout_date");
			timer_time = results->getInt("lockout_duration");		
			if (current_time < timer_time + lockout_time) {
				account_lockout = true;
				break;
			}
			if (account_lockout == false) { // since there are results and <account_lockout == false> from default above, unlock the account			
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`account_lockout` SET `lockout` = 0 WHERE `account_name` = '%s' AND %i > `lockout_date` + `lockout_duration`", mc_dbname, p_name, current_time);
				stmt->execute(query);
				delete stmt;

				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`account` SET `lockout` = 0 WHERE `account_name` = '%s'", mc_dbname, p_name);
				stmt->execute(query);
				delete stmt;

				break;
			}
		}
		delete results;

		DisconnectDB();		
	}
	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
}

void Database::ResetLockedAcct()
{
	sql::ResultSet * results2;

	int acctID;
	int adminacctID;

	std::string usertype = File.StringFormat("(`lockout_type` = 1 or `lockout_type` = 3 or `lockout_type` = 5)");
	std::string admintype = File.StringFormat("(`lockout_type` = 2 or `lockout_type` = 4 or `lockout_type` = 6)");

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT `account_id` FROM `%s`.`account` WHERE `account`.`lockout` = 1", mc_dbname);
		results2 = stmt->executeQuery(query);
		delete stmt;
		
		while (results2->next()) {
			acctID = results2->getInt("account_id");

			stmt = con->createStatement();
			query = File.StringFormat("SELECT `account_id` FROM `%s`.`account_lockout` WHERE `account_id` = %i AND `lockout` = 1 AND %s", mc_dbname, acctID, usertype);
			results = stmt->executeQuery(query);
			delete stmt;

			while (!results->next()) {
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`account` SET `lockout` = 0 WHERE `account_id` = %i", mc_dbname, acctID);
				stmt->execute(query);
				delete stmt;

				File.LogRecord(Logs::Normal, Logs::Database, "ERROR: Locked account - ID(%i) with no current entry in table `account_lockout`", acctID);
				break;
			}
			delete results;
		}		
		delete results2;

		stmt = con->createStatement();
		query = File.StringFormat("SELECT `account_id` FROM `%s`.`admin_account` WHERE `admin_account`.`lockout` = 1", mc_dbname);
		results2 = stmt->executeQuery(query);
		delete stmt;		
		
		while (results2->next()) {
			adminacctID = results2->getInt("account_id");

			stmt = con->createStatement();
			query = File.StringFormat("SELECT `account_id` FROM `%s`.`account_lockout` WHERE `account_id` = %i AND `lockout` = 1 AND %s", mc_dbname, adminacctID, admintype);
			results = stmt->executeQuery(query);
			delete stmt;

			while (!results->next()) {
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`admin_account` SET `lockout` = 0 WHERE `account_id` = %i", mc_dbname, adminacctID);
				stmt->execute(query);
				delete stmt;

				File.LogRecord(Logs::Normal, Logs::Database, "ERROR: Locked admin account - ID(%i) with no current entry in table `account_lockout`", adminacctID);
				break;
			}
			delete results;
		}
		delete results2;

		DisconnectDB();	
	}
	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
}

void Database::UpdateLockoutStatus()
{
	int acctID;
	int l_type;
	int timer_time;
	int lockout_time;
	int current_time = static_cast<int>(time(0));	

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT `account_id`, `lockout_type`, `lockout_date`, `lockout_duration` FROM `%s`.`account_lockout` WHERE `lockout` = 1", mc_dbname);
		results = stmt->executeQuery(query);
		delete stmt;

		while (results->next()) {
			acctID = results->getInt("account_id");
			l_type = results->getInt("lockout_type");
			lockout_time = results->getInt("lockout_date");
			timer_time = results->getInt("lockout_duration");			

			if (current_time > timer_time + lockout_time) { // since there are results but the lockout time has expired, unlock the account
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`account_lockout` SET `lockout` = 0 WHERE %i > `lockout_date` + `lockout_duration`", mc_dbname, current_time);
				stmt->execute(query);
				delete stmt;

				if (l_type == 1 || l_type == 3 || l_type == 5) {
					stmt = con->createStatement();
					query = File.StringFormat("UPDATE `%s`.`account` SET `lockout` = 0 WHERE `account_id` = %i", mc_dbname, acctID);
					stmt->execute(query);
					delete stmt;
				}
				else if (l_type == 2 || l_type == 4 || l_type == 6) {
					stmt = con->createStatement();
					query = File.StringFormat("UPDATE `%s`.`admin_account` SET `lockout` = 0 WHERE `account_id` = %i", mc_dbname, acctID);
					stmt->execute(query);
					delete stmt;
				}
			}			
		}
		delete results;

		DisconnectDB();
	}
	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
}

void Database::CheckAllAcctLockout()
{
	int acctIDs;
	int type;

	UpdateLockoutStatus();	

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT DISTINCT `account_id`, `lockout_type` FROM `%s`.`account_lockout` WHERE `lockout` = 1 ORDER BY `account_id`", mc_dbname);
		results = stmt->executeQuery(query);

		while (results->next()) {
			acctIDs = results->getInt("account_id");
			type = results->getInt("lockout_type");
			if (acctIDs > 0) {
				locked_accts_map [acctIDs] = type; // "locked_accts_map" used in <CheckBadLogAccounts()>
				//printf("\n  locked acctID-type:(%i-%i)", acctIDs, type); // for heavy debugging
			}			
		}
		delete results;
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
}

int Database::CheckAcctLockout(const char * p_name)
{
	int lock_type;

	UpdateAcctLockout(p_name);

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT `lockout_type` FROM `%s`.`account_lockout` WHERE `account_name` = '%s' AND `lockout` = 1", mc_dbname, p_name);
		results = stmt->executeQuery(query);

		if (results->next()) {
			lock_type = results->getInt("lockout_type");			
		}
		else {
			lock_type = 0;
		}
		delete results;
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return lock_type;
}

void Database::CheckDatabaseTables()
{
	const char * p_table = "time";
	const char * a_table = "account";
	const char * d_table = "admin_account";
	const char * l_table = "account_lockout";

	if (DB.CheckTable(p_table) == false) {
		DB.CreateTimeTable();
	}
	if (DB.CheckTable(a_table) == false) {
		DB.CreateAccountTable();
	}
	if (DB.CheckTable(d_table) == false) {
		DB.CreateAdminAccountTable();
	}
	if (DB.CheckTable(l_table) == false) {
		DB.CreateAccountLockoutTable();
	}
}

bool Database::CheckTable(const char * p_table)
{
	bool exists = true;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT * FROM information_schema.tables WHERE table_schema = '%s' AND table_name = '%s' LIMIT 1", mc_dbname, p_table);
		results = stmt->executeQuery(query);

		while (!results->next()) {
			exists = false;
			break;
		}
		delete results;
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return exists;
}

bool Database::DBAdminLogin(int p_acctID)
{
	bool success = false;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("UPDATE `%s`.`admin_account` SET `last_login` = %i WHERE `account_id` = %i"
			, mc_dbname, static_cast<int>(time(0)), p_acctID);
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
		success = true;
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return success;
}

bool Database::DBAdminLogout(int p_acctID)
{
	bool success = false;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("UPDATE `%s`.`admin_account` SET `last_logout` = %i WHERE `account_id` = %i"
			, mc_dbname, static_cast<int>(time(0)), p_acctID);
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
		AdminLoginTime(p_acctID);
		success = true;
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return success;
}

void Database::AdminLoginTime(int p_acctID)
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("UPDATE `%s`.`admin_account` SET `last_session_time` = (`last_logout` - `last_login`), "
			"`total_session_time` = (`total_session_time` + `last_session_time`) WHERE `account_id` = %i"
			, mc_dbname, p_acctID);
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
}

bool Database::CheckAdminRecord(int p_acctID)
{
	bool admin_record = false;	

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT * FROM `%s`.`admin_account` WHERE `account_id` = %i AND `admin` = 1", mc_dbname, p_acctID);

		results = stmt->executeQuery(query);
		while (results->next()) {
			admin_record = true;
			break;
		}
		delete results;
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return admin_record;
}

bool Database::NewAdminRecord(int p_acctID, const char * p_name, const char * p_password)
{
	bool success = false;		 

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("INSERT INTO `%s`.`admin_account` (`account_id`, `admin`, `account_name`, `password`, `creation_date`) VALUES(%i, 1, '%s', '%s', %i)"
			, mc_dbname, p_acctID, p_name, p_password, static_cast<int>(time(0)));
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
		success = true;
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return success;
}

bool Database::AdminNameConflict(const char * char_string)
{
	bool conflict = false;	

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT * FROM `%s`.`account` WHERE `account_name` = '%s' and `admin` = 1", mc_dbname, char_string);
		results = stmt->executeQuery(query);

		while (results->next()) { // name exists if results
			conflict = true;
			break;
		}
		delete results;
		delete stmt;
		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return conflict;
}

bool Database::NameConflict(const char * char_string)
{
	bool conflict = false;	

	try {
		ConnectDB();

		stmt = con->createStatement();		
		query = File.StringFormat("SELECT * FROM `%s`.`account` WHERE `account_name` = '%s'", mc_dbname, char_string);
		results = stmt->executeQuery(query);

		while (results->next()) { // name exists if results
			conflict = true;
			break;
		}
		delete results;
		delete stmt;
		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return conflict;	
}

int Database::GetAutoIncrementValue(const char * p_table)
{
	int next_ID;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT `AUTO_INCREMENT` FROM information_schema.tables WHERE table_schema = '%s' AND table_name = '%s'", mc_dbname, p_table);
		results = stmt->executeQuery(query);

		while (results->next()) {
			next_ID = results->getInt("AUTO_INCREMENT");
			break;
		}
		delete results;
		delete stmt;
		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return next_ID;
}

void Database::UpdateLoginCount(int p_acctID, bool admin)
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		if (admin == true) {			
			query = File.StringFormat("UPDATE `%s`.`admin_account` SET `login_count` = `login_count` + 1 WHERE `account_id` = %i", mc_dbname, p_acctID);			
		}
		else {
			query = File.StringFormat("UPDATE `%s`.`account` SET `login_count` = `login_count` + 1 WHERE `account_id` = %i", mc_dbname, p_acctID);			
		}
		stmt->execute(query);

		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
}

int Database::GetLoginCount(int p_acctID, bool admin)
{
	int count;

	try {
		ConnectDB();

		stmt = con->createStatement();

		if (admin == true) {			
			query = File.StringFormat("SELECT `login_count` FROM `%s`.`admin_account` WHERE `account_id` = %i", mc_dbname, p_acctID);			
		}
		else {
			query = File.StringFormat("SELECT `login_count` FROM `%s`.`account` WHERE `account_id` = %i", mc_dbname, p_acctID);
		}

		results = stmt->executeQuery(query);

		while (results->next()) {
			count = results->getInt("login_count");
			break;
		}		

		delete results;
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return count;
}

void Database::UpdateAverageSessionTime(int p_acctID, bool admin)
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		if (admin == true) {
			query = File.StringFormat("UPDATE `%s`.`admin_account` SET `average_session_time` = `total_session_time` / (`content_count` + `login_count`) WHERE `account_id` = %i", mc_dbname, p_acctID);
		}
		else {
			query = File.StringFormat("UPDATE `%s`.`account` SET `average_session_time` = `total_session_time` / `login_count` WHERE `account_id` = %i", mc_dbname, p_acctID);
		}
		stmt->execute(query);

		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
}

int Database::GetAverageSessionTime(int p_acctID, bool admin)
{
	int average_time;

	try {
		ConnectDB();

		stmt = con->createStatement();

		if (admin == true) {
			query = File.StringFormat("SELECT `average_session_time` FROM `%s`.`admin_account` WHERE `account_id` = %i", mc_dbname, p_acctID);
		}
		else {
			query = File.StringFormat("SELECT `average_session_time` FROM `%s`.`account` WHERE `account_id` = %i", mc_dbname, p_acctID);
		}

		results = stmt->executeQuery(query);

		while (results->next()) {
			average_time = results->getInt("average_session_time");
			break;
		}

		delete results;
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return average_time;
}

bool Database::CheckAccountIsLogged(int p_acctID, bool admin)
{
	bool logged_in = false;	

	try {
		ConnectDB();

		if (admin == false) {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT * FROM `%s`.`account` WHERE `account_id` = %i and `last_login` > `last_logout`", mc_dbname, p_acctID);
			results = stmt->executeQuery(query);
			while (results->next()) {
				logged_in = true;
				break;
			}
			delete results;
			delete stmt;
		}
		else {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT * FROM `%s`.`admin_account` WHERE `account_id` = %i and `last_login` > `last_logout`", mc_dbname, p_acctID);
			results = stmt->executeQuery(query);
			while (results->next()) {
				logged_in = true;
				break;
			}
			delete results;
			delete stmt;
		}
		
		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return logged_in;
}

void Database::CheckBadLogAccounts()
{
	sql::Statement * stmt1;
	sql::ResultSet * results1;

	int acct_int;

	CheckAllAcctLockout();

	try {
		ConnectDB();		

		stmt1 = con->createStatement();
		query = File.StringFormat("SELECT `account_id` FROM `%s`.`account` WHERE `last_login` > `last_logout`", mc_dbname); //  AND `lockout` != 1
		results1 = stmt1->executeQuery(query);		

		while (results1->next()) {
			acct_int = results1->getInt("account_id");			
			//printf("\n found a user account which should be checked for lockout"); // for heavy debugging			
			if (acct_int > 0) {				
				DBlogged_accts_map[acct_int] = 0;
				//printf("\n  DB logged-in acctID-user:(%i-0)", acct_int); // for heavy debugging
			}
		}
		delete stmt1;
		delete results1;		

		stmt1 = con->createStatement();
		query = File.StringFormat("SELECT `account_id` FROM `%s`.`admin_account` WHERE `last_login` > `last_logout`", mc_dbname); //  AND `lockout` != 1
		results1 = stmt1->executeQuery(query);		

		while (results1->next()) {
			acct_int = results1->getInt("account_id");			
			//printf("\n found an admin account which should be checked for lockout"); // for heavy debugging			
			if (acct_int > 0) {
				DBlogged_accts_map[acct_int] = 1;
				//printf("\n  DB logged-in acctID-admin:(%i-1)", acct_int); // for heavy debugging
			}
		}
		delete stmt1;
		delete results1;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}	
}

void Database::ConnectDB()
{
	driver = get_driver_instance();

	if (con = driver->connect(host, uname, dbpass)) {
		con->setSchema(dbname);
		return;
	}
	else {
		File.LogRecord(Logs::Normal, Logs::Database, "ERROR: Unable to open DB connection");
		return;
	}
}

void Database::DisconnectDB()
{
//	if (con->isClosed()) { // does not work for some reason
//		return;
//	}

	con->close();

	if (con->isClosed()) {
		delete con;
	}
	else {
		File.LogRecord(Logs::Normal, Logs::Database, "ERROR: Failed to close DB connection");
		delete con;
	}
	return;
}

void Database::CreateTimeTable()
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		File.LogRecord(Logs::Normal, Logs::Database, "Creating Table: `time`");
		query = (
			"CREATE TABLE `time` ("
			"`creation_date` int(11) UNSIGNED NOT NULL, "		
			"`server_time` int(11) UNSIGNED NOT NULL, "
			"`last_login` int(11) UNSIGNED NOT NULL, "
			"`last_logout` int(11) UNSIGNED NOT NULL, "
			"`last_session_time` int(11) UNSIGNED NOT NULL, "
			"`last_login_date` int(11) UNSIGNED NOT NULL "
			") ENGINE = InnoDB DEFAULT CHARSET = latin1"
			);
		stmt->execute(query);
		delete stmt;

		stmt = con->createStatement();
		query = File.StringFormat("INSERT INTO `%s`.`time` (`creation_date`) VALUES(%i)"
			, mc_dbname, static_cast<int>(time(0)));
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return;
}

void Database::CreateAdminAccountTable()
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		File.LogRecord(Logs::Normal, Logs::Database, "Creating Table: `admin_account`");
		query = (
			"CREATE TABLE `admin_account` ("
			"`account_id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT, "
			"`admin` int(4) UNSIGNED NOT NULL DEFAULT '0', "
			"`account_name` varchar(64) NOT NULL DEFAULT '', "
			"`password` varchar(64) NOT NULL DEFAULT '', "
			"`creation_date` int(11) UNSIGNED NOT NULL, "
			"`login_count` int(8) UNSIGNED NOT NULL DEFAULT '0', "
			"`content_count` int(8) UNSIGNED NOT NULL DEFAULT '0', "
			"`last_login` int(11) UNSIGNED NOT NULL, "
			"`last_logout` int(11) UNSIGNED NOT NULL, "
			"`last_session_time` int(11) UNSIGNED NOT NULL, "
			"`average_session_time` int(11) UNSIGNED NOT NULL, "
			"`total_session_time` int(11) UNSIGNED NOT NULL, "
			"`lockout` int(4) UNSIGNED NOT NULL DEFAULT '0', "
			"`invalid_pw_loops` int(4) UNSIGNED NOT NULL DEFAULT '0', "
			"PRIMARY KEY(`account_id`), "
			"UNIQUE `account_name` (`account_name`)"
			") ENGINE = InnoDB AUTO_INCREMENT = 1 DEFAULT CHARSET = latin1"
			);
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return;
}

void Database::CreateAccountTable()
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		File.LogRecord(Logs::Normal, Logs::Database, "Creating Table: `account`");
		query = (
			"CREATE TABLE `account` ("
			"`account_id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT, "
			"`admin` int(4) UNSIGNED NOT NULL DEFAULT '0', "
			"`account_name` varchar(64) NOT NULL DEFAULT '', "
			"`password` varchar(64) NOT NULL DEFAULT '', "
			"`creation_date` int(11) UNSIGNED NOT NULL, "
			"`login_count` int(8) UNSIGNED NOT NULL DEFAULT '0', "
			"`content_count` int(8) UNSIGNED NOT NULL DEFAULT '0', "
			"`last_login` int(11) UNSIGNED NOT NULL, "
			"`last_logout` int(11) UNSIGNED NOT NULL, "
			"`last_session_time` int(11) UNSIGNED NOT NULL, "
			"`average_session_time` int(11) UNSIGNED NOT NULL, "
			"`total_session_time` int(11) UNSIGNED NOT NULL, "
			"`lockout` int(4) UNSIGNED NOT NULL DEFAULT '0', "
			"`invalid_pw_loops` int(4) UNSIGNED NOT NULL DEFAULT '0', "	
			"PRIMARY KEY(`account_id`), "
			"UNIQUE `account_name` (`account_name`)"
			") ENGINE = InnoDB AUTO_INCREMENT = 1 DEFAULT CHARSET = latin1"
			);
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return;
}

void Database::CreateAccountLockoutTable()
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		File.LogRecord(Logs::Normal, Logs::Database, "Creating Table: `account_lockout`");
		query = (
			"CREATE TABLE `account_lockout` ("
			"`id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT, "
			"`account_id` int(11) UNSIGNED NOT NULL, "
			"`account_name` varchar(64) NOT NULL DEFAULT '', "
			"`lockout_date` int(11) UNSIGNED NOT NULL, "
			"`lockout_type` int(4) UNSIGNED NOT NULL, "
			"`lockout_duration` int(11) UNSIGNED NOT NULL, "
			"`lockout` int(4) UNSIGNED NOT NULL, "
			"PRIMARY KEY(`id`) "
			") ENGINE = InnoDB AUTO_INCREMENT = 1 DEFAULT CHARSET = latin1"
			);
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ### SQLState: ###)", error);
	}
	return;
}

void Database::AccountInfo(int socket_position, std::string p_name, bool admin)
{
	std::string acct_pw;

	int DBacct_id;

	try {
		ConnectDB();

		if (admin == false) {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT `account_id`, `password` FROM `%s`.`account` WHERE `account_name` = '%s'", mc_dbname, GProg.CharOutput(p_name));
			results = stmt->executeQuery(query);

			while (results->next()) {
				DBacct_id = results->getInt("account_id");
				acct_pw = results->getString("password");
				//printf("\n found a user - acct_int-password:(%i:%s)", DBacct_id, GProg.CharOutput(acct_pw)); // for heavy debugging				
				Net.AddToAcctInfoList(socket_position, DBacct_id, p_name, acct_pw); // put it in memory here			
				break;
			}
			delete stmt;
			delete results;
		}
		else {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT `account_id`, `password` FROM `%s`.`admin_account` WHERE `account_name` = '%s'", mc_dbname, GProg.CharOutput(p_name));
			results = stmt->executeQuery(query);

			while (results->next()) {
				DBacct_id = results->getInt("account_id");
				acct_pw = results->getString("password");
				// should add name here, otherwise all accounts who try to login as admin will be led to password
				//printf("\n found an admin - acct_int-password:(%i:%s)", DBacct_id, GProg.CharOutput(acct_pw)); // for heavy debugging				
				Net.AddToAcctInfoList(socket_position, DBacct_id, p_name, acct_pw); // put it in memory here				
				break;
			}
			delete stmt;
			delete results;
		}

		DisconnectDB();
	}
	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}	
}

std::string Database::GetAccountName(int account_id)
{
	std::string p_name;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT `account_name` FROM `%s`.`account` WHERE `account_id` = %i", mc_dbname, account_id);
		results = stmt->executeQuery(query);

		while (results->next()) {
			p_name = results->getString("account_name");				
		}
		delete results;
		delete stmt;
		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return p_name;
}

int Database::GetAccountID(const char * p_name)
{
	int i_acct_ID = 0;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT `account_id` FROM `%s`.`account` WHERE `account_name` = '%s'", mc_dbname, p_name);
		results = stmt->executeQuery(query);

		while (results->next()) {		
			i_acct_ID = results->getInt("account_id");
			break;
		}
		delete results;
		delete stmt;

		DisconnectDB();
	}

	catch (sql::SQLException &e) {
		std::string error(e.what());
		std::cout << "\n# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: SQLException in File:(%s) Function:(%s) on line:(%i)", __FILE__, __FUNCTION__, __LINE__);
		File.LogRecord(Logs::Normal, Logs::Database, "# ERROR: %s (MySQL error code: ####, SQLState: ####)", error);
	}
	return i_acct_ID;
}
