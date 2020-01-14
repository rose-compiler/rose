#include "DOM.h"
#include <cstdio>
int f(int* p) {
  *p=*p+1;
  p++;
  *p=*p+1;
}
int g(int* p) {
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
}
