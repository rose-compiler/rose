#include "eventSimulation.h"

void frob(int& x) {
  x = x ^ 5;
  x = x * x;
}

void fact(int n, int& result) {
  if (n == 0) {
    result = 1;
  } else {
    fact(n - 1, result);
    result = result * n;
  }
}

void processEvent(int x, int y, int z) {
  ++x;
  sendMessage(x, 3);
  x = x * 5;
  frob(x);
  fact(7, z);
  --x;
  --y;
}
