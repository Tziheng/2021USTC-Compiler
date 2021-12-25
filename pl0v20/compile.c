#include "define.h"
#include "set.h"
#include "get.h"
#include "identifier.h"
#include "code.h"
#include "error.h"
#include<stdlib.h>
#pragma warning (disable:4996)
void expression(symset fsys);

int  level = 0;  // 当前分程序所在层

int  dx;  // 名字分配到的相对地址 data allocation index


/**
*作用:测试当前sym符号是否合法，用来检测 } ] ) 是否缺少
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
		gen(STO, level, dx - 1);

		return;
	}
	/**
	 * 改动处
	 * 增加了数组声明的处理
	 * */
	 // 读取到左括号，是数组
	int l = 0;//记录[]数 var a[1][1][1][1]; 则l = 4
	int n[MAXARRAYD];
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
		case SYM_RSBRACKET:	// 读到右括号，直接continue
			n[l] = 0;
			getsym();
			continue;
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
		if (sym == SYM_RSBRACKET && i > 1)
			error(27);
		getsym();
	}
	
	// 以下到600行代码为他人所写
	// 以下到600行代码为他人所写
	// 以下到600行代码为他人所写
	// 最终版已优化至100行不到
	int i = 1;//循环变量
	int j = 2;//循环变量
	int k = 0;//循环变量
	while (j <= l)
	{
		if (!n[j])
		{
			error(33);
			break;
		}
		j++;
	}
	
	int Brace_Num = 0;//大括号数目，左括号+1，右括号-1，用于括号检验
	int address[MAXARRAYD + 1];//address[1],address[2]...address[l]保存数组第1到第l的坐标
	int level_offset[MAXARRAYD + 1];//每一层坐标+1产生的偏移量。
	int array_address;//指示数组头部或尾部的坐标
	int offset = 0;//即将填充的位置的偏移量
	int skip_distance;//补0时需要补的偏移量
	int data_distance;//数据存放区长度
	int data_address;//数据存放区首地址
	int LBrace_flag = 0;//是否紧邻着左括号
	int alone_flag = 0;//是否为{1}或{1,}
	for (j = 0; j <= l; j++)//初始化address
	{
		address[j] = 0;
	}
	level_offset[l] = 1;//计算每一层可产生的偏移量
	for (i = l - 1; i >= 0; i--)
	{
		level_offset[i] = n[i + 1] * level_offset[i + 1];
	}
	//数组宣称时初始化只能使用数字，不能使用变量与表达式
	if (n[1])//第一维不缺省
	{
		data_distance = 1;
		for (j = 1; j <= l; j++)
		{
			data_distance *= n[j];
		}//连乘后可得数据区长度
		enterArray(n, l);
		array_address = dx;//dx为下一个变量的首地址
		data_address = array_address - data_distance;//减去数据区长度后得到数据区首地址
		if (sym == SYM_BECOMES)//如果读入:=，则进行初始化
		{
			getsym();
			if (sym == SYM_LBRACE)//后读入{
			{
				getsym();
				Brace_Num++;//左括号，数目+1
				LBrace_flag = 1;//下一字符紧邻左括号
				while (sym != SYM_SEMICOLON)//读到分号停止
				{
					if (sym == SYM_LBRACE)//左括号
					{
						Brace_Num++;//括号数+1
						LBrace_flag = 1;//下一字符紧邻左括号
						getsym();
					}
					else if (sym == SYM_COMMA)//逗号
					{
						getsym();
						LBrace_flag = 0;//下一字符不紧邻左括号
						if (sym == SYM_COMMA)//不会连续两个逗号
						{
							error(12);
						}
					}
					else if (sym == SYM_NUMBER)//数字
					{
						alone_flag = 0;//假定不是单独一个{1}或{1,}
						if (offset >= data_distance)//数组越界
						{
							error(12);
						}
						gen(LEA, level, data_address);
						gen(LIT, 0, offset);
						gen(OPR, 0, OPR_ADD);
						gen(LIT, 0, num);
						gen(STOA, 0, 0);//存储该数值
						getsym();
						if (LBrace_flag)//紧邻左括号，有可能为{1}或{1,}
						{
							if (sym == SYM_COMMA)
							{
								getsym();
								if (sym == SYM_COMMA)//不能连续逗号
								{
									error(12);
								}
								else if (sym == SYM_RBRACE)//{1,}
								{
									alone_flag = 1;
								}
							}
							else if (sym == SYM_RBRACE)//{1}
							{
								alone_flag = 1;
							}
						}
						if (alone_flag)//{1}或{1,}，若为某一行第一元素，需要补0
						{
							skip_distance = 0;
							for (i = l; address[i] == 0 && i > 0; i--)//计算为首单元的行数
							{}
							if (i < l)//为某一行第一元素，需要补0
							{
								skip_distance = level_offset[i] - 1;//该行偏移量-1为需要补零长度
								for (j = 0; j < skip_distance; j++)
								{
									offset++;
									address[l]++;
									if (address[l] >= n[l])//坐标进位
									{
										for (k = l; k > 0; k--)
										{
											if (address[k] == n[k])
											{
												address[k] = 0;
												address[k - 1]++;
											}
										}
									}
									if (offset >= data_distance)//数组越界
									{
										error(12);
									}
									gen(LEA, level, data_address);
									gen(LIT, 0, offset);
									gen(OPR, 0, OPR_ADD);
									gen(LIT, 0, 0);
									gen(STOA, 0, 0);//存储0
								}
							}
							offset++;
							address[l]++;
							if (address[l] >= n[l])//坐标进位
							{
								for (k = l; k > 0; k--)
								{
									if (address[k] == n[k])
									{
										address[k] = 0;
										address[k - 1]++;
									}
								}
							}
						}
						else//不会补0
						{
							offset++;
							address[l]++;
							if (address[l] >= n[l])//坐标进位
							{
								for (i = l; i > 0; i--)
								{
									if (address[i] == n[i])
									{
										address[i] = 0;
										address[i - 1]++;
									}
								}
							}
						}
						LBrace_flag = 0;//下一字符不紧邻左括号
					}
					else if (sym == SYM_RBRACE)
					{
						LBrace_flag = 0;//下一字符不紧邻左括号
						Brace_Num--;//右括号，括号数-1
						getsym();
					}
					else//错误输入
					{
						error(12);
					}
				}//while
				if (offset < data_distance)//补全
				{
					for (; offset < data_distance; offset++)//补全剩余的0
					{
						gen(LEA, level, data_address);
						gen(LIT, 0, offset);
						gen(OPR, 0, OPR_ADD);
						gen(LIT, 0, 0);
						gen(STOA, 0, 0);
					}
				}
				if (Brace_Num != 0)//括号不匹配
				{
					error(12);
				}
			}
			else//缺‘{’
			{
				error(12);
			}
		}
		else//没有:=，没有初始化，进入下一阶段
		{
			return;
		}
	}
	else//第一维缺省，使用链表先缓存存储数据，最后一起存储。链表每个节点存储以第一维为首的一组数据。
	{
	AList A;
	A = (ANode*)malloc(sizeof(ANode));
	A->a = (int*)malloc(level_offset[1] * sizeof(int));
	n[1]++;//开辟第一个空间，第一维+1
	ANode* cur_node = A;
	ANode* New_node;
	offset = 0;
	if (l == 1)//一维数组第一维缺省。不存在补零。
	{
		if (sym == SYM_BECOMES)
		{
			getsym();
			if (sym == SYM_LBRACE)//{
			{
				getsym();
				Brace_Num++;
				while (sym != SYM_SEMICOLON)
				{
					if (sym == SYM_LBRACE)
					{
						Brace_Num++;
						getsym();
					}
					else if (sym == SYM_COMMA)
					{
						getsym();
						if (sym == SYM_COMMA)//不会连续两个逗号
						{
							error(12);
						}
					}
					else if (sym == SYM_NUMBER)
					{//除了第一个数据，剩下每个数据都会在此处开辟新空间
						if (offset >= level_offset[1])//数组越界
						{
							New_node = (ANode*)malloc(sizeof(ANode));
							New_node->a = (int*)malloc(level_offset[1] * sizeof(int));//level_offset[1]=1
							New_node->next = NULL;
							cur_node->next = New_node;
							cur_node = New_node;
							offset = 0;
							n[1]++;//新空间，第一维+1
						}
						cur_node->a[offset] = num;//数据存储到链表节点
						getsym();
						offset++;
					}
					else if (sym == SYM_RBRACE)
					{
						Brace_Num--;
						getsym();
					}
					else//错误输入
					{
						error(12);
					}
				}//while
				if (Brace_Num != 0)//括号不匹配
				{
					error(12);
				}
				enterArray(n, l);//全部读取完毕，存储数据
				array_address = dx;
				data_distance = 1;
				for (j = 1; j <= l; j++)
				{
					data_distance *= n[j];
				}
				data_address = array_address - data_distance;//计算数据区首地址
				cur_node = A;//从头开始
				offset = data_address;//偏移量初始为数据区首地址（配合STO）
				while (cur_node != NULL)//遍历链表
				{
					i = 0;
					while (i < level_offset[1])//逐个存储数据
					{
						gen(LIT, 0, cur_node->a[i]);
						gen(STO, 0, offset);
						offset++;
						i++;
					}
					cur_node = cur_node->next;
				}
			}
			else//缺‘{’
			{
				error(12);
			}
		}
		else//缺省必须在定义时初始化
		{
			error(12);
		}//
	}
	else//l>1,每个链表节点存储数据可能>1，需要考虑补零
	{
		if (sym == SYM_BECOMES)
		{
			getsym();
			if (sym == SYM_LBRACE)//{
			{
				getsym();
				Brace_Num++;
				LBrace_flag = 1;//紧邻左括号
				while (Brace_Num)
				{
					if (sym == SYM_LBRACE)
					{
						Brace_Num++;
						LBrace_flag = 1;//紧邻左括号
						getsym();
					}
					else if (sym == SYM_COMMA)
					{
						getsym();
						LBrace_flag = 0;
						if (sym == SYM_COMMA)//不会连续两个逗号
						{
							error(12);
						}
					}
					else if (sym == SYM_NUMBER)
					{
						alone_flag = 0;
						if (offset >= level_offset[1])//数组越界，需要开辟一片新的空间，第一维+1
						{
							New_node = (ANode*)malloc(sizeof(ANode));
							New_node->a = (int*)malloc(level_offset[1] * sizeof(int));
							New_node->next = NULL;
							cur_node->next = New_node;
							cur_node = New_node;
							offset = 0;
							n[1]++;
						}
						cur_node->a[offset] = num;//数据存入链表
						getsym();
						if (LBrace_flag)//判断补零可能
						{
							if (sym == SYM_COMMA)
							{
								getsym();
								if (sym == SYM_COMMA)
								{
									error(12);
								}
								else if (sym == SYM_RBRACE)
								{
									alone_flag = 1;
								}
							}
							else if (sym == SYM_RBRACE)
							{
								alone_flag = 1;
							}
						}
						if (alone_flag)//可能需要补零
						{
							skip_distance = 0;
							for (i = l; address[i] == 0 && i > 1; i--)
							{
							}
							if (i < l)
							{
								skip_distance = level_offset[i] - 1;
								for (j = 0; j < skip_distance; j++)
								{
									offset++;
									address[l]++;
									if (address[l] >= n[l])
									{
										for (k = l; k > 1; k--)
										{
											if (address[k] == n[k])
											{
												address[k] = 0;
												address[k - 1]++;
											}
										}
									}
									if (offset >= level_offset[1])//数组一维越界，需要开辟新空间。（理论上补零不会遇到这种情况）
									{
										New_node = (ANode*)malloc(sizeof(ANode));
										New_node->a = (int*)malloc(level_offset[1] * sizeof(int));
										New_node->next = NULL;
										cur_node->next = New_node;
										cur_node = New_node;
										offset = 0;
										n[1]++;
									}
									cur_node->a[offset] = 0;//将0存入链表
								}
							}
							offset++;
							address[l]++;
							if (address[l] >= n[l])
							{
								for (k = l; k > 1; k--)
								{
									if (address[k] == n[k])
									{
										address[k] = 0;
										address[k - 1]++;
									}
								}
							}

						}
						else
						{
							offset++;
							address[l]++;
							if (address[l] >= n[l])
							{
								for (i = l; i > 1; i--)
								{
									if (address[i] == n[i])
									{
										address[i] = 0;
										address[i - 1]++;
									}
								}
							}
						}
						LBrace_flag = 0;
					}
					else if (sym == SYM_RBRACE)
					{
						LBrace_flag = 0;
						Brace_Num--;
						getsym();
					}
					else//错误输入
					{
						error(12);
					}
				}//while
				if (offset < level_offset[1])//补全最后一个节点
				{
					for (; offset < level_offset[1]; offset++)//补全最后一个节点的0
					{
						cur_node->a[offset] = 0;
					}
				}
				if (Brace_Num != 0)//括号不匹配
				{
					error(12);
				}
				enterArray(n, l);//存储数据
				array_address = dx;
				data_distance = 1;
				for (j = 1; j <= l; j++)
				{
					data_distance *= n[j];
				}
				data_address = array_address - data_distance;//计算数据区首地址
				cur_node = A;
				offset = data_address;//偏移量初始为数据区首地址（配合STO）
				while (cur_node != NULL)//遍历链表
				{
					i = 0;
					while (i < level_offset[1])//遍历节点内数据
					{
						gen(LIT, 0, cur_node->a[i]);
						gen(STO, 0, offset);//存储数据
						offset++;
						i++;
					}
					cur_node = cur_node->next;
				}
			}
			else//缺‘{’
			{
				error(12);
			}
		}
		else//缺省必须在定义时初始化
		{
			error(12);
		}//
	}
	}
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

	if (inset(sym, facbegsys)) {
		switch (sym) {
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
			if (num > MAXADDRESS) {
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

/**
 * 计算表达式的值，存放在栈顶
 **/
void expression(symset fsys) {
	int addop;	// 用于保存正负号
	symset set;

	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));//新增开始符号集

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

/**
 * 条件处理
 * */
void condition(symset fsys) {
	int relop;
	symset set;

	if (sym == SYM_ODD) {
		getsym();
		expression(fsys);
		gen(OPR, 0, 6);
	}
	else {
		set = uniteset(relset, fsys);
		expression(set);
		destroyset(set);
		if (!inset(sym, relset))
			error(20);
		else {
			relop = sym;
			getsym();
			expression(fsys);
			switch (relop)
			{
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
		} // else
	} // else
} // condition

/**
 * 编译一条语句
 * 开始符号集为fsys
 **/
void statement(symset fsys) {
	int i, cx1, cx2;
	symset set1, set;

	if (sym == SYM_IDENTIFIER) { // 一行的开始是标识符
		if (!(i = position(id)))
			error(11); // 出现没有定义的identifier
		else if (table[i].kind != ID_VARIABLE && table[i].kind != ID_ARRAY) {	// 此标志符没有被声明为variable(变量)或者数组
			error(12); // Illegal assignment.
			i = 0;
		}
		if (table[i].kind == ID_VARIABLE) {
			getsym();
			if (sym == SYM_BECOMES) 	// 赋值语句
				getsym();
			else
				error(13); // ':=' expected.
			expression(fsys);
			if (i)
				gen(STO, level - table[i].level, table[i].address);
		}
		else {// 数组赋值
			getsym();
			gen(LEA, level - table[i].level, table[i].address);
			while (sym == SYM_LSBRACKET) {
				gen(LODA, 0, 0);
				getsym();
				set = uniteset(createset(SYM_RSBRACKET, SYM_NULL), fsys);
				expression(set);//计算[]内的表达式
				destroyset(set);
				gen(OPR, 0, OPR_ADD);
				if (sym == SYM_RSBRACKET)
					getsym();
				else
					error(22);// 缺少']'
			}
			if (sym == SYM_BECOMES) 	// 赋值语句
				getsym();
			else
				error(13); // ':=' expected.
			expression(fsys);
			if (i)
				gen(STOA, 0, 0);
		}
	}
	else if (sym == SYM_CALL) { // 一行的开始是call,如call a,则调动程序a
		getsym();
		if (sym != SYM_IDENTIFIER)
			error(14); // There must be an identifier to follow the 'call'.
		else {
			if (!(i = position(id)))
			{
				error(11); // Undeclared identifier.
			}
			else if (table[i].kind == ID_PROCEDURE) {
				gen(CAL, level - table[i].level, table[i].address);
			}
			else
				error(15); // A constant or variable can not be called. 
			getsym();
		}
	}
	else if (sym == SYM_IF) { // 一行的开始是if
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
	}
	else if (sym == SYM_BEGIN) { // 一行的开始是begin
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);	// 递归调用
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys)) {
			if (sym == SYM_SEMICOLON)
				getsym();
			else
				error(10);
			statement(set);
		}
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END)	// 读取 end， 则此begin结束
			getsym();
		else
			error(17);
	}
	else if (sym == SYM_WHILE) {	// 一行的开始是while
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



	/**
	 * 改动处
	 * */
	int cx0 = cx;
	int tx0 = tx;
	table[tx].address = cx;	// call调用时，pc移至此处

	gen(INT, 0, 3);

	if (level > MAXLEVEL) error(32); // There are too many levels.
	do {

		if (sym == SYM_CONST) {  //读取到const关键字
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

		if (sym == SYM_VAR) { // 读取到var关键字
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


		// const和var均完成，开始程序部分
		block_dx = dx; //save dx before handling procedure call!

		while (sym == SYM_PROCEDURE) { // 读取到 procedure 关键字


		/**
		 * 修改处
		 * 把jump放进里面，用来跳过声名produce
		 * */
			cx0 = cx;
			gen(JMP, 0, 0);	// 生成 jmp 0 0 指令


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

			/**
			 * produce a;
			 * begin
			 * xxx
			 * end;
			 * 对于xxx部分我们发现，可以再次调用block，原因转block注释
			 **/
			level++; // 层次加一，level = 0为main程序
			savedTx = tx;
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set);
			destroyset(set1);
			destroyset(set);
			tx = savedTx;
			level--;
			if (sym == SYM_SEMICOLON) { //end; 的分号，表示produce a 结束，然后将 a 加入终止符(当a出现时，该produce被调用)
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else
				error(5); // Missing ',' or ';'.


			/**
			 *  修改 jump 值
			 * */
			code[cx0].a = cx;	// 修改 jump 值
			cx0 = cx;

		}

		dx = block_dx; //restore dx after handling procedure call!
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		test(set, declbegsys, 7);
		destroyset(set1);
		destroyset(set);


	} while (inset(sym, declbegsys)); //所有声明结束
	//主体部分

//	gen(INT, 0, block_dx);	// INT 0 block_dx 指令，开辟block_dx个单元

	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);
	gen(OPR, 0, OPR_RET); // return
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);
	if (level) return;
	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
} // block



