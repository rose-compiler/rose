#include <cstdio>

int f(int x) {
  int y=0;
  return x+y;
}

int g() {
  return 2;
}

int main() {
  int x=1;
  x=x;
  x=f(4)*f(g());
  if(x!=10) {
    printf("%d",x);
  }
  if(x==10) {
    int z=f(0);
    f(z);
  }
  return 0;
}
