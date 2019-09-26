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
  
  // === OR ===
  
  if(t()||f()) {
    x=!false;
  }
  
  // === AND ===

  if(t()&&f()) {
    x=!false;
  }
    
  // === ORx2, == ===
  if((t()||f())||x==x) {
    x=!false;
  }
  return 0;
}
