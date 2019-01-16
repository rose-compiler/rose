#include <cassert>

bool t() {
  return true;
}
bool f() {
  return false;
}

int main() {
  bool x;
  x=t()||f();
  if(t()||f()) {
    x=!false;
  }
  return 0;
}
