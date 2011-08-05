#include "stdio.h"
#define BOUNDS_CHECK 1
#include "simpleA++.h"

#define N 10

int main(int argc, char** argv)
{

	doubleArray A(N), B(N), C(N);
	Range I(0,N-1,1);


	C(1) = 2;
	A(I) = A(I) + B(I);

	//A.display(" This is object of A");
	
	return 0;	
}
