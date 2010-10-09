int a;
int r;

int& foo(int a, int &b)
{
	return r;
}

class classB
{
	public:
	int field1;
	double field2[100][100];
	int field3[100][100][100];
	char* field4;
};

class classA
{
	public:
	classB field1;
	int field2;
	char* field298;
	
	void class_method()
	{
		int i=0;
		
		i+=field2;
	}
};

int globalArr[10][10][10];

namespace name{
int namespaceArr[10][10][10];
}
using namespace name;

namespace blah {
classA instanceA;
int main()
{
	int x=1, y=1, z=2, q;
	x=y+z;
	while(x!=0)
	{
		double y=2, z=3;
		x = (y=x+y) + z/z + a;
		namespaceArr[0][0][0] = 0;
	}
	
	classB instanceB;
	for(x=0; x<100; x++)
	{
		instanceA.field1.field2[instanceB.field3[x][x+x][x*x]][x]=(double)x;
		instanceB.field3[x][x+x][x*x] = (double)(x*2 - instanceA.field2);
		z=(y=x++)*(foo(a, z)/=x);
		y-=2;
		globalArr[x][x][x] = x;
	}
	return 0;
}
}
