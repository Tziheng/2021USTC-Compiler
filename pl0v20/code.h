#ifndef __CODE_H__
#define __CODE_H__
#pragma (disable:4996)
extern int cx;

extern instruction code[CXMAX];

void gen(int x, int y, int z);

void listcode(int from, int to);

#endif