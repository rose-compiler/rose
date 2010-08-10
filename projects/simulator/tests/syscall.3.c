#include "assert.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv) {
  int randomfd = open("/dev/urandom", O_NONBLOCK);
  char buf[1];
  ssize_t nread = read(randomfd,buf,1);
  assert( nread == 1 );
  exit(0);
}
