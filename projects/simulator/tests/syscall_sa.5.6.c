#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

char *TCID = "syscall.5";
int TST_TOTAL = 1;


char fname[100] = "";

void setup() {
  sprintf(fname,"tfile_%d",getpid());

  if( mknod(fname, S_IFIFO | 0644, (dev_t)0) < 0 )
    err(1,"mknod failed");
}

void cleanup() {
  unlink(fname);
}

int main() {
  setup();

  int result = open(fname, O_NONBLOCK | O_WRONLY);

  if( result != -1 )
    errx(1,"open succeeded unexpectedly");

  if( errno != ENXIO )
    errx(1,"Expected ENXIO");

  cleanup();

	return 0;
}
