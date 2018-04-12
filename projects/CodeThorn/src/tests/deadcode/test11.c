#include <stdio.h>

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
    printLine("Hello");
}

void g2() 
{
    return;
    int x;
    printLine("Hello");
}

void h(int x) 
{
  if(x) {
    x+f1
    return;
    x=1;
  } else {
    return;
    x=2;
  }
  x=3;
}
