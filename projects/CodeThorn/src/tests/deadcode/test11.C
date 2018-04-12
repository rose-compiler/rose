#include <cstdio>

int f(int x) {
  return x;
}

int g() {
  return 2;
}

int main() {
  int x=1;
  x=x;
  x=f(4)*g()+f(g());
  if(x!=10) {
    printf("%d",x);
  }
  return 0;
}
