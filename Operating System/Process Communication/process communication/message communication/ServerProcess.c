// File: experiment2nd_part2

/* 
Part2 :分别利用UNIX的消息通信机制、共享内存机制(用信号灯实施进程
间的同步和互斥)实现两个进程间的数据通信。具体的通信数据可从一个文件
读出，接收方进程可将收到的数据写入一个新文件，以便能判断数据传送的
正确性。
*/

/* Server 进程 */
#include "msgcom.h"
void main()
{
	struct msgtype buf;		// 创建消息缓冲区
	int qid;
	if((qid=msgget(MSGKEY, IPC_CREAT|0666)) == -1)	// 获得消息队列标识符
		return(-1);
	while(1){
		msgrcv(qid, &buf, 512, 1, MSG_NOERROR);		// 接收消息队列消息存入消息缓冲区
		printf("Server receive a request from process %d\n", buf.text);	// 打印消息内容(对方进程的进程号)
		buf.mtype = buf.text;		// 处理消息，将消息类型赋值为消息内容
		msgsnd(qid, &buf, sizeof(int), 0);		// 将消息缓冲区的消息发送给消息队列
	}
}
