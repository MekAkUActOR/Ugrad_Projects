//#include "pch.h"
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")// Used to link with Ws2_32.lib

#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
	WORD wVersion = MAKEWORD(2, 2); // Used to request version 2.2 of Windows sockets
    WSADATA wsaData;                // Data loaded by WSAStartup
    int iResult;                    // Error check if WSAStartup successful

    // Initialize Winsock
    iResult = WSAStartup(wVersion, &wsaData);
    if (iResult != 0) {
       cout << "WSAStartup failed: " << iResult << endl;
       return 1;
    }
    
    char host_name[128];
    gethostname(host_name, sizeof(host_name));
   	cout <<"local host:" <<host_name << endl;
   	
   	//To simplify determining buffer requirements for the host and serv parameters, the following values 
	//for maximum host name length and maximum service name are defined in the Ws2tcpip.h header 
	//file.
	//#define NI_MAXSERV    32
	//#define NI_MAXHOST  1025

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s IPv4 address\n", argv[0]);
		printf("to return hostname of a remote IP\n");
		printf("for example %s 127.0.0.1\n", argv[0]);
		return 1;
	}

	DWORD dwRetval;

	struct sockaddr_in saGHN;
	char hostname[NI_MAXHOST];
	char servInfo[NI_MAXSERV];
	u_short port = 27015;

//-----------------------------------------
	// Set up sockaddr_in structure which is passed
	// to the getnameinfo function
	saGHN.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &saGHN.sin_addr.s_addr);
	saGHN.sin_port = htons(port);

	//-----------------------------------------
	// Call getnameinfo
	dwRetval = getnameinfo((struct sockaddr *) &saGHN,
		sizeof(struct sockaddr),
		hostname,
		NI_MAXHOST, servInfo, NI_MAXSERV, 0);

	if (dwRetval != 0) {
		printf("getnameinfo failed with error # %ld\n", WSAGetLastError());
		return 1;
	}
	else {
		printf("getnameinfo returned hostname = %s\n", hostname);
		return 0;
	}



  	return 0;
}


