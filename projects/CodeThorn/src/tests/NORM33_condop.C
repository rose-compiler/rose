#include <cassert>

int f() {
  return 1;
}
int g() {
  return 2;
}
int h() {
  return 4;
}

int main() {

  // ******* in condition *******
  int res=0;
  if((f()==1?g():h())==2) {
    res=8;
  }

  // ******* assert *******
  assert(res==8);

  // ******* in expression *******
  res = res + (f()==1?g():h());
  return 0;
}
