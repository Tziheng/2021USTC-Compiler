/**
 * 此文件生成操作码(code)
 **/
#include "define.h"
#include <stdlib.h>
#include <stdio.h>
#pragma warning (disable:4996)
instruction code[CXMAX];
int  cx = 0;                // index of current instruction to be generated.

// 生成一条指令
void gen(int x, int y, int z){
	if (cx > CXMAX) { // 程序过长
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	code[cx].f = x;//操作码
	code[cx].l = y;//层次差
	code[cx++].a = z;//a值
}

char* mnemonic[MAXINS] ={
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC","LEA", "LODA", "STOA"
};

void listcode(int from, int to){
	printf("\n");
	for (int i = from; i < to; i++) 
		printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	printf("\n");
} 

