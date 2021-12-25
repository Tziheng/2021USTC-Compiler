#include "define.h"
#include "stack.h"
#include "code.h"
#include <stdio.h>
#include "error.h"

// 根据code运行程序
void interpret(){
	if(err){
		 printf("There are %d error(s) in PL/0 program.\n", err);
		 return;
	}
	int pc;        // program counter
	int top;       // top of stack
	int b;         // program, base, and top-stack register
	instruction i; // instruction register

	printf("Begin executing PL/0 program.\n");

	pc = 0;
	b = 1;
	top = 0;
	stack[1] = stack[2] = stack[3] = 0;
	do{
		i = code[pc++];
		switch (i.f){
		case LIT:	// LIT 0 a, 将a值放如栈中
			stack[++top] = i.a;
			break;
		case OPR:	// 栈顶与次栈顶的运算，保存在低栈中
			switch (i.a){ // operator
			case OPR_RET:// return
				top = b - 1;
				pc = stack[top + 3];
				b = stack[top + 2];
				break;
			case OPR_NEG:
				stack[top] = -stack[top];
				break;
			case OPR_ADD:
				top--;
				stack[top] += stack[top + 1];
				break;
			case OPR_MIN:
				top--;
				stack[top] -= stack[top + 1];
				break;
			case OPR_MUL:
				top--;
				stack[top] *= stack[top + 1];
				break;
			case OPR_DIV:
				top--;
				if (stack[top + 1] == 0){
					fprintf(stderr, "Runtime Error: Divided by zero.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] /= stack[top + 1];
				break;
			case OPR_ODD:
				stack[top] %= 2;
				break;
			case OPR_EQU:
				top--;
				stack[top] = stack[top] == stack[top + 1];
				break;
			case OPR_NEQ:
				top--;
				stack[top] = stack[top] != stack[top + 1];
				break;
			case OPR_LES:
				top--;
				stack[top] = stack[top] < stack[top + 1];
				break;
			case OPR_GEQ:
				top--;
				stack[top] = stack[top] >= stack[top + 1];
				break;
			case OPR_GTR:
				top--;
				stack[top] = stack[top] > stack[top + 1];
				break;
			case OPR_LEQ:
				top--;
				stack[top] = stack[top] <= stack[top + 1];
				break;
			} // switch
			break;
		case LOD:	// l层的第a个变量进栈
			stack[++top] = stack[base(b, i.l) + i.a];
			break;
		case STO:	// 栈顶内容给a
			stack[base( b, i.l) + i.a] = stack[top];
			top--;
			break;
		case CAL:	// 调用produce
			stack[top + 1] = base( b, i.l);
			stack[top + 2] = b;
			stack[top + 3] = pc;
			b = top + 1;
			pc = i.a;
			break;
		case INT:	// 开辟a个单元
			top += i.a;
			break;
		case JMP:	// 无条件跳转
			pc = i.a;
			break;
		case JPC:	// if true 跳转
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		case LEA:	// LEA l a, 取地址
			stack[++top] = base( b, i.l) + i.a;
			break;
		case LODA:	// LODA 0 0, 读地址，其中 LODA + LEA = LOD
			stack[top] = stack[stack[top]];
			break;
		case STOA:	// 将栈顶内容存入次栈顶的栈的单元
			stack[stack[top - 1]] = stack[top];
			top -= 2;
			break;
		case PRT:	// print x
			printf("%d\n", stack[top]);
			top--;
			break;
		} // switch
	} while (pc);
	printf("End executing PL/0 program.\n");
} // interpret
