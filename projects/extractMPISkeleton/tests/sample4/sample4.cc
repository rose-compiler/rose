#include <iostream>

typedef float Real;

int main( )
{
  int x=0;

  #pragma  skel loop iterate atleast(10)
  for (int i=0; x < 100 ; i++) {
    x = i + 1;
    if (x % 2)
      x += 5;
  }

  int j;
  #pragma  skel loop iterate atmost(30)
  for (j=0; x < 100 ; j++) {
    x = j + 1;
  }

  return x;
}
