
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

char tempfile[40] = "";

int main() {
  int fd;
  int result = 0;
  char pbuf[255];

  sprintf(tempfile, "tfile_%d", getpid());

  if( (fd = open(tempfile, O_RDWR | O_CREAT, 0600)) == -1 ) {
      fprintf(stderr, "  can't create '%s'", tempfile);
      abort();
  } else {
      close(fd);
      if( (fd = open(tempfile, 1) ) == -1) {
          fprintf(stderr, "  open failed");
          abort();
      }
  }
  result = read(fd, pbuf, 1);
  if( result != -1 ) {
      fprintf(stderr, "  read shouldn ot succeed");
      abort();
  }
  close(fd);

  if( (fd = open(tempfile, 0)) == -1 ) {
      fprintf(stderr, "  open failed");
      abort();
  } else {
      result = write(fd, pbuf, 1);
      if( result != -1) {
          fprintf(stderr, "  write should not succeed");
          abort();
      }
  }
  close(fd);

  unlink(tempfile);
  return 0;
}
