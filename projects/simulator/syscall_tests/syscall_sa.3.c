#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <err.h>

char *TCID = "syscall.3";
int TST_TOTAL = 1;


int main() {
  int randomfd = open("/dev/urandom", O_NONBLOCK);
  char buf[1];
  ssize_t nread = read(randomfd,buf,1);
  if( nread != 1 )
    err(1,"read failed. Expected 1 byte read");
  return 0;
}
