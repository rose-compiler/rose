#include <stdlib.h>
int main() {
  int data;
  data = 8;
  int* buffer[10] = { 0 };
  int* p;
  p=*buffer[data];
  int x;
  x=*p;
}
