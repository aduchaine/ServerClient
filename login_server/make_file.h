/*
3/7/16

begin) first original header file - to create original files
*/

#ifndef MAKE_FILE_H
#define MAKE_FILE_H

#include <mutex>


namespace Logs {
	enum LogLevel {
		Normal = 1,		// low debug level
		Med,			// medium debug level
		High			// highest debug level
	};

	enum Category {
		None = 0,
		NetError,
		Error,
		Database,
		LoginPacket,
		ContentPacket,
		Account,
		General,
		Admin,
		MaxCategoryID
	};

	static const char* CategoryName[Category::MaxCategoryID] = {
		"",
		"NETERROR",
		"ERROR",
		"Database",
		"LoginPacket",
		"ContentPacket",
		"Account",
		"General",
		"Admin"
	};
}


class MakeFile
{

public:

	// new - this does
	struct LogSettings {
		int log_to_file;
		int is_category_enabled;
	};

	// new, don't quite understand
	LogSettings log_settings[Logs::Category::MaxCategoryID];

	// main files
	void NewSessionLog();
	void EndSessionLog();
	void OpenSessionLog(bool reopen = false); // false implies a new session
	void LogRecord(Logs::LogLevel log_level, int log_category, std::string message, ...);

	// user files
	void StartUserLogs(const char * p_name, bool user_log, bool main_log);
	void CloseUserLogs(const char * p_name, bool user_log, bool main_log);
	void UserRecord(const char * p_name, std::string message, ...);

	// admin files
	void StartAdminLogs(const char * p_name, bool admin_log, bool main_log);
	void CloseAdminLogs(const char * p_name, bool admin_log, bool main_log);
	void AdminRecord(const char * p_name, std::string message, ...);

	// general
	void DefaultLogSettings(); // this is used at startup to set log settings to what this function says
	void CreateLogDirectories(); // doesn't seem to affect anything if the directories already exist
	std::string MessageFormat(int log_category, const std::string &in_message); // new
	const std::string StringFormat(const char * format, ...);	
	const std::string vStringFormat(const char * format, va_list args);
	void TimeStamp(char * time_stamp);
	void MakeDirectory(const std::string & directory_name);
	
};

extern MakeFile File;

extern std::mutex mutexcl;

#endif
