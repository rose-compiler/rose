class foo;
class foo {
	public:
		foo(int i) {};
		static int bar() {return 0;} ;
		int foobar() { return bar();};
};
int main() {
	foo* z = new foo(1);
	return foo::bar();
}
