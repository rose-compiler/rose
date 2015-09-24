int main(int argc, char** argv) {
	int a = atoi(argv[1]);
	short b = (short) atoi(argv[2]);
	long al = (long) a;
	long long all = (long long) a;
	a += a;
	a -= b;
	a *= b;
	a /= al;
	a %= all;
	a <<= al;
	a >>= al;
	a ^= b;
	a &= b;
	a |= b;
	a = b;
	return 0;
}
	
	
