typedef struct x {
	int i;
} y;

struct z {
	int foo;
};

main() {
	x alpha;
	struct z beta;
	beta.foo = 1;
	return 0;
}
