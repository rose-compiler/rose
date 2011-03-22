#include <sys/types.h>
#include <err.h>
#include <unistd.h>

char *TCID = "syscall.202";
int TST_TOTAL = 1;


int main() {
  int result = syscall(202);

  if( result == -1 )
    err(1,"getegid32 syscall(202) failed");
  return 0;
}
