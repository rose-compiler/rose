#include <cassert>

int f() {
  return 1;
}

int g(int p) {
  return p+1;
}

short h(int q) {
  return (short)q+2;
}

int main() {
  int x=f();
  x=x+0;
  const int y=g(x);
  const int z=h(g(y));
  const int w=(int)h(g(z));
  assert(w==8);
  return 0;
}
