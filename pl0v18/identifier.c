#include "define.h"
#include "error.h"
#include "get.h"
#include "code.h"
#include <string.h>
#include <stdlib.h>

int  tx = 0;			// 名字表当前尾指针
comtab table[TXMAX];	// id的总表,保存了所有变量名及其信息

extern int  level;  	// 当前分程序所在层
extern int  dx;  		// 名字分配到的相对地址 data allocation index

// 新增enter array
void enterArray(int n[], int l) {//不需要tx++，因为 var a[10]; 先读到a，先把a加入variable，然后因为读到'['，只用把kind改成array就行
	table[tx].kind = ID_ARRAY;
	table[tx].dim = l;
	table[tx].value = dx;
	table[tx].len = (int * )malloc(sizeof(int) * l);
	int s = 1;
	int a = dx - 1;
	for (int i = 0; i <= l;++i) {
		s = s * n[i];
		table[tx].len[i] = n[i];
		if (i != l)
			for (int j = 1; j <= s;++j) {// 初始化，令 a = &a[0], a[0] = &a[0][0], a[1] = &a[1][0]...
				gen(LEA, 0, a);
				gen(LEA, 0, dx);
				gen(STOA, 0, 0);
				a++;
				dx += n[i + 1];
			}
	}
}

void enter(int kind) {
	tx++;	// 增加一个新identifier
	strcpy(table[tx].name, id);
	table[tx].kind = kind;	// identifier 的类型
	switch (kind) {
	case ID_CONSTANT:
		if (num > MAXADDRESS){
			error(25); // The number is too great.
			num = 0;
		}
		table[tx].value = num;
		break;
	case ID_VARIABLE:
		table[tx].level = level;
		table[tx].address = dx++;
		break;
	case ID_PROCEDURE:
		table[tx].level = level;
		break;
	} // switch
} // enter

// 根据 名称 寻找在table中的位置
int position(char* id){
	strcpy(table[0].name, id);
	int i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
}