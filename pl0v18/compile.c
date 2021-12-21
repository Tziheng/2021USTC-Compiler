#include "define.h"
#include "set.h"
#include "get.h"
#include "identifier.h"
#include "code.h"
#include "error.h"
void expression(symset fsys);

int  level = 0;  // 当前分程序所在层

int  dx;  // 名字分配到的相对地址 data allocation index


/**
*作用:测试当前sym符号是否合法，用来检测 } ] ) ; 是否缺少
**/
void test(symset s1, symset s2, int n) {
	symset s;
	if (!inset(sym, s1)) {
		error(n);
		s = uniteset(s1, s2);
		while (!inset(sym, s))
			getsym();
		destroyset(s);
	}
} 

/**
 * 常量处理声明
 * 实例: const i=7;
 * 错误: const i:=7;
 * 依次读取 identifier 等号 number(数值)
 **/
void constdeclaration() {
	if (sym == SYM_IDENTIFIER) { // identifier
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES) {// 等号
			if (sym == SYM_BECOMES) 
				error(1);
			getsym();
			if (sym == SYM_NUMBER) { // number
				enter(ID_CONSTANT);
				getsym();
			}
			else 
				error(2); // 非number
		}
		else 	 
			error(3); // 非 等号
	}
	else		 
		error(4); // 非 identifier
}

//变量声明处理	var i,j,a[10];
void vardeclaration(symset fsys) {
	if (sym != SYM_IDENTIFIER) {
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
		return;
	}
	enter(ID_VARIABLE);	// 将sym加入到变量表中
	getsym();
	if (sym != SYM_LSBRACKET) {
		if (sym == SYM_BECOMES) 	// 赋值语句
			getsym();
			else
				return; // ':=' expected.
			expression(fsys);
			gen(STO, 0,dx-1);
		return;
	}
	/**
	 * 改动处
	 * 增加了数组声明的处理
	 * */
	// 读取到左括号，是数组
	int l = 0;//记录[]数 var a[1][1][1][1]; 则l = 4
	int n[MAXARRAYD] = {1};
	while (sym == SYM_LSBRACKET) {
		l++;
		getsym();
		switch (sym) {
		case SYM_NUMBER:
			n[l] = num;
			break;
		case SYM_CONST:
			n[l] = table[position(id)].value;
			break;
		case SYM_RSBRACKET:	// 读到右括号，直接continue, var a[],令a是指针
			n[l] = 0;
			break;
		default:
			error(27);// []内不是常数，也不是const中的数
			n[l] = 0;
			break;
		}
		int i = 0;
		while (sym != SYM_RSBRACKET && sym != SYM_COMMA && sym != SYM_SEMICOLON) { // 读到']' 或';'停止
			getsym();
			i++;
		}
		if (sym == SYM_COMMA && sym == SYM_SEMICOLON) {
			error(22);
			break;
		}
		if (sym == SYM_RSBRACKET && i > 1) error(27);
		getsym();
	}
	enterArray(n, l);
}

