#include <unistd.h>
#include <sys/types.h>
#include <err.h>

char *TCID = "syscall.64";
int TST_TOTAL = 1;


int main() {
  int result = getppid();

  if( result == -1 )
    err(1,"getppid failed");
  return 0;
}
