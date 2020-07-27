#include <cstdio>
#include <cassert>

int main() {
  int a0[2];
  int a1[2];
  int* b[2];
  a0[0]=100;
  a0[1]=101;
  a1[0]=1000;
  a1[1]=1001;
  b[0]=&a0[0];
  b[1]=&a1[0];
  printf("%d\n",b[0][0]);
  printf("%d\n",b[0][1]);
  printf("%d\n",b[1][0]);
  printf("%d\n",b[1][1]);
  b[0][0]=b[0][0]+1;
  b[0][1]=b[0][1]+1;
  b[1][0]=b[1][0]+1;
  b[1][1]=b[1][1]+1;
  printf("%d\n",b[0][0]);
  printf("%d\n",b[0][1]);
  printf("%d\n",b[1][0]);
  printf("%d\n",b[1][1]);
  return 0;
}
