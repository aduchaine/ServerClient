/*
3/12/16

gen_program.h
*/

#ifndef GEN_PROGRAM_H
#define GEN_PROGRAM_H

#pragma once

#include <sstream>
#include <ctime>


#define RESTART_CONN_TIMER 2000			// current use is to startup content server connection - default 2000
#define REESTABLISH_CONN_TIMER 12000	// wait for server connections if not shutdown properly - this value should coincide roughly with RESTART_CONN_TIMER in login - default 12000
#define CLIENT_ACTIVITY 24000			// sends check_activity packets to client at this interval after INACTIVE_CHECK interval - default 24000
#define INACTIVE_CHECK 120000			// used to do certain things if this condition is met within the function - should be 5 X CLIENT ACTIVITY
#define SERVER_TIME_UPDATE 30000		// will regularly update the server time in the DB after this amount of ms - default 30000
#define CHECK_MESS_EXPIRATION 3000		// amount of time between message-time-user purges to allow the user more chatting if at or above MAX_MESSAGES(below) - default 3000
#define IDLE_CHAT 180000				// purpose is to keep conversations continuous in the log file - after this amount of time, chat is considered idle - default 180000

#define MESSAGE_EXPIRATION 60			// amount of time until a chat message will drop off from list - this is not on a timer and is in seconds - default 60
#define MAX_MESSAGES 10					// defines max # of messages within MESSAGE_EXPIRATION(above) period of time

#define ALPHA_CHAR "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUMERIC_CHAR "0123456789"
#define SPECIAL_CHARS "`~!@#$%^&*()_+-=[]{}\"|;:,./?><"	// escape char included for "
#define RESTRICTED_CHARS "\'\\"							// escape char included for \ ' - these are not included in special char
#define NO_CHAR ""
#define LOGIN_CHAR "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890"	// curent acceptible login name/password chars - could expand for password
#define DB_CHARS "`~!@#$%^&*()_+-=[]{}|;:\",./?><abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890" // escape char included for " - this is not needed for the DB
#define ALL_CHARS "`~!@#$%^&*()_+-=[]{}|;:\\\'\",./?><abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890" // escape char included for " \ ' - this should work for DB


class GenProgram
{

public:

	// general
	void Enter();
	void Exit();

	// utilities
	std::string CheckPromptInput(std::string p_prompt);
	const char * CharOutput(const std::string & p_input);
	void RemoveCharsFromString(std::string & string, char * charsToRemove);
	void AddCharsToString(std::string & string, char * charsToAdd, int total_chars, bool middle_insert = false);
	int IsValidInput(std::string p_input, bool check_acceptable, bool check_restricted, bool check_length, const char * valid_chars, const char * invalid_chars, int min_length, int max_length);	
	std::string UnixTimeToString(time_t unix_seconds);
	void DisplayTime();

	// initial random number generator functions - max ran is 32767
	void ReseedRNG();
	int Random(int number);
	int Random100();
	int Random10000();
	bool RollNumber(const int what, int number); // <what> could be a constant value for something often defined to make roll become a % chance - ie. flipping a coin-> what = 1 number = 2
	bool Roll100(const int what);
	bool Roll10000(const int what);

};

extern GenProgram GProg;

#endif
