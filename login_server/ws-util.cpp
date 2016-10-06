/***********************************************************************
ws-util.cpp - Some basic Winsock utility functions.

This program is hereby released into the public domain.  There is
ABSOLUTELY NO WARRANTY WHATSOEVER for this product.  Caveat hacker.
***********************************************************************/

#pragma once

#include "stdafx.h"

#include "ws-util.h"

#include <algorithm>
#include <strstream>

#include "servernetwork.h"


using namespace std;


#if !defined(_WINSOCK2API_)
// Winsock 2 header defines this, but Winsock 1.1 header doesn't.  In
// the interest of not requiring the Winsock 2 SDK which we don't really
// need, we'll just define this one constant ourselves.
#define SD_SEND 1
#endif

//// Constants ////

// const int kBufferSize = 1024; // unused in favor of the default I use

//// Statics ////

// List of Winsock error constants mapped to an interpretation string.
// Note that this list must remain sorted by the error constants' values, because we do a binary search on the list when looking up items.
static struct ErrorEntry {
	int nID;
	const char* pcMessage;

	ErrorEntry(int id, const char* pc = 0) :
		nID(id),
		pcMessage(pc)
	{
	}

	bool operator<(const ErrorEntry& rhs) const
	{
		return nID < rhs.nID;
	}
} gaErrorList[] = {
	ErrorEntry(0, "No error"),
	ErrorEntry(WSAEINTR, "Interrupted system call"),
	ErrorEntry(WSAEBADF, "Bad file number"),
	ErrorEntry(WSAEACCES, "Permission denied"),
	ErrorEntry(WSAEFAULT, "Bad address"),
	ErrorEntry(WSAEINVAL, "Invalid argument"),
	ErrorEntry(WSAEMFILE, "Too many open sockets"),
	ErrorEntry(WSAEWOULDBLOCK, "Operation would block"),
	ErrorEntry(WSAEINPROGRESS, "Operation now in progress"),
	ErrorEntry(WSAEALREADY, "Operation already in progress"),
	ErrorEntry(WSAENOTSOCK, "Socket operation on non-socket"),
	ErrorEntry(WSAEDESTADDRREQ, "Destination address required"),
	ErrorEntry(WSAEMSGSIZE, "Message too long"),
	ErrorEntry(WSAEPROTOTYPE, "Protocol wrong type for socket"),
	ErrorEntry(WSAENOPROTOOPT, "Bad protocol option"),
	ErrorEntry(WSAEPROTONOSUPPORT, "Protocol not supported"),
	ErrorEntry(WSAESOCKTNOSUPPORT, "Socket type not supported"),
	ErrorEntry(WSAEOPNOTSUPP, "Operation not supported on socket"),
	ErrorEntry(WSAEPFNOSUPPORT, "Protocol family not supported"),
	ErrorEntry(WSAEAFNOSUPPORT, "Address family not supported"),
	ErrorEntry(WSAEADDRINUSE, "Address already in use"),
	ErrorEntry(WSAEADDRNOTAVAIL, "Can't assign requested address"),
	ErrorEntry(WSAENETDOWN, "Network is down"),
	ErrorEntry(WSAENETUNREACH, "Network is unreachable"),
	ErrorEntry(WSAENETRESET, "Net connection reset"),
	ErrorEntry(WSAECONNABORTED, "Software caused connection abort"),
	ErrorEntry(WSAECONNRESET, "Connection reset by peer"),
	ErrorEntry(WSAENOBUFS, "No buffer space available"),
	ErrorEntry(WSAEISCONN, "Socket is already connected"),
	ErrorEntry(WSAENOTCONN, "Socket is not connected"),
	ErrorEntry(WSAESHUTDOWN, "Can't send after socket shutdown"),
	ErrorEntry(WSAETOOMANYREFS, "Too many references, can't splice"),
	ErrorEntry(WSAETIMEDOUT, "Connection timed out"),
	ErrorEntry(WSAECONNREFUSED, "Connection refused"),
	ErrorEntry(WSAELOOP, "Too many levels of symbolic links"),
	ErrorEntry(WSAENAMETOOLONG, "File name too long"),
	ErrorEntry(WSAEHOSTDOWN, "Host is down"),
	ErrorEntry(WSAEHOSTUNREACH, "No route to host"),
	ErrorEntry(WSAENOTEMPTY, "Directory not empty"),
	ErrorEntry(WSAEPROCLIM, "Too many processes"),
	ErrorEntry(WSAEUSERS, "Too many users"),
	ErrorEntry(WSAEDQUOT, "Disc quota exceeded"),
	ErrorEntry(WSAESTALE, "Stale NFS file handle"),
	ErrorEntry(WSAEREMOTE, "Too many levels of remote in path"),
	ErrorEntry(WSASYSNOTREADY, "Network system is unavailable"),
	ErrorEntry(WSAVERNOTSUPPORTED, "Winsock version out of range"),
	ErrorEntry(WSANOTINITIALISED, "WSAStartup not yet called"),
	ErrorEntry(WSAEDISCON, "Graceful shutdown in progress"),
	ErrorEntry(WSAHOST_NOT_FOUND, "Host not found"),
	ErrorEntry(WSANO_DATA, "No host data of that type was found")
};
const int kNumMessages = sizeof(gaErrorList) / sizeof(ErrorEntry);


