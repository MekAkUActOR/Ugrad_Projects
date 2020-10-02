//#include "pch.h"
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")// Used to link with Ws2_32.lib

#include <iostream>
using namespace std;

int main(int argc, char *argv[] ) {
	WORD wVersion = MAKEWORD(2, 2); // Used to request version 2.2 of Windows sockets
    WSADATA wsaData;                // Data loaded by WSAStartup
    int iResult;                    // Error check if WSAStartup successful

    // Initialize Winsock
    iResult = WSAStartup(wVersion, &wsaData);
    if (iResult != 0) {
       cout << "WSAStartup failed: " << iResult << endl;
       return 1;
    }
    
    struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;
   	
   	//To simplify determining buffer requirements for the host and serv parameters, the following values 
//for maximum host name length and maximum service name are defined in the Ws2tcpip.h header 
//file.
//#define NI_MAXSERV    32
//#define NI_MAXHOST  1025

// Validate the parameters
	if (argc != 3) {
		printf("usage: %s <hostname> <servicename>\n", argv[0]);
		printf("       provides protocol-independent translation\n");
		printf("       from an ANSI host name to an IP address\n");
		printf("%s example usage\n", argv[0]);
		printf("   %s www.contoso.com 0\n", argv[0]);
		return 1;
	}

	DWORD dwRetval;

//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	printf("Calling getaddrinfo with following parameters:\n");
	printf("\tnodename = %s\n", argv[1]);
	printf("\tservname (or port) = %s\n\n", argv[2]);
//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the result variable will hold a linked list
	// of addrinfo structures containing response
	// information
	dwRetval = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (dwRetval != 0) {
		printf("getaddrinfo failed with error: %d\n", dwRetval);
		WSACleanup();
		return 1;
	}

	printf("getaddrinfo returned success\n");

	// Retrieve each address and print out the hex bytes
	struct sockaddr_in  *sockaddr_ipv4;
	struct sockaddr_in6 *sockaddr_ipv6;
	
	char ipstringbuffer[46];
	DWORD ipbufferlength = 46;

	int i = 1;

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		printf("getaddrinfo response %d\n", i++);
		printf("\tFlags: 0x%x\n", ptr->ai_flags);
		printf("\tFamily: ");
		switch (ptr->ai_family) {
		case AF_UNSPEC:
			printf("Unspecified\n");
			break;
		case AF_INET:
			printf("AF_INET (IPv4)\n");
			sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
			inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, sizeof(ipstringbuffer));
			printf("\tIPv4 address %s\n", ipstringbuffer);
			printf("\tport = %d \n ", ntohs(sockaddr_ipv4->sin_port));
			break;
		case AF_INET6:
			printf("AF_INET6 (IPv6)\n");
			// the InetNtop function is available on Windows Vista and later
			 sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
			 printf("\tIPv6 address %s\n",
			    inet_ntop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );
			printf("\tport = %d \n ", ntohs(sockaddr_ipv6->sin6_port));
			break;
		case AF_NETBIOS:
			printf("AF_NETBIOS (NetBIOS)\n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_family);
			break;
		}
		printf("\tSocket type: ");
		switch (ptr->ai_socktype) {
		case 0:
			printf("Unspecified\n");
			break;
		case SOCK_STREAM:
			printf("SOCK_STREAM (stream)\n");
			break;
		case SOCK_DGRAM:
			printf("SOCK_DGRAM (datagram) \n");
			break;
		case SOCK_RAW:
			printf("SOCK_RAW (raw) \n");
			break;
		case SOCK_RDM:
			printf("SOCK_RDM (reliable message datagram)\n");
			break;
		case SOCK_SEQPACKET:
			printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_socktype);
			break;
		}
		printf("\tProtocol: ");
		switch (ptr->ai_protocol) {
		case 0:
			printf("Unspecified\n");
			break;
		case IPPROTO_TCP:
			printf("IPPROTO_TCP (TCP)\n");
			break;
		case IPPROTO_UDP:
			printf("IPPROTO_UDP (UDP) \n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_protocol);
			break;
		}
		printf("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
		printf("\tCanonical name: %s\n", ptr->ai_canonname);
	}

	WSACleanup();

  	return 0;
}


