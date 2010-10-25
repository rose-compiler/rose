#include <stdio.h>
#include <stdlib.h>

int a;

int& foo(int a, int &b)
{
	return a;
}

class classB
{
	public:
	int field1;
	double field2[100][100];
	int field3[100][100][100];
};

class classA
{
	public:
	classB field1;
	int field2;
};

int main()
{
	int x=1, y=1, z=2;
	x=y+z;
	while(x!=0)
	{
		double y=2, z=3;
		x = (y=x+y) + z/z + a;
		printf("hi %d\n", x);
	}
	
	classA instanceA;
	classB instanceB;
	for(x=0; x<100; x++)
	{
		instanceA.field1.field2[instanceB.field3[x][x+x][x*x]][x]=(double)x;
//		instanceB.field3[x][x+x][x*x] = (double)x*2;
//		z=(y=x++)*(foo(a, z)/=x);
//		y-=2;
	}
}
