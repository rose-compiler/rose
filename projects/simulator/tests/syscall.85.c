#include <unistd.h>

#include <err.h>
#include <errno.h>

void setup() {
  int result = symlink("/var/tmp","/tmp/vartmp");

  if( result == -1 )
    err(1, "symlink failed");
}

void cleanup() {
  int result = unlink("/tmp/vartmp");

  if( result == -1 )
    err(1, "unlink failed");
}

int main() {
  char buf[100];

  setup();

  int count = readlink("/tmp/vartmp", buf, sizeof(buf));
  if (count < 0)
    err(1,"readlink failed");

  cleanup();
  return 0;
}
