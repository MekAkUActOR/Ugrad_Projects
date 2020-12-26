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
#include <string.h>
#include <iconv.h> 


#define LISTENQ 10
#define MAXLINE 4096

#define ALLOWED_SERVER "electsys.sjtu.edu.cn"

char FORBIDDEN_SERVER[1][20] = {"portal.sjtu.edu.cn"};	// 202.120.2.1

char ALLOWED_CLIENTIP[20] = "192.168.33.11";			// virtual machine

char ALLOWED_SERVERIP[20] = "202.120.2.2";				// public.sjtu.edu.cn

char FORBIDDEN_USER[10][40] = {"yaolh","scott0518","","","","","","","",""};

char FORBIDDEN_FILENAME[10][40] = {"test.txt","","","","","","","","",""};

static void * Connectionthread(void*);
void Data_Trans(int, int, int);
int Connect_Serv(struct sockaddr_in);
/*
 * main function: initialize the basic functions like port and the
 * client and server socket. Also create the connection thread for the 
 * tcp connection.
 */
int main(int argc, char **argv)
{
	struct sockaddr_in cli_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	int connfd, sockfd;
	int port;
	pthread_t Clitid;
	char opt;
	while((opt = getopt(argc, argv, "p:")) != EOF) {	// get the working port of proxy firewall
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
	printf("listening on port %s\n", argv[2]);
	for(;;){
		connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &sin_size);
	if(connfd<0) {
	    printf("accept error.\n");
	    continue;
	} 

	if(checkclient(cli_addr.sin_addr.s_addr) == -1){
	    close(connfd);
	    continue;
	}
	printf("Received a request from %s:%u \n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
	pthread_create(&Clitid, NULL, Connectionthread, &connfd);	
    }
    return 0;
}

/*
 * check client via checking the client address.
 * cli_addr: the client address and the client socket we capture.
 * return whether the checkclient succeeds. 1 for success and -1 for
 * failure.
 */
int checkclient(in_addr_t cli_addr) {
	int allowedip;
	inet_aton(ALLOWED_CLIENTIP, (struct in_addr *)&allowedip);

	if (allowedip == cli_addr)	{
		printf("Client IP authentication is passed !\n");
		return 1;
	}
	else{
		printf("Client IP authentication failed !\n ");
		return -1;
	}
}

/*
 * the method for comparing strings for string ip address
 * str1 and str2 are the two strings waiting for comparison.
 * return the result of comparison. 1 for success and 0 for failure.
 */
int str_compare(char *str1, char *str2){
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	if(len1 != len2){
		return 0;
	} else{
		int i;
		for(i = 0; i < len1; i++)
		{
		    if(str1[i] != str2[i]){
		    	return 0;
		    }	
		}
	}
	return 1;
}

/*
 * check the validation of server.
 * serv_addr is the server ip address we captured during the connection.
 * return whether the checkserver succeeds. 1 for success and -1 for
 * failure.
 */
int checkserver(in_addr_t serv_addr) {

	struct hostent *f_hostinfo;
	int j;
	char **forbiddenip;
	for(j=0; j < sizeof(FORBIDDEN_SERVER)/sizeof(FORBIDDEN_SERVER[1]); j++){
		f_hostinfo = gethostbyname(FORBIDDEN_SERVER[j]);  
		forbiddenip = f_hostinfo->h_addr_list;
		char f_str[32];
		char *f_serv_ip;
		struct in_addr f_s;  
		f_s.s_addr = serv_addr;
		//change net ip address to char string with dot
		f_serv_ip = inet_ntoa(f_s);
		//printf("%s\n",f_serv_ip);
		for(; *forbiddenip!=NULL; forbiddenip++){
			//change binary to string with dot
			//const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
			inet_ntop(f_hostinfo->h_addrtype, *forbiddenip, f_str, sizeof(f_str));
			if(str_compare(f_str, f_serv_ip) == 1){
			    printf("Server IP authentication is forbidden !\n");
			        return -1;
			}
		}
	} 
/*
 * originally, it uses network binary to check the ip address,
 * but the server address we get from the getsockopt would be
 * changed after the first remote connection because of the 
 * redirection as well as the different port request.
 * So we redesignt the method to check the ip address via 
 * changing the binary server address to the daily use string 
 * ip address and rewrite the string comparison method to 
 * compare!
 */
	//printf("%s,%s",ALLOWED_SERVERIP,inet_ntoa(*((struct in_addr*)&serv_addr)));
	if (str_compare(ALLOWED_SERVERIP, inet_ntoa(*((struct in_addr*)&serv_addr))))	{
		printf("Server IP authentication is passed !\n");
		return 1;
	}
	else{
		printf("Server IP authentication failed !\n ");
		return -1;
	}
}

/*
 * create a thread for the connection. The thread would be built
 * everytime a request was sent from the client to the server.
 * arg is the point for the clifd which means the num of client socket
 * 
 */
void* Connectionthread(void* arg){
	pthread_detach(pthread_self());
	int clifd,servfd;
	clifd = *(int*)(arg);
	struct sockaddr_in servaddr;
	socklen_t servlen;
	
	int is_ftp = 0;
	int over_flow = 0;
	
	if(clifd <= 0)
		return NULL;
	/*
	 * before getting the server address, we should first bind the 
	 * the address to the targeted socket, otherwise the client 
	 * could not get the server address.
	 */
	int ret = getsockname(clifd, (struct sockaddr*)&servaddr, &servlen);
	
	if ((getsockopt(clifd, SOL_IP, SO_ORIGINAL_DST, &servaddr, &servlen)) != 0 ){
		//printf("%i\n",errno);
		close(clifd);
		printf("Could not get original destination.\n");
		return NULL;
	}
	
	
	if (checkserver(servaddr.sin_addr.s_addr) == -1){
		//close(clifd);
		printf("Checkserver failed.\n");
		return NULL;
	}

	if(ntohs(servaddr.sin_port) == 21){
		is_ftp = 1;
	}
	
	servfd = Connect_Serv(servaddr);
	if(servfd <= 0){
		printf("no");
		close(clifd);
		return NULL;
	}
	Data_Trans(clifd, servfd, is_ftp);
	if(ntohs(servaddr.sin_port) != 21 && ntohs(servaddr.sin_port) != 20){
	    close(servfd);
	    close(clifd);
	}
	return NULL;
}

/*
 * listening the tcp port to capture the packet at that port
 * port is the socket port we initialized at first.
 * return the socket number created.
 */
int tcp_listen(int port)
{
	struct sockaddr_in cl_addr, proxyserver_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	int sockfd, accept_sockfd, on = 1;

	/* Build address structure to bind to socket. */
	// zero proxyserver_addr
	memset(&proxyserver_addr, 0, sizeof(proxyserver_addr));							
	proxyserver_addr.sin_family = AF_INET;
	proxyserver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	proxyserver_addr.sin_port = htons(port);
	// create socket
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);			
	if (sockfd < 0) {
		printf("Socket failed...Abort...\n");
		return;
	} 
	/* Set socket to be reusable */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

	if (bind(sockfd, (struct sockaddr *) &proxyserver_addr, sizeof(proxyserver_addr)) < 0) {
		printf("Bind failed...Abort...\n");
		return;
	} 
	if (listen(sockfd, LISTENQ) < 0) {
		printf("Listen failed...Abort...\n");
		return;
	} 
	return sockfd;
}

