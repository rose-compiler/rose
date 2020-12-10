#include <cstdio>
#include "DOM.h"

int main() {
  int x;
  x=sizeof(int);
  printf("%d\n",x);
  x=sizeof(x);
  printf("%d\n",x);
  return 0;
}
  
