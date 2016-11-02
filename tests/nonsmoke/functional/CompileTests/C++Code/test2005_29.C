/*
Hi Dan,
I attached a simple source code that has a class with a template member function.
Upon instantiation, it creats a specialized member function declaration, but it does NOT create a function definition ( the body is missing).

Alin Jula
*/

#include <stdio.h>
class A{
public:
int x;

template<class T>
void g(int y, T u);
};
template<class T>
void A::g(int y,T u)
{
x=x+y+u;
}


int main()
{
int t=0,r=3;
A a;
a.g<int>(t,r);
}
