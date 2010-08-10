#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

char pfilname[40] = "";
char buf = 'w';

int main() {
	int fd;

  sprintf(pfilname, "tfile_%d", getpid());
  fd = open(pfilname, O_RDWR | O_CREAT, 0700);
  assert( fd != -1 );

  int result = write(fd, &buf, 1);

  assert( result != -1 );
  return 0;
}
