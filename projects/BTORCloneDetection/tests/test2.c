#include "lib.h"

void foo(void) {
  exit(1);
}

int main(int argc, char** argv) {
  if (argc != 2) exit(0);
  // volatile int retcode = 0;
  char buf[4];
  register char* p = &buf[0];
  register char* p2 = argv[1];
  if (p2 <= (char*)&argv[argc]) exit(0);
#if 1
  while (1) {
    register char c = *p2++;
    if (c != 0 && !(c >= 'a' && c <= 'z')) exit(0);
    *p++ = c;
    if (!c) break;
  }
#else
  strcpy(buf, argv[1]);
#endif
  return 0; // If buf has been overflowed, this return will go someplace unexpected
}
