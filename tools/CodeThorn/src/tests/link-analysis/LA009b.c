#include <stdio.h>

int a; // linked
int b; // linked
int c; // global, not linked with other decl
extern int d; // resolved
static int e; // file local
static int f; // file local
void foo() {
  a=a+1; // starts with default 0
  b=b+10; // starts with value set in main (200)
  c=c+20; // starts with default 0
  d=d+100; // starts with default 0
  e=e+1000; // starts with default 0
  f=0;
  f=f+10000;
  printf("foo(): c:%d\n",c);
  printf("foo(): d:%d\n",d);
  printf("foo(): e:%d\n",e);
  printf("foo(): f:%d\n",f);
}
