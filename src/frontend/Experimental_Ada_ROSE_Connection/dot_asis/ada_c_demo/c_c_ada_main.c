#include <stdio.h>
#include "ada_init_final.h"
#include "c_code.h"

int main (int argc, char **argv) {
  adainit ();
  printf ("c_c_ada_main: Calling c_ada_caller\n");
  c_ada_caller ();
  printf ("c_c_ada_main: Returned from c_ada_caller\n");
  adafinal ();
}
