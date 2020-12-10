#ifndef TOOL_2_WRAPPER_H
#define TOOL_2_WRAPPER_H

#include "a_nodes.h"


#ifdef __cplusplus
  extern "C" {
#endif
void dot_asisinit (void);
void dot_asisfinal (void);

Nodes_Struct tool_2_wrapper
  (char *target_file_in,
   char *gnat_home,
   char *output_dir);

Nodes_Struct tool_2_wrapper_with_flags
  (char *target_file_in,
   char *gnat_home,
   char *output_dir,
   bool process_predefined_units,
   bool process_implementation_units,
   bool debug);
#ifdef __cplusplus
  }
#endif

#endif // ifndef TOOL_2_WRAPPER_H
