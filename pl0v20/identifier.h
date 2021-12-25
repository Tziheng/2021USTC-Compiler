#ifndef __IDRNTIFIER_H__
#define __IDRNTIFIER_H__
#pragma (disable:4996)
extern comtab table[TXMAX];	// id的总表,保存了所有变量名

extern int tx;

void enter(int kind) ;

void enterArray(int n[], int l);

int position(char* id);

#endif