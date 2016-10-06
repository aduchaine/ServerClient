/*
4/1/16

content.cpp
*/

#include "stdafx.h"

#include "client_packet.h"
#include "content_net.h"
#include "content_packet.h"

#include "content.h"
#include "gen_program.h"
#include "login.h"
#include "text_output.h"


std::vector<std::string> mail_name;
std::vector<std::string> mail_subject;
std::vector<std::string> mail_body;
std::vector<std::string> mail_reply;
std::vector<std::string> edit_mail_name;
std::vector<std::string> edit_mail_subject;
std::vector<std::string> edit_mail_body;
std::vector<std::string> edit_mail_reply;


int Content::DirectContent(int menu_status, int response_status, std::string p_response)
{
	std::string m_response = " ";

	m_response = GProg.CheckPromptInput(p_response);

	if (response_status == CONTENT_1ST_LOGIN) {
		response_status = CONTENT_INVALID_INPUT;
		return response_status;
	}

	if (m_response == "logout" || m_response == "quit") {
		response_status = LOGOUT_CONTENT_SERVER;
	}
	else if (m_response == "menu") {
		response_status = CONTENT_GOTO_MENU;
	}
	else if (m_response == "chat") {
		response_status = CHAT_MENU;
	}	
	else if (m_response == "mail") {
		response_status = MAIL_GOTO_MENU;
	}
	else if (m_response == "instructions" && (response_status == CHAT_MENU || menu_status == CHAT_ROOM)) {
		response_status = CHAT_INSTRUCTIONS;
	}
	else if (m_response == "back" && menu_status != CHAT_ROOM) {
		response_status = CONTENT_GOTO_MENU;
	}
	else if (m_response == "begin chat" && response_status == CHAT_MENU) {
		response_status = CHAT_BEGIN;
	}
	else if (response_status == 0) {
		response_status = CONTENT_INVALID_INPUT2;		
	}
	else {		
		response_status = CONTENT_INVALID_INPUT;
	}
	return response_status;
}

void Content::ProcessContent(int menu_status, int response_status)
{
	GProg.ProcessUserInput(key_prmt, remove_prmt);

	if (do_process == false) {
		return;
	}
	
	ContentMenu(menu_status, response_status);
	ChatOut(menu_status);
	MailMenu(menu_status, response_status);
	MailSend(menu_status, response_status);
	MailReply(menu_status, response_status);
	MailReadInbox(menu_status, response_status);
	MailReadOutbox(menu_status, response_status);
	MailDeleteInbox(menu_status, response_status);
	MailDeleteOutbox(menu_status, response_status);
}

void Content::ContentMenu(int menu_status, int response_status)
{
	if (menu_status != CONTENT_MENU) {
		return;
	}

	typing = false;
	do_process = false;

	std::cout << "\n\n";
	int response_stat;

	response_stat = DirectContent(menu_status, response_status, p_input);

	if (response_stat == CONTENT_INVALID_INPUT && response_status == CONTENT_1ST_LOGIN) { // user is prompted to hit enter which is CONTENT_INVALID_INPUT and 
		ConNet.StartConNetTimers(); // also located in ContentUpdate() - one of these may not be needed
		Net.ClientPackets(CONTENT_EMPTY, VALID_CREDS);
		return;
	}

	Net.ClientPackets(CONTENT_EMPTY, response_stat);
}

int Content::DirectMail(int response_status, std::string p_response)
{
	std::string m_response = " ";

	int response_stat = response_status;

	m_response = GProg.CheckPromptInput(p_response);	

	int m_resp_size = strlen(m_response.c_str());

	if (m_response == "logout" || m_response == "quit") {
		response_status = LOGOUT_CONTENT_SERVER;
	}	
	else if (m_response == "back" || m_response == "menu") {
		if (response_status == MAIL_GOTO_MENU) {
			response_status = CONTENT_GOTO_MENU;
		}
		else {
			response_status = MAIL_GOTO_MENU;
		}
	}
	else if (m_response == "instructions") {
		response_status = MAIL_INSTRUCTIONS;
	}
	else if (m_response == "mailbox") {
		response_status = MAIL_BOX;
	}
	else if (m_response == "inbox" && response_status == MAIL_BOX) {
		response_status = MAIL_INBOX;
	}
	else if (m_response == "outbox" && response_status == MAIL_BOX) {
		response_status = MAIL_OUTBOX;
	}
	else if (m_response == "compose" && response_status == MAIL_GOTO_MENU) {
		response_status = MAIL_TO_SEND;
	}
	else if (m_response == "read" && response_status == MAIL_INBOX) {
		response_status = MAIL_INBOX_READ;
	}
	else if (m_response == "unread" && response_status == MAIL_INBOX) {
		response_status = MAIL_INBOX_UNREAD;
	}
	else if (m_response == "sent" && response_status == MAIL_OUTBOX) {
		response_status = MAIL_OUTBOX_SENT;
	}
	else if (p_response == "reply" && (menu_stat == MAIL_READ_IN)) {
		response_status = MAIL_TO_REPLY;
	}
	else if (m_response == "delete") {
		if (response_status == MAIL_INBOX) {
			response_status = MAIL_DELETE_INBOX;
		}
		else if (response_status == MAIL_OUTBOX) {
			response_status = MAIL_DELETE_OUTBOX;
		}
		else {
			response_status = NONE;
		}
	}
	else if (response_stat == response_status) {
		response_status = DirectMailDelete(response_status, m_response);
		if (response_stat == response_status) {
			response_status = DirectMailList(response_status, m_response);
			if (response_stat == response_status) {
				response_status = DirectMailSending(response_status, m_response);
				if (response_stat == response_status) {
					response_status = NONE;
				} // input which cannot be identified in the "directsomewhere" functions is intentional because the input will often be specific to a sub-function
			}
		}
	}	
	return response_status;
}

