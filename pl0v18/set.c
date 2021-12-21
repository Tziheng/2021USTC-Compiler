#include <stdlib.h>
#include <stdarg.h>
#include "set.h"

/**
 * 合并集合s1，s2，同时维持集合性质(降序)
 **/
symset uniteset(symset s1, symset s2) {
	symset s;
	snode* p;
	s1 = s1->next;
	s2 = s2->next;
	s = p = (snode*)malloc(sizeof(snode));
	while (s1 && s2) {
		p = p->next = (snode*)malloc(sizeof(snode));
		if (s1->elem < s2->elem) {
			p->elem = s1->elem;
			s1 = s1->next;
		}
		else {
			p->elem = s2->elem;
			s2 = s2->next;
		}
	}
	while (s1) { // s2先空，将s1剩余加入集合
		p = p->next = (snode*)malloc(sizeof(snode));
		p->elem = s1->elem;
		s1 = s1->next;
	}
	while (s2) { // s1先空，将s2剩余加入集合
		p = p->next = (snode*)malloc(sizeof(snode));
		p->elem = s2->elem;
		s2 = s2->next;
	}
	p->next = NULL;
	return s;
}

/**
 *  降序插入，满足 p->next->elem < p->elem
**/
void setinsert(symset s, int elem) {
	snode* p = s, * q = (snode*)malloc(sizeof(snode));;
	while (p->next && p->next->elem < elem)
		p = p->next;
	q->elem = elem;
	q->next = p->next;
	p->next = q;
}

/**
* 创建一个集合
* 输入：e1,e2,e3....,en,NULL
* 创建过程：创建n个setnode，用next指针串起来
* 输出：返回头指针
**/
symset createset(int elem, .../* SYM_NULL */) {
	va_list list;
	symset s = (snode*)malloc(sizeof(snode));
	s->next = NULL;
	va_start(list, elem);	// 初始化list，与va_arg,va_end一起使用，用于获取...省略号的内容
	while (elem) {	// 当elen不为NULL
		setinsert(s, elem);	// 加入集合
		elem = va_arg(list, int);	// elen等于下一个输入
	}
	va_end(list);
	return s;
}

/**
 * 销毁集合s
 **/
void destroyset(symset s) {
	while (s) {
		snode* p = s;
		p->elem = -1000000;
		s = s->next;
		free(p);
	}
}

/**
 * 判断是否在集合中，如果在返回 1 不在则返回 0
 **/
int inset(int elem, symset s) {
	while ((s = s->next) && s->elem < elem);
	if (s && s->elem == elem) return 1;
	else	return 0;
}

// EOF set.c