/*
 * //connect server when all the checks have been passed.//
 * serveraddr is the server address we connected
 * return the successful socket
 */
int Connect_Serv(struct sockaddr_in serveraddr)
{
	int cnt_stat, remoteSocket;
	//struct sockaddr_in server_addr;                             
	//holds IP address
	remoteSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (remoteSocket < 0) {
		return printf("socket");
	}
	serveraddr.sin_family = AF_INET;
	/*
	 * int connect(int s, const struct sockaddr *name, int *namelen);
	 */
	cnt_stat = connect(remoteSocket, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
	if (cnt_stat < 0) {
		return printf("remote connect failed\n");
	}else{
	printf("connected remote server -------------------->%s:%u.\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
	}
	return remoteSocket;
}

/*
 * parse the cli_buff we need to integer from string
 * buf is the string we get from the request packet
 * return the related integer we defined
 */
int parse_buff(char *buf) {
	if(str_compare(buf, "USER"))
	{
		return 0;
	}
	if(str_compare(buf, "PASS"))
	{
		return 1;
	}
	if(str_compare(buf, "SIZE"))
	{
		return 2;
	}
	if(str_compare(buf, "RETR"))
	{
		return 3;
	}
	if(str_compare(buf, "PASV"))
	{
		return 4;
	}
	if(str_compare(buf, "LIST"))
	{
		return 5;
	}
	if(str_compare(buf, "CWD "))
	{
		return 6;
	}
	if(str_compare(buf, "SYST"))
	{
		return 7;
	}
	if(str_compare(buf, "FEAT"))
	{
		return 8;
	}
	if(str_compare(buf, "PWD"))
	{
		return 9;
	}
	if(str_compare(buf, "TYPE"))
	{
		return 10;
	}
	if(str_compare(buf, "PORT"))
	{
		return 11;
	}
	else 
	{
		return 1000;
	}
}

/*
 * convert the code into different version
 */
int code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen)  
{  
    iconv_t cd;  
    int rc;  
    char **pin = &inbuf;  
    char **pout = &outbuf;  

    cd = iconv_open(to_charset, from_charset);  
    if (cd == 0)  
            return -1;  
    memset(outbuf, 0, outlen);  
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1)  
            return -1;  
    iconv_close(cd);  
    return 0;  
}  
 
