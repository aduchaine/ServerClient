/*
3/12/16

gen_program.h
*/

#ifndef GEN_PROGRAM_H
#define GEN_PROGRAM_H

#pragma once

#include <sstream>
#include <ctime>


#define GEN_TIMER 3000					// current use is to startup content server connections - default 3000
#define RESTART_CONN_TIMER 2000			// re-start server connections if not shutdown properly - the counter(5) X this value should coincide roughly with REESTABLISH_CONN_TIMER in content - default 2000
#define CLIENT_ACTIVITY 12000			// current use is to check all current connections for activity - default 12000
#define SERVER_ACTIVITY 15000			// current use is to check all current connections for activity - default 15000
#define INACTIVE_CHECK 60000			// used to do certain things if this condition is met within the function - should be 5 X CLIENT_ACTIVITY
#define COMPROMISED_ACCOUNT 120000		// this value is inserted into the DB on possible compromised account - default 120000
#define LOCKOUT_TIMER 60000				// this value is inserted into the DB on failed login - default 60000
#define IMPROPER_LOGOUT 90000			// this value is inserted into the DB on server startup for account improperly logged out - default 90000
#define BAD_LOG_TIMER 12000				// when checked, send packet to trigger content server to check "User_struct" and send active acctIDs to login server - default 12000
#define BAD_LOG_WAIT 3000				// grace period of sorts to recv all packets from content server regarding bad log accounts - default 3000 (certain it could be less)

#define ALPHA_CHAR "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUMERIC_CHAR "0123456789"
#define SPECIAL_CHARS "`~!@#$%^&*()_+-=[]{}|;:\",./?><"		// escape char included for "
#define RESTRICTED_CHARS "\\''"								// escape char included for \ ' - these are not included in special char
#define NO_CHAR ""
#define LOGIN_CHAR "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890"	// curent acceptible login name/password chars - could expand for password
#define DB_CHARS "`~!@#$%^&*()_+-=[]{}|;:\",./?><abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890" // escape char included for " - this is not needed for the DB

// possible inclusions
// const char * Chat_char = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890 `~!@#$%^&*()-_=+[]{}|;',./<>?:\"";
// const char * Mail_char = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890 `~!@#$%^&*()-_=+[]{}|;',./<>?:\"";


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

	// initial random number generator functions - max ran is 32767, signed
	void ReseedRNG();
	int Random(int number);
	int Random100();
	int Random10000();
	bool RollNumber(const int what, int number); // <what> could be a constant value for something often defined to make roll become a % chance - ie. flipping a coin-> what = 1 number = 2
	bool Roll100(const int what);
	bool Roll10000(const int what);
	std::string RandomInttoString();

};

extern GenProgram GProg;

#endif
