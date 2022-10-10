#include <cstdio>

bool trueFun() {
  return true;
}

int main() {
  if(bool x = trueFun()) {
    printf("%d",1);
  }
  return 0;
}
