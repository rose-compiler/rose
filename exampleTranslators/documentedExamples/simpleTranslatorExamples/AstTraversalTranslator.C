#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class MyInheritedAttribute {};

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class MySynthesizedAttribute {};

class MyTraversal : public SgTopDownBottomUpProcessing<MyInheritedAttribute,MySynthesizedAttribute>
   {
     public:
          MyTraversal ( SgProject & project ) {};

       // Functions required by the tree traversal mechanism
          MyInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             MyInheritedAttribute inheritedAttribute )
             { return MyInheritedAttribute(); }

          MySynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             MyInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList )
             { return MySynthesizedAttribute(); }
   };

int
main( int argc, char * argv[] )
   {
  // This test code demonstrates the differences between an ordinary 
  // SgTopDownBottomUpProcessing traversal and a traversal using the 
  // AST Rewrite Mechanism.

     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!
     if (argc == 1)
        {
       // Print usage and exit with exit status == 1
          ROSE::usage (1);
        }

  // Build the project object which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     int EDG_FrontEndErrorCode = 0;
     SgProject* sageProject = new SgProject(argc,argv,EDG_FrontEndErrorCode); 

  // Warnings from EDG processing are OK but not errors
     ROSE_ASSERT (EDG_FrontEndErrorCode <= 3);

     cout << "EDG/SAGE Processing DONE! (manipulate with ROSE ...) " << endl;

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     AstPDFGeneration pdftest;
     pdftest.generateInputFiles(sageProject);

  // Build the inherited attribute
     MyInheritedAttribute inheritedAttribute;

  // The traversal uses the AST rewrite mechanism which requires the SgProject object to retrive the
  // command line for compilation of the intermeditate files (from strings to AST fragments) before
  // patching them into the application's AST.
     MyTraversal myTraversal(*sageProject);

  // Call the traversal starting at the sageProject node of the AST
     myTraversal.traverseInputFiles(sageProject,inheritedAttribute);

  // Generate the final C++ source code from the potentially modified SAGE AST
     sageProject->unparse();

     cout << "Generation of final source code (unparsing) DONE! (compile ...) " << endl;

  // What remains is to run the specified compiler (typically the C++ compiler) using 
  // the generated output file (unparsed and transformed application code) to generate
  // an object file.
     int finalCombinedExitStatus = sageProject->compileOutput();
     printf ("Program Compiled Normally (exit status = %d)! \n\n\n\n",finalCombinedExitStatus);
     return finalCombinedExitStatus;
   }
















