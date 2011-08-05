#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"

#define N 3000

int main()
{
	TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);

	doubleArray A(N,N,N);
	doubleArray B(N,N,N);
	Index I,J,K;

	int TIMES = 100;

	for(int i=0 ; i < TIMES; i++)
	{
		A(I,J,K) = B(I+1,J,K) + B(I-1,J,K)
						 + B(I,J+1,K)	+ B(I,J-1,K)
						 + B(I,J,K+1) + B(I,J,K-1)
						 + B(I,J,K); // 3D 7pt Stencil
	}

}
