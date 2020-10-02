/* server */
#include "sockcom.h"

main()
{
	int sockfd, newsockfd, length, count;
	char c;
	char buf[1024];
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	// 建立 TCP socket, TCP/IP – IPv4
	if (sockfd < 0)
	{
		printf("TCP SOCKET ERROR!\n");
		exit(-1);
	}
	FILE *fp2;
	fp2=fopen("file2.txt","w");		// 打开要将收到数据写入的文件
	struct sockaddr_in server;
	server.sin_family=AF_INET;		// TCP/IP – IPv4
	server.sin_addr.s_addr = INADDR_ANY;	// 接收任意ip (0.0.0.0)
	server.sin_port=0;		// 系统选择一个已释放的端口号
	if (bind(sockfd,(struct sockaddr *)&server,sizeof(server)) < 0)	// 绑定sockfd与套接字地址server
	{
		printf("bind stream socket ERROR!\n");
		exit(-1);
	}
	length=sizeof(server);
	if (getsockname(sockfd,(struct sockaddr *)&server,&length) < 0)	// 获取套接字sockfd的名字
	{
		printf("getting socket name ERROR!\n");
		exit(-1);
	}
	printf ("socket port #%d\n", ntohs(server.sin_port));
	listen(sockfd,5);	// 创建一个套接口并监听申请的连接, 等待连接队列的最大长度为5
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *)0, (int *)0);	// 从等待连接队列中抽取第一个连接，创建一个新的套接口并返回句柄
		if (!fork()){
			close(sockfd);
			bzero(buf,sizeof(buf));
			while ((count=recv(newsockfd,buf,sizeof(buf),0)) > 0){	// 接收数据
				fwrite(buf, 1, 1, fp2);
			}
			if(count < 0)
			{
				printf("Reading stream message ERROR!\n");
				exit(-1);
			}
			printf("\n one client closed.\n");
			exit(0);
		}
		close(newsockfd);
	}
}
