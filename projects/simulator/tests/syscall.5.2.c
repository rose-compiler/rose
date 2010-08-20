#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

char pfilname[40] = "";

int main() {
	struct stat statbuf;
	int fd;
	unsigned short filmode;

  sprintf(pfilname, "tfile_%d", getpid());
  fd = open(pfilname, O_RDWR, 01444);

  if( fd != -1 )
    errx(1,"open succeeded unexpectedly");

  if( errno != ENOENT )
    err(1,"Expected ENOENT");

	return 0;
}
