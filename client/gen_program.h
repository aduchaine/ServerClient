/*
3/12/16

gen_program.h
*/

#ifndef GEN_PROGRAM_H
#define GEN_PROGRAM_H


#pragma once

#include <sstream>
#include <vector>


#define CONNECTION_TIMER 5000			// check connection for server timeouts - any recv packet should reset/disable the timer
#define LOCKOUT_TIMER 60000				// this value is only for reference in the client and won't be needed when server can send the time amount
#define CHAT_PROBATION 60000			// timer value, when expired, sends a packet to content to allow more flexibility in # of chat messages - default 59000/60000		

#define ALPHA_CHAR "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUMERIC_CHAR "0123456789"
#define SPECIAL_CHARS "`~!@#$%^&*()_+-=[]{}|;:\",./?><"		// escape char included for "
#define RESTRICTED_CHARS "\'\\"								// escape char included for \ ' - these are not included in special char
#define NO_CHAR ""
#define LOGIN_CHAR "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890"	// curent acceptible login name/password chars - could expand for password
#define DB_CHARS "`~!@#$%^&*()_+-=[]{}|;:\",./?><abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890" // escape char included for " - this is not needed for the DB

#define CHAT_REQ_KEY '\r'

class GenProgram
{

public:

	// utilities
	void DisplayTime();
	std::string CheckPromptInput(std::string p_prompt);
	const char * CharOutput(const std::string & s_input);
	void RemoveCharsFromString(std::string & string, char * charsToRemove);	// removes specific chars from a string, shortening the string	
	void AddCharsToString(std::string & string, char * charsToAdd, int total_chars, bool middle_insert = false);
	void RemoveString(std::string message, bool newline); // removes entire string passed with \n option	
	int IsValidInput(std::string p_input, bool check_acceptable, bool check_restricted, bool check_length, const char * valid_chars, const char * invalid_chars, int min_length, int max_length);

	// windows-specific utilities - altered but courtesy of http://www.cplusplus.com/forum/general/74380/
	// conio.h(_getch()) not supported by other platforms and not recommended to use, apparently
	void GotoConsoleCoords(int column, int line);
	int GetConsoleXpos();
	int GetConsoleYpos();

	// new general user input functions
	void ProcessUserInput(bool key_prompt, bool remove_prompt);
	bool InitiateUserInput(char c, bool key_prompt, char required_key = ' ');	// work on passing the "required_key" variable
	bool HandleInput(unsigned char c, bool enable_masking);
	
	// initial random number generator functions - max ran is 32767 - most of these will not be needed in client
	void ReseedRNG();
	int Random(int number);
	int Random100();
	int Random10000();
	bool RollNumber(const int what, int number); // <what> could be a constant value for something often defined to make roll become a % chance - ie. flipping a coin-> what = 1 number = 2
	bool Roll100(const int what);
	bool Roll10000(const int what);

};

extern GenProgram GProg;

extern std::vector<char> c_key;

extern bool typing;
extern bool lock_input;
extern bool do_process;

extern std::string p_input;

#endif
