#include <stdlib.h>

int main(int argc, char** argv) {
	int a,b,c,d,e;
	a = atoi(argv[1]);
	b = atoi(argv[2]);
	c = atoi(argv[3]);
	d = atoi(argv[4]);
	#pragma smt [a < b, b < c]
	while(a < b+d) {
		a = a + b;
		b = b + c;
	}
	#pragma smt [c > d]
	while(b > d) {
		b = b - a;
	}
	return 0;
}
