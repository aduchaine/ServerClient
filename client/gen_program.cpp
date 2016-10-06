/*
3/12/16

account.cpp
*/

#include "stdafx.h"

#include <algorithm>
#include <conio.h>
#include <ctime>
#include <vector>

#include "login_packet.h"

#include "gen_program.h"
#include "login.h"
#include "text_output.h"


bool typing = false;
bool lock_input = false;
bool mask_input = false;

bool do_process = false;

int menu_stat = 0;

std::vector<char> c_key;
const std::vector<int> masking_status = { CHECK_PASSWORD_OLD, CHECK_PASSWORD_NEW_VALIDITY, CHECK_PASSWORD_PW_CHG_LOGIN, ADMIN_LOGIN, ADMIN_PW_LOGIN, CHECK_PASSWORD_PW_CHG_VALIDITY,
									CHECK_PASSWORD_PW_CHG_VALIDITY2, CHECK_PASSWORD_NEW_VALIDITY2, CHECK_PASSWORD_PW_CHG_LOGIN2 };


void GenProgram::DisplayTime()
{
	std::time_t result = std::time(nullptr);
	std::cout << "" << std::asctime(std::localtime(&result)) << std::endl;
}

std::string GenProgram::CheckPromptInput(std::string p_prompt)
{
	std::transform(p_prompt.begin(), p_prompt.end(), p_prompt.begin(), ::tolower);
	return p_prompt;
}

const char * GenProgram::CharOutput(const std::string & s_input)
{
	const char* m_output = s_input.c_str();

	return m_output;
}

// heavily modified but originally courtesy of http://stackoverflow.com/questions/5891610/how-to-remove-characters-from-a-string
void GenProgram::RemoveCharsFromString(std::string & string, char * charsToRemove)
{
	unsigned int c_pos = 0;		
	int c_pos1 = 0;
	int c_pos2 = 0;
	int count = 0;

	std::vector<int> char_pos;

	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {	
		if (charsToRemove == "\\") {
			for (c_pos = 0; c_pos < string.size(); c_pos++) {
				if (string[c_pos] == '\\') {
					char_pos.push_back(c_pos);
				}
			}
			while (char_pos.size() > 1) {
				for (unsigned int j = 0; j < char_pos.size(); j++) {
					c_pos1 = char_pos.at(0) + count;
					c_pos2 = char_pos.at(1) + count;
					count--;

					if (c_pos1 + 1 != c_pos2) {
						string.erase(string.begin() + c_pos1);
						char_pos.erase(char_pos.begin() + 0);
						break;
					}
					else if (c_pos1 + 1 == c_pos2) {
						string.erase(string.begin() + c_pos1);
						char_pos.erase(char_pos.begin() + 1);
						char_pos.erase(char_pos.begin() + 0);
						break;
					}
				}
			}
			if (char_pos.size() == 1) {
				for (unsigned int j = 0; j < char_pos.size(); j++) {
					c_pos1 = char_pos.at(0) + count;
					string.erase(string.begin() + c_pos1);
					char_pos.clear();
				}
			}
			char_pos.clear();
		}
		else {
			string.erase(remove(string.begin(), string.end(), charsToRemove[i]), string.end());
		}
	}	
}

void GenProgram::AddCharsToString(std::string & string, char * charsToAdd, int total_chars, bool middle_insert)
{
	int length_diff;

	unsigned int c_pos = 0;
	int c_pos1 = 0;
	int c_pos2 = 0;
	int count = 0;

	std::vector<int> char_pos;

	if (middle_insert == false) {
		length_diff = total_chars - strlen(string.c_str());
		for (int j = 0; j < length_diff; j++) {
			string += charsToAdd;
		}
	}
	else {
		if (charsToAdd == "\\") {
			for (c_pos = 0; c_pos < string.size(); c_pos++) {
				if (string[c_pos] == '\\' || string[c_pos] == '\'') {
					char_pos.push_back(c_pos);
				}
			}
			while (char_pos.size() > 0) {
				for (unsigned int j = 0; j < char_pos.size(); j++) {
					c_pos1 = char_pos.at(0) + count;
					count++;

					string.insert(c_pos1, charsToAdd);
					char_pos.erase(char_pos.begin() + 0);
				}
			}
			char_pos.clear();
		}
	}
}

void GenProgram::RemoveString(std::string message, bool newline)
{
	int message_len = strlen(message.c_str());

	for (int i = 0; i < message_len; ++i) {
		std::cout << '\b'; // back one char
		std::cout << " "; // write space - up one char
		std::cout << '\b'; // back to beginning of space char
	}
	if (newline == true) {
		std::cout << std::endl;
	}
}

