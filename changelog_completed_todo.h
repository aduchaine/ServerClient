/*
// issues/todo: completed

deleted many of the first dozen or so entries as non-important to archive


174. chat char output shows backslash as the escape char - fixed
	- this is from the revised handleinput() function, may need a switch for chat
	- the escape char does not show up in other modes: login, mail, or content menu, only chat
	- this is because the user output is deleted on ENTER and the vector which saves the input (p_input) spits out it's contents (as output) after the user presses ENTER

166. delete error message for single-read-message op after invalid input - other issue was error in length condition - I think it's fixed
	- letter characters(and likely special chars) crash the client at this stage - fixed backslashes "\" and punctuation "'"

165. need to distinguish between MAIL_READ_MESSAGE sent/recv messages to format text - think this is done

164. MAIL_CHANGE_STATUS will need some work to sync right with more sending options - done

163. sent message tried to delete when I wanted to view - fixed

162. need to add escape characters to messages to allow apostrophes in DB messages - they are too common to not use - done for ' \

158. AcceptStringLength() can probably be combined with IsValidInput() in client, at minimum - done

156. make a separate case for reading sent messages - done

155. either need to add two records per send or do searches for sender("from") and add sender acctID to the table to make it easier - done
	- this won't work if the recv person deletes the message
	- another option is to expand message type for recv/send - done
		- this would make two tables for messages unecessasry
		- have status read/unread/recv-deleted for recv and read/unread/sender-deleted
			- the recv delete won't be visible to recv and vice versa for sender
			- this will require 2 separate msg status
			- sender can know if message was read by recv person but this will still only require 2 msg status
		- once msg status reaches delete on both ends the mail could enter into an archival status on a separate table

154. immediately before sending the p_mail message packet over, send the message_id over which will be saved in client (separate packet/variable - doing nothing but assigning the variable to the data)
	- upon reciept of the p_mail message packet, the client will send the message_id back to the server to change the message to "read" (separate packet, confriming receipt)
	- ofc, no response is necessary from the server - done

153. need some kind of confirmation for deleting all inbox when 1st succeeds but 2nd does not - done

152. make another case for delete indv fail for input out of range (when less than 20 records shown) - seems to be done
	- remake the list after

151. change the list numbers to always read 1 to 20 on client - done

150. use the display affect for "instructions" on all the lists - done

149. combine IsValidName/Password in login into one function and maybe make it bool to allow more general use - done 
	- the switching of login_status can occur where it is called
	- pass a "bool check_char_count" to check num chars - done
		- pass two more variable to change the length parameters - done
			- think about defining or making these constants
		- think about passing the "accepted" chars from a string or const char * (it seems to require this) array/function possibly using an enumerated list of character types - done
		- could also add a bool to switch/include the unacceptable characters - done
			- this could mean if not all characters are identified, at least the BAD ones won't get through
		- could add a bool to check chars also (so, either/or/and both options) - done
	- could possibly add checkpromptinput() into the main function

148. limit inbox to mailbox status and below - ti accesses from before selecting mailbox - done

147. password masking not working on 2nd/3rd tries - fixed
	- additional issue discoverd which was impeding new accounts from doing the process
	- input was locked and the login_stat was not being passed when not sending a packet
	- fixed by creating "extern login_stat;" in logn.h - hopefully no conflicts
		- this will automatically pass the 4 login_stat values which do not get packets to login_net where loginprocess() uses them
		- a bit concerned with this and creating real packets for these cases would eliminate this although sending a packet is unecessary(except, perhaps, for this reason)

142. capitalize chat log file names - done

	// identifying input project - I'm kinda screwed with \n newline characters and the console with mail/chat atm
140. work on passing the "required_key" variable for InitiateUserInput() - I think this happens already

139. add a check in client for version number upon establishing a connection - done
	- this client version will be stoerd in login and if it does not match send notification
	- try to add a choice to download the new version
		- this will require a link to a place where the download is hosted
	- defined version#

138. create a way to turn the test into release - done
	- for other computers try to create a directory from root
	- define ip addresses/ports
	- define the file directories
	- see # 134 - this could still be useful
	- defined SERVER_CONNECTION and added to all functions where relevant

137. create a mail system - basic complete
	- use ideas from eqemu
	- basic system in place, lots of notes
	- expanded the program by about 1/3

136. issue with chat showing up to others and logs - fixed
	- exact line was: [adrian SAYS] ok, time to (go) - go was missing
	- size of message was incorrect for "say"

135. verify client program does not send time values which could mess up the server
	- doesn't appear client local time is ever sent

132. a chat message slipped in before joining chat
	- this likely occured just before transferring to the content server when user input is not locked - it's likely fixed

129. work on num packets for bad_log in content send and login recv - done
	- other issues with bad_log
		- admin logged in seems to mess things up
		- one of the timers does not disable or something - not this
		- when logged in accounts are a combination of content and adminlogin it doesn't work
			- all else seems to work
	- changed where adding admin accounts to content logged accounts

128. lockouts fucked up again, seems to happen once just after logout from content - seems fixed see below
	- likely a logic issue
	- lockout time occurs just before?? logout time
	- issue appears to be fixed by fixing bad log packet processing

127. issue with typing/delete-backspace and recognizing what was typed, look into special char expemptions - easy fix
	- if backspacing when at x-pos 0 it will go negative, need to make it unsigned or put some different logic in
	- added x-pos check

126. logout messages and joined/left chat messages do not clear the chat prompt
	- joined/left messages are removed, this bug was caused by other output (printf/cout)
	- forceful logout of any kind still leaves the message but this isn't a big deal

125. better packet send handling from entry and typing a valid command but no server to connect to
	- it's ok, it just times out after about 20 sec with no server response

123. shutdown issues on content - see grep error messages in content - done pending testing verification
	- occurs when loss of server connection performs shutdown
		- look into <SendClientPacketErrorHandling()>
	- this may be fixed and the issue could have been aroused from all the recent changes
	- keep an eye on logs for similar issues

// much of this is chat progress or related to chat implementation

122. make the instructions read easier by putting the first line at the top of the console and writing down until the bottom - done, quite easy with existing functions
	- this can be done by writing the first line then doing "\n" x the number of additional lines then going back up to the spot and writing back down

118. issue with server transfer on login not completely logging out the user to transfer - done pending testing verification
	- some of this may be the client timer checking then doing things incorrectly

117. dual log - "DUAL_PROCEED_OLD", etc. need to get name of account to logout - fix found
	- create getnamefromacctinfo by acct through "AccountInfo_struct"

116. LoginPacketTypes/Content and other action-types which are used together must be enumerated together - done
	- these "packettypes" should be renamed to reflect their more broad usage
	- to keep organized, non-packets could begin at 1000 or some other distinct and semi-arbitrary value
		- atm, 501 is the beginning and are joined with other non-packet actions already in use
		- more/better organizing should be done as the need arises

115. client constant looping - looping aspect complete and seesm to run well
	- can probably put the "GProg.GetInput()", etc into another function and run it when "getline" is currently called - representing a specific step - some of this used
	- pass the string from it and maybe login_stat to it or a new enumeration for the step requiring getline() - some of this used
	- before each "getline" run these two functions and pass the result - result is passed as a class variable
	- should think about either adding other platform functionality(to learn) or think about any big picture (lack of) uses for the _getch() functions
		- for example, are games made with _getch() and if not, what is used
	- need to limit which login functions are called - limited with "MenuStatus"
	- specific notes are located in login.cpp
	- most the the current fucntions can be executed within a switch statement with new enumeration for cases which cannot be identified with login_status - switch not used, "NonPacketActions" created
		- the current packet switch statements should be suffice for asigning functions/login_status
			- rethink, all the functions in the switch statement will be put into a master login function and login_status can be passed to that - done
			- process input should be the first function and the other functions called based on login_stat assigned - done
		- as stated, new login_status will need to be created when requesting two inputs for validation - done
			- a non_packet type similar to what exists for chat could be used - done, "NonPacketActions" created
	- assure the only login status changes while in between steps are the most essential like losing a connection/force logout - look over this a bit more
	- the new main function for logging in should include all functions related to login - done
		- the input functions should come before these functions - done
		- the t/f switches will work in conjunction with login_status to process the input - done
			- "process_login" and login_status will be checked at the beginning of each function 
		- the current login functions will need qutie a bit of alteration
			- each distinct step will be labelled with login_stat with new status and will be a separate part of the function - done
	- first major portion completed
		- will need to change how client activity works or increase the timer a fair amount on client similar to content server

114. the other processes in login and the menu in content could be handled similar to chat to allow packets to be recv/processed as they come in rather than when enter is pressed - done
	- this will make things a bit easier and fewer packet recv errors will result
		- much of this is taken care of as a result of recent changes

112. major issue - need to handle the situations where multiple packets are recv by server "simultaneously" 
	- most of this complete except the main purpose - done but needs testings
	- will need to add a packet size header most likely
	- this will probably simplify much of the recv system
	- sleep value should probably be reduced a bit until recv protocol is redone
	- recv packets is complete for all apps
	- sending shouldn't be an issue although doing the same/similar should be done
	- some difficult testing remains

Likely need to send the size first then the opcode
	- size will need to be at least "int" length or 4 bytes(65kb) or 6 bytes(16mb)
	- 6 bytes would work well with all opcode lengths
		- 4 byte code for size is the prefix
		- significant issue with sending this value as a string
			- does not cause any problems atm but, the size is limited to the number of digits(4) -> 9999 instead of 65535
			- the same issue applies to other numbers included in the packets
			- alpha characters are fine to send as a string

	about half done - lots of testing needed - all complete several bugs fixed
		- login/content/client all send done
		- content/login/client all recv done		
	still inserting chars in string which are less than data type sizes
		- added chars to "size" and "op_code"
	this opens up the entire range for data to be sent not just 127 packet code values
	max packet is either ~65kb or ~16mb
	unsure if there wil be issues with opcodes with multipe functions based on how much data is sent - changes made in the switch
		- much of this was completed before doing this - only a few likely remain
	the big issue is -> still need to figure out multiple packets being recv
		- look into "remove_front" function
	quite a few variables in the recv function which should be named a little better
		- many small functions as well
	- need to clear out some variables after send/recv, specifically data3/4
		- doesn't seem to affect much atm but possibly could
	- will need to test/pay attention to chat functions which extract name/message
		- the xpos may need to be altered
	
	- issue with validate creds in content - fixed - was hard to find errant variable
	- possibly more efficient to not change the hex length to a string
	- need to test bad log - redone and seems to work well
	- proceed stalls just after pressing enter with "proceed" - logs out the other user - fixed
	- all client pckets can likely go into "client_packet" although having it separate is helpful to know what is used where

111. chat works on first login only, later logins do not scroll the message but seem to print correctly - done
	- type of logout doesn't matter
	- the line number is limited to 299 which is probably the max display
	- fixed by removing the portion which moves the line down and adding a newline "\n"
 
110. backspace should delete text - done

109. fix client chat sending name if is only tell text ie "@adrian" <-enter sends name + name - fixed

108. dual_log issue likely with deleting memory before/after switching login - done pending testing verification
	- also not logging out 1st user
	- seems fixed but, more testing should occur

107. need to add in buffer overrun handling in packet types, specifically chat - done
	- chat and name/password validation done, can't think of any other that need it
	- did not add during packet send

106. make message initiation require ENTER - difficulty with this - fixed but platorm dependant

105. add a "left the room" message when a user leaves or logs out/in - done

104. create a chat file - possibly each hour or something similar - only thing that reamains is creation of new folders at the end of this item
	- master chat file -> timestamp, labelled with name/recipient (if necessary) and message - done
	- daily for file name or possibly a timestamp filename with the file persistently open and closing when inactive for a period of time
	- should probably work similar to the session logs, at least in naming
		- maybe a timer or a check of the file name periodically in conjunction with activity stated above to make a new file
		- could create a new log category "chat" with subcategories for say/tell - too problematic, complete with current formatting
		- formatting the logged messages should be in 3rd person, ofc
	- timer created to allow/restrict creation of new chat files
		- purpose is to not cut off converstions by different files
		- logout/"soandso has left the room" messages will go into a diffenet file with current timers
			- chat IDLE_CHAT timer should be longer than INACTIVE_CHECK X 2 timer or (this isn't reasonable)
			- chat users should be booted to content menu in conjucntion with IDLE_CHAT timer or a similar timer
	- remove timestamp for date if the file name contains, leave in the actual time

103. put a limit on messages/minute - done
	- create a map/list? of client sock_pos, timestamp, message_count
		- maybe a map for client sock_pos/timestamp for each sent message and another for sock_pos/message_count for each client connection
			- records will be erased regularly(each second?) for the timestamp and updating message_count with an operator ++/--, possibly
			- one extra packet for "MESSAGES_EXCEEDED"
			- use #define MAX_MESSAGES 10 and #define MESSAGE_EXPIRATION 60000 - or some other values to be the basis of the conditional checks
	- this could occur in content "Net.ProcessChat()" or immediately before it processes
	- if process chat goes through, increase the counter
	- if the timer to remove count# checks or another message comes in, remove from timestamp map
		- after logging out from content, the maps should be cleared for the sock_pos
			- this implies needing an artificial wait to join chat, but not too high - forget the wait do below
				- one cycle of login from start to logging out may take a minimum of 20 sec
				- the MAX_MESSAGES counter should maybe be increased artificially upon entering chat to account for repetative logins
					- start with timestamp(now) and count(5) maybe
	- start the "ChatMessExpireTimer" when a user is in chat, stop it when noone is in chat
	- issue with initial inflated count and erasing via processing - fixed
		- if a user is present in the list/map and has used chat from the begining of login (this means the user was sending messages) and not inactive(60 sec timer) the counter will not go below 5
		- to resolve this subtract the 5 on the first successful check by checking DB login time or add another variable to the <list> - done
			- another option is to have a client timer send notification of expiration - this is a better option for overhead
	- the map associated with this process can also be used to see if a user is logged into content
	- erasing the records seems to have an issue with "RemoveChatProbation" - it seems fine after this
		- maybe searching the map first would eliminate the funny erasure
		- the issue is narrowed down to logging in
	- the timers are interfering with each other - a mutex needs to be done or something to coincide with the timer better
		- a few logic errors were fixed and a new function added, mutexes seem to be a bit more important

102. make no sending with empty messages - done

101. account for single character messages - done

100. users from content after inactive forcing program close - fixed
- variable in itializelogin was not defined

99. need a "chatroom" flag in useraccount_struct, or similar struct - done
	- this will make it easier than not sending to servers or DCd users

98. output should be threaded so a user can type and recv chat at the same time - done but with a different method
	- placement of user chat output will be an issue

97. some errant DCs do not seem to remove user/client from various lists - fixed account list removal - done pending testing verification

96. to implement a chat server into content:
	- perhaps this could be practice for multi-threading, as well
	- this can be added later
	- added a thread for the output function but it isn't necessary with loops set up properly and didn't notice much of a difference
	- get/save user name into memory for the tag at the beginning of the line - done
	- use "SoAndSo to All:" - "SoAndSo to Me:" or something
	- create the string packets from existing packet types in unimplemented - done
	- may need to give it a packet opvalue like all the rest based on the chat command entered
	- the packet opvalue will call the function to repeat the what was typed to the recipient(s) - done
	- this can easily go in one function with the opvalue as a passed variable

// much of this is chat progress

92. look over <AdminNameConflict> to see if it can be changed - done
	- it should use "account" table because a new admin will not have a record - which it does

91. issues with admin+shutdown - content shutdown but login did not
	- entered user name (adrian) then switched to admin
	- also fix text output - make shutdown message happen when it actually happens
	- packet error handling make better (10038/10053 I think) on send - this is probably okay
	- change file log messages
	- logging in on login then choosing admin and shutting down does not work sometimes
		- error 10053 loops endlessly
		- connected prior to timers started(was first socket/socket_position)
	- the above symptom does not alway occur (it shut down serveral times in a row regardless of apparent conditions then repeated the same loop) - 
		- it logs out admin then loops
		- I think <SendServerPacketErrorHandling()> may be causing this issue with <restart_conn = true> - may consider not including it?
	- added a check in <ProcessLogNetTimers()> to skip it if <restart_conn == false> which will solve the problem but, should look into the placement of this function

90. "ForceLogoutByAccount()" now only applies to dual_login and should be renamed or changed to allow more options for this useful function
	- the other option is to revert the function to "bool ForceLogoutByAccount(int account_id, bool admin)" and handle dual_login in content_net (better idea) - done

89. content: FindUserAccount() - need a better way to send packets to server - done

88. "User_Verif_struct" in content can be turned into a map - done

87. struct NewName_struct - this could be easily turned into a <map> - done - seems to work fine
	- wouldn't need 5 separate functions, maybe two fairly simple ones(3 actually)

86. GetReservedAcctID() - this also erases the record from memory on success - should look into the need to remove the acctID if something else happens
	- changed and created a removeacctID function

85. add "AccountInfo_struct" to content server, it may help #77
	- added but not implemented
	- done - removal of many DB suages and some other function usages for, hopefully, a bit smoother operation
	- there are many memory queries and I'm not certain how much faster these are - seems they are quite a bit faster even if there are more of them

84. fix logout on client to do the actual DB/file logout + cleanup sooner - done
	- would need to do most of the logout stuff separately making it a pain
	- an alternative is to do it all earlier and put all the client ending text in one place
	- make logout occur the moment the client chooses it and all the other stuff that happens after is after a logout on server

83. issue with locked accounts after logging in is the 3 second timer waiting on all packets - done
	- this means the login server check needs to occur at the same time the content server checks for logged in accounts - done
	- the fix will make the window(of time) of errant badlogs miniscule but still existing
		- this implies the login server should signal the content server to do a check for accounts logged into content
		- to eliminate any chance of users getting locked out errantly through this process, which will be easy to do
			- give the badlog timer in content to login
			- make a new packet to signal content rather than the timer signaling
			- obviously, when the packet is sent, login will do the DB check/save to memory

82. content: update server time on all server logouts where possible - this appears to be done

79. calling shutdown makes content server pause for a second or two <serveractivitytimer()> - seems like a non-issue
	- make sure it's not trying to send data
	- likely one of the timers waiting on something but possibly it's trying to send packets (if this were the case an error message would be noticed)

78. fix file logout for user/admin name - I think this is done

77. fix dual-login file logout order - seems fixed but another issue remains - done
	- content server logout probably has issue with the mutex lock
		- the login on content takes a bit of time because of DB queries, likely, and will result in out of order/non-existent logs
	- trying an alternative
		- login -> don't send success to newer client until logout of older by waiting on a response from content server - done
			- then send success response
		- in the process, creating server-specific packet sending function which will need to be expanded dramatically
			- this function is a pretty good way to send many kinds of packets as a server sending to a server-client
			- this function can be expanded to include clients if the lists/passed variables will be able to coincide

76. change -> CHANGE_TO_ADMIN_LOG to CHANGE_TO_ADMIN - seems to work - done

75. add bool to "net.removefromlists" to reduce the number of packet types - ie. CHANGE_TO_ADMIN_LOG is noted twice(i think) - done
	- producing a result from a "find" search will make it do certain things and not producing a result will do fewer things, in general

73. change data types of return values in certain DB queries (CheckFailedLoginCount())
	- changed a couple in both login and content - was using "atoi" which was unecessary

72. add a default for packet sizes not covered - done for all recv fucntions
	- add to all programs - client is important for proper notification(or lack of) and server for proper logging

71. not logging out admin on server force shutdown - fixed in CloseAllClientConnections()

69. create dual login functions rather than keeping them all in the specific account login functions - done

68. create a memory vector for names/passwords/acctID so users only need to send PW one time if valid - then delete the entry after logout/server transfer
	- could even load this into memory the moment a name is approved to make things faster - done
	- this will reduce DB queries by about 2/3 for login
	- memory and DB queries created - implemented in login_net.cpp immediately after name approval
	- many things to account for if the user decides to go back or change something and does not login and does not close the connection
		- change pw will require a remove and another add after(maybe)
		- maybe if the client_id does something other than enter a pw, the memory is erased or if going back
			- maybe not as difficult to monitor just an extra place to put remove("back")
			- this will go where removefromnamelist() will go since the same changes apply
	- complete, I think, and seems to work
	- a several things can change as a result of this:
		- client packet sending needs to correspond to any changes - packets numbers changed and data packets reordered somewhat logically
		- login success: - resolved
			- will not require user to send PW again
			- can get acctID from new "AccountInfo_struct" and not the DB
			- can also use use c/socket_position in place of p_name, in many circumstances
		- UserLogout(): - resolved
			- can probably use c/socket_position to logout users now in place of p-name
			- this is incorrect, files are currently labelled by name
		- many of the things that use "p_name" after a successful name entry can now use "acct_id"/"client_id" which may be faster due to data type differences - resolved
			- client_id cannot be used for many items due to file/DB records
		- technically, as long as client disconnects are tracked precisely, the user only needs to send a valid name/password once each - done
			- this will be more of an ongoing thing to change over since "p_name" is used so much
			- most changes are done
	- need to save pw into "AccountInfo_struct" for new/pw change accounts upon pw approval - done
		- "CheckAccountPassword" will change it's varaibles - done

67. create a memory vector/map for acct IDs reserved for new accounts by checking the next-in-line - done
	- for the purpose of reducing or eliminating DB issues and badlog lockouts
	- first check if map is empty
		- if not empty, increment 1 position to next ID value
		- if empty, get next autoincrement value from DB

66. users need to be added to userstruct before DB operations login the user/admin - done for all but new accounts which the solution is listed #67

65. bad log is not checking for current lockout properly - it is only working for users properly, it appears - fixed

64. login server does not logout admin when inactive in DB - fixed

63. admin account lock after loggin in as admin what happened: - seems to be fixed
	- login admin, logout admin, login user greg, continue, logout, specific steps: login->old->#admin->get accpeted -> crash
	- db/files show admin login and files immediately show "improper logout" then the crash
	- fixed crash, most likely, and issue is related to badlogin(sic) functions

62. intial admin login marks twice in file logs - startfilelogs could have a first login variable or logsuccess - fixed

61. issue not logging out correctly with new vector and socket_pos - mostly fixed
	- client needs a larger range in <if (packet_size >= 10 && packet_size <= 12) {>
		- changed likely issue in code to (packet_size > 2 && packet_size <= 12)
	- my guess is a packet or two are being sent before the server DC which then sends the server DC message packet to client but, doesn't pick it up
	- should add a default for packet sizes not covered	- see #72 for solution
	- need to test
	- assure db/file logging - seems to be working

60. being forcefully logged out by DC/force_log makes "lost connection to server" message appear - see #63, etc
	- this isn't a big deal but, for perfection's sake, I could change this - this is ok
	- more DC packet sending errors handled

59. put something in to make notification of locked accounts with no entry in "lock_accounts" table - done
	- missing one small thing for sql queries but, everything is in place
	- used combination of multiple selects and simple code to accomplish this

58. handle intial packet sends from client if they don't get through
- works as it should with ~20 sec delay if it can't intialize the connection

57. admin login is checking user lockout - the recent work on this feature should have fixed this

56. check other server connections from server as client(login server) similar to client and try to restart connection
- server to server(client) connection check implemented - done

55. re-implement the original <map> idea for client connections when accepted - done - could use a bit more testing before closure
	- instead of map use vector for socket_position
	- make it start at 0/1 and iterate through the vector until it finds an opening, then save it - whereever it starts, it is working
		- starts at 1 in most cases

54. need to fix startup lockout stuff, if multiple entries in `locked_account` table show account locked it will flag accounts as not locked - fixed
- this isn't a big issue because the table checked when a user logs in is the `locked_account` table
- with <DB.CheckAllAcctLockout()>

53. add a "isserver" variable to the function <LogoutByConnection()> to gracefully close server connections and log them appropriately - done
- this is the cause of the error messages in content

52. make a packet to send to login to update server time instead of content server with error handling to shutdown and do it "himself" if not recv
- this isn't necessary although a regular "check-in" with login server wouldn't be a bad idea - done

51. a memory allocation needs to be made for locked out accounts and compared to the memory allocation for accoutns which have bad logs, or combine them into one memory - done

47. add a timer to update server time in regular intervals while active - done

46. see <AddToContentUserList> in content server - issue resolved

45. get a DB check in that will not add another entry if lockout is active - maybe update the time with a new time or ignore(probably ignore) - complete
- major features added <CheckBadLogAccounts()>

44. if DB connection is closed, it throws an exception when it tries to close it again - figure this out
	- easy fix, changed DisconnectDB() function to return if already closed

43. client error handling with new timers - error 10054 connection reset by peer at packet send could be handled a little different and the text handling on recv - done
	- errors are no different and they are correct but, could omit the technical jargon through an if statement similar to ws_util.cpp
	- missing a message for content server abrupt DC - I think this is debug again

42. figure out reconnections after a disconnect - some pseudocode above - done
	- a recheck of connection could be done or reinitialize
	- being DCd will change "connected == false"
	- a controlled DC is done by admin which could give it an additional flag to actually close down the conenction
	- other instances could start a timer to try a reconnect(intialization of the connection, again)
	- testing could be done by unplugging the internet
	- a new flag should be implemented first signalling a controlled DC
	- much of it works but still several issues
		- the content activity timer does not fire often enough to remove a previous server connection if the same server re-connects quickly
			- this makes content disable the timer which trigers a connection close
			- this makes me think rather than waiting to close the connection, it should be closed immediately
				- the only thing to wait for is a possible server re-connect or server shutdown
			- after disconnect, the handling of the timer/closeallconnections should be done somewhere else
				- possibly in the same function that sent the packet to check on the connection
			- the timer will need to be disabled in two places for the most accurate handling
	- everything seems to work although it's difficult to test the server's own connection and it may be unecessary
		- added to content server <tryconnectcontent()> to retry if connecting fails

41. test the timer variables more
	- enable, disable, start, restart
	- make sure the values don't carry over to other timers - it doesn't seem to but I have only 2 timers atm
	- some quirks with the system but it can likely be overcome
	- timers seem to operate independantly
	- disable doesn't seem to work independently - possibly need to add as a function variable
		- really stupid mistake on my part not giving the function the correct operator
		- seems to work better now although should come up with a test of multiple timers on various timer amounts which would overlap
			- the test seems to have been accomplished from normal expansion of the program
			- everything with timers works if implemented properly
			- some inuitive improvements could be put in over time

40. content not closing login server connection on certain abrupt closes although it was in debug mode - see #43,#53 - done
	- make sure notification/logging is on
	- it closes the connection in normal mode but the DC handling shows ERROR because no case exists for servers yet

39. check for logout descrepancies periodically when the server is up - done I think
	- this will require a working timer system - done
	- compare is what is in the DB minus what is in the "User_struct"
		- get account_ids by iterating through the socket_positions
		- search the DB for time descrepancies from all accounts not including the ones returned from the previous step(currently logged in clients)
	- got most of this working but,
		- need to get data from content server on who is logged in or do this process on content because it will lockout active users from login
		- this may require a bit of work - done
			- the packets should not be sent one by one for each client due to possible large numbers of clientIDs/logfails
			- a new packet type may be needed, let alone a new packet
			- get the data/packets from content to login, can worry about the packet type later
	- login server implementation nearly complete
		- need to add a <FindUserAccount()> similar to content server to check for logged in admins - done
		- quite a bit of organizing needs to be done when complete - nearly complete
	- new packet types, everything seems to work in this regard, a few small bugs in logic
		- I think all bugs have been worked out

38. create LogFail() categories in the DB to return the category value to send the proper packet upon user login
- enumeration done
- do DB recording - categories are done
- need to finish client packets/text for the various categories - done
- issue with multiple entries in locked account table, an expired entry makes newly locked entries unlocked - fixed

37. issue with quit again, before loggin in - fixed, did not have send coption in LogoutByConnection() when logged in was = false

36. create an opton for notification in StartUserLogs() for the general log file
- also did this for admin logs
- renamed function variable and added another - user_log, admin_log, main_log

35. possible issues with QUIT and text and/or notification of closure to client - fixed

34. Rename client "textmessage" to "LoginMessage" and create a new general category, "ClientMessage" before the list gets too big - done
- the text response system could use more standardization which would make it easier to follow and possibly easier to expand upon
- loginmenu() and goon() use <text.loginmessage(value);> within their functions to simplify things but it could cause issues down the road
- it's an intuitive idea but changing this would eliminate the need for the "hack" in these places and give more flexibility of direction - done

33. need to work on timers - make them easier to use, duplicate, etc - done
- specifically the timer starting server comm
- make a timer class similar to eqemu
- mostly done, works well although some tweaks will likely occur

32. LogoutByConnection() is capable of handling all instances where CloseClientConnections() is used - it should only be replaced outside of it's class
- it's not necessary but it would simplify readability

31. logging in normally shows being logged in already when the server shutdown unexpectantly, this is apparent by DB entries, a check needs to be done on server startup to resolve this - done
- the client accounts affected should be file logged when resolved
- half of it is working, the file logging but, updating the DB with the results of a different query inside a while loop is not working - fixed
- try LogFail()
- the issue was multiple items or items being destructed, had to create multiple instances of these things (DB Statement, ResultSet)
- another issue was closing the DB with a DB query within a query

30.	test c = 1 at some point in fucntion <DirectIncomingConnections()> where clients are accepted but NOT where the fd is set to zero
- changed max_clients = 31 and client_sock[31] from 30
- this could allow certain things to be a little easier with no 0 involved

29. test server packet sending with changed function variables regarding error/client DC handling - seems to work
- hold off on closure for a bit

28. go through some of the packets and consolidate the more recent ones into a single OP and put into client packet, if necessary - done
- DUAL_LOCK_ADMIN(1,2), SERVER_DISCONNECT, may be candidates for consolidation
- TO_LOGIN in content may be deprecated or could be touched up with the new SERVER_DISCONNECT
- negative, TO_LOGIN is needed for certain cases and SERVER_DISCONNECT for others
- only one was needed two cases of FORCE_LOGOUT(1,2) were put into one, they do the same thing respective to their server

27. login server fails shutdown if it can't connect with content server - fixed

26. typing quit after logging in and before continue gives two bytes extra -> response from server somewhere - fixed

25. RemoveUserFromList() could go in removeclientconnection(), I'm unaware of an instance where these two would not happen together - done
- RemoveUserFromList() does not always need to happen when closing the connection although I doubt it would affect anything

24. try to figure out better data types in client for user input (p_name, etc) so I don't need to use <GProg.CharOutput()> constantly, or forget to use it
	- THIS MAY NOT BE A GOOD IDEA! POSSIBLY REVERT - it should be okay for functions which will end up converting anyways, DB queries/LogRecord being the most often used
	- get back to this later, when I know more
	- some of this was taken care of with the different memory usage
		- all/most instances where p_name/p_password is used is required with current implementations
		- there are no current password usages on content server
	- majority of <GProg.CharOutput()> usage is in File.LogRecord()
	- second most is in passing variable "p_name" from client/DB/memory
	- switching over to <const char *> is going to take a fair amount of work
		- most DB functions changed, very few acct functions changed
		- logrecord/adminlogrecord will take a bit of work
		- <GProg.CharOutput()> usage cut down drastically but the real difference is neglibile
			- some <const char *> variables are passed which may make a small difference 
	- most conversions completed where passing the variable will lead to a DB query or LogRecord
	- several which could be converted were not due to possible future uses
		- it's recommended to use strings where possible because c++ has usuful libraries dedicated to them
		- the overhead will be mostly neglibile in using strings although the program decreased in size after the conversions implying a more efficient program overall
	- client has limited usage of "CharOutput" but the name/password variables should be looked at a bit more

23 think about recent dual-login and admin+user being logged in - I originally wanted user to be logged out for admin to work - done
- maybe add double the checks for admin
- <RemoveUserFromList> issue
- fixed many of the issues althoguh there may be outliers

22. need to add something to recheck server/client connection, on a timer, if necessary - done
THIS COULD BE USED FOR SPECIFIC PACKETS WHICH WOULD IDENTIFY CERTAIN ACTIVITY - some of this was implemented in #39
server DC timer on client does not work as it should
- it starts the timer on packet send and will check the timer immediately if client had been idle for some time before server DC

- server should have a general timer to check a memory struct and enable it upon the first receive - thisseems like a lot of work for the machine
- upon packet recv -> purge old client entity if applicable
- save the time of receive and entity_id/client_id/c into this struct
- timer gets checked and then checks the elapsed time (set the elapsed time boundry to 10/20/30 seconds, w/e)
- close client sockets if they closed their end?
- it won't get to this point if client sends a packet
- send out a specific dummy packet if the client is beyond timestamp+boundry to see if the socket is open
- if not open, close socket and any files/DB records etc
- if open, set another timer or make the new boundry time 40/50/60 seconds or w/e
- after this checks close socket and any files/DB records etc
- it won't get to this point if client sends a packet

// this should probably be placed before the main loops starts - done
---- start the timer soon after server startup -> GProg.Some_Timer(some_timer, true); (true is to restart the timer when it checks)
// placed within recv loop - done
---- on recv, check memory for ClientSocket given in recv fucntion(ClientSocket)
---- if not exists, create "new timer1_struct" and insert time -> client_id && (current_time(clock()) || current_time in sec) - no break/return/continue
---- if exists, erase memory and insert new time -> client_id && (current_time(clock()) || current_time in sec) - no break/return/continue
// this can be easily placed within the main loop but has to omit certain server connections
---- check the timer when the timer expires -> if (GProg.Check_Some_Timer(some_timer) == true) - timer will reset
--- start a for loop for below
---- check memory and cycle through ClientSocket/timer_value -> find ClientSocket	- nothing to find, it should loop through all ClientSocket in memory - they are there for a reason
---- if (timer_value + grace_period < current_time)
---- send a dummy packet to check connectivity -> LoginPackets(dummy_packet, ClientSocket, c);
---- else -> do nothing - client either sent a recent packet or grace_period isn't expired
// this needs to be separate and somehow distinguished, it may occur within the packetsend or recv function?
---- if (certain_error_code signalling thier socket is closed) - sending a packet to a closed client socket will close the server socket for the client - done
---- erase memory - closeclientconnection - close any files/DB records etc -> probably a "break;" - break won't be necessary in certain functions
- this is standard behavior within send packet fucntions
---- else -> check memory and cycle through client_id/timer_value
---- if (timer_value + grace_period2 < current_time) -> closeclientconnection -> close any files/DB records etc
---- else -> do nothing - client either sent a fairly recent packet or grace_period2 isn't expired
//--// to exclude servers from this process, maybe remove and add the server to the struct at "servertalk#" or whereever the packet is identified
- adding another column to the struct to identify the connection as a server is necessary - done
- checking the memory and skipping the properly labelled servers from the client DC process -done

New needs:
timer - done
checktimer - done
- "enabled" variable may cause issues - unused, always enabled
packet_time_struct or map? - deal with learning/implementing map later
all functions dealing with inserting/checking/deleting memory
packet type - done
Decide on:
time format - should be seconds because the stored value will be too big for the variable, if in ms, after a while - a division of the timer value is necessary
timer_value - for timer - done
grace_period(1,2) - for memory - use one constant and just multiply it for the second - done

what happens if client DCs and new client takes the client_id/socket_position?
	- pretty sure this means the socket was closed on server side, otherwise, it would not be available for a new client

21. assure the only time the client leaves the program is by choice - seems to be the case
	- quit does this
	- being disconnected takes client to entry menu in all cases so far

20. solve possible name conflicts with simultaneous new accounts - I think it's all covered
	- writing to the DB an intial name entry acceptance - not good, too much time invloved
	- having a list - this one is likely more fail safe
	- will need to mutex lock the list/variable within the function during writing/checking - done
	- this is where the server timer would make sense so users can't hang on names for long periods w/o doing the next step
	- erase the entry if user does not continue the process or finishes login reg
	- struct should be socket_position and name
	- if the connection is lost, the name is deleted
	- if a new connection is created with the same client_id/socket_position of a client that ninja DCd, the memory position should be deleted
	- names are deleted from memory in several locations, I think I am missing a case though - maybe not intial testing seemed fine
	- name are deleted if server closes client connection, regardless of whether or not a new name was created for the socket_position - not efficient but it works
	- names are deleted if the user changes a menu option

18. think about what happens if server intialization fails but DB/file logs are active - done
	- appears to be taken care of in both servers - if winsock, etc can't be intialized, connected = false and skips the loop going directly to exit

17. figure out how a program overrides a prompt for user input and splashes an "are you there?" message - no message but done
	- must be a threaded client app?
	- by this I mean not being blocked by "getline"
	- must enable chat-like features through a thread, most likely
	- during chat, this has been accomplished
		- all of content does this now when user input is requested
	- changed how the program operates to function in this way

16. implement a new packet case for double name on registration - done, packet similar to double pw
	- and blocking of logging in more than once - done large project

15. database.cpp "query" recognizes the escape "\" and "'" character(in odd incremenets) and throws an exception, tested several others with no error - workaround?
	- it's the input it's receiving from "getline()"
	- there may need to be something to catch/change these characters - catch these characters server side
	- it probably needs to happen before it gets to querying the DB - since names cannot have special characters, tell the client invalid
	- this will work but does not solve the more important issue of how to format the string to accept special chars when accessing DB info - may not be possible with mysql
	- created function to use prior to doing a DB query of any kind with user input <Acct.CheckValidChar()>

7. need to add something to recheck client/server connection, on a timer, if necessary
	- client is done and working okay - see below
		- should try a reconnect when timer is up and if fails set another timer and try reconnect if that fails go to entry
	- a delay isn't practical when explicitly waiting for a user's input on the client
		- if the server DCd at any point after the timer expired but before the client provided input, the client will immediately go to default status - this doesn't apply any more
	- a recheck of connection could be done or reinitialize, although saving the "state" or "place" will take some work, server side
	- if the process is fluid it will make sense to have this in place, it's currently poitnless unless the above is implemented
	- timers redone, works well - after a packet send, the timer starts and if server was disconnected when the packet was sent, it will go to entry menu
	- error 10054 connection reset by peer at packet send could be handled a little different and the text handling on recv

5. have "loginserver" send authorization key to the client and also the content server - done
- think about the timing of recv packets to content server and variables involved with this
- don't think it's an issue now that the data is stored immediately upon receipt
-------- this is not very important except to know how to do it -----------
- so, this should expire slowly(in comp speed) - .5 seconds(maybe less) because of send to client and client send to content serv on slow connections
-this will hang other connections making the attempt, I think - no longer an issue
-the 3 tries lockout would work for this too - random number plus acct id from client three times and perm lockout with notification to client
-each try will erase the array values and request a new one from the server, up to 3 times, ofc - this won't matter much if these numbers are intercepted

2. may need to shutdown client connection when going to admin from GoOn or going back from before logged into admin - think about this briefly
- changed a few small things, seesm to work with no errors now

*/