int Content::DirectMailDelete(int response_status, std::string p_response)
{
	if (p_response == "delete unread" && response_status == MAIL_DELETE_INBOX) {
		response_status = MAIL_DELETE_UNREAD;
	}
	else if (p_response == "delete read" && response_status == MAIL_DELETE_INBOX) {
		response_status = MAIL_DELETE_READ;
	}
	else if (p_response == "delete sent" && response_status == MAIL_DELETE_OUTBOX) {
		response_status = MAIL_DELETE_SENT;
	}
	else if (p_response == "delete options" && response_status == MAIL_DELETE_INBOX) {
		response_status = MAIL_DELETE_ALL_OPTIONS;
	}
	else if (p_response == "delete all outbox" && response_status == MAIL_DELETE_OUTBOX) {
		response_status = CONFIRM_DELETE_ALL_OUTBOX;
	}
	else if (p_response == "delete all unread" && response_status == MAIL_DELETE_ALL_OPTIONS) {
		response_status = CONFIRM_DELETE_ALL_UNREAD;
	}
	else if (p_response == "delete all read" && response_status == MAIL_DELETE_ALL_OPTIONS) {
		response_status = CONFIRM_DELETE_ALL_READ;
	}
	else if (p_response == "delete all inbox" && response_status == MAIL_DELETE_ALL_OPTIONS) {
		response_status = CONFIRM_DELETE_ALL_INBOX;
	}
	else if (p_response == "yes") {
		if (response_status == CONFIRM_DELETE_ALL_OUTBOX) {
			response_status = MAIL_DELETE_ALL_OUTBOX;
		}
		else if (response_status == CONFIRM_DELETE_ALL_READ) {
			response_status = MAIL_DELETE_ALL_READ;
		}
		else if (response_status == CONFIRM_DELETE_ALL_UNREAD) {
			response_status = MAIL_DELETE_ALL_UNREAD;
		}
		else if (response_status == CONFIRM_DELETE_ALL_INBOX) {
			response_status = MAIL_DELETE_ALL_INBOX;
		}
		else {
			response_status = NONE;
		}
	}
	return response_status;
}

// mail lists for individual options
int Content::DirectMailList(int response_status, std::string p_response)
{
	int m_resp_size = strlen(p_response.c_str());

	if (p_response == "next" || p_response == "previous") {
		if (response_status == MAIL_DELETE_READ || response_status == MAIL_DELETE_UNREAD || response_status == MAIL_DELETE_SENT
			|| response_status == MAIL_INBOX_READ || response_status == MAIL_INBOX_UNREAD || response_status == MAIL_OUTBOX_SENT) {
			response_status = 1;			
		}
		else {
			response_status = NONE;
		}
	}
	else if (GProg.IsValidInput(p_response, true, false, true, NUMERIC_CHAR, NO_CHAR, 1, 2) == 0) { // selection of individual mail message
		std::string::size_type sz;
		int im_response = stoi(p_response, &sz);

		if (im_response <= 20 && im_response > 0) {
			if (response_status == MAIL_DELETE_READ || response_status == MAIL_DELETE_UNREAD || response_status == MAIL_DELETE_SENT
				|| response_status == MAIL_INBOX_READ || response_status == MAIL_INBOX_UNREAD || response_status == MAIL_OUTBOX_SENT) {
				response_status = 2;
			}
			else {
				response_status = NONE;
			}
		}
		else {
			response_status = NONE;
		}
	}
	return response_status;
}

