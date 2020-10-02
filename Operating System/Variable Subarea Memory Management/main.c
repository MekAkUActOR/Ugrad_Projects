//
//  main.c
//  experiment1st_OS_theory
//
//  Created by 幻梦浏光 on 2020/3/12.
//  Copyright © 2020 sjtu_F17. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#define MEMSIZE 1000

/*双向链表的定义*/
struct map
{
    unsigned m_size;
    char *m_addr;
    struct map *next, *prior;
};

struct map *coremap;    //链表起始指针
struct map *flag;       //起始查找指针

/*循环适应的分配函数*/
char *lmalloc(unsigned size)
{
    register char *a;
    register struct map *bp;
    bp = flag;
    do
    {
      if(bp->m_size >= size)
        {
            a = bp->m_addr;
            bp->m_addr += size;
            flag = bp->next;
            if((bp->m_size -= size) == 0)   // 删除用光的空闲区项
            {
                bp->prior->next = bp->next;
                bp->next->prior = bp->prior;
                free(bp);
            }
            printf("Success: lmalloc size: %d, addr:%p\n", size, a);    //分配空间成功
            return(a);
        }
        bp = bp->next;
    }while(bp != flag);
    printf("Error: There is no appropriate memory to be allocated for the size!\n");    //分配空间失败
    return(0);
}

/* 循环首次适应的释放函数 */
char *lfree(unsigned size, char *aa)
{
    struct map *bp;
    char *a;
    
    a = aa;
    bp = coremap;
    
    for(bp = coremap; bp->m_addr <= a; bp = bp->next);
    if (bp->prior->m_addr + bp->prior->m_size >= a && a >= bp->prior->m_addr && bp->next != bp) /* 情况1,2 */
    {
        if(a + size > bp->m_addr + bp->m_size)  // 防止向下溢出一整个空闲项
        {
            printf("Error: Release area out of bounds!\n");
            return(0);
        }
        else if(bp->prior->m_addr + bp->prior->m_size > a + size)   // 防止重复释放已空闲的区域
        {
            printf("Error: Already released!\n");
            return(0);
        }
        else
        {
            if(bp->prior->m_addr + bp->prior->m_size > a)       // 警告：所释放的区域与上一个空闲项的后部有重叠，但仍将执行
            {
                printf("Warning: Release area out of bounds!\n");
                bp->prior->m_size = a + size - bp->prior->m_addr;
            }
            else bp->prior->m_size += size;     /* 情况1 */
        }
        
        if (a + size >= bp->m_addr)     /* 情况2 */
        {
            if(a + size > bp->m_addr)       // 警告：所释放的区域与下一个空闲项的前部有重叠，但仍将执行
            {
                printf("Warning: Release area out of bounds!\n");
                bp->prior->m_size = bp->m_addr + bp->m_size - bp->prior->m_addr;
            }
            else bp->prior->m_size += bp->m_size;
            
            bp->prior->next = bp->next;
            bp->next->prior = bp->prior;
            if(bp == flag)
                flag = bp->next;
            free(bp);
        }
      }
    else
    {
        if(a + size > bp->m_addr + bp->m_size)      // 防止向下溢出一整个空闲项
        {
            printf("Error: Release area out of bounds!\n");
            return(0);
        }
        else if (a+size == bp->m_addr) /* 情况3 */
        {
            bp->m_addr -= size;
            bp->m_size += size;
        }
        else if(a+size > bp->m_addr)        // 警告：所释放的区域与下一个空闲项的前部有重叠，但仍将执行
        {
            printf("Warning: Release area out of bounds!\n");
            bp->m_size = bp->m_addr + bp->m_size - a;
            bp->m_addr = a;
        }
        else/* 情况4 */
        {
            struct map *novel;
            novel = (struct map *)malloc(sizeof(struct map));
            novel->m_addr = a;
            novel->m_size = size;
            novel->prior = bp->prior;
            novel->next = bp;
            bp->prior->next = novel;
            bp->prior = novel;
            if(coremap->m_addr > novel->m_addr)
                coremap = novel;
        }
      }
    printf("Success: lfree mem size=%u, addr=%p\n", size, aa);
    return(0);
}


/*  coremap链表的初始化程序 */
void initcoremap(char *addr, unsigned size)
{
    printf("init coremap, first addr: %p\n", addr);
    coremap = (struct map *)malloc(sizeof(struct map));
    coremap->m_size = size;
    coremap->m_addr = addr;
    coremap->next = coremap;
    coremap->prior = coremap;
    flag = coremap;
  // 初始化coremap双向链表
}

/*  输出链表的内容 */
void printcoremap()
{
    struct map *fg;
    fg = coremap;
    do
    {
        printf("flag->m_addr = %p  ", fg->m_addr);
        printf("flag->m_size = %d\n", fg->m_size);
        fg = fg->next;
    }while(fg != coremap);
    /* Function body:  打印coremap链表中各项的m_size和m_addr  */
}

/*  主程序的框架 */
int main()
{
    char *mymem;
    int size;
    int addr;
    char cmdchar;
    char c;
    if ((mymem = malloc(MEMSIZE)) == NULL)      //在真实主存中申请一块作业空间
    {
      printf("Not enough memory to allocate buffer\n");
      exit(1);
    }
    initcoremap(mymem, MEMSIZE);    //初始化空闲双向链表
    
    while(c!='q')
    {
        do
        {
            c = getchar();
        }while(c=='\n'||c=='\t'||c==' ');
        
        cmdchar = c;
        switch (cmdchar)
        {
        case 'm':
            scanf("%u", &size);
            if(size <= 0)
            {
                printf("Error: Wrong size!\n");
                break;
            }
            lmalloc(size);
            break;
        case 'f':
            scanf("%u %u", &size, &addr);
            if(size <= 0)
            {
                printf("Error: Wrong size!\n");
                break;
            }
            if(addr > MEMSIZE || addr < 0)
            {
                printf("Error: Wrong address!\n");
                break;
            }
            lfree(size, mymem + addr);
            break;
        case 'p':
            printcoremap();
            break;
        default:
            break;
        }
    }
    free(mymem);
    return 0;
}
