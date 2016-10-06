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

#include "content_net.h"
#include "content_packet.h"
#include "servernetwork.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"


sql::Driver * driver;
sql::Connection * con;
sql::Statement * stmt;
sql::ResultSet * results;
sql::PreparedStatement * pstmt;

const std::string host = "tcp://127.0.0.1:3306";
const std::string uname = "root";
const std::string dbpass = "bmoney";
const std::string dbname = "novice";

const char * mc_dbname = dbname.c_str();

std::string p_table = "";
std::string query = "";

const char * get_unreadrecv_param = "(`status` = 0 or `status` = 2) ";									// 1/4	get unread			- change to read recv/delete unread recv
const char * get_sent_param = "(`status` = 0 or `status` = 1 or `status` = 4 or `status` = 5) ";		// 2	get sent			- change to delete sender
const char * get_readrecv_param = "(`status` = 1 or `status` = 3) ";									// 4	get read			- change to delete read recv
const char * get_allrecv_param = "(`status` = 0 or `status` = 1 or `status` = 2 or `status` = 3) ";		// 4	get read/unread		- change to delete all recv

std::vector<int> del_vec;

std::map<int, std::vector<int>> mail_status_map;

std::vector<int> read_rsv = { MAIL_INBOX_UNREAD };
std::vector<int> d_sent_rsv = { MAIL_DELETE_SENT, MAIL_DELETE_ALL_OUTBOX };
std::vector<int> d_recv_rsv = { MAIL_DELETE_ALL_INBOX, MAIL_DELETE_UNREAD, MAIL_DELETE_ALL_UNREAD, MAIL_DELETE_READ, MAIL_DELETE_ALL_READ };


