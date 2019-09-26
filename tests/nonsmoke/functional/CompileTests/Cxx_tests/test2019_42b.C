#include <stdio.h>

int returnSixFunction()
{
	return 3*2;
}

static int staticFunction(int a)
{
     // printf("FILE >f1.C<: staticFunction(%i)\n",a);
	return returnSixFunction()+a;
}



int f1(int a)
{
	int retVal=0;
	for (int i=0;i<a;i++)	
	{
		retVal+=staticFunction(a);
	}
	return retVal;
}
