// 使用系统调用`read(fd, buf, nbytes)`, `write(fd, buf, nbytes)`，nbytes取1
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#define NUM 1

main()
{
	clock_t start,end;
	start=clock();
	int fd1, fd2;
	int n;
	char buf1[NUM];
	FILE *fp1, *fp2;
	fd1 = open("file1.txt", O_RDONLY);
	fd2 = open("file2.txt", O_WRONLY|O_CREAT, 0644);
	while((n=read(fd1,buf1,NUM)) > 0) write(fd2, buf1, n);
	close(fd1);
	close(fd2);
	end=clock();
	printf("%d\n",end-start);
}
