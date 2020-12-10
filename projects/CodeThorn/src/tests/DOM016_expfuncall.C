#include "DOM.h"

int f() {
  return 1;
}
// function call inside expression
int main() {
  int x;
  x=f()+1;
  printf("x:%d\n",x);
  return 0;
}
