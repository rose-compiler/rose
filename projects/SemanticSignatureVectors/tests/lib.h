#ifndef TESTS_LIB_H
#define TESTS_LIB_H

#include <errno.h>

static inline void exit(int x) {
  // asm volatile ("int $0x80; hlt" : : "a" (1), "b" (x)); 
  // asm volatile ("jmp 1f; 1: hlt" : : "b" (x));
  asm volatile ("pushl %%ebx\n\t"
                "movl %1, %%ebx\n\t"
                "int $0x80\n\t"
                "hlt\n\t"
                :
                : "a" (1), "r" ((long)x)
                );
}

static inline int raw_write(int fd, const char* buf, unsigned int count) {
  long retval;
  asm volatile ("pushl %%ebx\n\t"
                "movl %1, %%ebx\n\t"
                "int $0x80\n\t"
                "popl %%ebx\n\t"
                : "=a" (retval)
                : "0" (4), "r" ((long)fd), "c" ((long)buf), "d" ((long)count)
                );
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
  unsigned long count = 0xFFFFFFFFU;
  asm volatile ("repne scasb"
                : "+S" ((long)c), "+c" ((long)count)
                : "a" (0)
                );
  return -count;
}

static inline char* strcpy(char* dest, const char* src) {
    long d0, d1, d2;
    asm volatile("1: lodsb\n\t"
                 "stosb\n\t"
                 "testb %%al,%%al\n\t"
                 "jne 1b"
                 : "=&S" (d0), "=&D" (d1), "=&a" (d2)
                 : "0" (src), "1" (dest)
                 : "memory");
    return dest;
}

int main(int argc, char** argv);

void _start(int foo) {
  int argc = (&foo)[-1];
  char** argv = (char**)(&foo);
  exit (main(argc, argv));
}

#endif // TESTS_LIB_H
