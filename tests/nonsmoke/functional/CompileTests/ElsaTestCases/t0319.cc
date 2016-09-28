// t0319.cc
// index a const array with an enumeration

enum E { a,b,c };

int const arr[5] = {1,2,3,4,5};

void f()
{
  E e;
  arr[e];
}