// 0 = passed, 1 = badchar, 2 = badlength
int GenProgram::IsValidInput(std::string p_input, bool check_acceptable, bool check_restricted, bool check_length, const char * valid_chars, const char * invalid_chars, int min_length, int max_length)
{
	int passed_check = 0;

	std::string check_char(p_input);

	const unsigned int maxcharlength = max_length;
	const unsigned int mincharlength = min_length;

	if (check_acceptable == true) {
		if (check_char.find_first_not_of(valid_chars) != std::string::npos) {
			passed_check = 1;
		}
	}
	if (check_restricted == true) {
		if (check_char.find_first_of(invalid_chars) != std::string::npos) {
			passed_check = 1;
		}
	}
	if (check_length == true) {
		if (p_input.size() > maxcharlength || p_input.size() < mincharlength) {
			passed_check = 2;
		}
	}
	return passed_check;
}

// windows-specific utilities - ideas courtesy of http://www.cplusplus.com/forum/general/74380/
void GenProgram::GotoConsoleCoords(int column, int line)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int GenProgram::GetConsoleXpos()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
		return -1;
	}
	return csbi.dwCursorPosition.X;
}

int GenProgram::GetConsoleYpos()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
		return -1;
	}
	return csbi.dwCursorPosition.Y;
}

bool GenProgram::InitiateUserInput(char c, bool key_prompt, char required_key)
{
	if (lock_input == true) {
		return false;
	}
	if (typing == true) {
		return false;
	}

	if (_kbhit()) {
		if (key_prompt == true) {
			c = _getch();

			if (c == required_key) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return true;
		}
	}
	return false;
}

void GenProgram::ProcessUserInput(bool key_prompt, bool remove_prompt)
{
	char key_press = ' ';

	if (key_prompt == true) {
		if (InitiateUserInput(key_press, true, CHAT_REQ_KEY) == true) {
			if (remove_prompt == true) {
				GProg.RemoveString(prmt_to_remove, false);
			}
			typing = true;
		}
	}	
	else if (InitiateUserInput(key_press, false) == true) {		
		if (remove_prompt == true) {
			GProg.RemoveString(prmt_to_remove, false);
		}
		typing = true;
	}
	//	printf(" PUI InitiateUserInput key_prompt-remove_prompt:(%i-%i)\n", key_prompt, remove_prompt); // for heavy debugging
	if (key_prompt == true) {
		char key_press2 = ' ';
		if (GProg.HandleInput(key_press2, mask_input) == true) {
			do_process = true;
			lock_input = true;
		}
	}
	else if (GProg.HandleInput(key_press, mask_input) == true) {
		do_process = true;
		lock_input = true;
	}
	//	printf(" PUI HandleInput key_prompt-remove_prompt:(%i-%i) p_input:(%s) \n", key_prompt, remove_prompt, GProg.CharOutput(p_input)); // for heavy debugging
}

bool GenProgram::HandleInput(unsigned char c, bool enable_masking)
{
	if (lock_input == true) {
		return false;
	}
	if (typing == false) {
		return false;
	}
	if (do_process == true) {
		return false;
	}

	std::string s;
	std::string s_line;
	std::string string_line;

	if (_kbhit()) {
		c = _getch();

		if (c == '\r') { // enter workaround
			c = '\n';
			c_key.push_back(c);

			std::string s_line(c_key.begin(), c_key.end());
			for (size_t j = 0; j < s_line.length(); j++) {
				if (s_line[j] == '\n') {
					p_input = s_line.substr(0, j);
				}
			}
			c_key.clear();
			return true;
		}
		else if (c == '\b') { // backspace
			if (GetConsoleXpos() == 0) {
				return false;
			}
			else {
				c_key.erase(c_key.end() - 1);
				std::cout << "\b \b";
				return false;
			}
		}
		else if (c == 9) { // tab key
			return false;
		}
		else if (c == 0 || c == 224) { // f-keys and other cursor function keys add an additional char to _getch()
			c = _getch();
			return false;
		}
		else {
			if (c == '\\') {
				c = '\\';
				c_key.push_back(c);
				c = '\\';
				s = std::string(1, c);
				c_key.push_back(c);
			}
			else if (c == '\'') {
				c = '\\';
				c_key.push_back(c);
				c = '\'';
				s = std::string(1, c);
				c_key.push_back(c);
			}
			else {
				s = std::string(1, c);
				c_key.push_back(c);
			}

			if (enable_masking == true) {
				std::cout << "*";			
			}
			else {
				std::cout << s;
			}
			return false;
		}
	}
	return false;
}

bool GenProgram::RollNumber(const int what, int number)
{
	if (GProg.Random(number) < what) {
		return true;
	}
	else {
		return false;
	}
}

bool GenProgram::Roll100(const int what)
{
	if (GProg.Random100() < what) {
		return true;
	}
	else {
		return false;
	}
}

bool GenProgram::Roll10000(const int what)
{
	if (GProg.Random10000() < what) {
		return true;
	}
	else {
		return false;
	}
}

int GenProgram::Random(int number)
{
	return rand() % number;
}

int GenProgram::Random100()
{
	return rand() % 100;
}

int GenProgram::Random10000()
{
	return rand() % 10000;
}

void GenProgram::ReseedRNG()
{
	if (rand() % 3 < 2) {
		if (rand() % 3 < 2) {
			return;
		}
		else {
			std::srand(clock() + 5);
		}
	}
	else {
		std::srand(clock());
	}
}
