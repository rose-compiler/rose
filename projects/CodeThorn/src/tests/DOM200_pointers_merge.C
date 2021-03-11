#include "DOM.h"

int rand();

struct S {
  int s1;
  int s2;
};

int main() {
  int x=rand(); // uninitialized
  int a=1;
  int b=3;
  int* p;
#if 0
  p=&a;
#else
  if(x) {
    p=&a;
    ;
    ;
  } else {
    p=&b;
    ;
    ;
    //   b=b+1;
  }
#endif
  ;
 int ** pp;
 pp=&p;
 **pp=100;
// empty statement
  printf("a:%d\n",a);
  printf("b:%d\n",a);
  printf("*p:%d\n",*p);
  printf("*p:%d\n",**pp);
}
