// C++ code calling an Cobol frontend function.

#include <rose_paths.h>

#include <assert.h>

#include "cobol_support.h"

#define ROSE_USING_GNUCOBOL 0

#if ROSE_USING_GNUCOBOL
// DQ (9/29/2017): We need a way to know when ROSE has been configured with the location of the GNUCobol compiler.
// We need something like "ROSE_USING_GNUCOBOL" to be setup.

// Rasmussen (9/28/2017): First two files from GNUCobol
#include "cobc.h"
#include "tree.h"
#include "rose_cobol.h"
#endif

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

#if ROSE_USING_GNUCOBOL
  // DQ (9/29/2017): We need a way to know when ROSE has been configured with the location of the GNUCobol compiler. 
     status = rose_convert_cb_program(const struct cb_program* prog, struct rose_base_list* local_cache);
#endif

     assert (status != 0);

     return status;
   }



