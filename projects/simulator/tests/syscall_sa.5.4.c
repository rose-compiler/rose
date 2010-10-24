#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include "err.h"

char *TCID = "syscall.5";
int TST_TOTAL = 1;


int fd, ifile, mypid, first;
int nfile;
int *buf;
char fname[40] = "";

void setup() {
  mypid = getpid();
  nfile = getdtablesize();

  sprintf(fname, "tfile_%d", mypid);

  if ((first = fd = open(fname, O_RDWR | O_CREAT, 0777)) == -1)
    err(1,"open failed");

  close(fd);
  unlink(fname);

  if(( buf = (int *)malloc( sizeof(int) * nfile - first ) ) == NULL )
    err(1,"malloc failed");

  for( ifile = first; ifile <= nfile; ifile++ ) {
    sprintf(fname, "tfile_%d_%d", ifile, mypid);
    if( (fd = open(fname, O_RDWR | O_CREAT, 0777)) == -1) {
        if( errno != EMFILE)
          errx(1,"open succeeded unexpectantly");
        break;
    }
    buf[ifile - first] = fd;
  }
}

void cleanup() {
  for( ifile = first; ifile <= nfile; ifile++ ) {
    close(ifile);
    sprintf(fname, "tfile_%d_%d", ifile, mypid);
    unlink(fname);
  }
}

int main() {
  setup();

  int result = open(fname, O_RDWR | O_CREAT, 0777);

  if( result != -1 )
    errx(1,"open succeeded unexpectedly");
  if( errno != EMFILE )
    err(1,"Expected EMFILE");

  cleanup();

	return 0;
}
