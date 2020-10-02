
//#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#define WIN32_LEAN_AND_MEAN

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "80"

int __cdecl main(int argc, char** argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
 
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ClientSocket = INVALID_SOCKET;
    // Attempt to connect to the first address returned by
   // the call to getaddrinfo
    ptr = result;

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ClientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ClientSocket == INVALID_SOCKET) {
            cout << "Error at socket(): " << WSAGetLastError() << endl;
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }


        // Connect to server.
        iResult = connect(ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ClientSocket);
            ClientSocket = INVALID_SOCKET;
        }

        break;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ClientSocket == INVALID_SOCKET) {
        cout << "Unable to connect to server!\n";
        WSACleanup();
        return 1;
    }


    const char* sendbuf = "GET / HTTP/1.0\n\n";

    // Send an initial buffer
    iResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        cout << "send failed: " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Bytes Sent: " << iResult << endl;


    // shutdown the connection for sending since no more data will be sent
   // the client can still use the ClientSocket for receiving data
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown failed: " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }


    // Receive data until the server closes the connection
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            cout << "Bytes received: " << iResult << endl;
            cout << recvbuf << endl;
        }
        else if (iResult == 0)
            cout << "Connection closed\n";
        else
            cout << "recv failed: " << WSAGetLastError() << endl;
    } while (iResult > 0);


    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}