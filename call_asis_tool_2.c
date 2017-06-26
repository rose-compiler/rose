#include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

extern void adainit (void);
extern void adafinal (void);

int main (int argc, char *argv[])
{
  adainit();
  char *target_file = "test_units/unit_2.adb";
  struct Element_Struct *elements = 0;
  printf ("c_main: Calling tool_2_wrapper with %s\n" ,target_file);
  tool_2_wrapper (target_file, elements);
  adafinal();
}
