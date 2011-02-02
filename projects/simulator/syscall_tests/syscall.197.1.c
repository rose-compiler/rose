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
#include <err.h>

char fname[255];
int fd;
struct stat64 statter;

int main() {

  sprintf(fname, "tfile_%d", getpid());
  fd = open(fname, O_RDWR | O_CREAT, 0700);

  int result = fstat64(fd, &statter);
  if( result == -1 )
    err(1,"fstat64 failed");

  close(fd);
  unlink(fname);

  return 0;
}
