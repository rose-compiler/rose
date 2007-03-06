int main() {
	enum foo {BAR, BARBAR};
	int x = foo(BAR);
	enum foo y = (enum foo) x;
	x = y;
	return 0;
}
