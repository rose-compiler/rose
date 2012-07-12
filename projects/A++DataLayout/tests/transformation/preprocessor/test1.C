#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"


int main()
{
	doubleArray A(10);
	doubleArray B(10);
	doubleArray C(10);
	A = B;
	C = 0;
	A = 0;
}
