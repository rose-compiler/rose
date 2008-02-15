// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

// Treating config.h separately from other include files was 
// Brian Gunney's good suggestion.
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "rose.h"
#include "ROSE_Transformations.h"

// Array transform functions
void mainTransform ( SgProject & project );
void mainTransform ( SgFile *file );

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
          ROSE::usage (1);
        }

  // Build the project object which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     int EDG_FrontEndErrorCode = 0;
     SgProject sageProject (argc,argv,EDG_FrontEndErrorCode);

  // Warnings from EDG processing are OK but not errors
     ROSE_ASSERT (EDG_FrontEndErrorCode <= 3);

     cout << "EDG/SAGE Processing DONE! (manipulate with ROSE ...) " << endl;

  // ******************************************************************************
  // Place function to traverse SAGE AST here ...
  // ******************************************************************************
     mainTransform ( sageProject );

     cout << "ROSE Processing DONE! (unparse ...) " << endl;

  // Generate the final C++ source code from the potentially modified SAGE AST
     sageProject.unparse();

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
     int finalCombinedExitStatus = sageProject.compileOutput();

     printf ("Program Compiled Normally (exit status = %d)! \n\n\n\n",finalCombinedExitStatus);

  // either of these will work similarly
  // exit (finalCombinedExitStatus);
     return finalCombinedExitStatus;
   }


// *********************************************************
// *********************************************************
//           FUNCTION CALLED BY EDG FRONT END
// *********************************************************
// *********************************************************

void
mainTransform( SgProject & project )
   {
 //! Loop through all the files in the project and call the mainTransform function for each file
     int i = 0;
     for (i=0; i < project.numberOfFiles(); i++)
        {
          SgFile & file = project.get_file(i);

       // Call the mainTransform function for each file
          mainTransform ( &file );
        }
   }

void
mainTransform( SgFile *file )
   {
  // This is the main transform function for the preprocessor.

#if ROSE_INTERNAL_DEBUG
     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
        {
       // Find out what file we are doing transformations upon
          printf ("In mainTransform: globalSourceFileName = %s \n",ROSE::getFileName(file));
        }
#endif

#if 0
     if (ROSE::outputGrammarTreeFiles == TRUE)
        {
       // Output the program tree
          printf ("## Dumping the program tree (AST) to a file ## \n");

       // Permit the AST for the  header files to be output
       // as well (this makes the output files very large)
          bool writeOutHeaderFiles = ROSE::outputGrammarTreeFilesForHeaderFiles;

       // Output the source code file (as represented by the EDG AST) as a PDF file (with bookmarks)
       // If the mainTransform function is called after the EDG AST is released then it will fail so
       // we need to call it from within a function using the set_sage_transform_function() function.
       // This is now called directly by ROSE/SAGE internally.
       // pdfPrintAbstractSyntaxTreeEDG ( file, writeOutHeaderFiles );

       // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
          pdfPrintAbstractSyntaxTreeSage ( file, writeOutHeaderFiles );

       // printf ("EXting after testing pdf file output of source code! \n");
       // ROSE_ABORT();
        }
       else
        {
          printf ("## Skip dumping the program tree to a file ## \n");
        }
#endif

#if 1
  // In the process of changing this preprocessor to be a default preprocessor used to test the 
  // ROSE infrastructure we are removing all transformations.  Such transformations will be implemented
  // as part of the A++ array preprocessor (in the ExamplePreprocessors/A++Preprocessor

     if (file->get_skip_buildHigherLevelGrammars() == FALSE)
        {
#if 0
       // New method for handling array grammar (build it automatically)
       // Here we use the grammar automatically build from the A++ header file
       // in place of the "hand-built" grammar that was developed previously.

       // NOTE: I forget the current state of this work.
#if 1
       // Now we parse the C++ (modified Sage) grammar into
       // the X_Grammar (call the static parse function)!
          X_Grammar::parse(file);
#else
          printf ("### Skipping parsing into higher level grammar! \n");
#endif
#else
       // New way to arrange transformations (this permits multiple independent passes)
       // Build a transformation object for a specific type of transformation
          ArrayAssignmentUsingGrammar simpleTransform_2 (file);
          ArrayAssignmentUsingTransformationGrammar simpleTransform_3 (file);
          globalArrayAssignmentUsingTransformationGrammar = &simpleTransform_3;

       // temporary code to turn on doem internal print statements
          ROSE_DEBUG = 1;

#if ROSE_INTERNAL_DEBUG
          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
             {
               printf ("######################################################################################### \n");
               printf ("######################################################################################### \n");
               printf ("######################  PASS 2  #####  PASS 2  #####  PASS 2  ########################### \n");
               printf ("######################################################################################### \n");
               printf ("######################################################################################### \n");
	     }
#endif

       // Initial program tree is a NULL pointer
       // The main program should represented at the top by a list of declarations instead of a StatementBlock object.
          ROSE_StatementBlock* statementBlock = simpleTransform_2.pass(NULL);

#if 0
       // DQ (3/29/2001): Comment this out for now while we try to get original array preprocessor working again!
       // CW: apply Array Padding
          ArrayPaddingTransform paddingTransform;
          statementBlock = paddingTransform.pass(statementBlock);
          if(ROSE::verbose==TRUE) paddingTransform.printStatistics(cout);
#endif

#if ROSE_INTERNAL_DEBUG
          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
             {
               printf ("######################################################################################### \n");
               printf ("######################################################################################### \n");
               printf ("######################  PASS 3  #####  PASS 3  #####  PASS 3  ########################### \n");
               printf ("######################################################################################### \n");
               printf ("######################################################################################### \n");
	     }
#endif

       // Now perform the transformations
          if (file->get_skip_transformation() == FALSE)
             {
               ROSE_TransformableStatementBlock* statementBlock2 = simpleTransform_3.pass(statementBlock);
             }

          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
               printf ("\nDONE with grammar based simple array assignment transformation! \n");

#if ROSE_INTERNAL_DEBUG
          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
             {
               printf("\n mainTransform done\n");
             }
#endif
#endif
        }
       else
        {
          if ( ROSE::verbose == TRUE )
               printf ("### ROSE::skip_transformation == TRUE: Skipping all parsing into higher level grammars > C++! \n");
        }
#endif

     if ( ROSE::verbose == TRUE )
          printf ("At END of main_transform (call back function provided to EDG) \n");
   }









