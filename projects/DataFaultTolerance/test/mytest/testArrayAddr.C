#include <iostream>

using namespace std;

void foo( int *ptr, int LB, int UB )
{
	int i;

	for(i=LB;i<UB;i++)
	{
		cout << "val : " << *(ptr+i) << endl;
	}
}

void foo2( int *ptr, int index1, int index2 )
{
	cout << " 2d Array:" << *(ptr+index1*100+index2) << endl;
}

int main()
{

 int i, B[100], C[100][100], *ptr;

 for(i=0;i<100;i++)
 {
	B[i] = i;	
 }
	C[10][12]= 50;
	foo2(&C, 10, 10);

 foo(B, 90, 100);
}