// A function similar in spirit to Unix's perror() that tacks a canned interpretation of the value of WSAGetLastError() onto the end of a passed string, separated by a ": ".
// Generally, you should implement smarter error handling than this, but for default cases and simple programs, this function is sufficient.

// This function returns a pointer to an internal static buffer, so you must copy the data from this function before you call it again. It follows that this function is also not thread-safe.
const char* WSAGetLastErrorMessage(const char* pcMessagePrefix, int nErrorID /* = 0 */)
{
	static char acErrorBuffer[256]; // Build basic error string
	ostrstream outs(acErrorBuffer, sizeof(acErrorBuffer));
	outs << pcMessagePrefix << ": ";

	// Tack appropriate canned message onto end of supplied message prefix. Note that we do a binary search here: gaErrorList must be sorted by the error constant's value.
	ErrorEntry* pEnd = gaErrorList + kNumMessages;
	ErrorEntry Target(nErrorID ? nErrorID : WSAGetLastError());
	ErrorEntry* it = lower_bound(gaErrorList, pEnd, Target);
	if ((it != pEnd) && (it->nID == Target.nID)) {
		outs << it->pcMessage;
	}
	else { // Didn't find error in list, so make up a generic one
		outs << "unknown error";
	}
	outs << " (" << Target.nID << ")";

	outs << ends; // Finish error message off and return it.
	acErrorBuffer[sizeof(acErrorBuffer)-1] = '\0';
	return acErrorBuffer;
}

// Gracefully shuts the connection sd down.  Returns true if we're successful, false otherwise.
bool ShutdownConnection(SOCKET sd) // sd = socket descriptor
{
	// Disallow any further data sends.  This will tell the other side that we want to go away now. If we skip this step, we don't shut the connection down nicely.
	if (shutdown(sd, SD_SEND) == SOCKET_ERROR) {
		return false;
	}
	// Receive any extra data still sitting on the socket.  After all data is received, this call will block until the remote host acknowledges the TCP control packet sent by the shutdown above.
	// Then we'll get a 0 back from recv, signalling that the remote host has closed its side of the connection.
	while (true) {
		int nNewBytes = recv(sd, Net.packet, DEFAULT_BUFLEN, 0); // changed <Client.network_data> from <acReadBuffer> and DEFAULT_BUFLEN from <kBufferSize = 1024;>
		if (nNewBytes == SOCKET_ERROR) {
			int sock_error = WSAGetLastError();
			if (sock_error == WSAEWOULDBLOCK) {  // no data available - error 10035 and normal for non-blocking sockets
				// printf("%s\n", WSAGetLastErrorMessage("WSAEWOULDBLOCK error :")); // for heaving debugging
				break;
			}
			// I think <WSAECONNRESET> could be a little smoother but, it may not matter - changed for smoother shutdown
			else if (sock_error == WSAECONNRESET) { // aborted client connection, results in closed socket - wait till  <sock_error == WSAECONNABORTED> happens to diagnose
				printf("\nLost connection to client.\n");
				break;
			}
			else { // unsure about below with changes above
				// this is okay if one or more users still logged in and wide shutdown is called - it means they were doing stuff and the server shutdown
				printf("%s\n", WSAGetLastErrorMessage("Send packet FAILED with error :"));
				return false;
			}


		}
		else if (nNewBytes != 0) {
			cerr << endl << "FYI, received " << nNewBytes << " unexpected bytes during shutdown." << endl;
		}
		else { // nNewBytes == 0 = nothing
			//cerr << endl << "Okay, we're done!" << endl; // for heaving debugging
			break;
		}
	}
	if (closesocket(sd) == SOCKET_ERROR) { // Close the socket.
		return false;
	}
	return true;
}
