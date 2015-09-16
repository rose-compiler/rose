int main(int argc, char** argv) {
	int i = 0;
	int a = 1;
	#pragma smt init [i=0,a=1]
	#pragma smt [a>12,i<10]
	while (i < 10) {
		int b = 0;
		a = a + 1;
		if (a > 7) {
			a = a + 3;
		}
		if (a == 5) {
			a = a - 1;
		}
		a = a + 1;
		i = i + 1;
	}
	
	return 0;
}
