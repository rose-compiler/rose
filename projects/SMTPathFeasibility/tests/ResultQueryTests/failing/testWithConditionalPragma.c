int main(int argc, char** argv) {
	int i = 0;
	int a = 1;
	#pragma smt [(a%2=0) || (a=1)] 
	while (i < 10) {
		a = a - 1;
		if (a % 2 == 0) {
		a = a + 1;
		}
		else {
		a = a + 2;
		}
		i = i + 1;
	}
	return 0;
}
