namespace x {
	int i;
}
class foo {
	public:
		foo* bar();
	static int la;
		static int foobarbar();
		int foobar(int rval) {return (int) sizeof(this);};
	private:
		static int mVal;
};
class bar : public foo {
};
typedef foo* foop;
foo*
foo::bar() {
	int test;
	return new foo();
}
int
doSomething() {
	return 1;
}
int 
foo::foobarbar() {
	return 0;
}
int main() {
	foop lala;
	foo* x = new foo();
	x->bar();
	try {
		foo::foobarbar();
	} catch (int i) {
		foo::foobarbar();
	}
	foo::la = 1;
	return foo::foobarbar();
}
