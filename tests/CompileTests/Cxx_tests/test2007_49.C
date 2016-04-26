// Test code for computation of hidden type and declaration lists

int x;

typedef int MyInt;
typedef double MyDouble;

namespace Y { int x; int moo(); }

namespace Z { using namespace Y; int z; }

class B { public: int b; private: int a;};

// #if ( (__GNUC__ == 3) || (__GNUC__ == 4) && (__GNUC_MINOR__ < 1) )
#if ( defined(__clang__) == 0 && ( (__GNUC__ == 3) || (__GNUC__ == 4) && (__GNUC_MINOR__ < 1) ) )

// int b; unparses as int A::b; when using g++ 4.1.2, but not under g++ 3.4.6
class A : B {

	public:

		int a;

		int foo() {
			int x = a;
			int y;
			return x;
		}

		int y;

		int goo();

	private:

		int b;

};

int A::goo() {

	int a, b;

	b = 1;

	int wwwwwwwwwwwwww;

	return 0;

}

using namespace Y;

int main() {

	using Z::x;

	{

		int x, y;
		float moo;
		double MyInt;

	}

	return 0;

}
#endif
