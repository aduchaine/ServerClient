// login_server->cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "conio.h" // for testing

#include "login_net.h"
#include "login_packet.h"
#include "servernetwork.h"

#include "account.h"
#include "database.h"
#include "gen_program.h"
#include "make_file.h"
#include "timers.h"


Account Acct;
Database DB;
GenProgram GProg;
MakeFile File;

LoginNet LogNet;
ServerNetwork Net;

void TryConnectLogin();
void TryConnectServers();
bool BeginServerComm();
void ServerCommLoop();
void ReceiveServerCommPackets();
void EndServerComm();
void LoginLoop();

int login_loops = 0;
int connect_tries = 0;
int content_tries = 0;

Timers ServerCommTimer(GEN_TIMER, true);
Timers ServerConnectionTimer(RESTART_CONN_TIMER, true);


//		two tests, one timed and one by key press
/*bool InitiateInput(char c) // for testing
{
	if (_kbhit()) {
		c = _getch();
		return true;
	}
	return false;
}	
//		two tests, one timed and one by key press
char key_press = ' '; // for testing
if (InitiateInput(key_press) == true) { // for testing
	LogNet.DoTestPackets();
}
if (login_loops == 100) { // for testing (~5 seconds)

}
*/

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "BEGIN Login Server main-- " << std::endl;
	std::cout << "\n\n\t\t   -----PUSH ENTER TO START SERVER-----" << std::endl;
	char start[256];
	fgets(start, 255, stdin);

	ServerConnectionTimer.StartTimer(RESTART_CONN_TIMER);

	GProg.Enter();	

	// redo some of this logic and possibly add another bool variable - "restart_conn" is reused in <ServerCommLoop()> which I don't particularly like
	// it causes problems with "SHITDOWN/SERVERTALK2" - fixed immediate issue in <ProcessLognetTimer()>
	while (restart_conn == true) {

		TryConnectLogin();
		
		while (connected == true) {
			++login_loops;

			LoginLoop();		

			ServerCommLoop();

			Sleep(50);
		}
	}

	EndServerComm(); // this step should be done before it is reached - likely not needed	

	Net.CloseAllClientConnections(); // this step should be done before it is reached - likely not needed here

	GProg.Exit();

	std::cout << "\n\t\t    -----PUSH ENTER TO END SERVER-----" << std::endl;
	char end[256];
	fgets(end, 255, stdin);

	return 0;
}

void LoginLoop()
{
	Net.DirectIncomingConnections();
	LogNet.ReceiveLoginPackets();
	LogNet.ProcessLogNetTimers();
}

void TryConnectLogin()
{
	if (connect_tries >= 5) {
		restart_conn = false;
		return;
	}
	if (ServerConnectionTimer.IsEnabled() == false) {
		ServerConnectionTimer.StartTimer(RESTART_CONN_TIMER);
		//std::cout << " ServerConnectionTimer was disabled, enabling " << std::endl; // for heavy debugging
	}
	if (ServerConnectionTimer.CheckTimer(RESTART_CONN_TIMER) == true) {
		if (Net.InitializeConnection() == false) {
			++connect_tries;
			//std::cout << " ServerConnectionTimer - connection failed - tries = " << connect_tries << std::endl; // for heavy debugging
		}
		else {
			connect_tries = 0;
			ServerConnectionTimer.DisableTimer();
			ServerCommTimer.StartTimer(GEN_TIMER);
			//std::cout << " ServerConnectionTimer - connection success - ServerCommTimer: Start " << std::endl; // for heavy debugging
		}
	}
}

void ServerCommLoop()
{
	TryConnectServers(); // rename and allow expansion - expansion will likely require some kind of enumeration to identify servers

	ReceiveServerCommPackets();
}

