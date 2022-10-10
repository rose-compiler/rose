#include <stdio.h>

// dummy is not called (dead code)

void dummy() {
}

void f1() 
{
    return;
    dummy();
}

void f2() 
{
    return;
    int x;
    dummy();
}

void g1() 
{
    return;
    printf("Hello");
}

void g2() 
{
    return;
    int x;
    printf("Hello");
}

void h(int x) 
{
  if(x) {
    return;
    x=1;
  } else {
    return;
    x=2;
  }
  x=3;
}

int main() {
  h(3);
  g2();
  g1();
  f1();
  f2();
  return 0;
}
