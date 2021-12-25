#ifndef __CODE_H__
#define __CODE_H__

extern int cx;

extern instruction code[CXMAX];

void gen(int x, int y, int z);

void listcode(const char * fname,int from, int to);

#endif