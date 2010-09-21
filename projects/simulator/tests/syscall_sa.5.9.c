
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include <err.h>

char *TCID = "syscall.5";
int TST_TOTAL = 1;


char tempfile[40] = "";

int main() {
  int fd;
  int result = 0;
  char pbuf[255];

  sprintf(tempfile, "tfile_%d", getpid());

  if( (fd = open(tempfile, O_RDWR | O_CREAT, 0600)) == -1 )
    err(1,"can't create %s",tempfile);
  else {
      result = close(fd);
      if( result == -1 )
        err(1,"close failed");

      if( (fd = open(tempfile, O_WRONLY) ) == -1)
        err(1,"open %s failed",tempfile);
  }
  result = read(fd, pbuf, 1);
  if( result != -1 )
    errx(1,"read should not have succeeded");

  result = close(fd);
  if( result == -1 )
    err(1,"close failed");

  if( (fd = open(tempfile, O_RDONLY)) == -1 )
    err(1,"open failed");
  else {
      result = write(fd, pbuf, 1);
      if( result != -1)
        errx(1,"write should not have succeeded");
  }

  result = close(fd);
  if( result == -1 )
    err(1,"close failed");

  result = unlink(tempfile);
  if( result == -1 )
    err(1,"unlink failed");
  return 0;
}
