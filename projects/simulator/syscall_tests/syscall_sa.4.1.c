#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>

char *TCID = "syscall.4";
int TST_TOTAL = 1;


char pfilname[40] = "";
char buf = 'w';

int main() {
	int fd;

  sprintf(pfilname, "tfile_%d", getpid());
  fd = open(pfilname, O_RDWR | O_CREAT, 0700);
  if( fd == -1 )
    err(1,"open failed");

  int result = write(fd, &buf, 1);

  if( result == -1 )
    err(1,"write failed");

  close(fd);
  unlink(pfilname);

  return 0;
}
