#include <unistd.h>
#include <err.h>

int main() {
  int result = getpgrp();

  if( result == -1 )
    err(1,"getpgrp failed");
  return 0;
}
