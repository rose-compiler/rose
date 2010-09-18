#include <unistd.h>
#include <err.h>

char *TCID = "syscall.57";
int TST_TOTAL = 1;


int main() {
  int result = setpgid(0,0);

  if( result == -1 )
    err(1,"setpgid failed");

  return 0;
}
