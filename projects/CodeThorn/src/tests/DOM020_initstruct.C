#include "DOM.h"
int main() {
  struct S { int x; int y; };
  S s={1};
  s.x=s.x+1;
  printf("s.x:%d\n",s.x);
  printf("s.y:%d\n",s.y);
  return 0;
}
