// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
/*
These are the extra things needed for database functionality within the project.

Added:

Client/Server:
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS;
#define _SCL_SECURE_NO_WARNINGS
#endif

Server:
Files:
<mysqlcppconn.lib> goes where the source code is (as currently set)
<mysqlcppconn.dll> goes where the binaries are placed (release and/or debug)

project properties: (tabs/menu)

c++, general, add incl dir:
C:\Program Files %28x86%29\MySQL\MySQL Connector C++ 1.1.7\include;C:\Boost\boost_1_60_0;%(AdditionalIncludeDirectories)

c++, preprocessor, preprocessor definitions:
CPPCONN_PUBLIC_FUNC= ;

Linker, Input, addtl dependencies:
mysqlcppconn.lib;

*/


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iostream>

#include <winsock2.h> // network
#include <ws2tcpip.h> // client

// TODO: reference additional headers your program requires here

/*
// in use
#include <iostream> // cout
#include <sys/stat.h> // _stat - thing with the mkdir function
#include <direct.h> // _mkdir - works with directories
#include <fstream> // file streams - makes use of <std::ofstream> or file manipulation
#include <sstream> // getline(), cout with strings
#include <ctime> // unix tstamp - timestamps and other time functions

// not in use
#include <stdarg.h> // va_copy(args)
#include <sys/types.h> // possibly for UNIX MakeDirectory
#include <conio.h> // several utilities marked as platform independant and not recommended
#include <string.h>
#include <process.h> // used for multi-threading

#include <thread> // std::chrono,this_thread, etc
#include <list> // used for lists
#include <map> // for map/pair
#include <time.h> // clock()
#include <mutex> // mutex lock() to lock a function or something else
#include <algorithm> // transform function to change strings, etc
*/
