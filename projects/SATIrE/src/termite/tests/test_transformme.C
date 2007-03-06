int glob = 10;
int foo(int i) {
	if (i < 5) {	
		return 0;
	} else {
		glob++;
		return glob;
	}
}
int main() {
	int x = foo(3);
	int y = foo(5);
	int z = foo(y);
	int a = foo(x);
	z = (3 * a) * (2*z);
	int result = (3 +(a + ((foo(x) + (1 + foo(y))) + (foo(a) + 2))));
	return (result > 30);
}
