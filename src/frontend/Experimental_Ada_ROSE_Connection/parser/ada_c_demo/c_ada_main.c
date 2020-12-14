#include <stdio.h>
#include "ada_init_final.h"
#include "ada_code.h"

int main (int argc, char *argv[])
{
  adainit ();
  printf ("c_ada_main: Calling Ada_Proc\n");
  Ada_Proc ();
  printf ("c_ada_main: Returned from Ada_Proc\n");
  adafinal ();
}
