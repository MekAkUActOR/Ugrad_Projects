// File: experiment2nd_part1

/* 
Part1 :自己设计一个程序，该程序创建一个子进程，使父子进程合作，
协调地完成某一功 能。要求在该程序中还要使用进程的睡眠、进程图象
改换、父进程等待子进程终止、信号的设置与传送(包括信号处理程序)、
子进程的终止等有关进程的系统调用。
*/

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

int main(){
    int status;
    pid_t pid;
    void func();
    signal(SIGUSR1, func);              // 设置信号SIGUSR1的中断处理函数为func
    while((pid = fork()) == -1);    // 产生子进程
    if(pid){                                            // 父进程
        printf("It is the parent process.\n");
        printf("Parent: will send signal.\n");
        sleep(1);                               // 向子进程传送信号前先睡眠一秒，保证信号到达时子进程正在低优睡眠中
        kill(pid, SIGUSR1);         // 父进程向子进程传送SIGUSR1软中断信号
        pid = wait(&status);        // 父进程等待子进程终止
        printf("Child process %d, status = %d.\n", pid, status);
    }
    else{                                                   // 子进程
        sleep(10);
        printf("It is the child process.\n");
        printf("Child: signal is received.\n");
        execl("/bin/ls", "ls", "-l", (char*)0); // 进程图像改换，将子进程图像改换为ls命令
        printf("execl error\n");                                // 图像改换失败
        exit(2);
    }
    printf("Parent process finish.\n");
}

void func(){        // 自己设置的信号SIGUSR1的中断处理函数
    printf("It is a signal processing function.\n");
    system("date");
}

