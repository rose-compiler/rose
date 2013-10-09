class Foo {
public:
		template <class T> void bar();
		
		void bar();
};

//void Foo::bar() {}
//template <class T> void Foo::bar() {}

int main(void) {
	
//	Foo foo;
//	foo.bar();
// 	foo.bar<int>();
	
	return 0;
}

