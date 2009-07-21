#include "test8a.h"



int main(int argc, char**argv)
{
	TestClass inst;
	
	int (TestClass::*memberFuncPtr)();
	
	memberFuncPtr= &TestClass::memberFunc3;
	(inst.*memberFuncPtr)();
	

	
	TestClass * p = new DerivedClass();
	p->memberFunc3();
	
	return 0;	
}

