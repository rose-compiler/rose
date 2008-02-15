
// #include <list>
// #include <string>
// #include <algorithm>

#include "rose.h"

using namespace std;

#include "TauMidTraversal.h"

int
main ( int argc, char * argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject *sageProject = frontend(argc,argv);

  // The traversal uses the AST rewrite mechanism which requires the SgProject object to retrive the
  // command line for compilation of the intermeditate files (from strings to AST fragments) before
  // patching them into the application's AST.
     TauMidTraversal myTauTraversal;

  // Part of a temporary fix to explicitly clear all the flags
  // AstClearVisitFlags cleanTreeFlags;

  // Call the traversal starting at the sageProject node of the AST
     cout<<"Now tauifyPreprocessor is going to traverse the ast."<<endl;
  // cleanTreeFlags.traverse(sageProject);
     myTauTraversal.traverse(sageProject,preorder);

  // Generate the final C++ source code from the potentially modified SAGE AST
     sageProject->unparse();

  return backend(sageProject);
}









