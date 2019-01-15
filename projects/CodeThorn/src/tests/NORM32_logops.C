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
  return 0;
}
