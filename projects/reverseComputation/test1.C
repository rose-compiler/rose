#include "eventSimulation.h"

void frob(int& x) {
  x = x ^ 5;
  x = x * x;
}

void processEvent(int x, int y) {
  ++x;
  sendMessage(x, 3);
  x = x * 5;
  frob(x);
  --x;
  --y;
}
