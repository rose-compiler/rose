#ifndef ADAPTER_WRAPPER_H
#define ADAPTER_WRAPPER_H

#include "a_nodes.h"


#ifdef __cplusplus
  extern "C" {
#endif
void asis_adapterinit (void);
void asis_adapterfinal (void);

Nodes_Struct adapter_wrapper
  (char *target_file_in,
   char *gnat_home,
   char *asisArgs,
   char *output_dir);

Nodes_Struct adapter_wrapper_with_flags
  (char *target_file_in,
   char *gnat_home,
   char *asisArgs,
   char *output_dir,
   bool process_predefined_units,
   bool process_implementation_units,
   bool debug);
#ifdef __cplusplus
  }
#endif

#endif // ifndef ADAPTER_WRAPPER_H
