#include <sys/types.h>
#include <unistd.h>
#include <err.h>

char *TCID = "syscall.20";
int TST_TOTAL = 1;


int main() {
  int result = getpid();

  if( result == -1 )
    err(1,"getpid failed");
  return 0;
}
