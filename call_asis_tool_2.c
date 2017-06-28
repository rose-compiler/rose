#include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

extern void adainit (void);
extern void adafinal (void);

int main (int argc, char *argv[])
{
  adainit();
  char *target_file = "test_units/unit_2.adb";
  struct Element_Struct *last_element;
  printf ("c_main: Calling tool_2_wrapper with %s\n" ,target_file);
  last_element = tool_2_wrapper (target_file);
  printf ("c_main: Returned from tool_2_wrapper.\n");

  if (last_element == 0) {
    printf ("c_main: Returned NO elements.\n");
  } else {
    printf ("c_main: Returned %i elements.\n", last_element->next_count + 1);
  }

  // Skipping adafinal for now because it hangs in
  // system__standard_library__adafinal:
  // adafinal();
  // printf ("c_main: adafinal done.\n");
  printf ("c_main: Done.\n");
}