int Content::DirectMailSending(int response_status, std::string p_response)
{
	if (p_response == "review" && response_status == MAIL_TO_SEND_REVIEW) {
		response_status = MAIL_REVIEW_MESSAGE;
	}	
	else if (p_response == "edit name" && response_status == MAIL_TO_SEND_REVIEW) {
		response_status = MAIL_EDIT_RECIPIENT;
	}
	else if (p_response == "edit subject" && response_status == MAIL_TO_SEND_REVIEW) {
		response_status = MAIL_EDIT_SUBJECT;
	}
	else if (p_response == "edit message" && response_status == MAIL_TO_SEND_REVIEW) {
		response_status = MAIL_EDIT_BODY;
	}
	else if (p_response == "edit" && (response_status == MAIL_REVIEW_MESSAGE || response_status == MAIL_REVIEW_EDIT || response_status == MAIL_REVIEW_REPLY)) {		
		if (response_status == MAIL_REVIEW_REPLY) {
			response_status = MAIL_EDIT_REPLY;
		}
		else {
			response_status = MAIL_TO_SEND_REVIEW;
		}		
	}
	else if (p_response == "send" && (response_status == MAIL_REVIEW_MESSAGE || response_status == MAIL_REVIEW_EDIT || response_status == MAIL_REVIEW_REPLY || response_status == MAIL_REVIEW_REPLY_EDIT)) {
		if (response_status == MAIL_REVIEW_REPLY || response_status == MAIL_REVIEW_REPLY_EDIT) {
			response_status = MAIL_TO_REPLY;
		}
		else {
			response_status = MAIL_TO_SEND;
		}
	}
	else if (p_response == "revert" && (response_status == MAIL_REVIEW_EDIT || response_status == MAIL_REVIEW_REPLY_EDIT)) {
		if (response_status == MAIL_REVIEW_REPLY_EDIT) {
			response_status = MAIL_REVIEW_REPLY;
		}
		else {
			response_status = MAIL_REVIEW_MESSAGE;
		}
	}
	else if (p_response == "accept" && (response_status == MAIL_REVIEW_EDIT || response_status == MAIL_TO_SEND_REVIEW)) {
		response_status = MAIL_ACCEPT_EDIT;
	}
	return response_status;
}

void Content::MailMenu(int menu_status, int response_status)
{
	if (menu_status != MAIL_MENU) {
		return;
	}

	std::cout << "\n\n";

	typing = false;
	do_process = false;

	int response_stat;	

	response_stat = DirectMail(response_status, p_input);

	if (response_stat == NONE) {
		response_stat = CONTENT_INVALID_INPUT3;
	}

	Net.ClientPackets(CONTENT_EMPTY, response_stat);	
}

void Content::MailReadInbox(int menu_status, int response_status)
{
	if (menu_status != MAIL_READ_IN) {
		return;
	}

	std::cout << "\n\n";

	typing = false;
	do_process = false;

	std::string m_response;
	std::string s_mail_list_counter;

	int response_stat;

	response_stat = DirectMail(response_status, p_input);
	
	if (response_stat == 1) {		// next/previous		// MAIL_INBOX_READ/MAIL_INBOX_UNREAD
		m_response = GProg.CheckPromptInput(p_input);
		s_mail_list_counter = std::to_string(mail_list_counter);
		Net.ClientPackets(CONTENT_2VAL, response_status, m_response, s_mail_list_counter);
		return;
	}
	else if (response_stat == 2) {	// client # selection	// MAIL_INBOX_READ/MAIL_INBOX_UNREAD		
		m_response = GProg.CheckPromptInput(p_input);
		s_mail_list_counter = std::to_string(mail_list_counter);
		Net.ClientPackets(CONTENT_2VAL, response_status, m_response, s_mail_list_counter);		
		return;
	}
	else if (response_stat == NONE) {
		response_stat = CONTENT_INVALID_INPUT3;
	}
	else {
		mail_list_counter = 0;
	}	
	Net.ClientPackets(CONTENT_EMPTY, response_stat);
}

void Content::MailReadOutbox(int menu_status, int response_status)
{
	if (menu_status != MAIL_READ_OUT) {
		return;
	}

	std::cout << "\n\n";

	typing = false;
	do_process = false;

	std::string m_response;
	std::string s_mail_list_counter;

	int response_stat;

	response_stat = DirectMail(response_status, p_input);

	if (response_stat == 1) {
		m_response = GProg.CheckPromptInput(p_input);
		s_mail_list_counter = std::to_string(mail_list_counter);
		Net.ClientPackets(CONTENT_2VAL, MAIL_OUTBOX_SENT, m_response, s_mail_list_counter);
		return;
	}
	else if (response_stat == 2) {
		m_response = GProg.CheckPromptInput(p_input);
		s_mail_list_counter = std::to_string(mail_list_counter);
		Net.ClientPackets(CONTENT_2VAL, MAIL_OUTBOX_SENT, m_response, s_mail_list_counter);
		return;
	}
	else if (response_stat == NONE) {
		response_stat = CONTENT_INVALID_INPUT3;
	}
	else {
		mail_list_counter = 0;
	}
	Net.ClientPackets(CONTENT_EMPTY, response_stat);
}

