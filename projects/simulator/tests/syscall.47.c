#include <sys/types.h>
#include <unistd.h>
#include <err.h>

int main() {
  int result = getgid();

  if( result == -1 )
    err(1,"getgid failed");
  return 0;
}
