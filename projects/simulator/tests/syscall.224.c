#include <sys/types.h>
#include <unistd.h>

int main() {
  syscall(224);
  return 0;
}
