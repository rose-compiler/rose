int main() {
	int a = 1;
	#pragma smt init [a=1]
	#pragma smt [a > 5]
	while (a < 5) {
		a+=1;
	}
	return 0;
}
