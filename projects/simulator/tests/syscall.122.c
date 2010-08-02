#include <sys/utsname.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  struct utsname name;
  uname(&name);
  // TODO: Assert on struct values
  exit(0);
}
