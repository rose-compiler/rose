#include <stdlib.h>
int main() {
  int data;
  data = rand();
  int buffer2[10] = { 0 };
  buffer2[data] = 1;
}
