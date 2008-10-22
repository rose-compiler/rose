#ifndef TESTS_LIB_H
#define TESTS_LIB_H

#include <errno.h>

static inline void exit(int x) {
  // asm volatile ("int $0x80; hlt" : : "a" (1), "b" (x));
  asm volatile ("jmp 1f; 1: hlt" : : "b" (x));
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
  unsigned int count = 0xFFFFFFFFU;
  asm volatile ("repne scasb" : "+S" (c), "+c" (count) : "a" (0));
  return -count;
}

static inline char* strcpy(char* dest, const char* src) {
  char* oldDest = dest;
  int len = strlen(src) + 1;
  asm volatile ("rep movsb" : "+S" (src), "+D" (dest), "+c" (len));
  return oldDest;
}

int main(int argc, char** argv);

void _start(int foo) {
  int argc = (&foo)[-1];
  char** argv = (char**)(&foo);
  exit (main(argc, argv));
}

#endif // TESTS_LIB_H
