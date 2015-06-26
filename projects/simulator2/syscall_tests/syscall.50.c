#include <sys/types.h>
#include <unistd.h>
#include <err.h>

int main() {
  int result = getegid();

  if( result == -1 )
    err(1,"getegid failed");
  return 0;
}
