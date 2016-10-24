#include <stdio.h>

extern int returnSixFunction();

int returnFiveFunction()
{
	return 5;
}

static int staticFunction(int a)
{
	printf("FILE >f2.C<: staticFunction(%i)\n",a);
	return returnSixFunction()+a;
}



int f2(int a)
{
	int(*fptr)();	
	int retVal=0;
	if (a%1)
	{
		fptr=returnFiveFunction;
	}
	else
	{
		fptr=returnSixFunction;
	}
	for (int i=0;i<a;i++)	
	{
		
		retVal+=fptr()+staticFunction(2);
	}
	return retVal;
}
