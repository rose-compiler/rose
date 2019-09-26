#include <cassert>
struct S {
  short x;
  int y;
  long z;
};

struct timex {
  int tai;
  int x :2;
  int y :2;
  int :2;
  int :2;
  int z :2;
};

struct T {
  short tx;
  int ty;
  long tz;
};

int main() {
  S a;
  T b;
  a.y=5;
  b.ty=6;
  int r=0;
  if(a.y==5 && b.ty==6) {
    r=1;
  } else {
    r=2;
  }
  assert(r==1);
  return 0;
}