void TryConnectServers()
{
	if (restart_conn == false) {
		return;
	}
	if (content_tries >= 5) {
		restart_conn = false;
		connected = false;
		return;
	}

	if (ServerCommTimer.IsEnabled() == false) {
		ServerCommTimer.StartTimer(GEN_TIMER);
		//std::cout << " ServerCommTimer was disabled, enabling " << std::endl; // for heavy debugging
	}
	if (ServerCommTimer.CheckTimer(GEN_TIMER) == true) {
		if (BeginServerComm() == false) {
			++content_tries;
			restart_conn = true;
			//std::cout << " ServerCommTimer - connection failed - tries = " << content_tries << std::endl; // for heavy debugging
		}
		else {
			content_tries = 0;
			ServerCommTimer.DisableTimer();
			restart_conn = false;
			//std::cout << " ServerCommTimer - connection success - server connections established " << std::endl; // for heavy debugging
		}
	}
}

bool BeginServerComm()
{
	bool connection = false;

	if (Net.InitializeContentConnection() == true) {
		std::cout << "\t\t   >>>--Connected to Content Server--<<<\n" << std::endl;
		LogNet.LoginPackets(CONTENT_EMPTY, SERVERTALK1, Net.ContentSocket, 0);
		connection = true;
	}
	return connection;
}

void ReceiveServerCommPackets()
{
	LogNet.ReceiveContentCommPackets();
}

// likely not needed
void EndServerComm()
{
	ShutdownConnection(Net.ContentSocket);
}

