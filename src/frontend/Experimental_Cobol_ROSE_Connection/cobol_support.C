// C++ code calling an Cobol frontend function.

#include <rose_paths.h>

#include <assert.h>

#include "cobol_support.h"

// Rasmussen (9/28/2017): First two files from GNUCobol
#include "cobc.h"
#include "tree.h"
#include "rose_cobol.h"

// using namespace std;

#ifdef BUILD_EXECUTABLE
int main(int argc, char** argv)
#else
int cobol_main(int argc, char** argv)
#endif
   {
     int status = 0;

  // Rasmussen (9/28/2017): 
  // The main remaining issues are:
  //   - create a compiler wrapper that runs the compiler (primarily to call codegen.c)
  //      - this should create a pointer to the cb_program struct
  //      - this will then call call the modified codegen function passing the cb_program* struct
  //   - create a modified version of codegen.c that create the local_cache struce
  //      - mostly already done
     const struct cb_program* prog = NULL;
     struct rose_base_list* local_cache = NULL;

     status = rose_convert_cb_program(const struct cb_program* prog, struct rose_base_list* local_cache);

     assert (status != 0);

     return status;
   }