void Content::MailDeleteInbox(int menu_status, int response_status)
{
	if (menu_status != MAIL_DELETE_IN) {
		return;
	}

	std::cout << "\n\n";

	typing = false;
	do_process = false;

	std::string m_response;
	std::string s_mail_list_counter;

	int response_stat;
		
	response_stat = DirectMail(response_status, p_input);	

	if (response_stat == 1) {			// MAIL_DELETE_UNREAD/MAIL_DELETE_READ
		m_response = GProg.CheckPromptInput(p_input);
		s_mail_list_counter = std::to_string(mail_list_counter);
		Net.ClientPackets(CONTENT_2VAL, response_status, m_response, s_mail_list_counter);
		return;
	}
	else if (response_stat == 2) {		// MAIL_DELETE_UNREAD/MAIL_DELETE_READ	
		m_response = GProg.CheckPromptInput(p_input);
		s_mail_list_counter = std::to_string(mail_list_counter);
		Net.ClientPackets(CONTENT_2VAL, response_status, m_response, s_mail_list_counter);
		return;
	}	
	else if (response_stat == CONFIRM_DELETE_ALL_READ) {
		Text.NonPacketTextMessage(CONFIRM_DELETE_ALL_READ, pm_name);
		content_stat = CONFIRM_DELETE_ALL_READ;
		lock_input = false;
		return;
	}
	else if (response_stat == CONFIRM_DELETE_ALL_UNREAD) {
		Text.NonPacketTextMessage(CONFIRM_DELETE_ALL_UNREAD, pm_name);
		content_stat = CONFIRM_DELETE_ALL_UNREAD;
		lock_input = false;
		return;
	}
	else if (response_stat == CONFIRM_DELETE_ALL_INBOX) {
		Text.NonPacketTextMessage(CONFIRM_DELETE_ALL_INBOX, pm_name);
		content_stat = CONFIRM_DELETE_ALL_INBOX;
		lock_input = false;
		return;
	}
	else if (response_stat == NONE) {
		response_stat = CONTENT_INVALID_INPUT3;
	}
	else {
		mail_list_counter = 0; // potential issue
	}	
	Net.ClientPackets(CONTENT_EMPTY, response_stat);
}

void Content::MailDeleteOutbox(int menu_status, int response_status)
{
	if (menu_status != MAIL_DELETE_OUT) {
		return;
	}

	std::cout << "\n\n";

	typing = false;
	do_process = false;

	std::string m_response;
	std::string s_mail_list_counter;

	int response_stat;

	response_stat = DirectMail(response_status, p_input);	

	if (response_stat == 1) {
		m_response = GProg.CheckPromptInput(p_input);
		s_mail_list_counter = std::to_string(mail_list_counter);
		Net.ClientPackets(CONTENT_2VAL, MAIL_DELETE_SENT, m_response, s_mail_list_counter);
		return;
	}
	else if (response_stat == 2) {		
		m_response = GProg.CheckPromptInput(p_input);
		s_mail_list_counter = std::to_string(mail_list_counter);
		Net.ClientPackets(CONTENT_2VAL, MAIL_DELETE_SENT, m_response, s_mail_list_counter);
		return;
	}
	else if (response_stat == CONFIRM_DELETE_ALL_OUTBOX) {
		Text.NonPacketTextMessage(CONFIRM_DELETE_ALL_OUTBOX, pm_name);
		content_stat = CONFIRM_DELETE_ALL_OUTBOX;
		lock_input = false;
		return;
	}
	else if (response_stat == NONE) {
		response_stat = CONTENT_INVALID_INPUT3;
	}
	else {
		mail_list_counter = 0;
	}	
	Net.ClientPackets(CONTENT_EMPTY, response_stat);
}

void Content::MailSend(int menu_status, int response_status)
{
	if (menu_status != MAIL_SEND) {
		return;
	}

	std::cout << "\n\n";

	typing = false;
	do_process = false;
	
	std::string p_mail;
	std::string s_edit_stat;

	int response_stat;

	s_edit_stat = std::to_string(edit_stat);
	GProg.AddCharsToString(s_edit_stat, "!", 2);
	p_mail = s_edit_stat + p_input;

	if (response_status == MAIL_TO_SEND_RECIPIENT) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			mail_name.push_back(p_input);			
			Net.ClientPackets(CONTENT_MAIL, MAIL_TO_SEND_RECIPIENT, p_mail);
			return;
		}
	}
	else if (response_status == MAIL_TO_SEND_SUBJECT) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			mail_subject.push_back(p_input);
			Net.ClientPackets(CONTENT_MAIL, MAIL_TO_SEND_SUBJECT, p_mail);
			return;
		}
	}
	else if (response_status == MAIL_TO_SEND_BODY) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			mail_body.push_back(p_input);
			Net.ClientPackets(CONTENT_MAIL, MAIL_TO_SEND_BODY, p_mail);
			return;
		}
	}
	else if (response_status == MAIL_TO_SEND_REVIEW) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			response_stat = CONTENT_INVALID_INPUT3;
		}
		else if (response_stat == MAIL_ACCEPT_EDIT) {
			MailOutput(edit_stat, true, false, false, false);
			edit_stat = 0;
			response_stat = MAIL_REVIEW_MESSAGE;
		}
	}
	else if (response_status == MAIL_REVIEW_MESSAGE) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			response_stat = CONTENT_INVALID_INPUT3;
		}
		if (response_stat == MAIL_TO_SEND) {
			MailOutput(edit_stat, false, true, false, false);
			edit_stat = 0;
			MailClearVec(0);
			return;
		}
	}
	else if (response_status == MAIL_REVIEW_EDIT) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == MAIL_ACCEPT_EDIT) {
			MailOutput(edit_stat, true, false, false, false);
			edit_stat = 0;
			response_stat = MAIL_REVIEW_MESSAGE;
		}
		else if (response_stat == NONE) {
			response_stat = CONTENT_INVALID_INPUT3;
		}
		else if (response_stat == MAIL_TO_SEND) {
			MailOutput(edit_stat, false, true, false, false);
			edit_stat = 0;
			MailClearVec(0);
			return;
		}
	}
	else if (response_status == MAIL_EDIT_RECIPIENT) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			edit_mail_name.clear();
			edit_mail_name.push_back(p_input);
			Net.ClientPackets(CONTENT_MAIL, MAIL_EDIT_RECIPIENT, p_mail);
			return;
		}
	}
	else if (response_status == MAIL_EDIT_SUBJECT) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			edit_mail_subject.clear();
			edit_mail_subject.push_back(p_input);
			Net.ClientPackets(CONTENT_MAIL, MAIL_EDIT_SUBJECT, p_mail);
			return;
		}
	}
	else if (response_status == MAIL_EDIT_BODY) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			edit_mail_body.clear();
			edit_mail_body.push_back(p_input);
			Net.ClientPackets(CONTENT_MAIL, MAIL_EDIT_BODY, p_mail);
			return;
		}
	}
	Net.ClientPackets(CONTENT_EMPTY, response_stat);
}

