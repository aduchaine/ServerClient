/*
4/1/16

content.h
*/

#ifndef CONTENT_H
#define CONTENT_H


#include <vector>

/*
#define EDIT_NAME 1
#define EDIT_SUBJECT 2
#define EDIT_BODY 4

bit 1 = name keep/edit MAIL_TO_SEND_RECIPIENT/MAIL_EDIT_RECIPIENT
bit 2 = subj keep/edit MAIL_TO_SEND_SUBJECT/MAIL_EDIT_SUBJECT
bit 3 = body keep/edit MAIL_TO_SEND_BODY/MAIL_EDIT_BODY

0 = MAIL_TO_SEND_RECIPIENT,	MAIL_TO_SEND_SUBJECT,	MAIL_TO_SEND_BODY
1 = MAIL_EDIT_RECIPIENT,	MAIL_TO_SEND_SUBJECT,	MAIL_TO_SEND_BODY
2 = MAIL_TO_SEND_RECIPIENT,	MAIL_EDIT_SUBJECT,		MAIL_TO_SEND_BODY
3 = MAIL_EDIT_RECIPIENT,	MAIL_EDIT_SUBJECT,		MAIL_TO_SEND_BODY
4 = MAIL_TO_SEND_RECIPIENT, MAIL_TO_SEND_SUBJECT,	MAIL_EDIT_BODY
5 = MAIL_EDIT_RECIPIENT,	MAIL_TO_SEND_SUBJECT,	MAIL_EDIT_BODY
6 = MAIL_TO_SEND_RECIPIENT, MAIL_EDIT_SUBJECT,		MAIL_EDIT_BODY
7 = MAIL_EDIT_RECIPIENT,	MAIL_EDIT_SUBJECT,		MAIL_EDIT_BODY

// new packet, "do you want to keep your edit" - take edit vector and overwrite mail vector, clear edit vector
*/

class Content
{

public:

	void ProcessContent(int menu_status, int response_status);

	int DirectContent(int menu_status, int response_status, std::string p_response);
	void ContentMenu(int menu_status, int response_status);		
	
	// mail
	int DirectMail(int response_status, std::string p_response);
	int DirectMailDelete(int response_status, std::string p_response);
	int DirectMailList(int response_status, std::string p_response);
	int DirectMailSending(int response_status, std::string p_response);

	void MailMenu(int menu_status, int response_status);
	void MailSend(int menu_status, int response_status);
	void MailReply(int menu_status, int response_status);
	void MailReadInbox(int menu_status, int response_status);
	void MailReadOutbox(int menu_status, int response_status);
	void MailDeleteInbox(int menu_status, int response_status);
	void MailDeleteOutbox(int menu_status, int response_status);
	
	void MailIn(int response_status, std::string p_mail); // could possibly use some efficiency stuff with all related functions
	void MailOutput(int edit_status, bool save_edits, bool send, bool reply, bool review);

	std::string MailVecData(int data_category, bool save_edits);
	void MailClearVec(int data_category);

	// chat
	int CheckChatPrompt(std::string s_input);

	void ChatOut(int menu_status);
	void ChatIn(int response_status, std::string p_chat);

	std::string GetNameFromChatString(std::string p_chat, bool send);
	std::string GetCommandFromString(std::string p_chat);
	std::string GetMessageFromString(int response_status, std::string p_chat, bool send);

};

extern Content Cont;

extern std::vector<std::string> mail_name;
extern std::vector<std::string> mail_subject;
extern std::vector<std::string> mail_body;
extern std::vector<std::string> edit_mail_name;
extern std::vector<std::string> edit_mail_subject;
extern std::vector<std::string> edit_mail_body;

extern int content_stat;
extern int edit_stat;

#endif
