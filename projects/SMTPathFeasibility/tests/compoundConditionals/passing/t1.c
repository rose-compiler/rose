#include<stdio.h>

int main() {
	int a = 1;
	int b = 1 + a;
	int i = 0;
	#pragma smt init [a=1,b=2,i=0]
	#pragma smt [a > 10]
	while (i < 10) {
	//int a = 1;
	//int b = 1 + a;
	if (a > b && b == a) {
		a++;
	}
	if (a > b || b > a || a == 1 && b == 2) {
		b++;
	}
	i++;
	}
	printf("a: %i, b: %i\n",a,b);
	return 0;
}
	
