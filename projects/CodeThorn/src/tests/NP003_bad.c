#include <stdlib.h>

int main() {
  int a=1;
  int b=2;
  int* p=0;
  int x=-1;
  if(rand()) {
    p=&a;
  } else {
    p=0; // bad case, p can be 0
  }
  x=*p;
  printf("%d\n",x);
  return 0;
}
