#include <stdio.h>

template <class T>
class templated
{
	T val;
	public:
	templated();
	
	T getVal();
};


int foo(int x){ printf("x=%d\n", x); return x; }
void bar() {}

int main()
{
   int x=(4,5);
   foo((bar(), x));

   int b;
   b=x, 6;
printf("b=%d\n", b);

	templated<int> instance;
	
	return instance.getVal();
}