void Content::MailReply(int menu_status, int response_status)
{
	if (menu_status != MAIL_REPLY) {
		return;
	}

	std::cout << "\n\n";

	typing = false;
	do_process = false;

	std::string p_mail;
	std::string s_edit_stat;

	int response_stat;

	s_edit_stat = std::to_string(edit_stat);
	GProg.AddCharsToString(s_edit_stat, "!", 2);
	p_mail = s_edit_stat + p_input;

	if (response_status == MAIL_TO_SEND_REPLY) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			mail_reply.push_back(p_input);
			Net.ClientPackets(CONTENT_MAIL, MAIL_TO_SEND_REPLY, p_mail);
			return;
		}
	}
	else if (response_status == MAIL_REVIEW_REPLY) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			response_stat = CONTENT_INVALID_INPUT3;
		}
		if (response_stat == MAIL_TO_REPLY) {
			MailOutput(edit_stat, false, true, true, false);
			edit_stat = 0;
			MailClearVec(0);
			return;
		}
	}
	else if (response_status == MAIL_REVIEW_REPLY_EDIT) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			response_stat = CONTENT_INVALID_INPUT3;
		}
		else if (response_stat == MAIL_TO_REPLY) {
			MailOutput(edit_stat, false, true, true, false);
			edit_stat = 0;
			MailClearVec(0);
			return;
		}
	}
	else if (response_status == MAIL_EDIT_REPLY) {
		response_stat = DirectMail(response_status, p_input);

		if (response_stat == NONE) {
			edit_mail_reply.clear();
			edit_mail_reply.push_back(p_input);
			Net.ClientPackets(CONTENT_MAIL, MAIL_EDIT_REPLY, p_mail);
			return;
		}
	}
	Net.ClientPackets(CONTENT_EMPTY, response_stat);
}


void Content::MailOutput(int edit_status, bool save_edits, bool send, bool reply, bool review)
{
	std::string s_edit_status;
	std::string vec_name;
	std::string vec_subject;
	std::string vec_body;
	std::string vec_reply;

	std::string p_mail;

	s_edit_status = std::to_string(edit_status);

	switch (edit_status) {
	case 0:
		vec_name = MailVecData(1, save_edits);
		vec_subject = MailVecData(2, save_edits);
		vec_body = MailVecData(3, save_edits);
		if (reply == true) {
			vec_reply = MailVecData(4, save_edits);
		}
		break;
	case 1:
		vec_name = MailVecData(5, save_edits);
		vec_subject = MailVecData(2, save_edits);
		vec_body = MailVecData(3, save_edits);
		break;
	case 2:
		vec_name = MailVecData(1, save_edits);
		vec_subject = MailVecData(6, save_edits);
		vec_body = MailVecData(3, save_edits);
		break;
	case 3:
		vec_name = MailVecData(5, save_edits);
		vec_subject = MailVecData(6, save_edits);
		vec_body = MailVecData(3, save_edits);
		break;
	case 4:
		vec_name = MailVecData(1, save_edits);
		vec_subject = MailVecData(2, save_edits);
		vec_body = MailVecData(7, save_edits);
		break;
	case 5:
		vec_name = MailVecData(5, save_edits);
		vec_subject = MailVecData(2, save_edits);
		vec_body = MailVecData(7, save_edits);
		break;
	case 6:
		vec_name = MailVecData(1, save_edits);
		vec_subject = MailVecData(6, save_edits);
		vec_body = MailVecData(7, save_edits);
		break;
	case 7:
		vec_name = MailVecData(5, save_edits);
		vec_subject = MailVecData(6, save_edits);
		vec_body = MailVecData(7, save_edits);
		break;
	case 8:
		vec_name = MailVecData(1, save_edits);
		vec_subject = MailVecData(2, save_edits);
		vec_body = MailVecData(3, save_edits);
		vec_reply = MailVecData(8, save_edits);
		break;
	}

	if (send == true) {
		if (reply == false) {
			GProg.AddCharsToString(vec_name, "!", 12);
			GProg.AddCharsToString(vec_subject, "!", 25);
			p_mail = vec_name + vec_subject + vec_body;
			Net.ClientPackets(CONTENT_MAIL, MAIL_TO_SEND, p_mail);
		}
		else {
			GProg.AddCharsToString(vec_name, "!", 12);
			GProg.AddCharsToString(vec_subject, "\\", 0, true); // this should be handled server side, maybe
			GProg.AddCharsToString(vec_subject, "!", 25);
			vec_body = "\t--- begin message--- \n" + vec_body + "\t--- end message ---\n\n"; // this should be handled server side - it may be difficult to do this in this way, see notes
			GProg.AddCharsToString(vec_body, "\\", 0, true); // this should be handled server side, maybe - unless able to separate the old body from reply, this will be tough - the reply will have the extra "\\"
			p_mail = vec_name + vec_subject + vec_body + vec_reply;
			Net.ClientPackets(CONTENT_MAIL, MAIL_TO_REPLY, p_mail);
		}		
	}
	else if (review == true) {
		if (reply == false) {
			GProg.RemoveCharsFromString(vec_subject, "\\"); // a new variable for the subject and body may be needed 
			GProg.RemoveCharsFromString(vec_body, "\\");
			if (edit_status != 0) {
				Text.MailMessage(MAIL_REVIEW_EDIT, pm_name, vec_name, vec_subject, vec_body);
			}
			else {
				Text.MailMessage(MAIL_REVIEW_MESSAGE, pm_name, vec_name, vec_subject, vec_body);
			}
		}
		else {
			GProg.RemoveCharsFromString(vec_reply, "\\");
			Text.MailMessage(MAIL_REVIEW_REPLY, pm_name, vec_name, vec_subject, vec_body, vec_reply);
		}
	}
}

