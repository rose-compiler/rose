#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"


int main()
{
	int n=15;
	doubleArray A(n,n,n);
	Range I(1,4),J(2,4),K(2,4);
	A(I,J,K) = 3;
}
