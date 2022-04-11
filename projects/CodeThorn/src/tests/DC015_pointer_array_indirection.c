#include <stdlib.h>
#include <stdio.h>

int main() {

  int c=0;
  int* p[10];
  p[1]=&c;
  *(p[1])=1;
  printf("%d",c);
  if(c!=0) {
    printf("reachable code\n");    
  }
}
