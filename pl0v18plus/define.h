#ifndef __DEFINE_H__
#define __DEFINE_H__

#define DEMO		0			// 1 : listcode; 0 : not list code

#define LINEMAX     256			// 一行代码最大长度
#define STACKSIZE  	1000        // 栈的最大值
#define MAXIDLEN   	10          // identifiers最大长度
#define TXMAX      	500         // 标识符表的长度
#define CXMAX      	65536    		// size of code array
#define MAXNUMLEN  	14     		// number最大位数
#define NSYM       	14     		// maximum number of symbols in array ssym and csym
#define MAXSYM     	35     		// maximum number of symbols  
#define NRW        	12     		// 关键字个数
#define MAXINS   	12			// 指令个数
#define MAXARRAYD  	60     		// 数组最大维度
#define MAXADDRESS 	32767  		// 最大地址
#define MAXLEVEL   	32     		// maximum depth of nesting block

enum idtype {
	ID_CONSTANT,ID_VARIABLE,ID_PROCEDURE,ID_ARRAY
};
enum oprcode {
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,OPR_NEQ, OPR_LES, OPR_LEQ, OPR_GTR,OPR_GEQ
};
enum opcode {
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC, LEA, LODA, STOA, PRT  
};
enum symtype {		// 标识符类型，
	SYM_NULL,		// null
	SYM_EOF,		// 终止符,文件结束
	SYM_IDENTIFIER,	// 标识符
	SYM_NUMBER,		// number 数字，常数
	SYM_PLUS,		// +
	SYM_MINUS,		// -
	SYM_TIMES,		// *
	SYM_SLASH,		// 斜杠/
	SYM_ODD,		// 关键字，判断奇偶
	SYM_EQU,		// =  eq
	SYM_NEQ,		// <> not eq
	SYM_LES,		// <  less
	SYM_LEQ,		// >= less and eq
	SYM_GTR,		// >  greater
	SYM_GEQ,		// >= greater and eq
	SYM_LPAREN,		// 左括号(
	SYM_RPAREN,		// 右括号)
	SYM_COMMA,		// 逗号,
	SYM_SEMICOLON,	// 分号;
	SYM_PERIOD,		// 句号.
	SYM_BECOMES,	// := 赋值(与==不同)
// 以下是关键字(保留字) ， 还有一个关键字在上方 odd
	SYM_BEGIN,		// begin
	SYM_END,		// end
	SYM_IF,			// if
	SYM_THEN,		// then 
	SYM_WHILE,		// while
	SYM_DO,			// do
	SYM_CALL,		// call
	SYM_CONST,		// const
	SYM_VAR,		// var 
	SYM_PROCEDURE,	// procedure
	SYM_PRINT,
//改动处: 新增 "[" , "]" , "print" 
	SYM_LSBRACKET,	// 左方括号 [ left square bracket
	SYM_RSBRACKET,	// 右方括号 ]
	SYM_LBRACE,		// 左大括号 '{'
	SYM_RBRACE		// 右大括号 '}'
};
typedef struct instruction{
	int f; // 操作码 opcode
	int l; // 层次差
	int a; // 多用途  displacement address
}instruction;
typedef struct comtab{
	char name[MAXIDLEN + 1];	// 名称
	int  kind;					// 类型 const, var, array or procedure
	int  value;					// 值
	int  level;          		// 所处层
	int  address;				// 地址
	int  dim;					// 数组维度
	int  *len;					// 数组长度
} comtab;
typedef struct ANode{
	int *a;
	struct ANode *next;
}ANode;
typedef struct snode {
	int elem;			// 一个结构体存储一个元素
	struct snode* next;	// 指向下一个集合结点
} snode, * symset;

#endif