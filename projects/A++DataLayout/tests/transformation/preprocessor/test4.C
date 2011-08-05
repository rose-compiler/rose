#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"


int main()
{
	int n=15;
	doubleArray A(n,n);
	doubleArray B(n,n);
	Range I(2,n-1,1);
	A(I,I) =  B(I-1+2,I+1);
}
