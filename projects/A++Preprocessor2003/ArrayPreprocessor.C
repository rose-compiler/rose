// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it show the desing of a transformation to be used with an AST Restructuring Tool
// built by ROSETTA.

// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

// Main header file for array preprocessor
#include "arrayPreprocessor.h"

// This might be handled better in the future (without using a global variable)
// This is required as a way to hand the SgProject to the prependSourceCode, 
// appendSourceCode, replaceSourceCode, and insertSourceCode functions.
extern SgProject* globalProject;

int
main ( int argc, char * argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example tests the ROSE infrastructure

     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

  // Declare usage (if incorrect number of inputs):
     if (argc == 1)
        {
       // Print usage and exit with exit status == 1
          rose::usage (1);
        }

  // Build the project object which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     int EDG_FrontEndErrorCode = 0;
     SgProject* sageProject = new SgProject(argc,argv,EDG_FrontEndErrorCode);

  // Warnings from EDG processing are OK but not errors
     ROSE_ASSERT (EDG_FrontEndErrorCode <= 3);

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     AstPDFGeneration pdftest;
     pdftest.generateInputFiles(sageProject);

  // cout << "EDG/SAGE Processing DONE! (manipulate AST with ROSE ...) " << endl;

     ArrayStatementTraversal treeTraversal(*sageProject);

#if 1
  // Ignore the return value since we don't need it
     ProgramTransformationInheritedAttributeType inheritedAttribute(sageProject);
     treeTraversal.traverseInputFiles(sageProject,inheritedAttribute);
#else
  // Generate a simple traversal so that we can generate a dot file (for debugging)!
     list<string> typeNameStringList = NameQuery::getTypeNamesQuery ( sageProject );
#endif

     cout << "ROSE Processing DONE! (now calling unparse ...) " << endl;

  // Generate the final C++ source code from the potentially modified SAGE AST
  // sageProject->set_verbose(TRUE);
     sageProject->unparse();

     cout << "Generation of final source code (unparsing) DONE! (compile ...) " << endl;

  // ******************************************************************************
  // At this point in the control flow we have returned from the processing via the 
  // EDG frontend (or skipped it if that option was specified).
  // The following has been done or explicitly skipped if such options were specified 
  // on the commandline:
  //    1) The application program has been parsed
  //    2) All AST's have been build (one for each grammar)
  //    3) The transformations have been edited into the C++ AST
  //    4) The C++ AST has been unparsed to form the final output file (all code has 
  //       been generated into a different filename "rose_<original file name>.C")
  // ******************************************************************************

  // What remains is to run the specified compiler (typically the C++ compiler) using 
  // the generated output file (unparsed and transformed application code) to generate
  // an object file.
     int finalCombinedExitStatus = sageProject->compileOutput();

     printf ("Program Compiled Normally (exit status = %d)! \n\n\n\n",finalCombinedExitStatus);

  // either of these will work similarly
  // exit (finalCombinedExitStatus);
     return finalCombinedExitStatus;
   }











