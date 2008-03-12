#include "eventSimulation.h"

void processEvent(int x, int y) {
  ++x;
  sendMessage(x, 3);
  x = x * 5;
  --x;
  --y;
}
