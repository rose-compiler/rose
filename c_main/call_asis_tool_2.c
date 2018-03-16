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
  Nodes_Struct nodes;

  printf ("%s:  BEGIN.\n", prefix);

  dot_asisinit();
  nodes = tool_2_wrapper (target_file, gnat_home, output_dir);
  dot_asisfinal();

  if (nodes.Units == 0) {
    printf ("%s:  tool_2_wrapper returned NO Units.\n", prefix);
  } else {
    printf ("%s:  tool_2_wrapper returned %i Units.\n" , prefix,
	    nodes.Units->Next_Count + 1);
  }
  if (nodes.Elements == 0) {
    printf ("%s:  tool_2_wrapper returned NO Elements.\n", prefix);
  } else {
    printf ("%s:  tool_2_wrapper returned %i Elements.\n" , prefix,
	    nodes.Elements->Next_Count + 1);
  }

  printf ("%s:  END.\n", prefix);
}
