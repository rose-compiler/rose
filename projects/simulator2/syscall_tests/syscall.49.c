#include <sys/types.h>
#include <unistd.h>
#include <err.h>

int main() {
  int result = geteuid();

  if( result == -1 )
    err(1,"geteuid failed");
  return 0;
}
