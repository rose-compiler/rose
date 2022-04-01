#include <stdio.h>
#include <stdlib.h>

int f() {
  return 1;
}

int g() {
  return 2;
}


int main() {
  int a=1;
  int b=2;
  int (*fp)();
  int x=-1;
  if(rand()) {
    fp=f;
  } else {
    fp=g;
  }
  x=fp();
  printf("%d\n",x);
  return 0;
}
