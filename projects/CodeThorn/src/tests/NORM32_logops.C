#include <cassert>

bool t() {
  return true;
}
bool f() {
  return false;
}
bool g() {
  return false;
}

int main() {
  bool x;
  if(t()||f()) {
    x=!false;
  }

  // ======================

  if((t()||f())||g()) {
    x=!false;
  }
  return 0;
}
