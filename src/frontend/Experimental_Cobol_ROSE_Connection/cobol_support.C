// cobol_main function to initiate parsing
// ---------------------------------------

#include <iostream>
#include <assert.h>

#include "rose_config.h"
#include "sage3basic.h"

#include "cobol_support.h"
#include "gnucobpt.h"
#include "rose_convert_cobol.h"
#include "CobolGraph.h"

int cobol_main(int argc, char** argv, SgSourceFile* sg_source_file)
   {
     char* cobol_argv[2];
     int cobol_argc;
     int status = 1;

     assert(sg_source_file != NULL);

     std::string filenameWithPath = sg_source_file->getFileName();
     std::string filename = Rose::StringUtility::stripPathFromFileName(filenameWithPath);

  // Initialize argc and argv variables for call to Cobol parser
     cobol_argc = 2;
     cobol_argv[0] = strdup("ROSE::cobol_main");
     cobol_argv[1] = strdup(filename.c_str());

  // Call the main entry function for access to GnuCOBOL parse-tree information
  //   - this function subsequently calls cobpt_convert_cb_program() for each file parsed

     status = gnucobol_parsetree_main (cobol_argc, cobol_argv);

     std::cout << "\ncobol_main: IN DEVELOPMENT (called gnucobol_parsetree_main) ........................\n\n";

     assert (status == 0);

     free(cobol_argv[0]);
     free(cobol_argv[1]);

  // Output digraph file
     CobolSupport::CobolGraph cobol_graph(filename);

     cobol_graph.graph(cobpt_program, cobpt_local_cache, NULL);

     return status;
   }



