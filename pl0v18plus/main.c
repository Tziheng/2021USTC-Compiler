// pl0 compiler source code
#include "PL0.h"


void main() {

	openfile();

	FILE* hbin;
	int i;
	symset set, set1, set2;

	phi = createset(SYM_NULL); // 空集合
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);	// 关系运算符 
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);				// 声明关键字 const,var,procedure
	statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);			// 执行语句
	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_MINUS, SYM_NULL);	// 赋值语句赋值

	getsym(); // 获取第一个标识符 symbol

	set = uniteset(set1 = createset(SYM_PERIOD, SYM_EOF,SYM_NULL), set2 = uniteset(declbegsys, statbegsys));	destroyset(set1);destroyset(set2);
	block(set);	// 分块编译
	destroyset(set);destroyset(phi);destroyset(relset);destroyset(declbegsys);destroyset(statbegsys);destroyset(facbegsys);

	interpret();
	
	writefile();

	getchar();getchar();return;
} 