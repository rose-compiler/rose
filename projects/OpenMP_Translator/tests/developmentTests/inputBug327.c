/*
test input

By C. Liao
*/
#include <stdio.h>

extern void foo(double y[]);
 double mm[10];
int main(void)
{
 //double mm[10];
  foo(mm+1);
  return 0;
}

void foo(double y[]){}
