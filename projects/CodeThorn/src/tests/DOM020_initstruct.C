#include "DOM.h"
int main() {
  struct S { int x; };
  S s={1};
  s.x=s.x+1;
  printf("s.x:%d\n",s.x);
  return 0;
}
