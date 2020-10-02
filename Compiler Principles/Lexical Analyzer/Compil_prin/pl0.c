/************  PL0.c  *************/
//517021911065_刘浩文_词法分析

// pl0 compiler source code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pl0.h"

// print error message.
void error(n)
{
	int i;

	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	e_rr++;
} // error

void g_etch(void) //由于笔者的Xcode的C环境的curses.h文件中已有getch()函数的定义，因此这个project中的原getch()函数的命名都改为了g_etch()
{
	if (cc == ll)                       //若一行已结束，这里读入下一行代码
	{
        if (feof(infile))               //若文件结束。feof():文件结束，返回非0值，文件未结束，返回0值
		{
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
		ll = cc = 0;                    //若文件未结束，ll和cc（代码读入指针）重新计数
		printf("%05d  ", cx);           //输出的整型宽度为n位，右对齐，%05d即宽度为5位，位数小于5则前补零
		while (!feof(infile) && (ch = getc(infile))!='\n')
		{
			printf("%c", ch);
            line[++ll] = ch;            //读取该行下一个符号存入数组，计算出该行源码长度保存在变量 ll 中

		}                               // while 这里读入了源代码符号串中的一行字符串
		printf("\n");
		line[++ll] = ' ';               //最后ll等于该行代码长度
        cx++;
	}
    /////////////////////////////////////////////
	ch = line[++cc];                     //若一行还没有结束，这里读入该行下一个字符
} // getch

// gets a symbol from input stream.
void getsym(void)
{
	int i, k;
	char a[MAXIDLEN + 1];

	while (ch == ' ' || ch=='\t')
    {
        g_etch();
        if (cc >= ll) printf("\n");     //该行是增加用来使输出更整齐美观，防止行末空格干扰换行
    }

	if (isalpha(ch))//isalpha是一种函数：判断字符ch是否为英文字母，若为英文字母，返回非0（小写字母为2，大写字母为1）。若不是字母，返回0。这里判断首字母是否为英文字母。
	{ // symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			g_etch();//读入一个可能的标识符或保留字
		}
		while (isalpha(ch) || isdigit(ch));//直到读入一个空格或换行，此时a[k]=空格或换行
		a[k] = 0;//结束符
		strcpy(id, a);//将a复制到id中
		word[0] = id;//word[0]在头文件中是空的
		i = NRW;
		while (strcmp(id, word[i--]));//字符串比较直到返回0（相等），得到i
		if (++i)
			sym = wsym[i]; // symbol is a reserved word
		else
			sym = SYM_IDENTIFIER;   // symbol is an identifier
		
	}
	else if (isdigit(ch))//一个单词的开头为数字
	{ // symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			g_etch();
		}
		while (isdigit(ch));//得到十进制数字大小并存在num中
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	else if (ch == ':')//':'本身不属于合法符号
	{
		g_etch();
		if (ch == '=')
		{
            sym = SYM_BECOMES;
            C_all++;           // :=
            C_becomes++;
			
			g_etch();
		}
		else
		{
			sym = SYM_NULL;       // illegal?
		}
	}
	else if (ch == '>')
	{
		g_etch();
		if (ch == '=')
		{
			sym = SYM_GEQ;     // >=
			g_etch();
		}
		else
		{
            sym = SYM_GTR;
            C_all++;
            C_gtr++;// >
			
		}
	}
	else if (ch == '<')
	{
        g_etch();
        if (ch == '=')
        {
            sym = SYM_LEQ;
            g_etch();
        }
        else if(ch == '>')
        {
            sym = SYM_NEQ;
            C_all++;
            C_neq++;
            g_etch();
        }
        else
        {
            sym = SYM_LES;
            C_all++;
            C_les++;// <
        }
	}
	
	/* Skip  Notes*/
    else if (ch == '(')
    {
        g_etch();
        if (ch == '*')
        {
            addr = cx - 1;//记录注释起始位置，方便报错
            g_etch();
            char ch1 = ch;//ch1与ch保存最近经过的两个符号
            while((ch1 != '*') || (ch != ')') )//检查最近经过的两个符号是否组成注释结束符
            {
                ch1 = ch;
                g_etch();
                if(feof(infile))
                {
                    printf("\nPROGRAM INCOMPLETE\n");
                    printf("ERROR: Line %d Unterminated Notes.\n",addr);
                    exit(1);
                    //若直到程序结束都没有注释结束符，则报错，指出注释起始位置
                }
            }//这里跳过所有注释开始符之后的字符串，直到遇到注释结束符
            g_etch();
            getsym();//直接跳过注释，读取下一个符号
        }
        else sym = SYM_LPAREN;
    }
	
	
	else
	{ // other tokens
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i)
		{
			sym = ssym[i];
			if (sym != SYM_PERIOD)
			{
				g_etch();
			}
		}
		else
		{
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
} // getsym

int main ()
{
	char s[80];
	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}

	e_rr = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	while(ch != '.')
	{   
		getsym();  
	    /* Please output types of words in every line */
        printf("%d ",sym);      //直接将 sym 输出即可
        if (cc >= ll) printf("\n");
    }


	if (sym != SYM_PERIOD)
		error(9); // '.' expected.

    /* Please output the number of “:=”，“>”，“<”，“<>”  to file "test.out"*/
    outfile = fopen("/Users/hongxing/Desktop/大三上学期/编译原理/大作业/词法分析大作业/报告/test.out", "w");
    fprintf( outfile, "All ':=','>','<','<>' : %d\n':=' : %d\n'>' : %d\n'<' : %d\n'<>' : %d\n", C_all, C_becomes, C_gtr, C_les, C_neq);
    
    //printf("\nAll ':=','>','<','<>' : %d\n':=' : %d\n'>' : %d\n'<' : %d\n'<>' : %d\n", C_all, C_becomes, C_gtr, C_les, C_neq);
    
    printf("\nCompile Success！\n");
    exit(0);
} // main    END OF PL0.c
