// test for using declarations for classes/structs: 1

class A {

        public:

		int x;

		A() {}

};

class B : A {

        public:

		using A::x;

		int y;

		B() {}

};

int main() {

        return 0;

}
