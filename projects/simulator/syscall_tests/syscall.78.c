#include <sys/time.h>
#include <err.h>

int main() {
  struct timeval val;
  struct timezone zone;
  int result = gettimeofday(&val,&zone);

  if( result == -1 )
    err(1,"gettimeofday failed");
  return 0;
}
