#include <cstdio>
#include "DOM.h"

int main() {
  int x;
  x=sizeof(int);
  printf("int:%d\n",x);
  x=sizeof(x);
  printf("x(int):%d\n",x);
  int a[]={1,2,3};
  x=sizeof(a);
  printf("a:%d\n",x);
  x=sizeof(a[1]);
  printf("a[1]:%d\n",x);

  char b[]={'a','b','c'};
  x=sizeof(b);
  printf("b:%d\n",x);
  x=sizeof(b[1]);
  printf("b[1]:%d\n",x);

  char c[]={'a','b','c','d'};
  x=sizeof(c);
  printf("c:%d\n",x);
  x=sizeof(c[1]);
  printf("c[1]:%d\n",x);
  x=sizeof(b);
  printf("b:%d\n",x);
  return 0;
}
  
