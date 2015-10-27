int main(int argc, char** argv) {
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);
	int c = a + b - a * b ^ b | a & b / a + a++ + ++a + b-- + --b % a + b || b && a;
	c = !c;
	return 0;
}
