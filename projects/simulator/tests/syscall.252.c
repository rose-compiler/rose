#include <linux/unistd.h>

int main(int argc, char** argv) {
  syscall(252, 4);
}
