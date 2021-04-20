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
    printf("dead:%d",*p);
  }
  if(x+y==101 || x+y==102) {
    printf("%d",*p);
  }
  printf("%d",*p);
  return 0;
}

  
