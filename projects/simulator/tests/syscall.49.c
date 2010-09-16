#include <sys/types.h>
#include <unistd.h>
#include <err.h>

char *TCID = "syscall.49";
int TST_TOTAL = 1;


int main() {
  int result = geteuid();

  if( result == -1 )
    err(1,"geteuid failed");
  return 0;
}
