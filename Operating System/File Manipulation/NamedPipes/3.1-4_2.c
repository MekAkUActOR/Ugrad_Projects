// 进程二
#include <fcntl.h>

main()
{
	int fd, fd2, n;
	char buf1[1];
	fd = open("fifo", O_RDONLY);
	fd2 = open("file2.txt", O_WRONLY|O_CREAT, 0644);
	while((n=read(fd,buf1,sizeof(buf1))) != -1) write(fd2, buf1, n);
	close(fd);
	close(fd2);
} 