/*
// issues/todo:

94. see if <LogNet.ProcessLogNetTimers()> is in the right place or should be moved to an earlier position

119. work on efficiency with content and possibly login and accessing data in packet types
	- several functions access acctID/name and some may be redundant
	- "chat_string/&char_seg1" conversion could possibly be combined
		- "&char_seg1" removed

131. organize the content/login timer start/disable/process functions to follow something reasonably standard
	- especially content

133. error message shown when user tries to login with account locked
	- user can get past login and continue until the error is thrown
	- I think this was a result of not logging out properly and trying to log back in just before the server updated lockout
	- this is likely defined behavior, this circumstance seems to occur only when testing

143. still having issues with server error messages and shutting down clients
	- client abrupt DC - client logged out in DB but socket_position still valid on content with no sd
	- the shutdown failure occurs immediately after the client sock_pos/socket is removed but "sock_pos" appears to not be removed until later the final "closeclientconnection" call
	- this is likely not a big issue but for completions' sake
		- it could be the servre cycling through the different close connection checks

145. make sure client connection checks are done independant of activity checks
	- I don't remember why this was noted - the first check will occur at 1 min on login and 2 min on content
	- if the timer is on and there is a client connected, this check should occur

168. "proceed" crashed content - dual login
	- content user was not actually online at the time of the check so it was probably trying to logout a nonexistent user
	- this circumstance seems to occur only when testing

169. SERVER_ACTIVITY timer needs to be restarted and independant of a ping from content
	- commented out portion in servernetwork which disabled this timer
	- some circumstances of admin "shutdown" make login server loop server activity packets and not shutdown
	- probably a timer issue

175. client - at "HandleInput()" can add another passed variable to require a certain key-press combination to accept the input
	- currently, '\r' or the ENTER key is the setting
	- define the key press operation 

177. server fails make logging out users happen in a funny way
	- connection said closed but DB did not show logout until the server Net error with sock-pos 0:2

180. mail - truncate subject if too long in server
	- this will occur with replies when "RE:" is added
	- the length check is done only upon "compose"
	
181. a few funny things with input and backspace
	- number selections also
	- I think the commonality is: user giving input before the text is done writing-waiting

182. mail - need to format reply names(soandso said:) a little better, they currently go around the other messages
	- the formatting could probably be done server side with the mail not actually changing
		- have to think about it like a thread, various replies/forwards, etc. are only available to the specific users

185. add ip address logging for users

189. "SendPacketErrorHandling()" needs some work as mentioned implicitly in other #s


/// do these -------------  /////////

159. if a user is connected/logged in, send a text notification of a mail
	- "instant" text notification of new mail if logged in - this won't interrupt any input you are doing
		- this will require the same input functions as in chat to save the input to a vector until user presses enter

167. expansion of the mail system to allow "Re"/replies(done), send to multiple users(CC, BCC), check if your sent message was opened(done), mail archives separate from mailbox(done)
	- increase functionality of mail system to allow better user message formatting/editing
		- editing and character recognition improved	
	- mail threads see #188
	- sending messages to multiple users:
		- will make databse status tracking a nightmare in it's current state
		- it should be fairly easy to implement the sending portion of it
			- make an option to send to multiple, prompt "send to" with packet type until user types "subject" or something else to go to subject
				- editing users could be listed similar to how the mail lists work with # choice then option choice (remove, what else?)
				- editing/adding users should be fairly simple
				- separation with the normal ways to recognize at server
		- server side would maybe save all the names to a vector and iterate through them at send

188. mail threads: - for the sake of this example/program [userID = email address]
		- eventually/possibly make replies use the same mailID and switch around status' and send/recv id/names
		- and/or link up multiple messages like a thread with a "threadID" and when deleted/read, change the status of the threadID accordingly
		- reply messages could copy/paste the entire threadID records similar to current mail systems (ie. - with #5,6, 9 & 15 as replies for userID 3477)
			- this may require a separate table for individual user/message threadID tracking so different users don't recv all of the messages
			- the main fields will be userID, threadID and messageIDs separated by some kind of delimiter - a separate packet type may be necessary for efficiency
				- userID(int), threadID(float/double), replyIDs(string)
			- ie. user 7 sends message 3 to user 4 so threadID is 3-7 - there should never be another thread ID 3-7
				- decide if the original message will be counted as 0 or 1 if at all so it would show as 3-7-0
				- original message should count and probably as 0
			- if user 4 replies, the DB entry for each will be 3-7-0,1 or 3-7-1
				- if user 5 is forwarded then user 5 gets and entry for 3-7-1
			- the status can be handled in this table for each user/thread
			- each message is given a threadID with it's place in the thread denoted by messageID

			- this implies that the original sender could spawn a huge tree of a message thread and could not be a part of it beyond it's inception and it could also come full circle
				- I understand this is akin to reinventing the wheel, the value is accessing/presenting data
			
			- status could get a bit more complicated if impementing a "mark unread from here" or if a message by a user goes unread from a certain point
				- below would work for read/unread but forward/CC/etc. will need some kind of status identifier
				- add a single bit, if possible, between each replyID denoting read/unread or use a flag between the replyIDs to denote this point
				- ie.	7-3-7-1,2,3,!4,5,6 would mean - user 7 entry for threadID 3-7 started by userID 3, 7th thread, with replyIDs 1,2,3 read but anything 4+ is not read
						3-3-7-1,2,3,4,5,!6 would mean - user 3 entry for threadID 3-7 started by userID 3, 7th thread, with replyIDs 1,2,3,4,5 read and 6 is not read
			- the actual message table would simply be who said it and what 
				- userID(int), threadID(float/double), replyID(int), message
			
			- the archive will get more complicated from this in it's current state but to avoid this all messages could simply be archived from the beginning along with threadID table
				- the table with threadID and other reply metadata could have a delete column instead of using it within the metadata since each user will have a separate entry for each message thread

			

		- ID # limitations seem apparent, think GMail, etc.
			- threadIDs (which should be the main ID for the message), could begin with the userID, then an ASC count, then message/reply IDs
				- if user IDs are guaranteed to be unique, which they are, threadIDs can begin at 0/1 for each user
				- theoretically this is still infinite but the number of users can be finite, yet large - as well as the number of user messages (rediculous numbers could be flagged as spam acounts)
				- a "spam-reply" option can be added to the account table where account locking can occur upon reaching a certain number
					- tracking the "spam" is not overly complicated but the necessity for accuracy is important - ie. don't want to lable the original sender as spam if a user 
		- this is good practice for other things though



176. implement bitmasking in place of mail-edit and mail-message_status usage
	- it's not too far off
	- look at how eqemu does it
	- see "test_pallet" program

/// ------ ^^^^^^

// done, pending a bit more testing

70. create a welcome message script for content server for 1st_login - done
	- purpose is to identify a "first login"
	- it could be done by checking the DB for total login time or last_login and giving the account a different response status for "first login" - done see #179
	- after 1st login stuff, resume with normal content
	- simple general instructions/message

144. make a separate chat menu for begin and instructions - done
	- "instructions" on the main menu for chat is out of place

146. need a "menu" option from "goon" - done

157. need to add replies to mail - done

160. add a second vector for each piece of send mail data to allow confirmation, then replace/clear one and wait for user send - done
	- see #161

161. for editing mail messages:
	- may want to clear out the vector data in a different way when requesting confirmation and adding to the same vector when client edits
	- currently, the vector is cleared when editing not allowing a choice or confirmation
	- new vectors created, need new confirm packets for edit then swap the edit vector with the regualr - done
	- some issue with leint crashing likely related to data - I think this was fixed
	- it took a while but the editing update appears to work
		- this allows much more flexibility with editing

167. if next/previous gives no records, set the message_counter to allow the following command, if opposite, to list mail entries - seems to work
	- ie. types next, reset message counter to a value which will allow previous, types previous, gets records
	- did it a little different, if no next/previous, it reprints the last/first list, respectively

176. chat log files are acting funny, they aren't labelled properly - fixed
	- the file never closes
	- a timer may be necessary to sync the hour or w/e time scale is used

178. if mail records = 0 goto mailbox - done

171. login server - "CheckValidChar()" probably doesn't need to be used as much as it is and should be replaced by "IsValidInput()" - done

170. mail archives
	- delete mail - once msg status reaches delete on both ends the mail could enter into an archival status on a separate table - archiving is done
	- 1/2 done
		- db queries need a bit of work - worked out except the note below
	- issue with \ and ' and this process
		- need to reverse the process of omission when reading user input - this is done and revised addcharstostring() to accommodate this


172. add number of successful logins in account/admin account - either from login or content - likley content to get average login time

179. send login count and maybe average login time to client upon login - done
	- db work is done - just a matter of packet sending and text output
	- this will relate to a new user messsage after logging in to content
	- it may be a new packet but most of the work will be doing text
	- see #70

183. Mail replies:
	- keep in mind "mail_counter" and "edit_stat" values - done
		- mail_counter is 0'd out after selecting an option that isn't invalid and isn't a list number(1-20)
		- edit_stat will only have 2 values (0,8) so, keeping track of these will be easy
		- edit_stat is 0'd out after sending and before giving input - and a couple other times in between
	- no editing of the reply name/subject - done
	- add "RE: " to the subject portion - done		
	- need to format the reply message to add the reply to the end of the first message - done
		- fair amount of formatting done
		- the reply has the escape characters removed; need to test with the original messages
			- original message bodies do not seem to account for special characters when sending a reply at least for \
			- apostrophes seem okay after adding "addcharstostring()"
			- backslashes appear to work also

186. issue with content server occurs about 1 minute after a user connects - fixed
	- hunch is with the regular checks the server performs
		- correct, look into <ProcessMessageCounters> with the timer etc..
		- unsure why this doesn't happen on laptop

187. added exe icon to client - done
	- this is a bit of a process to complete, it's not as simple as it should be but, it's not difficult
	- it's about 4-5 individual steps compressed into the 2 below
	- added resource header with <IDI_AD_ICON ICON "ad_icon.ico">
		- "ad_icon.ico" is the file converted from .png with online tool in bookmarks
		- "ICON" is C++ icon standrad thing
		- "IDI_AD_ICON" is icon name with IDI as an icon nomenclature standard, but not necessary
	- added text file and renamed and added to resources in solution explorer for the project
		- I don't remember the exact order of doing above but it is important
		- <#define IDI_AD_ICON 1975> is the only entry but others can be added
	- created resource.h file?

*/

