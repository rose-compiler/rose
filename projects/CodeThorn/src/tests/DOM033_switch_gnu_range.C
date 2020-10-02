#include <stdio.h>

int main() {
  int x=2;
  int y;
  switch(x) {
  case 0:
    y=100;
    printf("at x:%d\n",x);
    printf("y:%d\n",y);
    break;
  case 1 ... 3:
    y=200;
    printf("at range:%d\n",x);
    printf("y:%d\n",y);
    break;
  default:
    printf("at default:%d\n",x);
    y=300;
    printf("y:%d\n",y);
  }        
  printf("fin:x:%d\n",x);
  printf("fin:y:%d\n",y);
}
