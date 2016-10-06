/*
3/30/16

text_output.h
*/

#ifndef TEXT_OUTPUT_H
#define TEXT_OUTPUT_H


#define CHAT_USER_PROMPT std::cout << chat_user_prompt;


enum NonPacketActions {

	CHECK_NAME_NEW_VALIDITY2 = 501,
	CHECK_PASSWORD_NEW_VALIDITY2 = 502,
	CHECK_PASSWORD_PW_CHG_LOGIN2 = 503,
	CHECK_PASSWORD_PW_CHG_VALIDITY2 = 504,
	CONTENT_INVALID_INPUT2 = 505,
	CHAT_SELF_TALK = 506,
	CHAT_INCHAT = 507,
	CHAT_NO_MESSAGE = 508,
	CHAT_COMMAND = 509,
	CHAT_TOO_LONG = 510,
	MORE_MAIL = 511,
	CONFIRM_DELETE_ALL_OUTBOX = 512,
	CONFIRM_DELETE_ALL_READ = 513,
	CONFIRM_DELETE_ALL_UNREAD = 514,
	CONFIRM_DELETE_ALL_INBOX = 515,
	MAIL_SENDING = 516,
	MAIL_EDITING = 517,	
	MAIL_LIST_HEAD = 518,
	MAIL_ACCEPT_EDIT = 519,
	ACCOUNT_LOGIN_COUNT = 520,
	ACCOUNT_AVG_TIME = 521,
	MAIL_REPLYING = 522

};

class TextOutput
{

public:

	void ClientMessage(int client_status, std::string p_name = "");

	void LoginMessage(int login_status, std::string name_string = "");

	void ContentMessage(int content_status, std::string p_name);

	void NonPacketTextMessage(int client_status, std::string p_message);

	void ChatMessage(int content_status, std::string p_name, std::string p_message);

	void MailMessage(int content_status, std::string p_name, std::string p_message1 = "", std::string p_message2 = "", std::string p_message3 = "", std::string p_message4 = ""); // refine this

};

extern TextOutput Text;

const std::string chat_user_prompt = "Press ENTER to type:";

extern bool key_prmt;
extern bool remove_prmt;
extern std::string prmt_to_remove;

// work on these variables' locations
extern std::string pm_acctID;
extern std::string random;
extern std::string pm_name;
extern std::string pu_password;


#endif

