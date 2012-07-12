#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"

#define N 10000

int main()
{
	TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);

	doubleArray A(N,N);
	doubleArray B(N,N);
	Index I,J;

	int TIMES = 100;

	for(int i=0 ; i < TIMES; i++)
	{
		A(I,J) = B(I-1,J-1)  + B(I-1,J) + B(I-1,J+1)
						+ B(I,J-1)  + B(I,J) + B(I,J+1)
						+ B(I+1,J-1)  + B(I+1,J) + B(I+1,J+1);
	}

}