/*
* change from utf-8 to gb2312
*/
int u2g(char *inbuf, int inlen, char *outbuf, int outlen)  
{  
	return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);  
}  
 
/*
* change from gb2312 to utf-8
*/
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)  
{  
	return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);  
}  

/*
 * //parsing the ftp request sent from the client//
 * cli_buf is the content of the request
 * length is the length of cli_buf
 */
int parse_ftp_client(char* cli_buf, int length) {
	char buf[5];
	int sig = 1000;
	char user[length-5];
	char passwd[length-5];
	char file[length-5];
	char temp[MAXLINE];

	int i = 0;
	for(i = 0; i<4; i++){
		buf[i] = *(cli_buf+i);
	}
	//add eof to the end of string to easily pass the string and compare
	buf[4]='\0';
	printf("\033[1;33m[->]\033[mclient---%s---->server: ", buf);
	
	sig = parse_buff(buf);
	switch(sig) 
	{	
		case 0: 
		{
			int j = 0;
			for(j = 5; j<length; j++){
				user[j - 5] = *(cli_buf + j);
			}
			user[length-7] = '\0';
			printf("CLIENT'S USERNAME: %s\n", user);
			int i = 0;
			//printf("userlen: %ld\n", strlen(user));
			//printf("forbiduser: %s\n", FORBIDDEN_USER[0]);
			//printf("forbuserlen: %ld\n", strlen(FORBIDDEN_USER[0]));
			for(i = 0; i < 10; i++)
			{
				if(str_compare(user, FORBIDDEN_USER[i]))
				{
					printf("FORBIDDEN USER: %s\n", user);
					return 1;
				}
			}
			break;
		}
		case 1: 
		{
			int j = 0;
			for(j=5; j<length-1; j++){
				passwd[j-5] = *(cli_buf + j);
			}
			passwd[length-6] = '\0';
			printf("CLIENT'S PASSWD: %s\n", passwd);
			break;
		}
		case 2: 
		{
			int j = 0;
			for(j=5; j<length-1; j++){
				file[j-5] = *(cli_buf+j);
			}
			file[length-6] = '\0';
			g2u(file, length-5, temp, MAXLINE);
			printf("CLIENT'S SIZE: %lu\n", sizeof(temp)/sizeof(temp[0]));
			break;
		}
		case 3: 
		{
			int j = 0;
			for(j=5; j<length; j++){
				file[j-5] = *(cli_buf+j);
			}
			file[length-7] = '\0';
			g2u(file, length - 5, temp, MAXLINE);
			printf("DOWNLOAD FILE: %s\n", temp);
			int i = 0;
			//printf("filelen: %ld\n", strlen(temp));
			//printf("forbidfile: %s\n", FORBIDDEN_FILENAME[0]);
			//printf("forbfilelen: %ld\n", strlen(FORBIDDEN_FILENAME[0]));
			for(i = 0; i < 10; i++)
			{
				if(str_compare(temp, FORBIDDEN_FILENAME[i]))
				{
					printf("FORBIDDEN FILE: %s\n", temp);
					return 1;
				}
			}
			break;
		}
		case 4: 
		{
			printf("CLIENT'S PASSIVE MODE %s\n", "PASV");
			break;
		}
		case 5: 
		{
			printf("LIST DIRECTORY\n");
			break;
		}
		case 6: 
		{
			char dir[length-4];
			int j = 0;
			for(j=4; j<length; j++){
			    dir[j-4] = *(cli_buf+j);
			}
			dir[length-5] = '\0';
			//printf("CHANGE TO DIRETORY %s\n",dir);
			g2u(dir, length-5, temp, MAXLINE);
			printf("CHANGE TO DIRETORY %s\n", temp);
			break;
		}
		case 7: 
		{
			printf("BACK TO FTP OS!\n");
			break;
		}
		case 11:
		{
			printf("CLIENT'S ACTIVE MODE %s\n", "PORT");
			break;
		}
		default:
			printf("OTHERS!\n");
	}
	return 0;
}

