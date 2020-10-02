#include <winsock2.h>
#include <ws2tcpip.h>
#include<iostream>
#include<sstream>
#include<string>
#include <process.h>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

int      Count;

void sendmsg(void* client);
void receivemsg(void* client);

struct clientinfo
{
	SOCKADDR_IN addrClt;
	SOCKET sockServer;
};

u_short ss2n(string s)
{
	stringstream ss;
	u_short u;
	ss << s;
	ss >> u;
	return u;
}

void main(int argc, char** argv)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return;
	}

	if (argv[1] == NULL)
	{
		argv[1] = (char*)"10000";
		argv[2] = (char*)"127.0.0.1";
		argv[3] = (char*)"10001";
	}

	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN addrClt;
	inet_pton(AF_INET, argv[2], &addrClt.sin_addr);
	addrClt.sin_family = AF_INET;
	addrClt.sin_port = ss2n(argv[3]);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = ss2n(argv[1]);

	bind(sockServer, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	clientinfo clienti;
	clienti.addrClt = addrClt;
	clienti.sockServer = sockServer;

	int len = sizeof(SOCKADDR);
	Count = 2;
	if (_beginthread(sendmsg, 8192, (void*)&clienti) < 0)
	{
		printf("ERROR - Unable to create thread \n");
		exit(1);
	}

	if (_beginthread(receivemsg, 8192, (void*)&clienti) < 0)
	{
		printf("ERROR - Unable to create thread \n");
		exit(1);
	}

	while (Count);

	closesocket(sockServer);
	WSACleanup();
}

void sendmsg(void* client)
{
	char sendBuf[2048];
	int len = sizeof(SOCKADDR);
	SOCKADDR_IN addrClt;
	SOCKET sockServer;
	addrClt = ((clientinfo*)client)->addrClt;
	sockServer = ((clientinfo*)client)->sockServer;
	while (1)
	{
		cout << "please input the data: " << endl;
		gets_s(sendBuf);
		sendto(sockServer, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR*)&addrClt, len);
		if (strcmp(sendBuf, "quit") == 0) {
			cout << "Chat end!" << endl;
			break;
		}
	}
	Count--;
	_endthread();
}

void receivemsg(void* client)
{
	char recvBuf[2048];
	int len = sizeof(SOCKADDR);
	SOCKADDR_IN addrClt;
	SOCKET sockServer;
	addrClt = ((clientinfo*)client)->addrClt;
	sockServer = ((clientinfo*)client)->sockServer;

	while (1)
	{
		recvfrom(sockServer, recvBuf, strlen(recvBuf) + 1, 0, (SOCKADDR*)&addrClt, &len);
		char str[INET_ADDRSTRLEN];
		char* ptr = (char*)inet_ntop(AF_INET, &addrClt.sin_addr, str, sizeof(str));
		cout << ptr << " " << addrClt.sin_port << " say: " << recvBuf << endl;
		if (strcmp(recvBuf, "quit") == 0) {
			cout << "Chat end!" << endl;
			break;
		}
	}
	Count--;
	_endthread();
}


