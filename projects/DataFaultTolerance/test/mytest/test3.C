#include <iostream>

using namespace std;

void p1(int *ptr)
{
	cout << ptr << endl;
}

void p2(void *ptr)
{
	cout << "Inside p2" << endl;
	cout << ptr << endl;
}

int main()
{
	int i=5, *ptr;
	float k=6.7;
	ptr = &i;

	p1(ptr);
	p2(ptr);
	p2(&k);

}
