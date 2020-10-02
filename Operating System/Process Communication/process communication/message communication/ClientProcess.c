// File: experiment2nd_part2

/* 
Part2 :分别利用UNIX的消息通信机制、共享内存机制(用信号灯实施进程
间的同步和互斥)实现两个进程间的数据通信。具体的通信数据可从一个文件
读出，接收方进程可将收到的数据写入一个新文件，以便能判断数据传送的
正确性。
*/

/* Client 进程 */
#include "msgcom.h" 
void main()
{
	struct msgtype buf;		// 创建消息缓冲区
	int qid, pid;
	qid = msgget(MSGKEY, IPC_CREAT|0666);		// 获得消息队列标识符
	buf.mtype = 1;		// 赋消息类型为1
	buf.text = pid = getpid();	// 赋消息内容为本进程进程号
	msgsnd(qid, &buf, sizeof(buf.text), 0);	// 将消息缓冲区的消息发送给消息队列
	msgrcv(qid, &buf, 512, pid, MSG_NOERROR);	// 接收消息队列消息存入消息缓冲区
	printf("Request received a massags from server, type is:%d\n ", buf.mtype);	// 打印消息内容(本进程进程号)
}
