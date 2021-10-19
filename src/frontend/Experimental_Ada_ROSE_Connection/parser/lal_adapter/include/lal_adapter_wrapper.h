#ifndef LAL_ADAPTER_WRAPPER_H
#define LAL_ADAPTER_WRAPPER_H

#include "a_nodes.h"

#ifdef __cplusplus
  extern "C" {
#endif
void lal_adapterinit (void);
void lal_adapterfinal (void);

Nodes_Struct lal_adapter_wrapper
  (char *project_file_name,
   char *input_file_name,
   char *output_dir_name,
   bool process_predefined_units,
   bool process_implementation_units,
   bool debug);
#ifdef __cplusplus
  }
#endif

#endif // ifndef LAL_ADAPTER_WRAPPER_H
