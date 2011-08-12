#include <iostream>
using namespace std;


void foo(int *ptr)
{
	cout << "Val: "<< *ptr << " Address: " << ptr <<endl;
}

int main()
{
	int A[50][50];

	A[1][5]=5;
	foo(A[1]+5);
}
