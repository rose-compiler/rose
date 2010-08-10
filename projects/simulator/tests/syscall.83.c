#include <unistd.h>

int main() {
  symlink("/dev/null","/tmp/devnull");
  return 0;
}
