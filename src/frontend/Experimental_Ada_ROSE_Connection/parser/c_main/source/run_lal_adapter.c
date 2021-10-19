#include <stdio.h>
#include "a_nodes.h"
#include "lal_adapter_wrapper.h"

// Parameters:
//  Project file name
//  Input file name
//  Output directory
int main (int argc, char *argv[])
{
  char *prefix = "run_lal_adapter.main";
  char *project_file_name = argv[1];
  char *input_file_name = argv[2];
  char *output_dir_name  = argv[3];
  bool process_predefined_units = false;
  bool process_implementation_units = false;
  bool debug = true;
  Nodes_Struct nodes;

  printf ("%s:  BEGIN.\n", prefix);

  lal_adapterinit();
  nodes = lal_adapter_wrapper
    (project_file_name,
    input_file_name,
    output_dir_name,
    process_predefined_units,
    process_implementation_units,
    debug);
  lal_adapterfinal();

  if (nodes.Units == 0) {
    printf ("%s:  lal_adapter_wrapper returned NO Units.\n", prefix);
  } else {
    printf ("%s:  lal_adapter_wrapper returned %i Units.\n" , prefix,
        nodes.Units->Next_Count + 1);
  }
  if (nodes.Elements == 0) {
    printf ("%s:  lal_adapter_wrapper returned NO Elements.\n", prefix);
  } else {
    printf ("%s:  lal_adapter_wrapper returned %i Elements.\n" , prefix,
        nodes.Elements->Next_Count + 1);
  }

  printf ("%s:  END.\n", prefix);
}
