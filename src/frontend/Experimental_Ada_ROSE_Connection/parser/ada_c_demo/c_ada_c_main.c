#include <stdio.h>
#include "ada_init_final.h"
#include "ada_code.h"

int main (int argc, char *argv[])
{
  adainit ();
  printf ("c_main: Calling Ada_C_Caller\n");
  Ada_C_Caller ();
  printf ("c_main: Returned from Ada_C_Caller\n");
  adafinal ();
}
