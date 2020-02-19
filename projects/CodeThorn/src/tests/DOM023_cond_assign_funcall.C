#include "DOM.h"
int f() {
  int x;
  x=1;
  return x;
}

int f();

int main() {
  int y;
  y=1;
#if 1
  if((y=f())==0) {
    y=y+2;
  }
  printf("y:%d\n",y);
#endif
  int a,b,c,d;
#if 1
  a=b=c=f();
  printf("a,b,c:%d,%d,%d\n",a,b,c);
#endif
  int ar[2];
  int x1=ar[0]=ar[1]=f();
  int x2=y=f();
  x2=a+b+c;
  printf("ar[0]:%d\n",ar[0]);
  printf("ar[1]:%d\n",ar[1]);
  printf("x1:%d\n",x1);
  printf("x2:%d\n",x2);
  printf("y:%d\n",y);
  return 0;
}
