#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main() {
  char fname[40] = "";

  sprintf(fname, "tfile_%d", getpid());

  int fd = open(fname, O_RDWR | O_CREAT, 0700);

  int fdup = dup(fd);
  if( fdup == -1 )
    err(1,"dup failed");

  close(fd);
  unlink(fname);

  return 0;
}
