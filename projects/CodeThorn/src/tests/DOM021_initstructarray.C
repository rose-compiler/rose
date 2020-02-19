#include "DOM.h"

struct S {
  int x;
  long a[3];
};

void printStruct(S* s) {
  printf("s->x\n",s->x);
  printf("s->a[0]\n",s->a[0]);
  printf("s->a[1]\n",s->a[1]);
  printf("s->a[2]\n",s->a[2]);
}

// variants of initializing the same data structure
int main() {
  S s1={1,{2,3,4}};
  s1.x=s1.x+1;
  S s2={1,2,3,4};
  s2.x=s2.a[1]+1;
  printStruct(&s1);
  printStruct(&s2);
  return 0;
}
