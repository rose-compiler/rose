#include <assert.h>
#include <stdio.h>

int x10_main(int argc, char **argv);

int x10_main(int argc, char **argv)
{
  assert(! "ROSE was not built with support for the X10 language, "
           "see $ROSE/configure --help.");
  return 1;
}

