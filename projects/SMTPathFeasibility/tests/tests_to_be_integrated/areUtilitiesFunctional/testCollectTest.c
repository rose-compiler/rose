int main() {
	int a = 0;
	int b = 0;
	#pragma path 1 2
	for (int i = 0; i < 10; i++) {
		a = a + i;
		if (b < 5) {
			b = b + a;
		}
		else {
			#pragma endPath 1
			break;
		}
	}
	#pragma endPath 1
	return 0;
}
