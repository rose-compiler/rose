#include <unistd.h>

#include <stdio.h>
#include <err.h>

char vartmp[40];

void setup() {
  sprintf(vartmp,"/tmp/vartmp.%d",getpid());

  int result = symlink("/var/tmp",vartmp);

  if( result == -1 )
    err(1, "symlink failed");
}

void cleanup() {
  int result = unlink(vartmp);

  if( result == -1 )
    err(1, "unlink failed");
}

int main() {
  char buf[100];

  setup();

  int count = readlink(vartmp, buf, sizeof(buf));
  if (count < 0)
    err(1,"readlink failed");

  cleanup();
  return 0;
}
