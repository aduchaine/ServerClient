/*
3/12/16

make_file.cpp
*/

#include "stdafx.h"

#include <sys/stat.h>
#include <direct.h>
#include <fstream>
#include <mutex>

#include "content_net.h"
#include "content_packet.h"

#include "account.h"
#include "gen_program.h"
#include "make_file.h"


std::ofstream chat_file;
std::ofstream content_file;
std::ofstream user_log;
std::ofstream admin_log;

std::mutex mutexcl;


// need to get an admin command system in place to be able to change things on the fly, like the log settings below
void MakeFile::DefaultLogSettings()
{	
	memset(log_settings, 0, sizeof(LogSettings)* Logs::Category::MaxCategoryID); // zero out array(memory)

	// this sets the log level (Normal = 1, Med = 2, High = 3) - ofc, doing it as admin would be helpful
	log_settings[Logs::NetError].log_to_file = Logs::High;
	log_settings[Logs::Error].log_to_file = Logs::High;
	log_settings[Logs::Database].log_to_file = Logs::High;
	log_settings[Logs::LoginPacket].log_to_file = Logs::High;
	log_settings[Logs::ContentPacket].log_to_file = Logs::High;
	log_settings[Logs::Account].log_to_file = Logs::High;
	log_settings[Logs::General].log_to_file = Logs::High;
	log_settings[Logs::Admin].log_to_file = Logs::High;
}

void MakeFile::NewSessionLog()
{
	OpenSessionLog();
	std::cout << "\nContent server started\n" << std::endl;
	File.LogRecord(Logs::Normal, Logs::General, "Content server started");
	std::cout << "\n\tLog files started..." << std::endl;
}

void MakeFile::LogRecord(Logs::LogLevel log_level, int log_category, std::string message, ...)
{
	if (log_settings[log_category].log_to_file == 0)
		return;
	if (log_settings[log_category].log_to_file < log_level)
		return;

	char time_stamp[80];
	TimeStamp(time_stamp);

	va_list args;
	va_start(args, message);
	std::string output_message = vStringFormat(message.c_str(), args);
	va_end(args);

	std::string output_log_message = MessageFormat(log_category, output_message);

	content_file << time_stamp << " " << output_log_message << std::endl;
}

void MakeFile::EndSessionLog()
{
	if (!content_file.is_open()) {
		OpenSessionLog(true);
	}
	LogRecord(Logs::Normal, Logs::General, "Content server shutdown");
	content_file.close();
}

