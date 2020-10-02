// File: experiment2nd_part2

/* 
Part2 :分别利用UNIX的消息通信机制、共享内存机制(用信号灯实施进程
间的同步和互斥)实现两个进程间的数据通信。具体的通信数据可从一个文件
读出，接收方进程可将收到的数据写入一个新文件，以便能判断数据传送的
正确性。
*/

 /* 信号灯集+共享内存*/
 /* 
 要求：父子进程通过SystemV信号灯集同步对共享内存的读写 
  1.父进程从键盘输入字符串到共享内存
  2.子进程删除字符串中的空格并打印
  3.父进程输入quit后删除共享内存和信号灯集，程序结束
 思路：
实现内存（缓冲区）读写同步机制+互斥机制，需要2个信号灯，一个为读，一个为写。
 1.创建共享内存                                                              
 2.创建信号灯
 3.fork进程
 4.父进程阻塞等待键盘输入
 4..阻塞等待P写资源
 4...写入共享内存
 4....判断是否结束，如果是则kill子进程
 4.....如果否，则V读资源
 5.子进程阻塞等待P读资源
 5..读取共享内存
 5...V写资源
 5....打印输出
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <wait.h>
#include <string.h>
#define SEM_WRITE		0		// 写信号灯编号
#define	SEM_READ		1		// 读信号灯编号
#define SIZE			1024	// 共享内存容量
#define	P_OPERATION		-1		// P操作
#define V_OPERATION		1		// V操作
typedef	union semunion{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct seminfo *__buf;
	}semunion;

/*
func:对num个信号灯赋值
para:
	semid 信号灯集id
	sema_value[] 信号灯值缓冲区
	num 信号灯个数

*/
void init_sema_value(int semid,int *sema_value,int num){

	semunion semu;
	for(int i = 0; i < num; i++){
		semu.val = sema_value[i];
		semctl(semid, i, SETVAL, semu);
	}
}
/*
func:P/V操作封装
para:
*/
void operate(int semid, int whichnum, int op){
	struct sembuf opbuf;
	opbuf.sem_num = whichnum;
	opbuf.sem_op = op;
	opbuf.sem_flg = 0;
	semop(semid, &opbuf, 1);
}

int main(void)
{
	int shmid, semid, sema_value[2];
	pid_t pid;
	char *addr = NULL;
	key_t Key;
	Key = ftok(".", 'm');
	//创建共享内存
	if((shmid=shmget(Key, SIZE, IPC_CREAT|0666)) < 0){
		perror("shmget");
		exit(-1);
	}
	//创建信号灯集
	if((semid=semget(Key, 2, IPC_CREAT|0666|IPC_EXCL)) < 0){
		perror("semget");
		goto error1;
	}
	//信号灯初始化
	sema_value[SEM_WRITE] = 1;
	sema_value[SEM_READ] = 0;
	init_sema_value(semid, sema_value, 2);
	//映射共享内存
	if((addr=(char *)shmat(shmid, NULL, 0)) < 0){
		perror("shmat");
		goto error2;
	}
	if((pid=fork()) == 0){
		char *new, *old;
		while(1)		//子进程循环
		{
			operate(semid, SEM_READ, P_OPERATION);	//对读信号灯的P操作
			new = old = addr;
			while(*old){
				if((*old) != ' '){
					(*new++) = (*old);
				}
				old++;
			}
			(*new)='\0';
			printf("Kid process %d is receiving : %s\n", getpid(), addr);
			operate(semid, SEM_WRITE, V_OPERATION);	//对写信号灯的V操作
		}
	}
	else
	{
		while(1)		//父进程
		{
			operate(semid, SEM_WRITE, P_OPERATION);
			printf("Parent process %d is sending :\n", getpid());
			fgets(addr, SIZE, stdin);
			operate(semid, SEM_READ, V_OPERATION);
			if(strcmp(addr ,"quit\n")==0)
			{
				kill(pid, SIGUSR1);
				break;
			}
		}
	}

error2:
	semctl(semid, 0, IPC_RMID);		//删除信号灯集
	shmdt(addr);					//撤销共享内存映射
error1:
	shmctl(shmid, IPC_RMID, NULL);	//删除共享内存

	return 0;
}


