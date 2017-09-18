#include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

int main (int argc, char *argv[])
{
  char *prefix = "call_asis_tool_2.main";
  // argv[1] should be "-f":
  char *target_file = argv[2];
  // argv[3] should be "-g":
  char *gnat_home   = argv[4];
  // argv[o] should be "-o":
  char *output_dir  = argv[6];
  struct List_Node_Struct *head_node;

  printf ("%s:  BEGIN.\n", prefix);

  dot_asisinit();
  head_node = tool_2_wrapper (target_file, gnat_home, output_dir);
  dot_asisfinal();

  if (head_node == 0) {
    printf ("%s:  tool_2_wrapper returned NO elements.\n", prefix);
  } else {
    printf ("%s:  tool_2_wrapper returned %i elements.\n" , prefix, head_node->Next_Count + 1);
  }

  printf ("%s:  END.\n", prefix);
}
