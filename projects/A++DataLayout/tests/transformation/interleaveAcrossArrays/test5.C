#include "stdio.h"
#define BOUNDS_CHECK 1
#include "A++.h"

#define N 10

void foo()
{
	printf(" Test ");
}

//void foo()
//{
//	doubleArray A(N);
//	doubleArray B(N);
//	A=9;
//}

int main(int argc, char** argv)
{
	Index::setBoundsCheck (On);
	doubleArray A(N);
	doubleArray B(N);
	Range I(0,N-1,1);

	B=2;

	for(int i=0; i<10; i++)
	{
		doubleArray A(N);
		foo();
	}


	//A.display(" This is object of A");
	
	return 0;	
}
