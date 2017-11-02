// C++ code calling an Cobol frontend function.

#include <iostream>
#include <assert.h>

#include "rose_config.h"
#include "sage3basic.h"

#include "cobol_support.h"
#include "gnucobpt.h"

int cobol_main(int argc, char** argv, SgSourceFile* sg_source_file)
   {
     int status = 0;

  // Rasmussen (10/13/2017):
  // TODO
  //   - make sure argc and argv are correct
  //   - may have to create local variables from SgSourceFile information
  //   - how will the converter class get access to this?
  //      - perhaps create class and set member variable before calling parser
  //

  // Call the main entry function for access to GnuCOBOL parse-tree information
  //   - this function subsequently calls cobpt_convert_cb_program() for each file parsed
     status = gnucobol_parsetree_main (argc, argv);

     std::cout << "\ncobol_main: IN DEVELOPMENT...................................\n\n";

     assert (status != 0);

     return status;
   }



