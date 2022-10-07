#include <stdio.h>

int f() {
  printf("f\n");
  return 1;
}

int g() {
  int x=1;
  printf("g\n");
  return x*10;
}

int main() {
  int a=0;
  printf("main:p1\n",a);
  printf(" %d\n",a);
  int (*fp)();
  printf("main:p2\n",a);
  fp=f;
  printf(" %x\n",fp);
  printf("main:p3\n",a);
  a=f();
  printf("main:p4\n",a);
  printf(" %d\n",a);
  fp=g;
  printf(" %x\n",fp);
  printf("main:p5\n",a);
  a=g();
  printf("main:p6\n",a);
  printf(" %d\n",a);
  printf("main:p7\n",a);
  int (*fp2)();
  //(*fp2);
  printf("main:p8\n",a);
  return 0;
}
