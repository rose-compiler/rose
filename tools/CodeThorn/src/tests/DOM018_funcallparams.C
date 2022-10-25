#include "DOM.h"
#include <cstdio>
void f(int* p) {
  *p=*p+1;
  p++;
  *p=*p+1;
}
void g(int* p) {
  *p=*p+1;
  p++;
  *p=*p+1;
}

int main() {
  int a[]={1,2,3};
  f(a);
  g(a+1);
  printf("%d\n",a[0]);
  printf("%d\n",a[1]);
  printf("%d\n",a[2]);
  return 0;
}
