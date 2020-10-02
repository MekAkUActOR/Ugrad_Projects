/************  PL0.h  *************/
//517021911065_刘浩文_词法分析

#include <stdio.h>

#define NRW        11     // number of reserved words
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       10     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     30     // maximum number of symbols  

#define STACKSIZE  1000   // maximum storage


enum symtype
{
	SYM_NULL,            // 0
	SYM_IDENTIFIER,      // 1        
	SYM_NUMBER,          // 2
	SYM_PLUS,            // 3  +
	SYM_MINUS,           // 4  -
	SYM_TIMES,           // 5  *
	SYM_SLASH,           // 6  /
	SYM_ODD,             // 7  odd
	SYM_EQU,             // 8  =
	SYM_NEQ,             // 9  <>
	SYM_LES,             // 10 < 
	SYM_LEQ,             // 11 <=
	SYM_GTR,             // 12 >
	SYM_GEQ,             // 13 >=
	SYM_LPAREN,          // 14 (
	SYM_RPAREN,          // 15 )
	SYM_COMMA,           // 16 ,
	SYM_SEMICOLON,       // 17 ;
	SYM_PERIOD,          // 18 .
	SYM_BECOMES,         // 19 :=
    SYM_BEGIN,			 // 20 begin
	SYM_END,             // 21 end
	SYM_IF,              // 22 if
	SYM_THEN,            // 23 then
	SYM_WHILE,           // 24 while
	SYM_DO,              // 25 do
	SYM_CALL,            // 26 call
	SYM_CONST,           // 27 const
	SYM_VAR,             // 28 var
	SYM_PROCEDURE,        // 29 procedure
	
    /* TO BE MODIFIED (IF NECESSARY)*/
	
};

enum idtype
{
	ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE
};

enum opcode
{
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC
};

enum oprcode
{
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,
	OPR_NEQ, OPR_LES, OPR_LEQ, OPR_GTR,
	OPR_GEQ
};


typedef struct
{
	int f; // function code
	int l; // level
	int a; // displacement address
} instruction;

//////////////////////////////////////////////////////////////////////
char* err_msg[] =
{
/*  0 */    "",
/*  1 */    "Found ':=' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "':=' expected.",
/* 14 */    "There must be an identifier to follow the 'call'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "The number is too great.",
/* 26 */    "",
/* 27 */    "",
/* 28 */    "",
/* 29 */    "",
/* 30 */    "",
/* 31 */    "",
/* 32 */    "There are too many levels."
};

//////////////////////////////////////////////////////////////////////
char ch;         // last character read
int  sym;        // last symbol read
char id[MAXIDLEN + 1]; // last identifier read
int  num;        // last number read
int  cc;         // character count
int  ll;         // line length
int  kk;
int  e_rr;
int  cx;         // index of current instruction to be generated.
int  level = 0;
int  tx = 0;
/* TO BE MODIFIED */
int  addr = 0;      //指示注释开始符的位置
int  C_all = 0;
int  C_becomes = 0;
int  C_gtr = 0;
int  C_les = 0;
int  C_neq = 0;     //全局变量来存储各符号的个数


char line[80];

instruction code[CXMAX];

char* word[NRW + 1] =
{
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while"
};

int wsym[NRW + 1] =
{
	SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE
};


int ssym[NSYM+1] =
{
	/* TO BE MODIFIED */
    SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH, SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON
};//ssym 保存了 10 个运算符与界符的类别，用于之后进行 符号匹配。

char csym[NSYM+1] =
{
	' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';'
};

#define MAXINS   8

char* mnemonic[MAXINS] =
{
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC"
};



typedef struct
{
	char name[MAXIDLEN + 1];
	int  kind;
	int  value;
} comtab;

comtab table[TXMAX];





typedef struct
{
	char  name[MAXIDLEN + 1];
	int   kind;
	short level;
	short address;
} mask;

FILE* infile;
FILE* outfile;
/*c语言中用来打开文件的操作，c语言中文件是以指针形式存在的。

FILE*fp  定义一个名为fp的指针，属于FILE（文件）类型。FILE是一类特殊的指针，用来操作文件。

fp=fopen("文件名"，"r");调用fopen（）函数，尝试打开同目录下的文件名相同的文件，参数“r"代表以只读方式打开。如果打开成功，则将文件起始位置 的地址返回给fp指针，否则，返回NULL到fp指针。
*/

// EOF PL0.h
