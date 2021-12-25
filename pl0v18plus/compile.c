#include "define.h"
#include "set.h"
#include "get.h"
#include "identifier.h"
#include "code.h"
#include "error.h"
#include "expression.h"
#include <stdlib.h>

int  level = 0;  // 当前分程序所在层

int  dx;  // 名字分配到的相对地址 data allocation index

ANode* creatnextnode(ANode* t, int size) {
	if (t)
		t = t->next = (ANode*)malloc(sizeof(ANode));
	else
		t = (ANode*)malloc(sizeof(ANode));
	t->a = (int*)malloc(size * sizeof(int));
	t->next = NULL;
	return t;
}

// 常量处理声明
void constdeclaration() {
	if (sym != SYM_IDENTIFIER) {
		error(4);
		return;
	}
	getsym();
	switch (sym) {
	case SYM_BECOMES:error(1); // 不需要break
	case SYM_EQU: getsym();
		if (sym == SYM_NUMBER) { // number
			enter(ID_CONSTANT);
			getsym();
		}
		else error(2); // 非number
		break;
	default:error(3);
	}
}

//变量声明处理	var i,j,a[10];
void vardeclaration(symset fsys) {
	if (sym != SYM_IDENTIFIER) {
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
		return;
	}
	enter(ID_VARIABLE);		// 将sym加入到变量表中
	getsym();
	int dim, n[MAXARRAYD];	// dim 记录维度， var a[1][1][1][1] 是 4维数组，则dim = 4
	switch (sym) {
	case SYM_EQU: error(13);// 不用break
	case SYM_BECOMES:
		getsym();
		expression(fsys);// ':=' expected.
		gen(STO, 0, dx - 1);
		return;
	case SYM_LSBRACKET: // 增加了数组声明的处理 , 读取到左括号，是数组
		n[dim = 0] = 1;	// 初始化
		while (sym == SYM_LSBRACKET) {
			dim++;
			getsym();
			switch (sym) {
			case SYM_NUMBER:
				getsym();
				if (n[dim] = num) break;
				error(27);
				break;
			case SYM_CONST:
				n[dim] = table[position(id)].value;
				getsym();
				break;
			case SYM_RSBRACKET:	// 读到右括号，直接continue, var a[],令a是指针
				n[dim] = 0;
				if (dim != 1) error(27);
				break;
			default:
				error(27);// []内不是常数，也不是const中的数
				n[dim] = 0;
				getsym();
				break;
			}
			if (sym == SYM_RSBRACKET)
				getsym();
			else
				error(22);
		}
		if (sym != SYM_BECOMES &&  sym != SYM_NEQ) {
			if(n[1] == 0) error(26);
			enterArray(n, dim);//存储数据
			return;
		}
		// TODO:	数组初始化	
	}	
	
}

// 条件处理
int condition(symset fsys) {
	if (sym == SYM_ODD) {
		getsym();
		expression(fsys);
		gen(OPR, 0, 6);
		return 0;
	}
	symset set = uniteset(relset, fsys);
	expression(set);
	destroyset(set);
	if (!inset(sym, relset))
		return error(20);
	int relop = sym;
	getsym();
	expression(fsys); // 计算右值
	switch (relop) {
	case SYM_EQU:
		gen(OPR, 0, OPR_EQU);
		break;
	case SYM_NEQ:
		gen(OPR, 0, OPR_NEQ);
		break;
	case SYM_LES:
		gen(OPR, 0, OPR_LES);
		break;
	case SYM_GEQ:
		gen(OPR, 0, OPR_GEQ);
		break;
	case SYM_GTR:
		gen(OPR, 0, OPR_GTR);
		break;
	case SYM_LEQ:
		gen(OPR, 0, OPR_LEQ);
		break;
	} // switch
}

