// test for using declarations for classes/structs: 2

struct A {

	int x;

	enum c { i };

};

struct B : A {

	// here: parent of usingdecl. is a class-def
	using A::x;

	using A::i;

	int y;

};

int main() {


	return 0;

}
