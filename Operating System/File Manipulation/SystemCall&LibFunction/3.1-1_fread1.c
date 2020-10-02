// 使用文件的库函数`fread(buf, size, nitems, fp)`, `fwrite(buf, size, nitems, fp)`，size取1
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#define NUM 1

main()
{
	clock_t start,end;
	start=clock();
	int n;
	char buf4[NUM];
	FILE *fp1, *fp2;
	fp1 = fopen("file1.txt", "r");
	fp2 = fopen("file2.txt", "w");
	while((n=fread(buf4,NUM,1,fp1)) > 0) fwrite(buf4, NUM, n, fp2);
	fclose(fp1);
	fclose(fp2);
	end=clock();
	printf("%d\n",end-start);
}