// don't think "save_edits" is needed
// data_category could be the bitmask switches, unimplemented but listed in content.h
std::string Content::MailVecData(int data_category, bool save_edits)
{
	unsigned int i;

	std::string vec_string = "";

	switch (data_category) {
	case 1:
		for (i = 0; i < mail_name.size(); i++) {
			vec_string = mail_name.at(i);
		}
		break;
	case 2:
		for (i = 0; i < mail_subject.size(); i++) {
			vec_string = mail_subject.at(i);
		}
		break;
	case 3:
		for (i = 0; i < mail_body.size(); i++) {
			vec_string = mail_body.at(i);
		}
		break;
	case 4:
		for (i = 0; i < mail_body.size(); i++) {
			vec_string = mail_reply.at(i);
		}
		break;
	case 5:
		for (i = 0; i < mail_name.size(); i++) {
			vec_string = edit_mail_name.at(i);
		}
		break;
	case 6:
		for (i = 0; i < mail_subject.size(); i++) {
			vec_string = edit_mail_subject.at(i);
		}
		break;
	case 7:
		for (i = 0; i < mail_body.size(); i++) {
			vec_string = edit_mail_body.at(i);
		}
		break;
	case 8:
		for (i = 0; i < mail_body.size(); i++) {
			vec_string = edit_mail_reply.at(i);
		}
		break;
	}

	if (save_edits == true) {
		if (data_category == 5) {
			MailClearVec(1);
			mail_name.push_back(vec_string);
			MailClearVec(5);
		}
		else if (data_category == 6) {
			MailClearVec(2);
			mail_subject.push_back(vec_string);
			MailClearVec(6);
		}
		else if (data_category == 7) {
			MailClearVec(3);
			mail_body.push_back(vec_string);
			MailClearVec(7);
		}
		else if (data_category == 8) {
			MailClearVec(4);
			mail_reply.push_back(vec_string);
			MailClearVec(8);
		}
	}
	return vec_string;

/*	if (data_category == 1) {
		for (i = 0; i < mail_name.size(); i++) {
			vec_string = mail_name.at(i);
		}
	}
	else if (data_category == 2) {
		for (i = 0; i < mail_subject.size(); i++) {
			vec_string = mail_subject.at(i);
		}
	}
	else if (data_category == 3) {
		for (i = 0; i < mail_body.size(); i++) {
			vec_string = mail_body.at(i);
		}
	}
	else if (data_category == 4) {
		for (i = 0; i < mail_body.size(); i++) {
			vec_string = mail_reply.at(i);
		}
	}
	else if (data_category == 5) {
		for (i = 0; i < mail_name.size(); i++) {
			vec_string = edit_mail_name.at(i);
		}
	}
	else if (data_category == 6) {
		for (i = 0; i < mail_subject.size(); i++) {
			vec_string = edit_mail_subject.at(i);
		}
	}
	else if (data_category == 7) {
		for (i = 0; i < mail_body.size(); i++) {
			vec_string = edit_mail_body.at(i);
		}
	}
	else if (data_category == 8) {
		for (i = 0; i < mail_body.size(); i++) {
			vec_string = edit_mail_reply.at(i);
		}
	} */
	
}

// data_category = 0 is clear all
void Content::MailClearVec(int data_category)
{
	switch (data_category) {
	case 0:
		mail_name.clear();
		mail_subject.clear();
		mail_body.clear();
		mail_reply.clear();
		edit_mail_name.clear();
		edit_mail_subject.clear();
		edit_mail_body.clear();
		edit_mail_reply.clear();
		break;
	case 1:
		mail_name.clear();
		break;
	case 2:
		mail_subject.clear();
		break;
	case 3:
		mail_body.clear();
		break;
	case 4:
		mail_reply.clear();
		break;
	case 5:
		edit_mail_name.clear();
		break;
	case 6:
		edit_mail_subject.clear();
		break;
	case 7:
		edit_mail_body.clear();
		break;
	case 8:
		edit_mail_reply.clear();
		break;

	}
}

