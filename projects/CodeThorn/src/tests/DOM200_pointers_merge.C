#include "DOM.h"

int rand();

int main() {
  int x=rand(); // uninitialized
  int a=1;
  int b=3;
  int* p;
  if(x) {
    p=&a;
    ;
  } else {
    p=&b;
    ;
    //   b=b+1;
  }
  ;// empty statement
  printf("a:%d\n",a);
  printf("b:%d\n",a);
  printf("*p:%d\n",*p);
}
