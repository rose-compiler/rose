int main(int a, int b) {
	a = a + 1;
	b = b + a;
	if (a == 0) {
		a = b + 1;
		b = a + 1;
	}
	else {
		a = 2+b;
		b = 2-a;
	}
	return 0;
}
