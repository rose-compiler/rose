#include <stdio.h>
#include "a_nodes.h"
#include "adapter_wrapper.h"

int main (int argc, char *argv[])
{
  char *prefix = "run_parser_adapter.main";
  // argv[1] should be "-f":
  char *target_file = argv[2];
  // argv[3] should be "-g"
  // asisArgs should specify the include path with prefix "-I"
  char *asisArgs = argv[3];
  char *gnat_home   = argv[4];
  // argv[o] should be "-o":
  char *output_dir  = argv[6];
  bool process_predefined_units = false;
  bool process_implementation_units = false;
  bool debug = false;
  Nodes_Struct nodes;

  printf ("%s:  BEGIN.\n", prefix);

  asis_adapterinit();
  nodes = adapter_wrapper_with_flags 
    (target_file,
    gnat_home, 
    asisArgs;
    output_dir,
    process_predefined_units,
    process_implementation_units,
    debug);
  asis_adapterfinal();

  if (nodes.Units == 0) {
    printf ("%s:  adapter_wrapper returned NO Units.\n", prefix);
  } else {
    printf ("%s:  adapter_wrapper returned %i Units.\n" , prefix,
        nodes.Units->Next_Count + 1);
  }
  if (nodes.Elements == 0) {
    printf ("%s:  adapter_wrapper returned NO Elements.\n", prefix);
  } else {
    printf ("%s:  adapter_wrapper returned %i Elements.\n" , prefix,
        nodes.Elements->Next_Count + 1);
  }

  printf ("%s:  END.\n", prefix);
}