// 数组处理
void ar(symset fsys) {
	symset set = uniteset(fsys, createset(SYM_RSBRACKET, SYM_NULL));
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

//因子处理
void factor(symset fsys) {
	int i;
	symset set;

	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	if (inset(sym, facbegsys)){
		switch (sym){
		case SYM_IDENTIFIER:
			if ((i = position(id)) == 0)
				error(11); // Undeclared identifier.
			else {
				getsym();
				switch (table[i].kind) {
				case ID_CONSTANT:
					gen(LIT, 0, table[i].value);
					break;
				case ID_VARIABLE:
					gen(LOD, level - table[i].level, table[i].address);
					break;
				case ID_PROCEDURE:
					error(21); // Procedure identifier can not be in an expression.
					break;
					/**
					 * 改动出
					 * 新增了数组因子处理
					 * 读到array时 LEA 将栈顶载入地址
					 * a[5][6]的读取为，a地址入栈(LEA)，计算[]内5的值入栈，将最高两层栈相加放于低栈(OPR_ADD),访问顶栈地址(LODA)
					 * */
				case ID_ARRAY:
					gen(LEA, level - table[i].level, table[i].address);
					gen(LODA, 0, 0);
					ar(fsys);
					break;
				} // switch
			}
			break;
		case SYM_NUMBER:
			if (num > MAXADDRESS){
				error(25); // The number is too great.
				num = 0;
			}
			gen(LIT, 0, num);
			getsym();
			break;
		case SYM_LPAREN:
			getsym();
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			expression(set);
			destroyset(set);
			if (sym == SYM_RPAREN)
				getsym();
			else
				error(22); // Missing ')'.
			break;
		case SYM_MINUS:	 // 取负,  Expr -> '-' Expr
			getsym();
			factor(fsys);
			gen(OPR, 0, OPR_NEG);
			break;
		}
		test(fsys, createset(SYM_LPAREN, SYM_LSBRACKET, SYM_NULL), 23);
	} // if
}

/**
 * 计算乘除，优先级高于加减
 * */
void term(symset fsys) {
	int mulop;
	symset set;

	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));
	factor(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH){
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

/**
 * 计算表达式的值，存放在栈顶
 **/
void expression(symset fsys) {
	int addop;	// 用于保存正负号
	symset set;

	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));//新增开始符号集

	term(set);//先乘除
	while (sym == SYM_PLUS || sym == SYM_MINUS){
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

/**
 * 条件处理
 * */
int condition(symset fsys){
	if (sym == SYM_ODD)	{
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
	switch (relop){
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

/**
 * 编译一条语句
 * 开始符号集为fsys
 **/
void statement(symset fsys) {
	int i, cx1, cx2;
	symset set1, set;

 	
	switch (sym){
	case SYM_IDENTIFIER:// 一行的开始是标识符
		if (!(i = position(id))) error(11); // 出现没有定义的identifier
		else if (table[i].kind != ID_VARIABLE && table[i].kind != ID_ARRAY){	// 此标志符没有被声明为variable(变量)或者数组
			error(12); // Illegal assignment.
			i = 0;
		}
		getsym();
		gen(LEA, level - table[i].level, table[i].address);
		while (sym == SYM_LSBRACKET) {//数组赋值
			gen(LODA, 0, 0);
			getsym();
			set1 = createset(SYM_RSBRACKET, SYM_NULL);
			set = uniteset(set1, fsys);
			expression(set);//计算[]内的表达式
			destroyset(set);
			destroyset(set1);
			gen(OPR, 0, OPR_ADD);
			if (sym == SYM_RSBRACKET) 
				getsym();
			else 
				error(22);// 缺少']'
		}
		sym == SYM_BECOMES ? getsym():error(13);	// 判断是否是 := 符号
		expression(fsys);	// 计算 := 右边的值
		i?gen(STOA, 0, 0):gen(INT,0,-1);	// 赋值
		break;
	case SYM_CALL:// 一行的开始是call,如call a,则调动程序a
		getsym();
		if (sym != SYM_IDENTIFIER)
			error(14); // There must be an identifier to follow the 'call'.
		else {
			if (!(i = position(id)))
				error(11); // Undeclared identifier.
			else if (table[i].kind == ID_PROCEDURE)
				gen(CAL, level - table[i].level, table[i].address);
				else
					error(15); // A constant or variable can not be called. 
			getsym();
		}
		break;
	case SYM_IF:  // 一行的开始是if
		getsym();
		set1 = createset(SYM_THEN, SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_THEN)	// if then 中的then
			getsym();
		else
			error(16); // 'then' expected.
		cx1 = cx;
		gen(JPC, 0, 0);
		statement(fsys);
		code[cx1].a = cx;
		break;
	case SYM_BEGIN: // 一行的开始是begin
		getsym();
		statement(fsys);	// 递归调用
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys)){
			if (sym == SYM_SEMICOLON)
				getsym();
			else
				error(10);
			statement(fsys);
		}
		if (sym == SYM_END)	// 读取 end 则此begin结束
			getsym();
		else
			error(17);
		break;
	case SYM_WHILE:	// 一行的开始是while
		cx1 = cx;
		getsym();
		set1 = createset(SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
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
		break;
	case SYM_PRINT:
		getsym();
		expression(fsys);
		gen(PRT, 0, 0);
		break;
	}
	test(fsys, phi, 19);
} // statement

/**
 * 编译程序主体
 * 所有语法分析和代码生成都在BLOCK中完成
 * fsys:当前模块后跟符号集合
 * pl0实例:
 * (const var produce 都是声明，是可缺省的)
 * const i=1;	// 常量声明，类似 define i 1
 * var j=2; 	// 变量声明，然后 jump 到***处运行
 * produce a;	begin xxxx end	// 声明程序a
 * produce b;	begin xxxx end	// 声明程序b
 * produce c;	begin xxxx end
 * begin ***
 * 	xxx	// 主体部分		必要部分！！！其他var const produce都是可以不出现的，故block可递归调用
 * end
 *
 **/
void block(symset fsys) {
	int block_dx;
	int savedTx;
	symset set1, set;
	dx = 3;
	int cx0; 	
	int tx0 = tx;
	table[tx].address = cx;	// call调用时，pc移至此处

	int cx_INT = cx;
	gen(INT, 0, 0);
	
	if (level > MAXLEVEL) error(32); // There are too many levels.
	while (inset(sym, declbegsys)) {
		while (sym == SYM_CONST) {  //读取到const关键字
			getsym();
			do {
				constdeclaration();//赋值
				while (sym == SYM_COMMA) {	// 读取到逗号，如 const i=1,j=2;
					getsym();
					constdeclaration();//赋值
				}
				if (sym == SYM_SEMICOLON)	// 读取到分号，结束常量声明
					getsym();
				else
					error(5); // 缺少 ',' or ';'.
			} while (sym == SYM_IDENTIFIER);
		}
		while (sym == SYM_VAR) { // 读取到var关键字
			getsym();
			do {
				set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
				set = uniteset(set1, fsys);
				vardeclaration(set);
				while (sym == SYM_COMMA) {	// 读取到逗号
					getsym();
					vardeclaration(set);
				}
				if (sym == SYM_SEMICOLON)	// 读取到分号，结束
					getsym();
				else
					error(5); // Missing ',' or ';'.
			} while (sym == SYM_IDENTIFIER);
		}
		while (sym == SYM_PROCEDURE) { // 读取到 procedure 关键字
			block_dx = dx; //save dx before handling procedure call!
			cx0 = cx;
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
			savedTx = tx;
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set);
			destroyset(set1);
			destroyset(set);
			tx = savedTx;
			level--;
			if (sym == SYM_SEMICOLON){ //end; 的分号，表示produce a 结束，然后将 a 加入终止符(当a出现时，该produce被调用)
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else
				error(5); // Missing ',' or ';'.
			code[cx0].a = cx;	// 修改 jump 值
			cx0 = cx;
			dx = block_dx; //restore dx after handling procedure call!
		}
	
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		test(set, declbegsys, 7);
		destroyset(set1);
		destroyset(set);
	} 
	//主体部分

	code[cx_INT].a = dx;
	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);
	gen(OPR, 0, OPR_RET); // return
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(0,cx0, cx);
	if (level) return;
	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
} // block



