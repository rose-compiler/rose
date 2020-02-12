#include <cstdio>
#include "DOM.h"
int f(int a) {
  return a+1;
}

int main() {
  int x;
  x=sizeof(int);
  printf("%d\n",x);
  x=x+sizeof(x);
  printf("%d\n",x);
  return 0;
}
  
