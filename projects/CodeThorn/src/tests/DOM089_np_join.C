#include <cstdlib>
#include <cstdio>
int main() {
  int x;
  int y;
  x=1;
  y=2;
  int* p;
  p=0;
  int c=rand();
  if(c) {
    p=&x;
    ;
  } else {
    p=&y;
    ;
  }
  *p=100;
  int** pp;
  pp=&p;
  if(*p==50) {
    // this is dead code
    printf("B1:dead:%d (1/2)\n",*p);
  }
  if(x+y==101 || x+y==102) {
    printf("B2:live:%d (1/2)\n",*p);
  } else {
    // this is dead code
    printf("B2:dead:%d (2/2)\n",*p);
  }
  printf("live:%d (2/2)\n",*p);
  return 0;
}

  
