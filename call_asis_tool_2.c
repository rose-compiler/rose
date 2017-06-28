#include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

extern void adainit (void);
extern void adafinal (void);

int main (int argc, char *argv[])
{
  adainit();
  char *target_file = "test_units/unit_2.adb";
  struct List_Node_Struct *head_node;
  printf ("c_main: Calling tool_2_wrapper with %s\n" ,target_file);
  head_node = tool_2_wrapper (target_file);
  printf ("c_main: Returned from tool_2_wrapper.\n");

  if (head_node == 0) {
    printf ("c_main: Returned NO elements.\n");
  } else {
    printf ("c_main: Returned %i elements.\n", head_node->next_count + 1);
  }

  // Skipping adafinal for now because it hangs in
  // system__standard_library__adafinal:
  // adafinal();
  // printf ("c_main: adafinal done.\n");
  printf ("c_main: Done.\n");
}
