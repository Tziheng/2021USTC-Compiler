#include "define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "error.h"
#include "code.h"


char csym[NSYM + 1] ={
	' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';','[', ']','{','}'
};

// 关键字，保留字
char* word[NRW + 1] ={
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while","print"
};

int wsym[NRW + 1] ={
	SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE,SYM_PRINT
};

int ssym[NSYM + 1] ={
	SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
	SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON,
	SYM_LSBRACKET,SYM_RSBRACKET,SYM_LBRACE,	SYM_RBRACE	
};

int  sym;               	// 最后读取的一个标识符类型symbol
char id[MAXIDLEN + 1];  	// 最后读取的一个标识符identifier名称
int  num;               	// 最后读取的一个数字
char ch = ' ';              // 最后读取的一个字符char
int  cc = 0;                // 此行已读长度
int  ll = 0;                // 此行总长度
char line[80];

FILE* infile;
FILE* outfile;
char infilename[80];

extern int cx;
//获取下一个字符
void getch(void) {
	if (cc == ll) { // cc = ll时，说明此行已经读完，line转下一行
		if (feof(infile)) {
			printf("\nPROGRAM INCOMPLETE\n");//程序未完成
			getchar();getchar();exit(1);
		}
		ll = cc = 0;
		printf("line:%5d  ", cx);	// 输出行号
		while (!feof(infile) && (ch = getc(infile)) != '\n') {// 读取一行内容存储到line中
			printf("%c", ch);
			line[++ll] = ch;
		}
		printf("\n");
		line[++ll] = ' ';
	}
	ch = line[++cc]; // 下一个字符
}

// 从输入流中获取一个标识符symbol

void getsym(void) {

	int i, k;
	char a[MAXIDLEN + 1];

	while (ch == ' ' || ch == '\t') // 跳过空格和'\t'
		getch();	// 获得第一个字符

	if (isalpha(ch)){ //判断标识符开头是否为英文字母
		k = 0;
		do {
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		} while (isalpha(ch) || isdigit(ch)); // 读取整个标识符
		a[k] = 0;
		strcpy(id, a);//将a传入id
		word[0] = id;
		i = NRW;
		while (strcmp(id, word[i--])); // 判断是否是关键字
		if (++i)
			sym = wsym[i]; // 匹配成功，是关键字
		else
			sym = SYM_IDENTIFIER;   //匹配失败，是标识符identifier
	}
	else if (isdigit(ch)) { // 判断是否是数字
		k = num = 0;
		sym = SYM_NUMBER;
		do {
			num = num * 10 + ch - '0';
			k++;
			getch();
		} while (isdigit(ch));
		if (k > MAXNUMLEN) // 溢出
			error(25);     // The number is too great.
	}
	else if (ch == ':') { // 判断是否为:=
		getch();
		if (ch == '=') {
			sym = SYM_BECOMES; // :=
			getch();
		}
		else
			sym = SYM_NULL;       // illegal?
	}
	else if (ch == '>') { // >=
		getch();
		if (ch == '=') {
			sym = SYM_GEQ;     // >=
			getch();
		}
		else
			sym = SYM_GTR;     // >
	}
	else if (ch == '<') { // <=
		getch();
		if (ch == '=') {
			sym = SYM_LEQ;     // <=
			getch();
		}
		else if (ch == '>') {
			sym = SYM_NEQ;     // <>
			getch();
		}
		else
			sym = SYM_LES;     // <
	}
	else {// 单字符 且不满足以上情况
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch); // 寻找匹配的单字符关键字
		if (++i) {
			sym = ssym[i];
			getch();
		}
		else{
			printf("Fatal Error: Unknown character or PROGRAM INCOMPLETE\n");
			getchar();getchar();exit(0);
		}
	}
}

void openfile(){
	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", infilename);
	if ((infile = fopen(infilename, "r")) == NULL) {
		printf("File %s can't be opened.\n", infilename);
		exit(1);
	}
}

void writefile(){
	strcat(infilename,".code");
	listcode(infilename,0,cx);
}