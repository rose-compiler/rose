#include <sys/types.h>
#include <err.h>

char *TCID = "syscall.199";
int TST_TOTAL = 1;


int main() {
  int result = syscall(199);
  if( result == -1 )
    err(1,"getuid32 syscall(199) failed");
  return 0;
}
