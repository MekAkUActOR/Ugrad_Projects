// File: experiment2nd_part2

/* 
Part2 :分别利用UNIX的消息通信机制、共享内存机制(用信号灯实施进程
间的同步和互斥)实现两个进程间的数据通信。具体的通信数据可从一个文件
读出，接收方进程可将收到的数据写入一个新文件，以便能判断数据传送的
正确性。
*/

/* msgcom.h */
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MSGKEY 5678

struct msgtype{
    long mtype;		// 消息类型
    int text;		// 消息正文
};

