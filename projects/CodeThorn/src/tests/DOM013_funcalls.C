#include "DOM.h"
int globalReturnsTrue() 
{
    return 1;
}

int globalReturnsFalse() 
{
    return 0;
}

int main() {
  int x=1;
  if(globalReturnsTrue()) {
    x++;
  } else {
    x-=10;
  }
  if(globalReturnsFalse()) {
    x-=100;
  } else {
    x+=1;
  }
  printf("x:%d",x);
  return 0;
}
