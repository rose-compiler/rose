#ifndef TESTS_LIB_H
#define TESTS_LIB_H

#include <errno.h>

static inline void exit(int x) {
  asm volatile ("int $0x80; hlt" : : "a" (1), "b" (x));
}

static inline int raw_write(int fd, const char* buf, unsigned int count) {
  int retval;
  asm volatile ("int $0x80" : "=a" (retval) : "a" (4), "b" (fd), "c" (buf), "d" (count));
  return retval;
}

static inline int write(int fd, const char* buf, unsigned int count) {
  int val;
  do {
    val = raw_write(fd, buf, count);
  } while (val == -EINTR);
  return val;
}

static inline int strlen(const char* c);

static inline void print(const char* buf) {
  write(1, buf, strlen(buf));
}

static inline int strlen(const char* c) {
  int x;
  for (x = 0; *c; ++x, ++c) {}
  return x;
}

int main(int argc, char** argv);

void _start() {
  int argc;
  char** argv;
  asm volatile ("mov 4(%%ebp), %0" : "=r" (argc));
  asm volatile ("lea 8(%%ebp), %0" : "=r" (argv));
  exit (main(argc, argv));
}

#endif // TESTS_LIB_H
