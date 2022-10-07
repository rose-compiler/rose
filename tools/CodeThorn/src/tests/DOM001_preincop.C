#include "DOM.h"
int main() {
  int x;
  int y;
  x=1;
  y=++x;
  assert(y==2);
  printf("x:%d\n",x);
  printf("y:%d\n",y);
  return 0;
}
