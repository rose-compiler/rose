#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



int
main ( int argc, char * argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     int frontEndErrorCode = 0;
     SgProject* sageProject = new SgProject(argc,argv,frontEndErrorCode);

  // Existance of warnings are reported as a return code (from the front-end internally)
     assert(frontEndErrorCode <3);

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     AstPDFGeneration pdftest;
     pdftest.generateInputFiles(sageProject);

     return 0;
   }









