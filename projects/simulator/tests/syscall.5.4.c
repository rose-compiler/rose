#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

int fd, ifile, mypid, first;
int nfile;
int *buf;
char fname[40] = "";

void setup() {
  mypid = getpid();
  nfile = getdtablesize();

  sprintf(fname, "tfile_%d", mypid);

  if ((first = fd = open(fname, O_RDWR | O_CREAT, 0777)) == -1) {
    fprintf(stderr,"open failed");
    abort();
  }
  close(fd);
  close(first);
  unlink(fname);

  if(( buf = (int *)malloc( sizeof(int) * nfile - first ) ) == NULL ) {
    fprintf(stderr,"malloc failed");
    abort();
  }

  for( ifile = first; ifile <= nfile; ifile++ ) {
    sprintf(fname, "tfile_%d", ifile, mypid);
    if( (fd = open(fname, O_RDWR | O_CREAT, 0777)) == -1) {
        if( errno != EMFILE) {
            fprintf(stderr,"open succeeded unexpectantly");
            abort();
        }
        break;
    }
    buf[ifile - first] = fd;
  }
}

int main() {
  setup();

  int result = open(fname, O_RDWR | O_CREAT, 0777);

  assert( result == -1 );
  assert( errno = EMFILE );

  close(first);
  close(fd);

	return 0;
}
