#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"

#define N 10000

int main()
{
	TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);

	doubleArray A(N,N);
	doubleArray B(N,N);
	doubleArray C(N,N);
	doubleArray D(N,N),E(N,N);

	int TIMES = 100;

	for(int i=0 ; i < TIMES; i++)
	{
		A = B + C +D +E;
	}

}
