#include <stdlib.h>

int main() {
  int a=1;
  int b=2;
  int* p=0;
  int x=-1;
  if(rand()) {
    p=&a;
  } else {
    p=&b;
  }
  x=*p;
  printf("%d\n",x);
  return 0;
}
