#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"

#define N 10000000

int main()
{
	TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);

	doubleArray A(N);
	doubleArray B(N);
	Index I;

	int TIMES = 100;

	for(int i=0 ; i < TIMES; i++)
	{
		A(I) = B(I-1) + B(I) + B(I+1);
	}

}
