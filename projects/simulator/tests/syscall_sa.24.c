#include <sys/types.h>
#include <unistd.h>

char *TCID = "syscall.24";
int TST_TOTAL = 1;


int main() {
  int result = getuid();
  
  if( result == -1 )
    err(1,"getuid failed");
  return 0;
}
