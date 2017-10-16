#ifndef TOOL_2_WRAPPER_H
#define TOOL_2_WRAPPER_H

#include "a_nodes.h"

extern void dot_asisinit (void);
extern void dot_asisfinal (void);

extern Nodes_Struct tool_2_wrapper
  (char *target_file_in,
   char *gnat_home,
   char *output_dir);

#endif // ifndef TOOL_2_WRAPPER_H
