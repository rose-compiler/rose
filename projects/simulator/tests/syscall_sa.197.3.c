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

char *TCID = "syscall.197";
int TST_TOTAL = 1;


char fname[255];
int fd;
struct stat64 statter;

void setup() {
  sprintf(fname, "tfile_%d", getpid());
  fd = open(fname, O_RDWR | O_CREAT, 0666);
  close(fd);
}

int main() {
  setup();

  int result = fstat64(fd, &statter);

  if( result != -1 )
    err(1,"fstat64 succeeded unexpectantly");
  if( errno != EBADF )
    err(1,"Expected EBADF");

  close(fd);
  unlink(fname);

  return 0;
}
