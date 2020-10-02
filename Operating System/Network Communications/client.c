/* client */
#include "sockcom.h"

main(argc,argv)
int argc;
char **argv;	// 键入服务器地址与端口
{
	int sockfd;
	struct sockaddr_in server;
	struct hostent *hp, *gethostbyname();
	char msg[1024];
	sockfd=socket(AF_INET, SOCK_STREAM, 0);	// 建立 TCP socket, TCP/IP – IPv4
	if (sockfd < 0)
	{
		printf("TCP SOCKET ERROR!\n");
		exit(-1);
	}
	FILE *fp1;
	fp1=fopen("file.txt","r");		// 打开要发送的文件
	if ((hp=gethostbyname(argv[1])) == NULL){	// 通过ip得到服务器地址
		fprintf(stderr,"%s:unknown host\n",argv[1]);
		exit(2);
	}
	server.sin_family=AF_INET;		// TCP/IP – IPv4
	bcopy((char *)hp->h_addr,(char *)&server.sin_addr.s_addr,hp->h_length);	// 服务器地址
	server.sin_port=htons(atoi(argv[2]));	// 服务器端口
	if (connect(sockfd,(struct sockaddr *)&server,sizeof(server)) < 0)	// 建立与server的连接
	{
		printf("connecting stream socket ERROR!\n");
		exit(-1);
	}
	while((fread(msg, 1, 1, fp1)) > 0){
		if(!strlen(msg)) break;
		printf(msg);
		if(send(sockfd,msg,strlen(msg),0) < 0)	// 发送数据
		{
			printf("sendint message ERROR!\n");
			exit(-1);
		}
		bzero(msg, sizeof(msg));
	}
	printf("EOF...disconnect\n");
	close(sockfd);
	exit(0);
}
