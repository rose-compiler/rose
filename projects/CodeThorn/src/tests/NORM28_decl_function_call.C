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
  const int y=g(x);
  const int z=(int)h(g(y));
  assert(z==5);
  return 0;
}
