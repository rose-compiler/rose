#include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

//extern void dot_asisinit (void);
//extern void dot_asisfinal (void);

int main (int argc, char *argv[])
{
  char *prefix = "call_asis_tool_2.main";
  char *target_file = "../test_units/unit_2.adb";
// char *gnat_home   = "/usr/workspace/wsb/charles/bin/adacore/gnat-gpl-2017-x86_64-linux";
  char *gnat_home   = "/home/quinlan1/ROSE/ADA/x86_64-linux/adagpl-2017/gnatgpl/gnat-gpl-2017-x86_64-linux-bin";
  struct List_Node_Struct *head_node;

  printf ("%s:  BEGIN.\n", prefix);
  //dot_asisinit();
  head_node = tool_2_wrapper (target_file, gnat_home);

  if (head_node == 0) {
    printf ("%s:  tool_2_wrapper returned NO elements.\n", prefix);
  } else {
    printf ("%s:  tool_2_wrapper returned %i elements.\n" , prefix, head_node->Next_Count + 1);
  }

  //dot_asisfinal();
  printf ("%s:  END.\n", prefix);
}
