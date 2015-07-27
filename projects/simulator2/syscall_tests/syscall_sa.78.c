#include <sys/time.h>
#include <err.h>

char *TCID = "syscall.78";
int TST_TOTAL = 1;


int main() {
  struct timeval val;
  struct timezone zone;
  int result = gettimeofday(&val,&zone);

  if( result == -1 )
    err(1,"gettimeofday failed");
  return 0;
}
