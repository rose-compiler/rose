#include <stdio.h>
#include <assert.h>
int main() {
  int x;
  scanf("%d",&x);
  if(x) {
  error_1: assert(0);
  }
  if(!x) {
    // do nothing
  } else {
    // not reachable
  error_4: assert(0);
  }
  printf("%d",x);
  return 0;
}
