#include "DOM.h"
int main() {
  bool x;
  int a=1;
  int b=3;
  int* p;
  if(x) {
    p=&a;
  } else {
    p=&b;
    //   b=b+1;
  }
  *p=10;
  a=a+1;
  if(x) {
    a=a+1;
  } else {
    a=a+2;
  }
  a=a+1;
  printf("a:%d\n",a);
  printf("*p:%d\n",*p);
}
