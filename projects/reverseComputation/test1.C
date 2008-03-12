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

void fact2(int n, int& result) {
  result = 1;
  while (n > 0) {
    result = result * n;
    --n;
  }
}

#if 0
void even(int& x, bool& result);
void odd(int& x, bool& result) {
  if (x == 0) {
    result = false;
  } else {
    --x;
    even(x, result);
    ++x;
  }
}
void even(int& x, bool& result) {
  if (x == 0) {
    result = true;
  } else {
    --x;
    odd(x, result);
    ++x;
  }
}
#endif

void processEvent(int x, int y, int z, bool evenResult) {
  ++x;
  sendMessage(x, 3);
  x = x * 5;
  frob(x);
  fact(7, z);
  fact2(8, z);
  // even(z, evenResult);
  --x;
  --y;
}
