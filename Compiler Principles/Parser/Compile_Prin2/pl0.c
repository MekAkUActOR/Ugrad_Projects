/************  PL0.c  *************/
//517021911065_刘浩文_语法分析

// pl0 compiler source code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "set.h"
#include "pl0.h"


symset uniteset(symset s1, symset s2)       //引入两个结点指针，返回一个结点指针
{
    symset s;
    snode* p;
    
    s = p = (snode*) malloc(sizeof(snode)); //动态分配一个snode类型大小的内存空间，并把分配空间的首地址强制转换成snode*类型的。即初始化结点
    //s为链表p的静态头部，p为链表p的末尾动态指针。
    
    s1 = s1->next;                          //将s1指向的结构体实例中的自数据next赋值给s1，即s1=s1.next，s1指向s1的下一个结点
    s2 = s2->next;                          //s2指向s2的下一个结点

    
    while (s1 && s2)                        //如果s1和s2均不为空
    {
        p->next = (snode*) malloc(sizeof(snode));                           //为p内部的指针next指向的下一个结点分配空间
        p = p->next;                                    //p指向p的下一个结点
        if (s1->elem < s2->elem)                        //若s1的数据小于s2的数据
        {
            p->elem = s1->elem;                         //让p的数据等于s1的数据
            s1 = s1->next;                              //s1指向s1的下一个结点
        }
        else
        {
            p->elem = s2->elem;
            s2 = s2->next;
        }                                              //该段使p的数据等于s1与s2中较小的那一个的数据，并将较小的那一项指向下一个结点
    }

    while (s1)//如果s1不为空，即s2已遍历完，将s1剩下的那段链表并到p之后
    {
        p->next = (snode*) malloc(sizeof(snode));
        p = p->next;
        p->elem = s1->elem;
        s1 = s1->next;
        
    }

    while (s2)
    {
        p->next = (snode*) malloc(sizeof(snode));
        p = p->next;
        p->elem = s2->elem;
        s2 = s2->next;
    }
    //以上三个while建立了一个链表p，该链表将链表s1与s2的所有结点从小到大排列并保存，即将链表s1与s2合并

    p->next = NULL;//链表p结束

    return s;//返回链表p的开始指针（头部）
} // uniteset


void setinsert(symset s, int elem)//该函数将一个新结点（数据为elem）按数据大小插入链表s中对应位置
{
    snode* p;
    snode* ss = s;
    p = (snode*) malloc(sizeof(snode));
    p->elem = elem;                 //这里新建一个结点，数据为elem
    s = s->next;                    //s指向s的下一个结点
    while((s != NULL) && (s->elem <= elem))
    {
        ss = s;
        s = s->next;
    }//为新结点找到合适的位置
    p->next = s;
    ss->next = p;
} // setinsert

symset createset(int elem, .../* SYM_NULL */)//此函数根据读入的可变数量的结点创建一个单链表
{
    va_list list;                       //定义一VA_LIST型的变量，这个变量是指向参数的指针。VA_LIST 是解决变参问题的一组宏，用于获取不确定个数的参数
    symset s;

    s = (snode*) malloc(sizeof(snode));
    s->next = NULL;                     //初始化结点s

    va_start(list, elem);
    while (elem)
    {
        setinsert(s, elem);
        elem = va_arg(list, int);
    }//通过不断插入结点的方式直接创建一个单链表
    va_end(list);
    return s;
} // createset

void destroyset(symset s)//delete掉一个链表里的所有结点
{
    snode* p;

    while (s)
    {
        p = s;
        s = s->next;
        free(p);
    }
} // destroyset

int inset(int elem, symset s)//该函数功能为查找一个元素是否在symset中，若在返回1，不在则返回0
{
    while((s != NULL) && (elem != s->elem))
        s = s->next;
    
    if (s != NULL) return 1;
    else return 0;
} // inset

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
        /////////////////////////////////////////////////////////////
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
            getsym();
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

//tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void test(symset s1, symset s2, int n)
{
	symset s;

	if (! inset(sym, s1))
	{
		error(n);
		s = uniteset(s1, s2);
		while(! inset(sym, s))
			getsym();
		destroyset(s);
	}//跳过所有错误语法的单词直至正确
} // test

void constdeclaration(void)//这个函数检测const语句语法是否正确，即const后要接赋值表达式。
{
    if (sym == SYM_IDENTIFIER)
    {
       getsym();
        if ((sym == SYM_EQU) || (sym == SYM_BECOMES))
        {
            if (sym == SYM_BECOMES)
                error(1);
            getsym();
            if (sym == SYM_NUMBER)
                getsym();
            else error(2);
        }
        else error(3);
    }
    else error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
} // constdeclaration

void vardeclaration(void)//这个函数检测var语句语法是否正确，即var后要接一个变量标识符。
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
        while((sym == SYM_EQU)||(sym == SYM_BECOMES))
        {
            error(26);
            getsym();
            if((sym == SYM_NUMBER) || (sym == SYM_IDENTIFIER))
                getsym();
        }
	}
	else
	{
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
	}
} // vardeclaration

//////////////////////////////////////////////////////////////////////
void factor(symset fsys)//因子
{
	void expression();
	symset set;
	
	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	while (inset(sym, facbegsys))
	{
		if (sym == SYM_IDENTIFIER)
		{
			getsym();
		}
		else if (sym == SYM_NUMBER)
		{
			if (num > MAXADDRESS)
			{
				error(25); // The number is too great.
				num = 0;
			}
			getsym();
		}
		else if (sym == SYM_LPAREN)
		{
			getsym();
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			expression(set);
			destroyset(set);
			if (sym == SYM_RPAREN)
			{
				getsym();
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);
	} // while
} // factor


void term(symset fsys)//项
{
	int mulop;
	symset set;
	
	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));
	factor(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH)
	{
		mulop = sym;
		getsym();
		factor(set);
	} // while
	destroyset(set);
} // term


