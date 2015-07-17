int main() {
	int a = 1;
	int i = 0;
	#pragma smt init [a=1,i=0]
	#pragma smt [a < 10]
	while (i < 10) {
		a = i++ + a;
	}
	return 0;
}
		
