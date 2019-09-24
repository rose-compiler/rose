#include <cstdio>

using namespace std;

int f1(int a, int b) {
  return a+b;
}

float f2(int a, int b) {
  return 1;
}

int f3(int a, float b) {
  return 1;
}

int main() {
  int (*fp)(int,int);
  fp=f1;
  int x;
  x=fp(1,2);
  fp=f2;
  printf("%d\n",x);
}
