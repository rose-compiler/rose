#include <unistd.h>
#include "assert.h"

int main() {
   int fd = open("/dev/urandom");
   int fdup = dup(fd);
   assert(fd == fdup);
   return 0;
}
