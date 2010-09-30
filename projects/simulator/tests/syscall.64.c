#include <unistd.h>
#include <sys/types.h>
#include <err.h>

int main() {
  int result = getppid();

  if( result == -1 )
    err(1,"getppid failed");
  return 0;
}
