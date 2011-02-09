#include <sys/types.h>
#include <unistd.h>
#include <err.h>

char *TCID = "syscall.47";
int TST_TOTAL = 1;


int main() {
  int result = getgid();

  if( result == -1 )
    err(1,"getgid failed");
  return 0;
}
