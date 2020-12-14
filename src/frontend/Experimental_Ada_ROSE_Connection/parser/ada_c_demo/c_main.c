#include <stdio.h>
#include "c_code.h"

int main (int argc, char **argv) {
  printf ("c_main: Calling c_func\n");
  c_func ();
  printf ("c_main: Returned from c_func\n");
}
