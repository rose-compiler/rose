#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include <unistd.h>

char *TCID = "syscall.5";
int TST_TOTAL = 1;


char pfilname[40] = "";

int main() {
	struct stat statbuf;
	int fd0, fd1;
	unsigned short filmode;

  sprintf(pfilname, "tfile_%d", getpid());
  fd0 = open(pfilname, O_RDWR | O_CREAT, 01444);

  if( fd0 == -1 )
    err(1,"open failed");

	fstat(fd0, &statbuf);
	filmode = statbuf.st_mode;
	if (!(filmode & S_ISVTX))
    errx(1,"Save test bit cleared, but should not have been");

	fd1 = open("/tmp", O_DIRECTORY);

  if( fd1 == -1 )
    err(1,"open failed");

	fstat(fd1, &statbuf);
	filmode = statbuf.st_mode;
	if (!(filmode & S_IFDIR))
    errx(1,"directory bit cleared, but should not have been");

	if (close(fd0) == -1)
    err(1,"close #1 failed");
  if (unlink(pfilname) == -1)
    err(1,"can't remove file");
  if (close(fd1) == -1) 
    err(1,"close #2 failed");

	return 0;
}
