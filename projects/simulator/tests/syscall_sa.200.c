#include <sys/types.h>
#include <err.h>

char *TCID = "syscall.200";
int TST_TOTAL = 1;


int main() {
  int result = syscall(200);
  
  if( result == -1 )
    err(1,"getgid32 syscall(200) failed");
  return 0;
}