/*
 * parsing the ftp response sent from the server
 * serv_buf is the content of the response from the server
 * length is the length of serv_buf
 */
void parse_ftp_server(char* serv_buf, int length) {
	char buf[4];
	int sig = 0;

	int i = 0;
	for(i=0; i<3; i++){
		buf[i] = *(serv_buf + i);
    }
    buf[3]='\0';
    printf("\033[0;36m[<-]\033[mserver---%s--->client: ", buf);
    switch(atoi(buf)) 
    {
    	case 220:
  		{
  			printf("server is ready!\n");
  			break;
  		}
  		case 150:
  		{
  			printf("successfully built the connection! data channel opening!\n");
  			break;
  		}
  		case 200:
  		{
  			printf("succeed!\n");
  			break;
  		}
  		case 230:
  		{
  			printf("successfull log in!\n");
  			break;
  		}
  		case 331:
  		{
  			printf("need passwd!\n");
  			break;
  		}
  		case 125:
  		{
  			printf("start to transfer!\n");
  			break;
  		}
  		case 500:
  		{
  			printf("useless command!\n");
  			break;
  		}
  		case 530:
  		{
  			printf("not logged in!\n");
  			break;
  		}
  		case 226:
  		{
  			printf("end data link!\n");
  			break;
  		}
  		case 250:
  		{
  			printf("complete file behavior!\n");
  			break;
  		}
  		case 211:
  		{
  			printf("system condition recovered!\n");
  			break;
  		}
  		case 550:
  		{
  			printf("unusable file!\n");
  			break;
  		}
  		case 227:
  		{
  			printf("be in passive mode!\n");
  			break;
  		}
  		default:
  			printf("others\n");
    }
}

/*
 * transform the data after the socket has been successfully connected.
 * clifd is the client socket and servfd is the server socket
 */
void Data_Trans(int clifd, int servfd, int is_ftp)
{
	int maxfdp, length;
	fd_set rset;
	char cli_buf[MAXLINE];
	char serv_buf[MAXLINE];

	FD_ZERO(&rset);

	maxfdp = (clifd >= servfd?clifd:servfd ) + 1;
	for(;;)
	{
		FD_SET(clifd, &rset);
		FD_SET(servfd, &rset);
		if(select(maxfdp, &rset, NULL, NULL, NULL) < 0){
			printf("select error.");
			break;
		}

		if(FD_ISSET(clifd, &rset))
		{
			length = read(clifd, cli_buf, MAXLINE);
			if(length <= 0) return;
			else
			{
				if(is_ftp) 
				{
					if(parse_ftp_client(cli_buf, length))
						continue;
				}
				if(send(servfd, cli_buf, length, 0) <= 0)
				{
					return;
				}
			}
		}
		if(FD_ISSET(servfd, &rset))
		{
			length = read(servfd, serv_buf, MAXLINE);
			if(length <= 0) return;
			else 
				if(is_ftp)
				{
					parse_ftp_server(serv_buf, length);
				}
				if(send(clifd, serv_buf, length, 0) <= 0)
				{
					return;
				}
		}
	}
}
