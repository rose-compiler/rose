#include "stdio.h"
#define BOUNDS_CHECK 1
#include "A++.h"

#define N 10

void foo( doubleArray &temp)
{
	temp.display(" Temp display called");
}

int main(int argc, char** argv)
{
	Index::setBoundsCheck (On);

	doubleArray A(N), B(N), D(N);
	Range I(0,N-1,1);


	D(1) = 2;
	A(I) = A(I) + B(I);
	A = A + B;
	A = 2*B;

	foo ( A );
	//A.display(" This is object of A");
	
	return 0;	
}
