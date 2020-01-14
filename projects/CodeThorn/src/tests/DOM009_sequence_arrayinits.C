#include "DOM.h"
int main() {
  int a[5]={1,2};
  int b[]={1,2,3};
  int c[3];
  c[1]=a[0]+a[1]+a[2]+a[3]+a[4]+b[0]+b[1]+b[2];
  printf("c[1]:%d\n",c[1]);
  return 0;
}
