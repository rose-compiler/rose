// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it show the desing of a transformation to be used with an AST Restructuring Tool
// built by ROSETTA.


// We need this header file so that we can use the mechanisms within ROSE to build a preprocessor
#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

// We need this header file so that we can build the array statement specific transformations
// include "arrayStatementTransformation.h"

// Main Babel transfromation mechanism
#include "babelTransformation.h"
#include "findMethodCalls.h"

// Required for alternative use of nameQuery library
#include "nameQuery.h"

#include <set>
#include "caselessCompare.h"

// This might be handled better in the future (without using a global variable)
// This is required as a way to hand the SgProject to the prependSourceCode, 
// appendSourceCode, replaceSourceCode, and insertSourceCode functions.
extern SgProject* globalProject;
bool BTN = false;

typedef set<string, CaseLessCompare> FunctionNameSet;

static string
chooseConstructor(SgProject* project)
{
  static const char * const d_constructor_names[] = {
    "init",
    "initialize",
    "prepare",
    "ready",
    "outfit",
    NULL
  };
  static 
    FunctionNameSet functionNames;
  Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  for(Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); ++i) {
    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
    ROSE_ASSERT(functionDeclaration != NULL);
    functionNames.insert(functionDeclaration->get_name().str());
  }
  FunctionNameSet::iterator notFound = functionNames.end();
  for(const char * const *j = d_constructor_names; *j ; ++j) {
    string candidate = *j;
    if (functionNames.find(candidate) == notFound) return candidate;
  }
  return "";
}

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
     SgProject sageProject (argc,argv);

  // Warnings from EDG processing are OK but not errors
  //   ROSE_ASSERT (EDG_FrontEndErrorCode <= 3);

  // cout << "EDG/SAGE Processing DONE! (manipulate AST with ROSE ...) " << endl;

  // DQ (10/17/2004): Added internal testing of AST
     AstTests::runAllTests(&sageProject);

     SIDL_TreeTraversal treeTraversal(chooseConstructor(&sageProject));

     MethodSearchVisitor treeVisitor(&sageProject);
     treeVisitor.traverseInputFiles(&sageProject, preorder);

  // Ignore the return value since we don't need it
     treeTraversal.traverseInputFiles(&sageProject);
     AstDOTGeneration dotgen;
     dotgen.generateInputFiles(&sageProject, AstDOTGeneration::PREORDER);

     cout << "ROSE Processing DONE! (no need to call unparse or backend C++ compiler ...) " << endl;

#if 0
  // DQ (10/17/2004): This seems to be a problem, some sort of infinit loop is generated in the unparser!
  // the test code has been transfrered to another location for internal testing.  for now this should
  // be commented out.  It is not required for Babel Processing. This is now fixed!

  // Generate the final C++ source code from the potentially modified SAGE AST
  // sageProject.set_verbose(true);
     sageProject.unparse();
#endif

  // Later we want to call babel to process the generated SIDL file and generate the imple files which 
  // we will process as part of the next phase of work to completely automate the process.
     int finalCombinedExitStatus = 0;

     printf ("Program Compiled Normally (exit status = %d)! \n\n\n\n",finalCombinedExitStatus);

  // either of these will work similarly
  // exit (finalCombinedExitStatus);
     return finalCombinedExitStatus;
   }











