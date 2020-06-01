#include "DOM.h"
int main() {
  int a[5]={1,2};
  printf("a[0]:%d\n",a[0]);
  printf("a[1]:%d\n",a[1]);
  printf("a[2]:%d\n",a[2]);
  printf("a[3]:%d\n",a[3]);
  printf("a[4]:%d\n",a[4]);
  int b[]={1,2,3};
  printf("a[0]:%d\n",b[0]);
  printf("a[1]:%d\n",b[1]);
  printf("a[2]:%d\n",b[2]);
  int c[3];
  c[0]=a[0];
  c[1]=a[0]+a[1];
  c[2]=a[0]+a[1]+a[2]+a[3]+a[4]+b[0]+b[1]+b[2]+c[0];
  printf("c[0]:%d\n",c[0]);
  printf("c[1]:%d\n",c[1]);
  printf("c[2]:%d\n",c[2]);
  return 0;
}
