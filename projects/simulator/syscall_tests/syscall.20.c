#include <sys/types.h>
#include <unistd.h>
#include <err.h>

int main() {
  int result = getpid();

  if( result == -1 )
    err(1,"getpid failed");
  return 0;
}
