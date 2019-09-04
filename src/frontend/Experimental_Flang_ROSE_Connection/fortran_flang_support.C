#include "sage3basic.h"
#include "rose_config.h"

#include "fortran_flang_support.h"
#include "flang-external-builder-main.h"

using namespace Rose;

#include <iostream>
using std::string;
using std::cout;
using std::endl;

#define DEBUG_EXPERIMENTAL_FORTRAN 0
#define DOT_FILE_GENERATION 0

#if DOT_FILE_GENERATION
#   include "wholeAST_API.h"
#endif

int
experimental_fortran_main(int argc, char* argv[], SgSourceFile* sg_source_file)
   {
      // Perhaps do the following:
      //   1. Run f18 from the command line
      //   2. This function is a call back

      // Or:
      //   1. Call a function in the f18 frontend passing (argc, argv)
      //   2. The f18 function then calls this function as a call back

      cout << "\n";
      cout << "-->  experimental_fortran_flang_main: will call flang parser \n";

      int status = 0;

      status = flang_external_builder_main(argc, argv);

      cout << "\n";

      return status;
  }
