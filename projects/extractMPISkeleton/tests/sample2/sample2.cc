#include <iostream>

typedef float Real;

int main( )
{
  int x=0;

  #pragma skel  initializer repeat(0x5F)     // initialize array
  int myarray [10];

  #pragma  skel loop iterate exactly(10)
  for (int i=0; x < 100 ; i++) {

    x = i + 1;
  #pragma skel condition prob(60/100)  // make true 60% of time.
    if (x % 2)
      x += 5;
  }

  return x;
}

void f (int x) {

  #pragma skel condition prob(0)  // never do.
  if (x % 2)
    x += 5;

  #pragma skel condition prob(1)  // always do
  if (x % 2)
    x += 5;
  else
    x -= 5;

  #pragma skel condition prob(2/3)  // make true 2/3 of time.
  if (x % 2)
    x += 5;
}