void MakeFile::OpenSessionLog(bool reopen)
{
	if (content_file.is_open()) {
		content_file.close();
	}
	if (reopen == true) {
		for (int j = 1; j < 100000; ++j) {
			std::string filename = std::to_string(j);
			std::string filename2 = std::to_string(j - 1);
			const char * mc_filename = filename.c_str();
			const char * mc_filename2 = filename2.c_str();

			if (SERVER_CONNECTION == 1) {
				if (std::ifstream(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/content/content_file_%s.log", mc_filename))) {
					continue;
				}
				else {
					content_file.open(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/content/content_file_%s.log", mc_filename2), std::ios_base::app | std::ios_base::out);
					return;
				}
			}
			else {
				if (std::ifstream(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/content/content_file_%s.log", mc_filename))) {
					continue;
				}
				else {
					content_file.open(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/content/content_file_%s.log", mc_filename2), std::ios_base::app | std::ios_base::out);
					return;
				}
			}
			
		}
	}
	else {
		for (int i = 1; i < 100000; ++i) {
			std::string filename = std::to_string(i);
			const char* mc_filename = filename.c_str();

			if (SERVER_CONNECTION == 1) {
					if (std::ifstream(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/content/content_file_%s.log", mc_filename))) {
					continue;
				}
				else {
					content_file.open(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/content/content_file_%s.log", mc_filename), std::ios_base::app | std::ios_base::out);
					return;
				}
			}
			else {
				if (std::ifstream(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/content/content_file_%s.log", mc_filename))) {
					continue;
				}
				else {
					content_file.open(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/content/content_file_%s.log", mc_filename), std::ios_base::app | std::ios_base::out);
					return;
				}
			}

		}
	}
}

void MakeFile::StartUserLogs(const char * p_name, bool user_log, bool main_log)
{
	if (user_log == true) {
		UserRecord(p_name, "Content: User login");
	}
	if (main_log == true) {
		LogRecord(Logs::Normal, Logs::General, "User login(%s)", p_name);
	}
}

void MakeFile::UserRecord(const char * p_name, std::string message, ...)
{
	mutexcl.lock();

	if (user_log.is_open()) {
		user_log.close();
	}

	if (SERVER_CONNECTION == 1) {
		user_log.open(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/users/%s.log", p_name), std::ios_base::app | std::ios_base::out);
	}
	else {
		user_log.open(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/users/%s.log", p_name), std::ios_base::app | std::ios_base::out);
	}

	char time_stamp[80];
	TimeStamp(time_stamp);

	va_list args;
	va_start(args, message);
	std::string output_message = vStringFormat(message.c_str(), args);
	va_end(args);

	user_log << time_stamp << " " << output_message << std::endl;
	user_log.close();

	mutexcl.unlock();
}

void MakeFile::CloseUserLogs(const char * p_name, bool user_log, bool main_log)
{
	if (user_log == true) {
		UserRecord(p_name, "Content: User logout");
	}
	if (main_log == true) {
		LogRecord(Logs::Normal, Logs::General, "User logout(%s)", p_name);
	}
}

void MakeFile::StartAdminLogs(const char * p_name, bool admin_log, bool main_log)
{
	if (admin_log == true) {
		AdminRecord(p_name, "Content: Admin login");
	}
	if (main_log == true) {
		LogRecord(Logs::Normal, Logs::Admin, "Admin login(%s)", p_name);
	}	
}

void MakeFile::AdminRecord(const char * p_name, std::string message, ...)
{
	mutexcl.lock();

	if (admin_log.is_open()) {
		admin_log.close();
	}

	if (SERVER_CONNECTION == 1) {
		admin_log.open(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/admin/%s_admin.log", p_name), std::ios_base::app | std::ios_base::out);
	}
	else {
		admin_log.open(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/admin/%s_admin.log", p_name), std::ios_base::app | std::ios_base::out);
	}

	char time_stamp[80];
	TimeStamp(time_stamp);

	va_list args;
	va_start(args, message);
	std::string output_message = vStringFormat(message.c_str(), args);
	va_end(args);

	admin_log << time_stamp << " " << output_message << std::endl;
	admin_log.close();

	mutexcl.unlock();
}

void MakeFile::CloseAdminLogs(const char * p_name, bool admin_log, bool main_log)
{
	if (admin_log == true) {
		AdminRecord(p_name, "Content: Admin logout");
	}
	if (main_log == true) {
		LogRecord(Logs::Normal, Logs::Admin, "Admin logout(%s)", p_name);
	}	
}

void MakeFile::NewChatLog()
{
	if (OpenChatLog(true) == true) {
		ChatRecord("Server Login: New chat file started.");
		File.LogRecord(Logs::Normal, Logs::General, "New chat file started.");
	}
	else {
		ChatRecord("Server Login: Resuming chat file.");
		File.LogRecord(Logs::Normal, Logs::General, "Resuming chat file.");
	}
	std::cout << "\n\tChat file started..." << std::endl;
}

void MakeFile::EndChatLog()
{
	if (OpenChatLog(false) == true) {
		ChatRecord("New chat file started.");
		File.LogRecord(Logs::Normal, Logs::General, "New chat file started.");
	}
	ChatRecord("Server Logout: End chat log.");
	LogRecord(Logs::Normal, Logs::General, "End chat log.");
	chat_file.close();
}


// format the file name with a date/time stamp - need to change where this happens on startup or add a startup flag <- this
bool MakeFile::OpenChatLog(bool startup)
{
	if (chat_file.is_open()) {
		chat_file.close();
	}	

	bool new_log = false;

	std::string::size_type sz;
	unsigned int i;
	int ih_time;
	int i_time;
	std::string s_int;
	std::string s_time_stamp;
	std::string h_time;
	const char * mc_filename2;

	char time_stamp[80];
	ChatFileTime(time_stamp);	
	
	const char * mc_filename = time_stamp;
	bool is_timer_enabled = (ConNet.IsChatTimerEnabled() == true);

	if (SERVER_CONNECTION == 1) {
		if (is_timer_enabled == true || startup ==  true) { // open the last file created - a new day will automatically create a new file
			s_time_stamp = time_stamp;
			h_time = s_time_stamp.substr(11, 2);			
			ih_time = stoi(h_time, &sz);			

			for (i = ih_time; i < 24; i--) {
				s_int = std::to_string(i);

				if (strlen(s_int.c_str()) == 1) {
					s_int = "0" + s_int;
					s_time_stamp.replace(11, 2, s_int);
				}
				else {
					s_time_stamp.replace(11, 2, s_int);
				}
				mc_filename2 = s_time_stamp.c_str();
				i_time = stoi(s_int, &sz);

				if (std::ifstream(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename2)) && ih_time == i_time) {
					chat_file.open(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename2), std::ios_base::app | std::ios_base::out);
					return new_log;
				}
			}
			chat_file.open(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename), std::ios_base::app | std::ios_base::out);
			new_log = true;
		}
		else if (std::ifstream(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename))) {
			chat_file.open(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename), std::ios_base::app | std::ios_base::out);
		}
		else {
			chat_file.open(StringFormat("C:/Users/Owner/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename), std::ios_base::app | std::ios_base::out);
			new_log = true;
		}
	}
	else {
		if (is_timer_enabled == true || startup == true) { // open the last file created - a new day will automatically create a new file
			s_time_stamp = time_stamp;
			h_time = s_time_stamp.substr(11, 2);			
			ih_time = stoi(h_time, &sz);			

			for (i = ih_time; i < 24; i--) {
				s_int = std::to_string(i);

				if (strlen(s_int.c_str()) == 1) {
					s_int = "0" + s_int;
					s_time_stamp.replace(11, 2, s_int);
				}
				else {
					s_time_stamp.replace(11, 2, s_int);
				}
				mc_filename2 = s_time_stamp.c_str();
				i_time = stoi(s_int, &sz);

				// printf("\n start time = %i new ts %s ", ih_time, s_time_stamp.c_str()); // for heavy debugging

				if (std::ifstream(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename2)) && ih_time == i_time) {
					chat_file.open(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename2), std::ios_base::app | std::ios_base::out);
					return new_log;
				}
			}
			chat_file.open(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename), std::ios_base::app | std::ios_base::out);
			new_log = true;
		}
		else if (std::ifstream(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename))) {
			chat_file.open(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename), std::ios_base::app | std::ios_base::out);
		}
		else {
			chat_file.open(StringFormat("C:/Users/Dude/Documents/Test programs/Novice/logs/chat/chat_%s.log", mc_filename), std::ios_base::app | std::ios_base::out);
			new_log = true;
		}
	}
	return new_log;
}

// hopefully the mutexes will guard against simultaneous writes and the next one in line will wait to write
void MakeFile::ChatRecord(std::string message, ...)
{
	mutexcl.lock();

	if (chat_file.is_open()) {
		chat_file.close();
	}

	bool new_log = false;

	if (OpenChatLog(false) == true) {		
		new_log = true;		
	}

	ConNet.StartChatIdleTimer();
	
	char time_stamp[80];
	TimeStamp(time_stamp);

	va_list args;
	va_start(args, message);
	std::string output_message = vStringFormat(message.c_str(), args);
	va_end(args);

	if (new_log == true) {
		chat_file << time_stamp << " New chat file started." << std::endl;
		File.LogRecord(Logs::Normal, Logs::General, "New chat file started.");
	}
	chat_file << time_stamp << " " << output_message << std::endl;
	chat_file.close();

	mutexcl.unlock();
}

void MakeFile::CreateLogDirectories()
{
	//	MakeDirectory("C:/Test programs/"); // this works to create a directory
	//	MakeDirectory("C:/Test programs/Novice/");
	//	MakeDirectory("C:/Test programs/Novice/logs/");
	if (SERVER_CONNECTION == 1) {
		MakeDirectory("C:/Users/Owner/Documents/Test programs/Novice/logs/content/");
		MakeDirectory("C:/Users/Owner/Documents/Test programs/Novice/logs/users/");
		MakeDirectory("C:/Users/Owner/Documents/Test programs/Novice/logs/admin/");
		MakeDirectory("C:/Users/Owner/Documents/Test programs/Novice/logs/chat/");
	}
	else {
		MakeDirectory("C:/Users/Dude/Documents/Test programs/Novice/logs/content/");
		MakeDirectory("C:/Users/Dude/Documents/Test programs/Novice/logs/users/");
		MakeDirectory("C:/Users/Dude/Documents/Test programs/Novice/logs/admin/");
		MakeDirectory("C:/Users/Dude/Documents/Test programs/Novice/logs/chat/");
	}
}

std::string MakeFile::MessageFormat(int log_category, const std::string &in_message)
{
	std::string category_string;
	if (log_category > 0 && Logs::CategoryName[log_category])
		category_string = StringFormat("[%s] ", Logs::CategoryName[log_category]);
	return StringFormat("%s%s", category_string.c_str(), in_message.c_str());
}

const std::string MakeFile::StringFormat(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	std::string output = vStringFormat(format, args);
	va_end(args);
	return output;
}

const std::string MakeFile::vStringFormat(const char * format, va_list args)
{
	std::string output;
	va_list tmpargs;

	va_copy(tmpargs, args);
	int characters_used = vsnprintf(nullptr, 0, format, tmpargs);
	va_end(tmpargs);

	if (characters_used > 0) {
		output.resize(characters_used + 1);

		va_copy(tmpargs, args);
		characters_used = vsnprintf(&output[0], output.capacity(), format, tmpargs);
		va_end(tmpargs);

		output.resize(characters_used);

		if (characters_used < 0)
			output.clear();
	}
	return output;
}

void MakeFile::TimeStamp(char * time_stamp)
{
	time_t raw_time;
	struct tm * time_info;
	time(&raw_time);
	time_info = localtime(&raw_time);
	strftime(time_stamp, 80, "[%m-%d-%Y :: %H:%M:%S]", time_info);
}

void MakeFile::ChatFileTime(char * time_stamp)
{
	time_t raw_time;
	struct tm * time_info;
	time(&raw_time);
	time_info = localtime(&raw_time);
	strftime(time_stamp, 80, "%m-%d-%Y_%Hh", time_info);
}

void MakeFile::MakeDirectory(const std::string &directory_name)
{
#ifndef _WINDOWS
	struct _stat st;
	if (_stat(directory_name.c_str(), &st) == 0) // exists
		return;
	_mkdir(directory_name.c_str());
#endif
}
