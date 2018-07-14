#include<iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class CTest
{
public: int x;
int y;


inline int area()
{
  return ((this) -> x) * ((this) -> y);
}
CTest(int ,int );
}
;

CTest::CTest(int a,int b)
{
(this) -> x = a;
(this) -> y = b;
}

class CTest testFunc()
{
return CTest::CTest(3,5);
}

int main()
{
class CTest a(3,5);
(std::cout << a. area ()) << std::endl< char  , std::char_traits< char > >;
;
return 0;
}