void expression(symset fsys)//表达式
{
	int addop;
	symset set;

	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));
	if (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		term(set);
	}
	else
	{
		term(set);
	}

	while (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
        term(set);
	} // while

	destroyset(set);
} // expression


void condition(symset fsys)//条件
{
	int relop;
	symset set;

	if (sym == SYM_ODD)
	{
		getsym();
		expression(fsys);
	}
	else
	{
		set = uniteset(relset, fsys);
		expression(set);
		destroyset(set);
		if (! inset(sym, relset))
		{
			error(20);
		}
		else
		{
			relop = sym;
			getsym();
			expression(fsys);
			switch (relop)
			{
			} // switch
		} // else
	} // else
} // condition


void statement(symset fsys)//语句
{
	symset set1, set;

	if (sym == SYM_IDENTIFIER)
	{ // variable assignment
		getsym();
		if (sym == SYM_BECOMES)
		{
			getsym();
		}
		else
		{
			error(13); // ':=' expected.
		}
		expression(fsys);
	}
	else if (sym == SYM_CALL)
	{ // procedure call
		getsym();
		if (sym != SYM_IDENTIFIER)
		{
			error(14); // There must be an identifier to follow the 'call'.
		}
		else
		{
			getsym();
		}
	} 
	else if (sym == SYM_IF)
	{ // if statement
		getsym();
		set1 = createset(SYM_THEN, SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_THEN)
		{
			getsym();
		}
		else
		{
			error(16); // 'then' expected.
		}
		statement(fsys);
	}
	else if (sym == SYM_BEGIN)
	{ // block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(10);
			}
			statement(set);
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END)
		{
			getsym();
		}
		else
		{
			error(17); // ';' or 'end' expected.
		}
	}
	else if (sym == SYM_WHILE)
	{ // while statement
		getsym();
		set1 = createset(SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_DO)
		{
			getsym();
		}
		else
		{
			error(18); // 'do' expected.
		}
		statement(fsys);
	}
	test(fsys, phi, 19);
} // statement
			

void block(symset fsys)//程序体
{
	symset set1, set;

	if (level > MAXLEVEL)
	{
		error(32); // There are too many levels.
	}
	do
	{
		if (sym == SYM_CONST)
		{ // constant declarations
            getsym();
            if (sym == SYM_IDENTIFIER)
            {
                constdeclaration();
                while (sym == SYM_COMMA)
                {
                    getsym();
                    constdeclaration();
                }
                if (sym == SYM_SEMICOLON)
                    getsym();
                else error(5);
            }
            else error(4);
		} // if

		if (sym == SYM_VAR)
		{ // variable declarations
            getsym();
            if (sym == SYM_IDENTIFIER)
            {
                vardeclaration();
                while (sym == SYM_COMMA)
                {
                    getsym();
                    vardeclaration();
                }
                if (sym == SYM_SEMICOLON)
                    getsym();
                else error(5);
            }
            else error(4);
		} // if

		while (sym == SYM_PROCEDURE)
		{ // procedure declarations
			getsym();
			if (sym == SYM_IDENTIFIER)
			{
				getsym();
			}
			else
			{
				error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
			}


			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}

			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set);
			destroyset(set1);
			destroyset(set);

			if (sym == SYM_SEMICOLON)
			{
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}//测试接下来是否是新的程序段的开始
			else
			{
				error(5); // Missing ',' or ';'.
			}
		} // while
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		test(set, declbegsys, 7);
		destroyset(set1);
		destroyset(set);
	}
	while (inset(sym, declbegsys));

	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
} // block


int main ()
{
	char s[80];
	symset set, set1, set2;

	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}

	phi = createset(SYM_NULL);
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);//关系符号
	
	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);//声明开始单词符号
	statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);//语句开始单词符号
	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_NULL);//因子开始单词符号

	e_rr = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	getsym();

	set1 = createset(SYM_PERIOD, SYM_NULL);
	set2 = uniteset(declbegsys, statbegsys);
	set = uniteset(set1, set2);
	block(set);///////////////////////一个程序块的开始与结束///////////////////////
	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(relset);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);

	if (sym != SYM_PERIOD)
		error(9); // '.' expected.

	if (e_rr == 0) 
		//add
		printf("\n   success with no error!");
		//end of add
	else
		printf("There are %d error(s) in PL/0 program.\n", e_rr);
    

    /* Please output the number of “:=”，“>”，“<”，“<>”  to file "test.out"*/
    outfile = fopen("/Users/hongxing/Desktop/大三上学期/编译原理/大作业/语法分析大作业/报告/test.out", "w");
    fprintf( outfile, "All ':=','>','<','<>' : %d\n':=' : %d\n'>' : %d\n'<' : %d\n'<>' : %d\n", C_all, C_becomes, C_gtr, C_les, C_neq);
    
    //printf("\nAll ':=','>','<','<>' : %d\n':=' : %d\n'>' : %d\n'<' : %d\n'<>' : %d\n", C_all, C_becomes, C_gtr, C_les, C_neq);
    
    printf("\nCompile End！\n");
    exit(0);
    
} // main    END OF PL0.c
