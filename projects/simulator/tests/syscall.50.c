#include <sys/types.h>
#include <unistd.h>
#include <err.h>

char *TCID = "syscall.50";
int TST_TOTAL = 1;


int main() {
  int result = getegid();

  if( result == -1 )
    err(1,"getegid failed");
  return 0;
}
