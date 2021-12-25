#ifndef SET_H
#define SET_H
#pragma (disable:4996)

/**
 * 集合操作
 **/
typedef struct snode {
	int elem;			// 一个结构体存储一个元素
	struct snode* next;	// 指向下一个集合结点
} snode, * symset;

symset phi, declbegsys, statbegsys, facbegsys, relset;

symset createset(int data, .../* SYM_NULL */);
void destroyset(symset s);
symset uniteset(symset s1, symset s2);
int inset(int elem, symset s);

#endif
// EOF set.h
