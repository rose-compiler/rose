#include <cassert>
#include <cstdio>
int main() {
  int x;
  int y;
  x=1;
  y=++x;
  assert(y==2);
  printf("%d",x);
  printf("%d",y);
  return 0;
}
