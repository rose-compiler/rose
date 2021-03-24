#include <stdio.h>

int a;
int b;
int d;
int e;
static int f;
void foo();

int main() {
  int a; // local (global name in same file)
  int c; // local variable (global name in other file)
  a=100;
  b=200;
  c=300;
  d=4;
  e=5;
  f=6;
  foo();
  printf("main(): a:%d\n",a);
  printf("main(): b:%d\n",b);
  printf("main(): c:%d\n",c);
  printf("main(): d:%d\n",d);
  printf("main(): e:%d\n",e);
  printf("main(): f:%d\n",f);
  return 0;
}
