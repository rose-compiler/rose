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
	int fd0, fd1;
	unsigned short filmode;

  sprintf(pfilname, "tfile_%d", getpid());
  fd0 = open(pfilname, O_RDWR | O_CREAT, 01444);

  assert( fd0 != -1 );

	fstat(fd0, &statbuf);
	filmode = statbuf.st_mode;
	if (!(filmode & S_ISVTX)) {
		fprintf(stderr,"Save test bit cleared, but should not have been");
    abort();
  }

	fd1 = open("/tmp", O_DIRECTORY);

  assert( fd1 != -1 );

	fstat(fd1, &statbuf);
	filmode = statbuf.st_mode;
	if (!(filmode & S_IFDIR)) {
    fprintf(stderr, "directory bit cleared, but should not have been");
    abort();
  }

	/* clean up things is case we are looping */
	if (close(fd0) == -1) {
    fprintf(stderr, "close #1 failed");
	}

  if (unlink(pfilname) == -1) {
    fprintf(stderr, "can't remove file");
  }

  if (close(fd1) == -1) {
    fprintf(stderr, "close #2 failed");
	}

	return 0;
}
