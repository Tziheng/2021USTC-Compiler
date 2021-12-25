#ifndef __GET_H__
#define __GET_H__

extern int  sym,num,cc; 

extern char id[MAXIDLEN + 1],ch,line[80];

void getsym() ;

void openfile();

void writefile();
#endif