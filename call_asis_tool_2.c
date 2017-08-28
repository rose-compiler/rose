#include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

extern void dot_asisinit (void);
extern void dot_asisfinal (void);

int main (int argc, char *argv[])
{
  char *prefix = "call_asis_tool_2.main";
  char *target_file = "/g/g17/charles/code/ROSE/dot_asis/test_units/unit_2.adb";
  struct List_Node_Struct *head_node;

  printf ("%s:  BEGIN.\n", prefix);
  dot_asisinit();
  head_node = tool_2_wrapper (target_file);

  if (head_node == 0) {
    printf ("%s:  tool_2_wrapper returned NO elements.\n", prefix);
  } else {
    printf ("%s:  tool_2_wrapper returned %i elements.\n" , prefix, head_node->Next_Count + 1);
  }

  dot_asisfinal();
  printf ("%s:  END.\n", prefix);
}
