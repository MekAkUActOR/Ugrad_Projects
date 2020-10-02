// 进程一
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

main()
{
	int fd, fd1, n;
	char buf1[1];
	fd1 = open("file1.txt", O_RDONLY);
	mknod("fifo", S_IFIFO|0666,0);
	fd = open("fifo", O_WRONLY);
	while(n=read(fd1,buf1,1) > 0) write(fd, buf1, n);
	close(fd);
	close(fd1);
}
