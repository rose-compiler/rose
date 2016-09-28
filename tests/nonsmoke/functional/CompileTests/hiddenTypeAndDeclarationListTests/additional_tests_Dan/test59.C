// test for class elaboration

class B {
	public:
		B() {}
};

int B;

int main() {

	int a = B;

	class B b(); // elaboration

	return 0;

}
