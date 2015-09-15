#include <stdlib.h>

int main(int argc, char** argv) {
	int a,b,c,d,e;
	a = atoi(argv[1]);
	b = atoi(argv[2]);
	c = atoi(argv[3]);
	#pragma smt init [c>a]
	#pragma smt [a < b]
	while(a < c) {
		a = a + b;
	}
	return 0;
}
