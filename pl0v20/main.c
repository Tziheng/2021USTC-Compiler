// pl0 compiler source code
#pragma warning (disable:4996)
#include "PL0.h"
void main() {

	FILE* hbin;
	int i;
	symset set, set1, set2;

	openfile();
	phi = createset(SYM_NULL); // 空集合
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);

	// create begin symbol sets
	// 创建开始符号集合
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);
	statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);
	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_MINUS, SYM_NULL);

	getsym(); // 获取第一个标识符 symbol

	set1 = createset(SYM_PERIOD, SYM_NULL);
	set2 = uniteset(declbegsys, statbegsys);
	set = uniteset(set1, set2);
	block(set);	// 分块编译
	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(relset);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);

	
	if (err == 0){	// 无报错，则interpret运行
		interpret();
		//hbin = fopen("hbin", "w");for (i = 0; i < cx; i++)fwrite(&code[i], sizeof(instruction), 1, hbin);fclose(hbin);
	}
	else
		printf("There are %d error(s) in PL/0 program.\n", err);
	
	getchar();
	getchar();

	return;
} // main

//////////////////////////////////////////////////////////////////////
// eof pl0.c