/*
things to figure out, stumped issues or "backburner" todo

12. see if the I can put the DB "catch" handling in it's own function and save a bunch of space - no fucking idea how(put in a separate function and it showed that function/line - maybe usea pointer?)

14. go over the "deprecated" and "server_unimplememnted" files and separate the functions to create an archival file for future use and a reference file for examples

48.	the small descrepancies in recorded time are because "clock()" starts the moment the program is started and the the time value recorded is "now" time
	- clock() is used to calculate logout time and total server up-time
	- in addition, the DB operation to change the `server_time` was using "last_login" and was guessing the program will round rather than truncate which is probably not the case
	- figure out an equivalent "now" time in ms, this isn't very important atm

49. at server shutdown, I may need to delete all memory objects - look into this
	- doesn't make sense but check it out anyways (why would I need to deal with memory stuff if the program is exiting?)

50. timers - there will likely be issues with data type size and "current_time" when the server is up for long periods - will need to address this

----// create content

130. create a backup server for login which will be a copy of login to test remote usage on main server failure

134. make a file for the client which can be read to get login/content connection info
	- this will allow manual changing not requiring a new program if the IP address changes
	- kinda like the config file used in eqemu

173. think about making new chat file folders on a daily/weekly or some other time period to better organize the potential for massive amounts of chat data
	- this can likely be done similar to how the files are created
	- see content "OpenChatLog()" for a way to do it
		- it will be easier than this because there will be no "grace period" - if the time period expires, create a new directory

184. mail archives - packets/menu options need to be implemented if archiving will be a user option
	- accessing the archives remains - this should be straightforward
		- use current lists and add more options to it via the packet ops
		- admin permissions could be put in to view anything
			- this is a good side project to add special mail features for admins


----^^^ create content

----// possible variable data race

74. account info variables will/may need to be locked before they are assigned in each function
	- may need to lock acctID in login_net.cpp

80. figure out how to lock file usage rather than function - for use in <MakeFile::UserRecord> and adminrecord
	- then perform a wait if locked until unlocked

93. client has limited usage of "CharOutput" but the name/password variables should be looked at a bit more
	- some variable were deleted for lack of usage, lookinto this a bit more when things settle down

95. threading notes for implementation: - uncertain threading is necessary with login - but locking variables seems necessary
	- start thread at accept - more specifically, when a "client_id" is assigned, I think
	- end thread at "closeclientconnection()"
	- will need to gaurd(mutex) file reads in addition to writes, according to web info
		- if reading the data in the file will determine anything but there are currently no applications of this
		- unless mistaken, a new client thread would would begin as described above and call the function <ConNet.ReceiveContentPackets();> if the thread starts in content
	- variables passed in a function may not need to be locked because each thread will be executing these functions independently
	- so far, threading is not necessary

----^^^ possible variable data race

----// saving the "state" or "progress" notes

81. saving the "state" or "place" of progress will take some work, server side
	- this won't be necessary for a login thing but getting something in place using what exists in login + content will make expanding it later less work
	- periodic saves, on a timer similar to the server time, is fine, any more often and it could bog down the processes with DB/file updates

113. client ContentActivityTimer should be reset when any packets are recv
	- currently it doesn't work as it should
	- see # 120

120. fix activity timers in all applications
	- will need to change how client activity works or increase the timer a fair amount on client similar to content server
		- client activitytimer is pointless w/o a way to resume progress and/or restart the connection
	- login server client activity increased significantly
	- refer to #81

121. implement an "are you there?" message requiring input to resume progress
	- this can be easily done with current packet handling but not really necessary
	- simple to add with existing "check_client_connection" packet
	- probably not a good idea to have happen when doing mail
		- in fact, when doing mail, the timer should either be disabled or increased a fair amount

180. to work with welcome script:
	- maybe request  something to alter settings
		- this would create quite a bit of work to create various chat/account settings
		- ideas -> set inactive time until logout, requiring ENTER to type a message(this one is good), possibly some other keyboard options, set account lock protocol
		- ideas -> keymapping features for "tell/say"
		- this would work in conjunction with saving the state in a file/DB


----^^^ saving the "state" or "progress" notes

124. some thought should be put into how the packet data type is converted to a string and the conversion from hex to dec/string regarding space/usage
	- 4 dec digits vs 4 hex digits is a huge numerical difference (1.6^ndigits - 1 = how much more a hex can carry than a dec)
	- possibly more efficient to not change the hex length to a string during packet sending

141. make "DirectLogin" more efficient/clean by using menu_stat to direct to the proper prompts
	- low priority, works as is
	- some of this done for content Mail - not the best but not bad


*/