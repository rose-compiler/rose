#include "DOM.h"

int rand();

int main() {
  int x=rand(); // uninitialized
  int a=1;
  int b=3;
  do {
    a=a+1;
    b=b-1;
    x=rand();
  } while(x);
  ;// empty statement
  printf("a:%d\n",a);
  printf("b:%d\n",b);
  printf("x:%d\n",x);
}
