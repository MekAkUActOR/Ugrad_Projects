#include <sys/stat.h>
#include <sys/epoll.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <linux/netfilter_ipv4.h>
#include <stdarg.h>

#define LISTENQ 10
#define MAXLINE 4096

#define ALLOWED_SERVER "sjtu.edu.cn"

char ALLOWED_CLIENTIP[20] =  "192.168.33.11";

static void * Connectionthread(void*);
void Data_Trans(int, int);
int Connect_Serv(struct sockaddr_in);
int tcp_listen(int);
int checkserver(in_addr_t);
int checkclient(in_addr_t);


int main(int argc, char **argv)
{
	struct sockaddr_in cli_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
    int connfd, sockfd;
    int port;
	pthread_t Clitid;
	char opt;

	if (argc != 3){
        printf("Usage: %s -p port\n", argv[0]);
        return -1;
    }
	while( (opt = getopt(argc, argv, "p:")) != EOF ) {
			switch(opt) {
				case 'p':
					port = (short) atoi(optarg);
					break;
				default:
					printf("Usage: %s -p port\n", argv[0]);
					return -1;
			}
	 }

    sockfd = tcp_listen(port);
    printf("listening on %s\n", argv[1]);
	for(;;){
				connfd=accept(sockfd,(struct sockaddr *)&cli_addr, &sin_size);
				if(connfd<0) {
					printf("accept error.\n");
				    continue;
				}
				if (checkclient(cli_addr.sin_addr.s_addr) == 1)
					pthread_create(&Clitid, NULL, Connectionthread, (void*)connfd);  // create child threads
				else
					close(connfd);
    	}
	return 0;
}



void* Connectionthread(void* arg){
	int clifd,servfd;
	clifd = (int)(arg);
	struct sockaddr_in servaddr;
	socklen_t servlen = sizeof(struct sockaddr_in);

	pthread_detach(pthread_self());

    int ret = getsockname(clifd, (struct sockaddr*)&servaddr, &servlen);
    //printf("LISTEN INFO @ %s : %u \n",inet_ntoa(servaddr.sin_addr),ntohs(servaddr.sin_port));


//////////////////////////////////////////////////////////////////////////////////////////
	if ( (getsockopt(clifd, SOL_IP, SO_ORIGINAL_DST, &servaddr, &servlen)) != 0 ){	// get the former IP and port (Server)
		close(clifd);
		printf("Could not get original destination.");
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	if (checkserver(servaddr.sin_addr.s_addr) == -1){
		close(clifd);
		printf("Could not get original destination.");
		return NULL;
	}

	//if (  )


    servfd = Connect_Serv(servaddr);
  	Data_Trans(clifd, servfd);
    close(servfd);
    close(clifd);
    return NULL;
}


int tcp_listen(int port)
{
	struct sockaddr_in cl_addr, proxyserver_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	int sockfd, accept_sockfd, on = 1;

	/* Build address structure to bind to socket. */
	memset(&proxyserver_addr, 0, sizeof(proxyserver_addr));							// zero proxyserver_addr
	proxyserver_addr.sin_family = AF_INET;
	proxyserver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	proxyserver_addr.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);			// create socket
	if ( sockfd < 0 ) {
		printf("Socket failed...Abort...\n");
		return -1;
	}
	/* Set socket to be reusable */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

	if ( bind(sockfd, (struct sockaddr *) &proxyserver_addr, sizeof(proxyserver_addr)) < 0 ) {
		printf("Bind failed...Abort...\n");
		return -1;
	}
	if ( listen(sockfd, LISTENQ) < 0 ) {
		printf("Listen failed...Abort...\n");
		return -1;
	}
	return sockfd;
}

int checkserver(in_addr_t serv_addr) {
	struct hostent *hostinfo;
	int allowedip;
	hostinfo = gethostbyname(ALLOWED_SERVER);
	allowedip = (int )hostinfo->h_addr;

	if ( allowedip == serv_addr )	{
		printf("Server IP authentication is passed !\n");
		return 1;
	}
	else{
		printf("Server IP authentication failed !\n ");
		return -1;
	}
}


int checkclient(in_addr_t cli_addr) {
	int allowedip;
	inet_aton(ALLOWED_CLIENTIP,(struct in_addr *)&allowedip);

	if ( allowedip == cli_addr )	{
		printf("Client IP authentication is passed !\n");
		return 1;
	}
	else{
		printf("Client IP authentication failed !\n ");
		return -1;
	}
}


int Connect_Serv(struct sockaddr_in servaddr)
{
	int cnt_stat, remoteSocket;
	//struct sockaddr_in server_addr; 		// holds IP address

	remoteSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( remoteSocket < 0 ) {
		return printf("socket");
	}
	servaddr.sin_family= AF_INET;
	cnt_stat = connect(remoteSocket, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if ( cnt_stat < 0 ) {
		return printf("remote connect failed");
	}
	return remoteSocket;
}

void Data_Trans(int clifd, int servfd)
{
	int maxfdp, length;
	fd_set rset;
	char cli_buf[MAXLINE];
	char serv_buf[MAXLINE];

	FD_ZERO(&rset);
	FD_SET( clifd,&rset );
	FD_SET( servfd,&rset );

	maxfdp=(clifd >= servfd?clifd:servfd) + 1;
	for(;;)
	{
		if( select(maxfdp, &rset, NULL, NULL, NULL) < 0 ){
			printf("select error.");
			continue;
		}

		if( FD_ISSET(clifd, &rset) )
		{
			length = read(clifd, cli_buf, MAXLINE);
			if( length <= 0 ) return;
			else
				if( send(servfd, cli_buf, length, 0) <= 0 ) return;
		}
		if( FD_ISSET(servfd, &rset) ){
			length = read(servfd, serv_buf, MAXLINE);
			if( length <= 0 ) return;
			else
				if( send(clifd, serv_buf, length, 0) <= 0 ) return;
		}
	}
}
