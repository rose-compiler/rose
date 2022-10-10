#include <cassert>

struct T {
  int a;
  int b[6];
};

struct S {
  int a;
  T t;
};

int main() {
  S s;
  s.t.b[0]=1;
  s.t.b[1]=s.t.b[0];
  assert(s.t.b[1]==1);
  T t0;
  t0.b[1]=10;
  s.t=t0;
  assert(s.t.b[1]==10);
  return 0;
}