void Database::CheckDatabaseTables()
{
	const char * p_table = "time";
	const char * m_table = "mail";
	const char * ma_table = "mail_archive";
	const char * a_table = "account";
	const char * d_table = "admin_account";
	const char * l_table = "account_lockout";

	if (CheckTable(p_table) == false) {
		CreateTimeTable();
	}
	if (CheckTable(m_table) == false) {
		CreateMailTable();
	}
	if (CheckTable(ma_table) == false) {
		CreateMailArchiveTable();
	}
	if (CheckTable(a_table) == false) {
		CreateAccountTable();
	}
	if (CheckTable(d_table) == false) {
		CreateAdminAccountTable();
	}
	if (CheckTable(l_table) == false) {
		CreateAccountLockoutTable();
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
	if (con->isClosed()) {
		return;
	}

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

void Database::UpdateServerTime(bool login)
{
	int last_login;

	try {
		ConnectDB();

		if (login == true) {
			stmt = con->createStatement();
			query = File.StringFormat("UPDATE `%s`.`time` SET `last_login_date` = %i, `last_login` = `server_time`, "
				"`last_session_time` = 0", mc_dbname, static_cast<int>(time(0)));
			stmt->execute(query);
			delete stmt;
		}
		else {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT `server_time` FROM `%s`.`time`", mc_dbname);
			results = stmt->executeQuery(query);
			delete stmt;

			while (results->next()) {
				last_login = results->getInt("server_time");
				time_t check = (clock() / 1000);

				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`time` SET `server_time` = (`last_login` + %i)", mc_dbname, check);
				stmt->execute(query);
				delete stmt;

				break;
			}
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
	return;
}

bool Database::ServerLogout()
{
	bool success = false;
	int last_login;

	UpdateServerTime(false);

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT `last_login` FROM `%s`.`time`", mc_dbname);
		results = stmt->executeQuery(query);
		delete stmt;

		while (results->next()) {
			last_login = results->getInt("last_login");
			time_t logout = (clock() / 1000) + last_login;

			stmt = con->createStatement();
			query = File.StringFormat("UPDATE `%s`.`time` SET `last_logout` = %i, `last_session_time` = (`last_logout` - `last_login`)", mc_dbname, logout);
			stmt->execute(query);
			delete stmt;

			break;
		}
		delete results;
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

bool Database::DBUserLogin(int p_acctID)
{
	bool success = false;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("UPDATE `%s`.`account` SET `last_login` = %i WHERE `account_id` = %i"
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

bool Database::DBUserLogout(int p_acctID)
{
	bool success = false;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("UPDATE `%s`.`account` SET `last_logout` = %i WHERE `account_id` = %i"
			, mc_dbname, static_cast<int>(time(0)), p_acctID);
		stmt->execute(query);
		delete stmt;

		DisconnectDB();
		UserLoginTime(p_acctID);
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

void Database::UserLoginTime(int p_acctID)
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("UPDATE `%s`.`account` SET `last_session_time` = (`last_logout` - `last_login`), "
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
	return;
}

bool Database::CheckAcctLockout(int p_acctID)
{
	bool account_lockout = false;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT * FROM `%s`.`account` WHERE `account_id` = %i and `lockout` = 1", mc_dbname, p_acctID);

		results = stmt->executeQuery(query);
		while (results->next()) {
			account_lockout = true;
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
	return account_lockout;
}

bool Database::NewMailRecord(int ps_acctID, const char * p_sender, const char * p_recipient, int pr_acctID, const char * p_subject, const char * p_body)
{
	bool success = false;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("INSERT INTO `%s`.`mail` (`account_id`, `to`, `timestamp`, `sender_id`, `from`, `subject`, `body`, `status`) VALUES(%i, '%s', %i, %i, '%s', '%s', '%s', 0)"
			, mc_dbname, pr_acctID, p_recipient, static_cast<int>(time(0)), ps_acctID, p_sender, p_subject, p_body);
		stmt->execute(query);

		success = true;
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
	return success;
}

int Database::GetMailID(int p_acctID, int response_status_value, unsigned int counter_pos, bool ascending)
{
	int message_count = 0;
	int message_id = 0;

	const char * mail_status_param;

	mail_status_param = GetMailStatusParam(response_status_value);

	try {
		ConnectDB();

		if (ascending == true) {
			if (mail_status_param == DBSENT_PARAM) {
				stmt = con->createStatement();
				query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `sender_id` = %i AND %s ORDER BY `msgid` ASC", mc_dbname, p_acctID, mail_status_param);
				results = stmt->executeQuery(query);
			}
			else {
				stmt = con->createStatement();
				query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `account_id` = %i AND %s ORDER BY `msgid` ASC", mc_dbname, p_acctID, mail_status_param);
				results = stmt->executeQuery(query);
			}
		}
		else {
			if (mail_status_param == DBSENT_PARAM) {
				stmt = con->createStatement();
				query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `sender_id` = %i AND %s ORDER BY `msgid` DESC", mc_dbname, p_acctID, mail_status_param);
				results = stmt->executeQuery(query);
			}
			else {
				stmt = con->createStatement();
				query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `account_id` = %i AND %s ORDER BY `msgid` DESC", mc_dbname, p_acctID, mail_status_param);
				results = stmt->executeQuery(query);
			}
		}				

		while (results->next()) {
			++message_count;

			if (message_count == counter_pos) {
				message_id = results->getInt("msgid");
				break;
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
	return message_id;
}

std::string Database::ReadMailRecord(int message_id, bool sent)
{
	std::string p_from;
	std::string p_to;
	std::string p_subject;
	std::string s_date;
	std::string p_body;
	std::string p_message = "";

	int i_date;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `msgid` = %i", mc_dbname, message_id);
		results = stmt->executeQuery(query);

		while (results->next()) {
			i_date = results->getInt("timestamp");
			p_from = results->getString("from");
			p_to = results->getString("to");
			p_subject = results->getString("subject");
			p_body = results->getString("body");

			s_date = GProg.UnixTimeToString(i_date);
			GProg.AddCharsToString(s_date, "!", 16);
			GProg.AddCharsToString(p_subject, "!", 25);

			if (sent == true) {
				GProg.AddCharsToString(p_to, "!", 12);
				p_message = s_date + p_to + p_subject + p_body;
			}
			else {
				GProg.AddCharsToString(p_from, "!", 12);
				p_message = s_date + p_from + p_subject + p_body;
			}

		}
		//printf("\n DB.ReadMailRecord - size-message:(%i-%s)\n", strlen(p_message.c_str()), p_message.c_str()); // for heavy debugging

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
	return p_message;
}

int Database::GetMailCount(int p_acctID, int response_status_value)
{
	int message_count = 0;

	const char * mail_status_param;

	mail_status_param = GetMailStatusParam(response_status_value);

	try {
		ConnectDB();

		if (mail_status_param == DBSENT_PARAM) {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `sender_id` = %i AND %s ", mc_dbname, p_acctID, mail_status_param);
			results = stmt->executeQuery(query);
		}
		else {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `account_id` = %i AND %s ", mc_dbname, p_acctID, mail_status_param);
			results = stmt->executeQuery(query);
		}	

		message_count = (int)results->rowsCount();

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
	return message_count;
}

int Database::GetMailLists(int socket_position, int client_socket, int response_status_value, int p_acctID, unsigned int counter_pos, bool ascending)
{
	std::string s_status;
	std::string p_from;
	std::string p_to;
	std::string p_subject;
	std::string s_message_count;
	std::string s_date;
	std::string p_message;
	std::string s_record_status;

	unsigned int message_count = 0;
	int i_status;
	int i_date;

	const char * mail_status_param;

	mail_status_param = GetMailStatusParam(response_status_value);

	try {
		ConnectDB();

		if (mail_status_param == DBSENT_PARAM) {
			if (ascending == true) {
				stmt = con->createStatement();
				query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `sender_id` = %i AND %s ORDER BY `msgid` ASC", mc_dbname, p_acctID, mail_status_param);
				results = stmt->executeQuery(query);
			}
			else {
				stmt = con->createStatement();
				query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `sender_id` = %i AND %s ORDER BY `msgid` DESC", mc_dbname, p_acctID, mail_status_param);
				results = stmt->executeQuery(query);
			}
		}
		else {
			if (ascending == true) {
				stmt = con->createStatement();
				query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `account_id` = %i AND %s ORDER BY `msgid` ASC", mc_dbname, p_acctID, mail_status_param);
				results = stmt->executeQuery(query);
			}
			else {
				stmt = con->createStatement();
				query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `account_id` = %i AND %s ORDER BY `msgid` DESC", mc_dbname, p_acctID, mail_status_param);
				results = stmt->executeQuery(query);
			}
		}

		while (results->next()) {
			++message_count;

			if (message_count > counter_pos + 20) {
				break;
			}

			if (message_count > counter_pos) {
				i_status = results->getInt("status");
				i_date = results->getInt("timestamp");
				p_to = results->getString("to");
				p_from = results->getString("from");
				p_subject = results->getString("subject");

				s_status = std::to_string(i_status);
				s_message_count = std::to_string(message_count);
				s_date = GProg.UnixTimeToString(i_date);
				GProg.AddCharsToString(s_status, "!", 2);
				GProg.AddCharsToString(s_message_count, "!", 2);
				GProg.AddCharsToString(s_date, "!", 16);

				if (mail_status_param == DBSENT_PARAM) {
					GProg.AddCharsToString(p_to, "!", 12);
					p_message = s_status + s_message_count + s_date + p_to + p_subject;
				}
				else {
					GProg.AddCharsToString(p_from, "!", 12);
					p_message = s_status + s_message_count + s_date + p_from + p_subject;
				}

				if (message_count == counter_pos + 1) {
					s_record_status = std::to_string(MAIL_LIST_HEAD);
					ConNet.ContentPackets(CONTENT_1VAL, response_status_value, client_socket, socket_position, s_record_status);
				//	printf("\n GetMailLists-header - counter_pos-count:(%i-%i)\n", counter_pos, message_count); // for heavy debugging
				}				

				ConNet.ContentPackets(CONTENT_MAIL, response_status_value, client_socket, socket_position, p_message);
				// printf("\n GetMailLists - counter_pos-count-size-message:(%i-%i-%i-%s)\n", counter_pos, message_count, strlen(p_message.c_str()), p_message.c_str()); // for heavy debugging
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
	return message_count;
}

void Database::AddToMailArchive(int response_status_value, int acctID)
{
	const char * mail_status_param;
	
	int vec_msgid;

	mail_status_param = GetMailStatusParam(response_status_value);

	try {
		ConnectDB();		

		if (mail_status_param == DBSENT_PARAM) {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `sender_id` = %i AND %s ", mc_dbname, acctID, mail_status_param);
			results = stmt->executeQuery(query);

			while (results->next()) {
				vec_msgid = results->getInt("msgid");
				del_vec.push_back(vec_msgid);
			}
		}
		else {
			stmt = con->createStatement();
			query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `account_id` = %i AND %s ", mc_dbname, acctID, mail_status_param);
			results = stmt->executeQuery(query);

			while (results->next()) {
				vec_msgid = results->getInt("msgid");
				del_vec.push_back(vec_msgid);
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

int Database::SetMailMessageStatus(int response_status_value, int message_id, int acctID)
{
	bool delete_all = false;

	const char * mail_status_param;
	
	int vec_msgid;

	int message_count = 0;
	int status_change;

	if (response_status_value == MAIL_DELETE_ALL_OUTBOX || response_status_value == MAIL_DELETE_ALL_READ || response_status_value == MAIL_DELETE_ALL_UNREAD || response_status_value == MAIL_DELETE_ALL_INBOX) {
		delete_all = true;
	}

	if (delete_all == true) {
		AddToMailArchive(response_status_value, acctID);
	}

	status_change = GetMessageStatChgVal(response_status_value);
	mail_status_param = GetMailStatusParam(response_status_value);

	try {
		ConnectDB();
		
		if (delete_all == true) {
			if (mail_status_param == DBSENT_PARAM) {
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`mail` SET `status` = `status` + %i WHERE `sender_id` = %i AND %s", mc_dbname, status_change, acctID, mail_status_param);
				stmt->execute(query);				
			}
			else {
				stmt = con->createStatement();
				query = File.StringFormat("UPDATE `%s`.`mail` SET `status` = `status` + %i WHERE `account_id` = %i AND %s", mc_dbname, status_change, acctID, mail_status_param);
				stmt->execute(query);
			}
		}
		else {
			stmt = con->createStatement();
			query = File.StringFormat("UPDATE `%s`.`mail` SET `status` = `status` + %i WHERE `msgid` = %i", mc_dbname, status_change, message_id);
			stmt->execute(query);
		}
		message_count = (int)stmt->getUpdateCount();
		delete stmt;
		DisconnectDB();

		if (delete_all == true) {
			for (unsigned int i = 0; i < del_vec.size(); i++) {
				vec_msgid = del_vec.at(i);
				NewMailArchive(vec_msgid);
			}
		}
		else {
			NewMailArchive(message_id);
		}
		del_vec.clear();
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

		message_count = -1;
	}
	return message_count;
}

const char * Database::GetMailStatusParam(int response_status_value)
{
	const char * mail_status_param = "";

	if (response_status_value == MAIL_OUTBOX_SENT || response_status_value == MAIL_DELETE_SENT || response_status_value == MAIL_DELETE_ALL_OUTBOX) {
		mail_status_param = get_sent_param;
	}
	else if (response_status_value == MAIL_DELETE_ALL_INBOX) {		
		mail_status_param = get_allrecv_param;
	}
	else if (response_status_value == MAIL_INBOX_UNREAD || response_status_value == MAIL_DELETE_UNREAD || response_status_value == MAIL_DELETE_ALL_UNREAD) {
		mail_status_param = get_unreadrecv_param;
	}
	else if (response_status_value == MAIL_INBOX_READ || response_status_value == MAIL_DELETE_READ || response_status_value == MAIL_DELETE_ALL_READ) {
		mail_status_param = get_readrecv_param;
	}
	return mail_status_param;	
}

int Database::GetMessageStatChgVal(int response_status_value)
{
//	mail_status_map[READ] = read_rsv;
//	mail_status_map[DELETE_SENDER] = d_sent_rsv;
//	mail_status_map[DELETE_RECV] = d_recv_rsv;

	int status_change;

	/*
	for (unsigned int i = 0; i < read_rsv.size(); i++) {
		if (response_status_value == read_rsv.at(i)) {
			status_change = READ;
			return status_change;
		}
	}
	for (unsigned int i = 0; i < d_sent_rsv.size(); i++) {
		if (response_status_value == d_sent_rsv.at(i)) {
			status_change = DELETE_SENDER;
			return status_change;
		}
	}
	for (unsigned int i = 0; i < d_recv_rsv.size(); i++) {
		if (response_status_value == d_recv_rsv.at(i)) {
			status_change = DELETE_RECV;
			return status_change;
		}
	}
	*/

	if (response_status_value == MAIL_INBOX_UNREAD) {															// unread to read
		status_change = 1;		
	}
	else if (response_status_value == MAIL_DELETE_SENT || response_status_value == MAIL_DELETE_ALL_OUTBOX) {	// delete sent
		status_change = 2;		
	}
	else if (response_status_value == MAIL_DELETE_ALL_INBOX) {													// delete recv read/unread
		status_change = 4;		
	}
	else if (response_status_value == MAIL_DELETE_UNREAD || response_status_value == MAIL_DELETE_ALL_UNREAD) {	// delete recv unread
		status_change = 4;		
	}
	else if (response_status_value == MAIL_DELETE_READ || response_status_value == MAIL_DELETE_ALL_READ) {		// delete recv read
		status_change = 4;		
	}
	return status_change;
}

void Database::NewMailArchive(int message_id)
{
	bool exists = false;

	std::string p_from;
	std::string p_to;
	std::string p_subject;
	std::string s_date;
	std::string p_body;

	int i_date;
	int r_acctid;
	int s_acctid;
	int m_status;

	int success_ct = 0;

	try {
		ConnectDB();

		stmt = con->createStatement();
		query = File.StringFormat("SELECT * FROM `%s`.`mail_archive` WHERE `msgid` = %i", mc_dbname, message_id);
		results = stmt->executeQuery(query);

		if (results->next()) {
			exists = true;
		}

		delete results;
		delete stmt;


		stmt = con->createStatement();
		query = File.StringFormat("SELECT * FROM `%s`.`mail` WHERE `msgid` = %i", mc_dbname, message_id);
		results = stmt->executeQuery(query);

		while (results->next()) {
			r_acctid = results->getInt("account_id");
			p_to = results->getString("to");
			i_date = results->getInt("timestamp");
			s_acctid = results->getInt("sender_id");
			p_from = results->getString("from");
			p_subject = results->getString("subject");
			p_body = results->getString("body");
			m_status = results->getInt("status");
		}

		delete results;
		delete stmt;

		if (exists == true && (m_status == 6 || m_status == 7)) {
			stmt = con->createStatement();
			query = File.StringFormat("UPDATE `%s`.`mail_archive` SET `status` = %i WHERE `msgid` = %i", mc_dbname, m_status, message_id);
			stmt->execute(query);

			delete stmt;

			stmt = con->createStatement();
			query = File.StringFormat("DELETE FROM `%s`.`mail` WHERE `msgid` = %i", mc_dbname, message_id);
			stmt->execute(query);

			delete stmt;
		}
		else if (exists == false) {			
			GProg.AddCharsToString(p_subject, "\\", 0, true);
			GProg.AddCharsToString(p_body, "\\", 0, true);
			
			stmt = con->createStatement();
			query = File.StringFormat("INSERT INTO `%s`.`mail_archive` (`archive_date`, `msgid`, `account_id`, `to`, `timestamp`, `sender_id`, `from`, `subject`, `body`, `status`) VALUES(%i, %i, %i, '%s', %i, %i, '%s', '%s', '%s', %i)"
				, mc_dbname, static_cast<int>(time(0)), message_id, r_acctid, p_to.c_str(), i_date, s_acctid, p_from.c_str(), p_subject.c_str(), p_body.c_str(), m_status);
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

void Database::UpdateLoginCount(int p_acctID, bool admin)
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		if (admin == true) {
			query = File.StringFormat("UPDATE `%s`.`admin_account` SET `content_count` = `content_count` + 1 WHERE `account_id` = %i", mc_dbname, p_acctID);
		}
		else {
			query = File.StringFormat("UPDATE `%s`.`account` SET `content_count` = `content_count` + 1 WHERE `account_id` = %i", mc_dbname, p_acctID);
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
			query = File.StringFormat("SELECT `content_count` FROM `%s`.`admin_account` WHERE `account_id` = %i", mc_dbname, p_acctID);
		}
		else {
			query = File.StringFormat("SELECT `content_count` FROM `%s`.`account` WHERE `account_id` = %i", mc_dbname, p_acctID);
		}

		results = stmt->executeQuery(query);

		while (results->next()) {
			count = results->getInt("content_count");
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
			query = File.StringFormat("UPDATE `%s`.`account` SET `average_session_time` = `total_session_time` / `content_count` WHERE `account_id` = %i", mc_dbname, p_acctID);
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

		time_t creation = (clock() / 1000);

		stmt = con->createStatement();
		query = File.StringFormat("INSERT INTO `%s`.`time` (`creation_date`, `server_time`, `last_login_date`) VALUES(%i, %i, %i)"
			, mc_dbname, static_cast<int>(time(0)), creation, static_cast<int>(time(0)));
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

void Database::CreateMailTable()
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		File.LogRecord(Logs::Normal, Logs::Database, "Creating Table: `mail`");
		query = (
			"CREATE TABLE `mail` ( "
			"`msgid` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT, "
			"`account_id` INT(10) UNSIGNED NOT NULL DEFAULT '0', "
			"`to` VARCHAR(50) NOT NULL DEFAULT '', "
			"`timestamp` INT(11) NOT NULL DEFAULT '0', "
			"`sender_id` INT(10) UNSIGNED NOT NULL DEFAULT '0', "
			"`from` VARCHAR(50) NOT NULL DEFAULT '', "
			"`subject` VARCHAR(60) NOT NULL DEFAULT '', "
			"`body` TEXT NOT NULL, "
			"`status` TINYINT(4) NOT NULL DEFAULT '0', "
			"PRIMARY KEY(`msgid`), "
			"INDEX `account_id` (`account_id`)"
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

void Database::CreateMailArchiveTable()
{
	try {
		ConnectDB();

		stmt = con->createStatement();
		File.LogRecord(Logs::Normal, Logs::Database, "Creating Table: `mail_archive`");
		query = (
			"CREATE TABLE `mail_archive` ( "
			"`archive_id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT, "
			"`archive_date` INT(11) NOT NULL DEFAULT '0', "
			"`msgid` INT(10) UNSIGNED NOT NULL, "
			"`account_id` INT(10) UNSIGNED NOT NULL DEFAULT '0', "
			"`to` VARCHAR(50) NOT NULL DEFAULT '', "
			"`timestamp` INT(11) NOT NULL DEFAULT '0', "
			"`sender_id` INT(10) UNSIGNED NOT NULL DEFAULT '0', "
			"`from` VARCHAR(50) NOT NULL DEFAULT '', "
			"`subject` VARCHAR(60) NOT NULL DEFAULT '', "
			"`body` TEXT NOT NULL, "
			"`status` TINYINT(4) NOT NULL DEFAULT '0', "
			"PRIMARY KEY(`archive_id`, `msgid`), "
			"INDEX `archive_id` (`archive_id`)"
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
