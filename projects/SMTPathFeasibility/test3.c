int main(int argc, char** argv) {
	int i = 0;
	int a = 1;
	int b = 0;
	#pragma smt init [i=0,a=1,b=0]
	#pragma smt [1=a%2,i > 2]
	while (i < 10) {
		a = a - 1;
		if (a % 2 == 0) {
		a = a + 2;
		}
		else {
		a = a - 1;
		}
		i = i + 1;
		b = b + 1;
	}
	return 0;
}
