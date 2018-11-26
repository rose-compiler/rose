#include <assert.h>

// assertion,LINE,COLUMN,verified|falsified|unverified
int main() {
  int x;
  int y;
  x=1;
  y=++x;
  assert(y==2);
  return 0;
}
