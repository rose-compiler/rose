#include <unistd.h>
#include <err.h>

char *TCID = "syscall.65";
int TST_TOTAL = 1;


int main() {
  int result = getpgrp();

  if( result == -1 )
    err(1,"getpgrp failed");
  return 0;
}