void Content::MailIn(int response_status, std::string p_mail)
{
	std::string s_mess_num;
	std::string data1_pos;
	std::string data2_pos;
	std::string date;
	std::string person;
	std::string subject;
	std::string body;

	std::string::size_type sz;
	int record_status;
	int person_len;
	int mess_num;
	int n_mess_num;

	if (response_status == MAIL_TO_SEND_REPLY) {
		
		Text.MailMessage(MAIL_TO_SEND_REPLY, pm_name, date, person, subject, body);
	}
	if (strlen(p_mail.c_str()) < 30) {
		data1_pos = p_mail.substr(0, 12);
		GProg.RemoveCharsFromString(data1_pos, "!");
		record_status = stoi(data1_pos, &sz);

		mail_list_counter = record_status;
	}
	else if (response_status == MAIL_READ_RECV_MESSAGE || response_status == MAIL_READ_SENT_MESSAGE) {
		date = p_mail.substr(0, 16);

		person = p_mail.substr(16, 12);
		GProg.RemoveCharsFromString(person, "!");		
		
		subject = p_mail.substr(28, 25);
		GProg.RemoveCharsFromString(subject, "!");

		body = p_mail.substr(53, strlen(p_mail.c_str()) - 53);

		if (response_status == MAIL_READ_RECV_MESSAGE) {
			// could clear mail vec and save in preparation for reply here or send data from server again						
			Text.MailMessage(MAIL_READ_RECV_MESSAGE, pm_name, date, person, subject, body);
			MailClearVec(0);
			mail_name.push_back(person);
			subject = "RE: " + subject;
			mail_subject.push_back(subject);
			person[0] = toupper(person[0]);
			body = person + " said:\n\n" + body + "\n";
			mail_body.push_back(body);
		}
		else {
			Text.MailMessage(MAIL_READ_SENT_MESSAGE, pm_name, date, person, subject, body);
		}
	}
	else {
		data1_pos = p_mail.substr(0, 2);
		GProg.RemoveCharsFromString(data1_pos, "!");

		s_mess_num = p_mail.substr(2, 2);
		GProg.RemoveCharsFromString(s_mess_num, "!");
		
		mess_num = stoi(s_mess_num, &sz);
		n_mess_num = mess_num % 20;
		if (n_mess_num % 20 == 0) {
			n_mess_num = 20;
		}
		s_mess_num = std::to_string(n_mess_num);

		if (strlen(s_mess_num.c_str()) == 1) {
			s_mess_num = " " + s_mess_num;
		}
		date = p_mail.substr(4, 16);

		person = p_mail.substr(20, 12);
		GProg.RemoveCharsFromString(person, "!");
		person = "[" + person + "]";
		person_len = strlen(person.c_str());

		if (person_len < 14) {
			GProg.AddCharsToString(person, " ", 14);			
		}
		subject = p_mail.substr(32, strlen(p_mail.c_str()) - 32);	

		++mail_list_counter;
		
		Text.MailMessage(response_status, data1_pos, s_mess_num, date, person, subject);
		//printf("\n\tMailIn - mail_list_counter(%i)", mail_list_counter); // for heavy debugging
	}
}

int Content::CheckChatPrompt(std::string s_input)
{
	int chat_type = 0;

	std::string find_chars(s_input);

	std::string find_command("/");
	std::string find_tell("@");

	std::size_t found_command = find_chars.find(find_command);
	std::size_t found_tell = find_chars.find(find_tell);

	if (found_command == 0) {
		chat_type = 0;
	}
	else if (found_tell == 0) {
		chat_type = CHAT_OUT_TELL;
	}
	else {
		chat_type = CHAT_OUT_SAY;
	}
	return chat_type;
}

