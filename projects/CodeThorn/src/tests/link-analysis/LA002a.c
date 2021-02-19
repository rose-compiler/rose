#include <stdio.h>

extern int a;

void f();

int main() {
  printf("main(): a:%d\n",a);
  a=1;
  printf("main(): a:%d\n",a);
  f();
  printf("main(): a:%d\n",a);
  return 0;
}
