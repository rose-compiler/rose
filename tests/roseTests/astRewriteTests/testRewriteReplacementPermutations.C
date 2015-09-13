// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include "testNewRewrite.h"
// #include "AstPreorderTraversal.h"

int
main( int argc, char * argv[] )
   {
  // This test code tests the AST rewrite mechanism using an arbitrary targetCodeString string and
  // an arbitrary newCodeString string.  The targetCodeString is searched for in the AST (the
  // recognition is dependent upon the unparsed AST matching the target string, so this would be
  // problematic if this were not a test code).  The new code (newCodeString) is then inserted into
  // the AST at the relative position specified (scope and position in scope). All possible
  // positions are tested by this test code.

     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!
     if (argc == 1)
        {
       // Print usage and exit with exit status == 1
          rose::usage (1);
        }

  // Build the project object which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject* project= frontend (argc,argv); 

  // Warnings from EDG processing are OK but not errors
     ROSE_ASSERT (project->get_frontendErrorCode() <= 3);

     cout << "EDG/SAGE Processing DONE! (manipulate with ROSE ...) " << endl;

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     AstPDFGeneration pdftest;
     pdftest.generateInputFiles(project);

  // MyTraversal myTraversal(*sageProject);
     MyTraversal myTraversal;

  // Build the inherited attribute
  // MyInheritedAttributeType inheritedAttribute (sageProject);
     MyInheritedAttributeType inheritedAttribute;

  // Part of a temporary fix to explicitly clear all the flags
//   AstPreorderTraversal cleanTreeFlags;
//   AstClearVisitFlags cleanTreeFlags;

  // Initialize with a statement we expect to find in the original application
     string previousNewCodeString = "int x;";

#if 1
  // Now remove some of the newly added strings from the AST through replacement
     int i;
     for (i = HighLevelInterfaceNodeCollection::SurroundingScope;
          i < HighLevelInterfaceNodeCollection::LAST_SCOPE_TAG;
          i++)
        {
       // First insert the new string
          inheritedAttribute.removeTargetCode = false;
          inheritedAttribute.scope            = HighLevelInterfaceNodeCollection::ScopeIdentifierEnum(i);
          inheritedAttribute.location         = HighLevelInterfaceNodeCollection::ReplaceCurrentPosition;

       // Control the creation of strings in a new scope (true) or not (false)
          inheritedAttribute.buildInNewScope = false;

       // Search for this string in the AST
          inheritedAttribute.targetCodeString = previousNewCodeString;

       // Template used to construct new strings based on their scope and location in scope
          string newCodeTemplateString = "int replacementTarget_$SCOPE_NAME;";

       // Build and edit the new code string
          string tempString = newCodeTemplateString;
          tempString = StringUtility::copyEdit (
                            tempString,"$SCOPE_NAME",
                            HighLevelInterfaceNodeCollection::getRelativeScopeString(HighLevelRewrite::ScopeIdentifierEnum(i)));

       // printf ("tempString = %s \n",tempString.c_str());

          inheritedAttribute.newCodeString = tempString;

       // Save the new code string so that we can search for it and replace it next time
          previousNewCodeString            = tempString;
       // printf ("previousNewCodeString = %s \n",previousNewCodeString.c_str());

       // printf ("inheritedAttribute.targetCodeString = %s \n",inheritedAttribute.targetCodeString.c_str());
       // printf ("inheritedAttribute.newCodeString    = %s \n",inheritedAttribute.newCodeString.c_str());

          testPermutation (project,inheritedAttribute);

       // Clear the flags after modifying the tree processing the tree
//        printf ("In testRewriteReplacementPermutations (test code): clear internal visit flags \n");
//        cleanTreeFlags.traverse(project);

          string projectString = project->unparseToString().c_str();
          string newCodeString = inheritedAttribute.newCodeString;

#if 0
          printf ("################################################################### \n");
          printf ("newCodeString = -->%s<-- \n",newCodeString.c_str());
          printf ("project->unparseToString() = \n%s\n",projectString.c_str());
          printf ("################################################################### \n");
#endif

          bool replacementVerified = StringUtility::isContainedIn(projectString,newCodeString);

       // Transformations replaced in the preamble don't appear in the final AST 
       // (only in the intermediate file generated to compile the transformation 
       // strings and generate the AST fragements that are edited into place in 
       // the application's AST.
          ROSE_ASSERT ((replacementVerified == true) || (i == HighLevelInterfaceNodeCollection::Preamble));
        }
#else
  // First insert the new string
     inheritedAttribute.removeTargetCode = false;
  // inheritedAttribute.scope            = HighLevelInterfaceNodeCollection::SurroundingScope;
     inheritedAttribute.scope            = HighLevelInterfaceNodeCollection::GlobalScope;
     inheritedAttribute.location         = HighLevelInterfaceNodeCollection::ReplaceCurrentPosition;

  // Search for this string in the AST
     inheritedAttribute.targetCodeString = "int x;";

  // default setting for debugging AST rewrite mechanism
     inheritedAttribute.newCodeString    = "int y;";

  // Control the creation of strings in a new scope (true) or not (false)
     inheritedAttribute.buildInNewScope = false;

     testPermutation (project,inheritedAttribute);
#endif

#if 0
     printf ("Exiting after call to testPermutation ... \n");
     ROSE_ABORT();
#endif

  // Generate the final C++ source code from the potentially modified SAGE AST
     project->unparse();

     cout << "Generation of final source code (unparsing) DONE! (compile ...) " << endl;

  // What remains is to run the specified compiler (typically the C++ compiler) using 
  // the generated output file (unparsed and transformed application code) to generate
  // an object file.
     int finalCombinedExitStatus = project->compileOutput();
     printf ("Program Compiled Normally (exit status = %d)! \n\n\n\n",finalCombinedExitStatus);
     return finalCombinedExitStatus;
   }

