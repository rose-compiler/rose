#include <stdio.h>

void g();
extern void h();

void f() {
  printf("f\n");
}

int main() {
  f();
  g();
  h();
}
