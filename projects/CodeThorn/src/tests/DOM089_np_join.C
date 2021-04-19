#include <cstdlib>
#include <cstdio>
int main() {
  int x;
  int y;
  x=1;
  y=2;
  int* xp1;
  xp1=0;
  int c=rand();
  if(c) {
    xp1=&x;
    ;
  } else {
    xp1=&y;
    ;
  }
  *xp1=100;
  xp1=0;
  printf("%d",*xp1);
  return 0;
}

  
