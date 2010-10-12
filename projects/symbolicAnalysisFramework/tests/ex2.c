#include <stdio.h>
#include <stdlib.h>

int i=0, j=0;
void foo1()
{
	for(i=0; i<10; i++);
	{
		j--;
	}
}

void foo2()
{
	if(i<10)
	{
		j++;
	}
}

void foo3(int y)
{	
	if(i<10 || j>10)
	{
		j++;
	}
	else
	{
		j--;
	}
}

void foo4()
{	
	int array[10][10][10];
	
	array[0][0][0] = 1;
	
	foo3(array[0][0][0]);
}

int foo5()
{
	int x=0;
	if(x>5) { return 1;}

	return x=(i+j)*5/i;
}
