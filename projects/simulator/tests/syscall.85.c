#include <unistd.h>
#include "assert.h"

int main() {
  char buf[100];
  int count = readlink("/boot/vmlinuz", buf, sizeof(buf));
  assert(count >= 0);
  return 0;
}
