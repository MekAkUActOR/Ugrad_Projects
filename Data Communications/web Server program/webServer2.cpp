//#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>          // For binary handle options
#include <sys\stat.h>       // For binary write()
#include <io.h>             // Needed for open(), close(), write()
#include <process.h>        // Needed for _beginthread() and _endthread()


#undef UNICODE

#define WIN32_LEAN_AND_MEAN

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
//----- HTTP response messages ----------------------------------------------
#define OK_IMAGE  "HTTP/1.0 200 OK\r\nContent-Type:image/gif\r\n\r\n"
#define OK_TEXT   "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n" //目前只有两个文件种类，想支持更多的话可以另加
#define NOTOK_404 "HTTP/1.0 404 Not Found\r\nContent-Type:text/html\r\n\r\n"
#define MESS_404  "<html><body><h1>FILE NOT FOUND</h1></body></html>"


//----- Defines -------------------------------------------------------------
#define  BUF_SIZE            1024     // Buffer size (big enough for a GET)
#define  PORT_NUM              80     // Port number for a Web server


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "80"

int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    char outbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    //----- Function prototypes -------------------------------------------------
    void handle_get(void* in_arg);        // Thread function to handle GET

    //----Add these variables in main()----
    struct sockaddr client_addr;
    int addr_len;
    SOCKET client_s = INVALID_SOCKET;


    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    // Main loop to listen, accept, and then spin-off a thread to handle the GET
    while (1)
    {
        printf("main loop: linstening ... \n");
        // Listen for connections and then accept
        listen(ListenSocket, 50);
        addr_len = sizeof(client_addr);
        client_s = accept(ListenSocket, (struct sockaddr*) & client_addr, &addr_len);
        if (client_s == -1)
        {
            printf("ERROR - Unable to create a socket \n");
            exit(1);
        }
        printf("client socket accepted, %d... \n", client_s);
        // Spin-off a thread to handle this request (pass only client_s)
        if (_beginthread(handle_get, 4096, (void*)client_s) < 0)
        {
            printf("ERROR - Unable to create a thread to handle the GET \n");
            exit(1);
        }
    }

    printf("main loop completed. close server socket... WSAcleanup \n");
    // Close the server socket and clean-up winsock
    closesocket(ListenSocket);
    WSACleanup();
}

    //===========================================================================
    //=  This is the thread function to handle the GET                       =
    //===========================================================================
    void handle_get(void* in_arg)
    {
        unsigned int   client_s;             // Client socket descriptor
        char           in_buf[BUF_SIZE];     // Input buffer for GET request
        char           out_buf[BUF_SIZE];    // Output buffer for HTML response
        int            fh;                   // File handle
        int            buf_len;              // Buffer length for file reads
        char           command[BUF_SIZE];    // Command buffer
        char           file_name[BUF_SIZE];  // File name buffer
        int            retcode;              // Return code
        int			 j;

        // Set client_s to in_arg
        client_s = (unsigned int)in_arg;

        printf("thread %d... \n", client_s);
        // Receive the (presumed) GET request from the Web browser
        retcode = recv(client_s, in_buf, BUF_SIZE, 0);
        printf("thread %d...received web request: \n", client_s);
        for (j = 0; j < retcode; j++)
            printf("%c", in_buf[j]);

        // If the recv() return code is bad then bail-out (see note #3)
        if (retcode <= 0)
        {
            printf("ERROR - Receive failed --- probably due to dropped connection \n");
            closesocket(client_s);
            _endthread();
        }

        // Parse out the command from the (presumed) GET request and filename
        sscanf_s(in_buf, "%s %s \n", &command, BUF_SIZE, &file_name, BUF_SIZE);

        // Check if command really is a GET, if not then bail-out
        if (strcmp(command, "GET") != 0)
        {
            printf("ERROR - Not a GET --- received command = '%s' \n", command);
            closesocket(client_s);
            _endthread();
        }

        // It must be a GET... open the requested file
        //  - Start at 2nd char to get rid of leading "\"
        _sopen_s(&fh, &file_name[1], _O_RDONLY | _O_BINARY,
            _SH_DENYNO, _S_IREAD | _S_IWRITE);

        // If file does not exist, then return a 404 and bail-out
        if (fh == -1)
        {
            printf("File '%s' not found --- sending an HTTP 404 \n", &file_name[1]);
            strcpy_s(out_buf, NOTOK_404);
            send(client_s, out_buf, strlen(out_buf), 0);
            strcpy_s(out_buf, MESS_404);
            send(client_s, out_buf, strlen(out_buf), 0);
            closesocket(client_s);
            _endthread();
        }

        // Check that filename does not start with a "..", "/", "\", or have a ":" in
        // the second position indicating a disk identifier (e.g., "c:").
        //  - This is a security check to prevent grabbing any file on the server
        if (((file_name[1] == '.') && (file_name[2] == '.')) ||
            (file_name[1] == '/') || (file_name[1] == '\\') ||
            (file_name[2] == ':'))
        {
            printf("SECURITY VIOLATION --- trying to read '%s' \n", &file_name[1]);
            _close(fh);
            closesocket(client_s);
            _endthread();
        }

        // Generate and send the response
        printf("Thread %d, ...Sending file '%s' \n", in_arg, &file_name[1]);
        //search .gif in file_name
        if (strstr(file_name, ".gif") != NULL)
            strcpy_s(out_buf, OK_IMAGE);
        else
            strcpy_s(out_buf, OK_TEXT);
        send(client_s, out_buf, strlen(out_buf), 0);
        while (!_eof(fh))
        {
            buf_len = _read(fh, out_buf, BUF_SIZE);
            send(client_s, out_buf, buf_len, 0);
        }

        // Close the file, close the client socket, and end the thread
        _close(fh);
        printf("Thread %d, ...comleted sending file '%s' \n", client_s, &file_name[1]);
        closesocket(client_s);
        printf("socket %d closed. \n", client_s);
        printf("thread %d ended. \n", client_s);
        _endthread();
    }

