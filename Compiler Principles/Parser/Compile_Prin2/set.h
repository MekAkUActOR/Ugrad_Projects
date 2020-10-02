/************  SET.h  *************/
//517021911065_刘浩文_语法分析

#ifndef SET_H
#define SET_H

typedef struct snode    //定义链表结构的结点
{
	int elem;
	struct snode* next; //结构里包含一个指向结点的指针，一般是指向下一个结点
} snode, *symset;       //结构的对象snode, 指向结构的指针*symset

symset phi, declbegsys, statbegsys, facbegsys, relset;

symset createset(int data, .../* SYM_NULL */);
void destroyset(symset s);
symset uniteset(symset s1, symset s2);
int inset(int elem, symset s);

#endif
// EOF set.h
