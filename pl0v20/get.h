#ifndef __GET_H__
#define __GET_H__
#pragma (disable:4996)
extern int  sym,num,cc; 

extern char id[MAXIDLEN + 1],ch,line[80];

void getsym() ;
void getch() ;
void openfile();

#endif