// 编译一条语句
void statement(symset fsys) {
	int i, cx1, cx2;
	symset set1, set;
	switch (sym) {
		// 可以 完成 5[a] := 10; 这类赋值
	case SYM_NUMBER:
	case SYM_CONST:
	case SYM_LPAREN:
	case SYM_IDENTIFIER:// 一行的开始是标识符
		set = uniteset(set1 = createset(SYM_BECOMES, SYM_NULL), fsys);destroyset(set1);
		expression(set);//计算[]内的表达式
		if(code[--cx].f != LODA)
			error(24);
		sym == SYM_BECOMES ? getsym() : error(13);	// 判断是否是 := 符号
		expression(fsys);	// 计算 := 右边的值
		gen(STOA, 0, 0);	// 赋值
		break;
	case SYM_CALL:// 一行的开始是call,如call a,则调动程序a
		getsym();
		if (sym != SYM_IDENTIFIER) {
			error(14); // There must be an identifier to follow the 'call'.
			break;
		}
		if (!(i = position(id)))
			error(11); // Undeclared identifier.
		else if (table[i].kind == ID_PROCEDURE)
			gen(CAL, level - table[i].level, table[i].address);
		else
			error(15); // A constant or variable can not be called. 
		getsym();
		break;
	case SYM_IF:  // 一行的开始是if
		getsym();
		set = uniteset(set1 = createset(SYM_THEN, SYM_DO, SYM_NULL), fsys);destroyset(set1);
		condition(set);
		destroyset(set);
		if (sym == SYM_THEN)	// if then 中的then
			getsym();
		else
			error(16); // 'then' expected.
		cx1 = cx;
		gen(JPC, 0, 0);
		statement(fsys);
		code[cx1].a = cx;
		return;
	case SYM_BEGIN: // 一行的开始是begin
		getsym();
		statement(fsys);	// 递归调用
		while (sym != SYM_END) {
			if (sym == SYM_PERIOD) {
				error(17);
				return;
			}
			statement(fsys);
		}
		getsym();
		return;				// 不需要test
	case SYM_WHILE:	// 一行的开始是while
		cx1 = cx;
		getsym();
		set = uniteset(set1 = createset(SYM_DO, SYM_NULL), fsys);destroyset(set1);
		condition(set);
		destroyset(set);
		cx2 = cx;
		gen(JPC, 0, 0);
		if (sym == SYM_DO)
			getsym();
		else
			error(18); // 'do' expected.
		statement(fsys);
		gen(JMP, 0, cx1);
		code[cx2].a = cx;
		return;
	case SYM_PRINT:
		getsym();
		if (sym == SYM_IDENTIFIER || sym == SYM_NUMBER || sym == SYM_LPAREN || sym == SYM_CONST) {
			expression(fsys);
			gen(PRT, 0, 0);
		}
		else
			error(28);
		break;
	}
	test(fsys, phi, 19);
	getsym();
} // statement

// 编译程序主体
void block(symset fsys) {
	symset set1, set;
	dx = 3;
	int cxfrom = cx,tx0 = tx;
	table[tx].address = cx;	// call调用时，pc移至此处

	int cx_INT = cx;
	gen(INT, 0, 0);

	if (level > MAXLEVEL) error(32); // There are too many levels.
	while (inset(sym, declbegsys)) {
		while (sym == SYM_CONST) {  //读取到const关键字
			getsym();
			constdeclaration();//赋值
			while (sym == SYM_COMMA) {	// 读取到逗号，如 const i=1,j=2;
				getsym();
				constdeclaration();//赋值
			}
			sym == SYM_SEMICOLON ? getsym() : error(5); // 缺少  ';'.
		}
		while (sym == SYM_VAR) { // 读取到var关键字
			getsym();
			set = uniteset(set1 = createset(SYM_COMMA, SYM_SEMICOLON, SYM_NULL), fsys);
			vardeclaration(set);
			while (sym == SYM_COMMA) {	// 读取到逗号
				getsym();
				vardeclaration(set);
			}
			sym == SYM_SEMICOLON ? getsym() : error(5); // ';'
			destroyset(set);destroyset(set1);
		}
		while (sym == SYM_PROCEDURE) { // 读取到 procedure 关键字
			int block_dx = dx; //save dx before handling procedure call!
			int cx0 = cx;
			gen(JMP, 0, 0);	// 生成 jmp 0 0 指令，把jump放进里面，用来跳过声名produce
			getsym();
			if (sym == SYM_IDENTIFIER) {	// 读取identifier，如 procedure a; 的 a
				enter(ID_PROCEDURE);
				getsym();
			}
			else error(4); // 缺少identifier
			if (sym == SYM_SEMICOLON)	//读取分号，程序声明结束，接下来应该进入程序主体部分
				getsym();
			else
				error(5); // Missing ',' or ';'.
			level++; // 层次加一，level = 0为main程序
			int savedTx = tx;
			set = uniteset(set1 = createset(SYM_SEMICOLON, SYM_NULL), fsys);destroyset(set1);
			block(set);
			destroyset(set);
			tx = savedTx;
			level--;
			code[cx0].a = cx;	// 修改 jump 值
			cx0 = cx;
			dx = block_dx; //restore dx after handling procedure call!
		}
	}
	//主体部分

	code[cx_INT].a = dx;
	set = uniteset(set1 = createset(SYM_SEMICOLON, SYM_NULL), fsys);destroyset(set1);
	statement(set);
	destroyset(set);
	gen(OPR, 0, OPR_RET); // return
	if (level) {
		listcode(0, cxfrom, cx);
		return;
	}
	
	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
	listcode(0, cxfrom, cx);

} // block



