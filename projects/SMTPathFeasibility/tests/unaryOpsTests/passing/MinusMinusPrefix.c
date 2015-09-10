int main() {
	int a = 1;
	int i = 10;
	#pragma smt init [a=1,i=10]
	#pragma smt [a < 10]
	while (i > 0) {
		a = --i + a;
	}
	return 0;
}
		
