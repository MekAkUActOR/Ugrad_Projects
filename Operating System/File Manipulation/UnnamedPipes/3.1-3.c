#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include<string.h>

main()
{
	int fd1,fd2;
	char buf1[1];
	int chanel[2],n;
	char buf[100];
	fd1 = open("file1.txt", O_RDONLY);
	fd2 = open("file2.txt", O_WRONLY|O_CREAT, 0644);
	pipe(chanel);
	if(fork())
	{
		close(chanel[0]);
		while((n=read(fd1,buf1,1)) > 0) write(chanel[1], buf1, n);
		printf("Parent sends over.\n");
		close(chanel[1]);
	}
	else
	{
		close(chanel[1]);
		while((n=read(chanel[0],buf,sizeof(buf))) > 0) write(fd2, buf, n);
		printf("Child receives over.\n");
		close(chanel[0]);
	}
	close(fd1);
	close(fd2);
}
