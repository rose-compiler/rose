#include <stdio.h>

int a; // linked

void f() {
  printf("f(): a:%d\n",a);
  a=a+10; // starts with default 0
  printf("f(): a:%d\n",a);
}
