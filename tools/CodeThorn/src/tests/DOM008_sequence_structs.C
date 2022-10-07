#include "DOM.h"
struct S {
  short x;
  int y;
  int z;
};
int main() {
  S a;
  a.x=5;
  a.y=6;
  a.z=a.x;
  printf("a.z:%d\n",a.z);
  S b;
  b.y=a.x;
  printf("b.y:%d\n",b.y);
  int* p;
  p=&a.y;
  *p=*p+1;
  printf("*p:%d\n",*p);
  S* sp;
  sp=&a;
  sp->y=sp->y+1;
  printf("sp->x:%d\n",sp->x);
  printf("sp->y:%d\n",sp->y);
  printf("sp->z:%d\n",sp->z);
  p=&(sp->y);
  *p=*p+1;
  *&(sp->y)=*p+1;
  printf("sp->x:%d\n",sp->x);
  printf("sp->y:%d\n",sp->y);
  printf("sp->z:%d\n",sp->z);
  return 0;
}

