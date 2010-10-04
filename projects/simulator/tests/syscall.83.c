#include <unistd.h>

#include <stdio.h>
#include <err.h>

char sym[40] = "";
int main() {
  sprintf(sym,"/tmp/devnull.%d",getpid());

  int result = symlink("/dev/null",sym);

  if( result == -1 )
    err(1,"symlink failed");

  unlink(sym);
  return 0;
}
