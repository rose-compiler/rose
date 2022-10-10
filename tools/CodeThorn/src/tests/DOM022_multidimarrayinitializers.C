#include "DOM.h"

// multi-dimensional array initializers
int main() {
  int a[2][2]={{1,2},{3,4}};
  int b[][2]={{1,2},{3,4}};
  b[0][0]=a[1][1];
  printf("a[0][0]:%d\n",a[0][0]);
  printf("a[0][1]:%d\n",a[0][1]);
  printf("a[1][0]:%d\n",a[1][0]);
  printf("a[1][1]:%d\n",a[1][1]);
  printf("b[0][0]:%d\n",b[0][0]);
  printf("b[0][1]:%d\n",b[0][1]);
  printf("b[1][0]:%d\n",b[1][0]);
  printf("b[1][1]:%d\n",b[1][1]);
  return 0;
}
