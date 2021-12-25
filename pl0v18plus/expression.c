#include "define.h"
#include "set.h"
#include "get.h"
#include "identifier.h"
#include "error.h"
#include "code.h"

extern int level;

void expression(symset fsys);

// 指针访问
void Pointeraccess(symset fsys) {
	if(sym!= SYM_LSBRACKET)return;
	symset set1, set = uniteset(fsys, set1 = createset(SYM_RSBRACKET, SYM_NULL));destroyset(set1);
	while (sym == SYM_LSBRACKET) {
		getsym();
		expression(set);// 计算a[]中的值
		if (sym == SYM_RSBRACKET) {
			getsym();
			gen(OPR, 0, OPR_ADD);
			gen(LODA, 0, 0);
		}
		else
			error(22); // Missing ']'.
	}
	destroyset(set);
}

// 因子处理
void factor(symset fsys) {
	int i;
	symset set, set1;
	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.
	if (inset(sym, facbegsys)) {
		switch (sym) {
		case SYM_IDENTIFIER:
			if ((i = position(id)) == 0){
				error(11); // Undeclared identifier.
                break;
            }
            switch (table[i].kind) {
			case ID_CONSTANT:
                gen(LIT, 0, table[i].value);
                break;
            case ID_VARIABLE:
            case ID_ARRAY:
			  	gen(LEA, level - table[i].level, table[i].address);
                gen(LODA,0,0);
                break;
            case ID_PROCEDURE:
                error(21); // Procedure identifier can not be in an expression.
                break;
            } // switch
			getsym();
			break;
		case SYM_NUMBER:
			if (num > MAXADDRESS) 
				error(25); // The number is too great.
			gen(LIT, 0, num);
			getsym();
			break;
		case SYM_LPAREN:
			getsym();
			set = uniteset(set1 = createset(SYM_RPAREN, SYM_NULL), fsys);destroyset(set1);
			expression(set);
			destroyset(set);
			sym == SYM_RPAREN ? getsym() : error(22); //  ')'.
			break;
		case SYM_MINUS:	 // 取负,  Expr -> '-' Expr
			getsym();
			factor(fsys);
			gen(OPR, 0, OPR_NEG);
			break;
		}
        Pointeraccess(fsys);
		test(fsys, set = createset(SYM_LPAREN, SYM_NULL), 23);destroyset(set);
	} // if
}

// 计算乘除，优先级高于加减
void term(symset fsys) {
	int mulop;
	symset set, set1;

	set = uniteset(fsys, set1 = createset(SYM_TIMES, SYM_SLASH, SYM_NULL));destroyset(set1);
	factor(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH) {
		mulop = sym;
		getsym();
		factor(set);
		if (mulop == SYM_TIMES)
			gen(OPR, 0, OPR_MUL);
		else
			gen(OPR, 0, OPR_DIV);
	} // while
	destroyset(set);
} // term

// 计算表达式的值，存放在栈顶
void expression(symset fsys) {
	int addop;	// 用于保存正负号
	symset set, set1;

	set = uniteset(fsys, set1 = createset(SYM_PLUS, SYM_MINUS, SYM_NULL));destroyset(set1);//新增开始符号集
	term(set);//先乘除
	while (sym == SYM_PLUS || sym == SYM_MINUS) {
		addop = sym;
		getsym();
		term(set);
		if (addop == SYM_PLUS)
			gen(OPR, 0, OPR_ADD);
		else
			gen(OPR, 0, OPR_MIN);
	} // while
	destroyset(set);
} // expression
