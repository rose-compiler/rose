#include <cassert>

int main() {
  int x=1;
  int y=2;
  if(x && y) {
    x=x+1;
  } else {
    x=x-1;
  }
  return 0;
}
