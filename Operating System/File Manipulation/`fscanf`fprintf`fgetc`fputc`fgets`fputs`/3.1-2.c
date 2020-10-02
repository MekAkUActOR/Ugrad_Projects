#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>

main()
{
	int n;
	char c;
	char buf1[256], buf2[256];
	FILE *fp1, *fp2, *fp3, *fp4;
	fp1 = fopen("file1.txt", "r");
	fp2 = fopen("file2.txt", "w");
	fp3 = fopen("file3.txt", "w");
	fp4 = fopen("file4.txt", "w");
  	// `fscanf` 和 `fprintf`
  	while(fscanf(fp1, "%s", buf1)>0) fprintf(fp2, "%s", buf1);
	fclose(fp1);
  	// `fgetc` 和 `fputc`
  	fp1 = fopen("file1.txt", "r");
  	while((c=fgetc(fp1)) > 0) fputc(c, fp3);
	fclose(fp1);
  	// `fgets` 和 `fputs`
	fp1 = fopen("file1.txt", "r");
  	while(fgets(buf2, 1024, fp1)) fputs(buf2, fp4);
  	fclose(fp1);
}