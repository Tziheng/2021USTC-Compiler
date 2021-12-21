/**
 * 此文件生成操作码(code)
 **/
#include "define.h"
#include <stdlib.h>
#include <stdio.h>

instruction code[CXMAX];
int  cx = 0;                // index of current instruction to be generated.

// 生成一条指令
void gen(int x, int y, int z){
	if (cx > CXMAX) { // 程序过长
		printf("Fatal Error: Program too long.\n");
		getchar();getchar();exit(1);
	}
	code[cx].f = x;//操作码
	code[cx].l = y;//层次差
	code[cx++].a = z;//a值
}

char* mnemonic[MAXINS] ={
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC","LEA", "LODA", "STOA" , "PRT"
};

void listcode(const char * fname,int from, int to){
	if(DEMO){
		printf("\n");
		for (int i = from; i < to; i++) 
			printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		printf("\n");
	}
	if(fname){
		FILE *fp = fopen(fname,"w");
		fprintf(fp," line opt\tlev\ta\n");
		for (int i = from; i < to; i++)
			fprintf(fp,"%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		fclose(fp);
	}
} 

