#include <stdlib.h>
#include <stdio.h>

int main() {
  int x;
  int y;
  x=rand();
  if(x>10)
    y=1;
  printf("%d",y); // not working yet
  return 0;
}
