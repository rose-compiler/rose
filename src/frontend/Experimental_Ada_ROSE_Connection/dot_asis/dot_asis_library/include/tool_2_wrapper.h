#ifndef TOOL_2_WRAPPER_H
#define TOOL_2_WRAPPER_H

#include "a_nodes.h"

#if __cplusplus
#define EXTERN
extern "C"
{
#else
#define EXTERN extern
#endif

EXTERN void dot_asisinit (void);
EXTERN void dot_asisfinal (void);

EXTERN
Nodes_Struct tool_2_wrapper
  (char *target_file_in,
   char *gnat_home,
   char *output_dir);

#if __cplusplus
}
#endif

#endif // ifndef TOOL_2_WRAPPER_H
