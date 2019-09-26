#include <cassert>

int f() {
  return 1;
}

int g(int p) {
  int res;
  res=p+1;
  return res; 
}

short h(int p) {
  return p; 
}

int main() {
  int x=f();
  const int y=g(x);
  assert(y==2);
  return 0;
}
