#include <cstdlib>
#include <cstdio>
int main() {
  int x;
  int y;
  x=1;
  y=2;
  int* xp;
  xp=0;
  int c=rand();
  if(c) {
    xp=&x;
    ;
  } else {
    xp=&y;
    ;
  }
  *xp=100;
  int** xxp;
  xxp=&xp;
  printf("%d",*xp);
  return 0;
}

  
