#include <iostream>

using namespace std;

void foo(int i)
{
	cout << i << endl;
}

void foo(int i, int j)
{
        cout << i << j << endl;
}

int main()
{
	int i=1, j=2;
	foo(i);
	foo(i,j);
}
