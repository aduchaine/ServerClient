/*
3/12/16

login.h
*/

#ifndef LOGIN_H
#define LOGIN_H


class Login
{

public:
		
	void ProcessLogin(int menu_status, int login_status);

	int DirectLogin(int menu_status, int login_status, std::string p_prompt);
	int InitialLogin(int login_status);	
	void LoginMenu(int menu_status, int login_status);
	void GoOn(int menu_status, int login_status); // need to rename this	
	void ExistingAccountLogin(int menu_status, int login_status);
	void NewAccountLogin(int menu_status, int login_status);
	void ChangePasswordLogin(int menu_status, int login_status);
	
	int DirectAdmin(int admin_status, std::string p_prompt);
	void AdminLogin(int menu_status, int admin_status);
	void AdminMenu(int menu_status, int admin_status);
		
};

extern Login Log;

extern int login_stat;

#endif
