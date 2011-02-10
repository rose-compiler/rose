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


char fileName[40] = "";

int main() {
  
  int cwrite;
  int fd;
  int iws;
  int badcount = 0;
  char pwbuf[256];

  sprintf(fileName,"tfile_%d",getpid());

  for( iws = 0; iws < 254; iws++ ) {
    pwbuf[iws] = 'A' + (iws % 26);
  }
  pwbuf[255] = '\n';

  if( (fd = open(fileName, O_RDWR | O_CREAT, 0777)) == -1)
    err(1,"open %s failed",fileName);

  for (iws = 254; iws > 0; iws--) {
    if( (cwrite = write(fd, pwbuf, iws) ) != iws ) {
      badcount++;
    }
  }
  close(fd);
  unlink(fileName);

  if( badcount != 0 )
    errx(1,"write() failed to return proper count");

  return 0;
}
