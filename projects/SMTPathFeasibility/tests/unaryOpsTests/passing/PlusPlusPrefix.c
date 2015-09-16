int main() {
	int a = 1;
	int i = 0;
	#pragma smt init [i=0,a=1]
	#pragma smt [a < 10]

	while (i < 10) {
		a = ++i + a;
	}
	return 0;
}
		
