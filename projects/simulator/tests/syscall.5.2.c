#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

char pfilname[40] = "";

int main() {
	char *msg;		/* message returned from parse_opts */

	struct stat statbuf;
	int fd;
	unsigned short filmode;

  sprintf(pfilname, "tfile_%d", getpid());
  fd = open(pfilname, O_RDWR, 01444);

  assert( fd == -1 );
  assert( errno == ENOENT );

	return 0;
}
