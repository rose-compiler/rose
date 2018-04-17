// test for (not) hiding a function from a class' function:

//int foo;

int foo(int a) { return a; }

class A {

	public:

		int x,y,z;

		void foo() {
			int t = 5;
			// int r = foo(1); // not possible because void foo hides int foo
		}

};

int main() {

	return 0;

}
