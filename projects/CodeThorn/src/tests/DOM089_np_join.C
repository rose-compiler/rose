#include <cstdlib>
#include <cstdio>
int main() {
  int x;
  int y;
  int* xp1;
  xp1=&x;
  int c=rand();
  if(c) {
    xp1=&x;
  } else {
    xp1=&y;
  }
  *xp1=100;
  printf("%d",*xp1);
  return 0;
}

  
