#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

char fname[255];
int fd;
struct stat64 statter;

void setup() {
  sprintf(fname, "tfile_%d", getpid());
  fd = open(fname, O_RDWR | O_CREAT, 0666);
  assert( fd != -1 );

  int result = close(fd);
  assert( result != -1 );
}

void cleanup() {
}

int main() {
  setup();

  int result = fstat64(fd, &statter);
  assert( result == -1);
  assert( errno = EBADF );

  cleanup();

  exit(0);
}
