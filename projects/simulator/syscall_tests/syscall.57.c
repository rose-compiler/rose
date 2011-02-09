#include <unistd.h>
#include <err.h>

int main() {
  int result = setpgid(0,0);

  if( result == -1 )
    err(1,"setpgid failed");

  return 0;
}
