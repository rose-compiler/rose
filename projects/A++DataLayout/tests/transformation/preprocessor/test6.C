#include "simpleA++.h"
#include "transformationOptions.h"
#include "transformationMacros.h"


int main()
{
	TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

	TransformationAssertion assertion2 (TransformationAssertion::ConstantStrideAccess);

	TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);
	
	doubleArray A(10);
	A = 0;
}
