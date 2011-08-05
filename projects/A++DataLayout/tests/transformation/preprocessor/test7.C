#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"


int main()
{
	int n=15;
	doubleArray A(n,n);
	doubleArray B(n,n);
	doubleArray C(n,n);
	Range I(1,n,1);
	A(I,I) = A(I,I) + B(I,I) + C(I,I);
}
