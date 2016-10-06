/*
3/12/16

account.cpp
*/

#include "stdafx.h"

#include <algorithm>

#include "login_net.h"

#include "database.h"
#include "gen_program.h"
#include "make_file.h"


void GenProgram::Enter()
{
	File.DefaultLogSettings();	
	File.CreateLogDirectories();	
	File.NewSessionLog();	

	DB.CheckDatabaseTables();
	DB.ResetLockedAcct();
	DB.UpdateLockoutStatus();
	DB.CheckBadLogAccounts();
	LogNet.DoBadLogAccounts();

	std::cout << "\t\t\t ";
	GProg.DisplayTime();
	return;
}

void GenProgram::Exit()
{	
	File.EndSessionLog();
}

std::string GenProgram::CheckPromptInput(std::string p_prompt)
{
	std::transform(p_prompt.begin(), p_prompt.end(), p_prompt.begin(), ::tolower);
	return p_prompt;
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

std::string GenProgram::UnixTimeToString(time_t unix_seconds)
{
	struct tm * ptm = localtime(&unix_seconds);
	char time_stamp[30];
	strftime(time_stamp, 30, "%m-%d-%Y %H:%M", ptm);
	std::string date_time(time_stamp);
	return date_time;
}

void GenProgram::DisplayTime()
{
	std::time_t result = std::time(nullptr);
	std::cout << "" << std::asctime(std::localtime(&result)) << std::endl;
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

std::string GenProgram::RandomInttoString()
{
	ReseedRNG();
	int i_random = Random10000();
	std::string s_random = std::to_string(i_random);
	return s_random;
}

const char * GenProgram::CharOutput(const std::string & p_input)
{
	const char * m_output = p_input.c_str();

	return m_output;
}
