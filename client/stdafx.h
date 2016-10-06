// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
/*
These are the extra things needed for database functionality within the project.

Added:

Forget this --
To Change from Console to Windows app:

project properties: (tabs/menu)

c++, preprocessor, preprocessor definitions:
_WINDOWS; or _CONSOLE;

Linker, System, SubSystem:
Windows (/SUBSYSTEM:WINDOWS) or
Console (/SUBSYSTEM:CONSOLE)
----



Client/Server:
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS;
#define _SCL_SECURE_NO_WARNINGS
#endif

Server:
Files:	<mysqlcppconn.lib> goes where the source code is (as currently set)
<mysqlcppconn.dll> goes where the binaries are placed (release and/or debug)

project properties: (tabs/menu)

c++, general, add incl dir:
C:\Program Files %28x86%29\MySQL\MySQL Connector C++ 1.1.7\include;C:\Boost\boost_1_60_0;%(AdditionalIncludeDirectories)

c++, preprocessor, preprocessor definitions:
<CPPCONN_PUBLIC_FUNC= ;>

Linker, Input, addtl dependencies:
<mysqlcppconn.lib;>
*/

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <time.h> // clock()
#include <thread> // std::chrono,this_thread, etc

#include <winsock2.h> // network
#include <ws2tcpip.h> // client
//#include <map> // server
//#include <process.h> // main

// TODO: reference additional headers your program requires here

//#include <sstream> // getline(), cout with strings
//#include <fstream>
//#include <thread>
//#include <algorithm>
//#include <stdexcept> // test - RewriteInput() -> works w/o this include

//#include <stdarg.h> // test - unknown
//#include <stdlib.h>
//#include <conio.h>
//#include <string.h>
//#include <string>

//#include <process.h> // used for multi-threading
//#include <list> // for struct, etc memory searches, etc
//#include <ctime> // unix tstamp and other time functions
//#include <direct.h>
//#include <sys/stat.h>
