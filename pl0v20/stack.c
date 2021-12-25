#include "define.h"
#pragma (disable:4996)
int stack[STACKSIZE];

int base(int currentLevel, int levelDiff){
	int b = currentLevel;
	while (levelDiff--)
		b = stack[b];
	return b;
} 