void Content::ChatOut(int menu_status)
{
	if (menu_status != CHAT_ROOM) {
		return;
	}

	typing = false;
	do_process = false;

	std::string message;
	std::string rec_name;

	int response_stat = 0;

	response_stat = CheckChatPrompt(p_input);

	// printf(" ChatOut process_chat -> response_stat(%i) p_input(%s) \n", response_stat, GProg.CharOutput(p_input)); // for heavy debugging

	GProg.RemoveCharsFromString(p_input, "\\");

	if (response_stat == CHAT_OUT_SAY) {
		message = p_input;
		GProg.RemoveString(message, false);

		if (strlen(message.c_str()) > 0) {
			if (strlen(message.c_str()) == 1) {
				message = message + " ";
			}
			else if (GProg.IsValidInput(message, false, false, true, NO_CHAR, NO_CHAR, 0, 512) == 2) {
				Text.NonPacketTextMessage(CHAT_TOO_LONG, pm_name);
				message.resize(512);
			}
			Text.ChatMessage(CHAT_OUT_SAY, rec_name, message);

			Net.ClientPackets(CONTENT_CHAT, CHAT_OUT_SAY, message);
		}
		else {
			Text.NonPacketTextMessage(CHAT_NO_MESSAGE, pm_name);
			lock_input = false;
		}
	}
	else if (response_stat == CHAT_OUT_TELL) {
		rec_name = GetNameFromChatString(p_input, true);
		message = GetMessageFromString(CHAT_OUT_TELL, p_input, true);
		GProg.RemoveString(p_input, false);

		if (strlen(message.c_str()) > 0) {
			if (rec_name == pm_name) {
				Text.NonPacketTextMessage(CHAT_SELF_TALK, pm_name);
				lock_input = false;
				return;
			}
			if (GProg.IsValidInput(message, false, false, true, NO_CHAR, NO_CHAR, 0, 500) == 2) {
				Text.NonPacketTextMessage(CHAT_TOO_LONG, pm_name);
				message.resize(500);
			}
			Text.ChatMessage(CHAT_OUT_TELL, rec_name, message);

			Net.ClientPackets(CONTENT_CHAT, CHAT_OUT_TELL, rec_name, message);
		}
		else {
			Text.NonPacketTextMessage(CHAT_NO_MESSAGE, pm_name);
			lock_input = false;
		}
	}
	else {
		message = GetCommandFromString(p_input);
		GProg.RemoveString(p_input, false);
		Text.NonPacketTextMessage(CHAT_COMMAND, p_input);

		if (message == "chat") {
			Text.NonPacketTextMessage(CHAT_INCHAT, pm_name);
			lock_input = false;
			return;
		}
		response_stat = DirectContent(menu_status, response_stat, message);

		if (response_stat == CONTENT_INVALID_INPUT2) {
			Text.NonPacketTextMessage(CONTENT_INVALID_INPUT2, pm_name);
			lock_input = false;
		}
		else {			
			Net.ClientPackets(CONTENT_EMPTY, response_stat);
			if (response_stat != CONTENT_INVALID_INPUT2) {
				menu_stat = CONTENT_MENU;
				remove_prmt = false;
				key_prmt = false;
			}			
		}
	}
}

void Content::ChatIn(int response_status, std::string p_chat)
{
	if (menu_stat != CHAT_ROOM) {
		return;
	}

	typing = false;
	do_process = false;

	std::string c_message;
	std::string message;
	std::string sender_name;

	sender_name = GetNameFromChatString(p_chat, false);
	message = GetMessageFromString(response_status, p_chat, false);

	if (c_key.size() == 0 && GProg.GetConsoleXpos() == 20) {
		GProg.RemoveString(chat_user_prompt, false);

		if (response_status == CHAT_IN_SAY || response_status == CHAT_IN_TELL) {
			Text.ChatMessage(response_status, sender_name, message);
		}
		else {
			Text.ContentMessage(response_status, p_chat);
		}
		CHAT_USER_PROMPT;
	}
	else if (GProg.GetConsoleXpos() == c_key.size()) {
		std::string s_line(c_key.begin(), c_key.end());

		for (size_t j = 0; j < s_line.length(); j++) {
			c_message = s_line.substr(0, j + 1);
		}
		GProg.RemoveString(c_message, false);

		if (response_status == CHAT_IN_SAY || response_status == CHAT_IN_TELL) {
			Text.ChatMessage(response_status, sender_name, message);			
		}
		else {
			Text.ContentMessage(response_status, p_chat);
		}
		std::cout << c_message;
	}
}

std::string Content::GetNameFromChatString(std::string p_chat, bool send)
{
	std::string rec_name;
	std::string m_chat;

	if (send == true) {
		m_chat = p_chat.substr(1, strlen(p_chat.c_str()) - 1);
		std::string find_chars(m_chat);
		std::size_t found_space = find_chars.find_first_of(" ");
		rec_name = m_chat.substr(0, found_space);
	}
	else {
		rec_name = p_chat.substr(0, 12);
		GProg.RemoveCharsFromString(rec_name, "!"); // probably not necessary now
	}			
	return rec_name;
}

std::string Content::GetCommandFromString(std::string p_chat)
{
	std::string m_chat;
	std::string command;

	m_chat = p_chat.substr(1, strlen(p_chat.c_str()) - 1);
	std::string find_chars(m_chat);
	std::size_t found_space = find_chars.find_first_of(" ");
	command = m_chat.substr(0, found_space);

	return command;
}

std::string Content::GetMessageFromString(int response_status, std::string p_chat, bool send)
{
	if (response_status == CHAT_USER_JOIN || response_status == CHAT_USER_LEAVE) {
		return p_chat;
	}

	std::string m_chat;
	std::string message;

	if (send == true) {
		if (response_status == CHAT_OUT_TELL) {
			m_chat = p_chat.substr(1, strlen(p_chat.c_str()) - 1);
			std::string find_chars(m_chat);
			std::size_t found_space = find_chars.find_first_of(" ");
			if (found_space != -1) {
				message = m_chat.substr(found_space + 1, strlen(m_chat.c_str()) - (found_space + 1));
			}
			else {
				message = "";
			}
		}
	}
	else {
		message = p_chat.substr(12, strlen(p_chat.c_str()) - 12);
	}
	return message;
}
