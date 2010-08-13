#include <unistd.h>

int main() {
  setpgid(0,0);
  return 0;
}
