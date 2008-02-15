#include <stdio.h>
#include <stdlib.h>
extern int add(int a,int b);

int main()
{
  int x;
  int y = (5);
  int a = (8);
  int b = (7);
  x = 4;
  b = 2;
  int i = (add(x,y));
  if (i >= b && y != a) {
    b = b + x * x;
    x = x * i;
  }
  else {
  }
  y = x + b;
  printf(("%d %d %d"),y,x,b);
  return 0;
}


int add(int a,int b)
{
  int c;
  c = a + b;
  return c;
}

