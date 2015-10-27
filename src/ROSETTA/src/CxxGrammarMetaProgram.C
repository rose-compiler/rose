
// include definitions of grammars, terminals, and non-terminals
// (objects within ROSETTA)
#include "grammar.h"

#if 0
// There are a bunch of global objects used as tags for enhanced type checking.
// They are all defined here.

const ConstructParamEnum NO_CONSTRUCTOR_PARAMETER(TAG_NO_CONSTRUCTOR_PARAMETER);
const ConstructParamEnum CONSTRUCTOR_PARAMETER(TAG_CONSTRUCTOR_PARAMETER);

const BuildAccessEnum NO_ACCESS_FUNCTIONS(TAG_NO_ACCESS_FUNCTIONS);
const BuildAccessEnum BUILD_ACCESS_FUNCTIONS(TAG_BUILD_ACCESS_FUNCTIONS);
const BuildAccessEnum BUILD_FLAG_ACCESS_FUNCTIONS(TAG_BUILD_FLAG_ACCESS_FUNCTIONS);
const BuildAccessEnum BUILD_LIST_ACCESS_FUNCTIONS(TAG_BUILD_LIST_ACCESS_FUNCTIONS);

const CopyConfigEnum NO_COPY_DATA(TAG_NO_COPY_DATA);
const CopyConfigEnum COPY_DATA(TAG_COPY_DATA);
const CopyConfigEnum CLONE_PTR(TAG_CLONE_PTR);
const CopyConfigEnum CLONE_TREE(TAG_CLONE_TREE);

#endif

const TraversalFlag DEF_TRAVERSAL(true); // default traversal
const TraversalFlag NO_TRAVERSAL(false); // no traversal

// AJ (10/26/2004)
const DeleteFlag NO_DELETE(false);
// DQ (12/3/2004): Avoid calling delete
const DeleteFlag DEF_DELETE(true);
// DeleteFlag DEF_DELETE(false);

// This program demonstrates the Meta-Program Level where the details
// of a preprocessor are specified.  In this case this program
// represents the MetaProgram to build a preprocessor for the A++/P++
// array class.

// Currently the grammars are defined, the execution of this program
// (a C++ program) generates the source code for defining the grammars
// to be used in building a preprocessor.  So this example does not
// yet build all the code required to build a preprocessor (the rest
// is specified in the ROSE/src directory structure).

using namespace std;

int
main(int argc, char * argv[])
   {
  // Main Function for ROSE Preprocessor
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

     printf ("***************************************************************************************** \n");
     printf ("Build the C++ grammar (essentially an automated generation of a modified version of SAGE) \n");
     printf ("***************************************************************************************** \n");

  // First build the C++ grammar

     std::string target_directory = ".";
     if(argc == 2)
       target_directory = std::string(argv[1]);


  // For base level grammar use prefix "Sg" to be compatable with SAGE
     Grammar sageGrammar ( /* name of grammar */ "Cxx_Grammar", 
                           /* Prefix to names */ "Sg", 
                           /* Parent Grammar  */ "ROSE_BaseGrammar",
                           /* No parent Grammar */ NULL,
                           target_directory 
                           );

  // Build the header files and source files representing the
  // grammar's implementation
     sageGrammar.buildCode();

  // Support for output of constructors as part of generated documentation
     string documentedConstructorPrototypes = sageGrammar.staticContructorPrototypeString;

     printf ("documentedConstructorPrototypes = %s \n",documentedConstructorPrototypes.c_str());

     printf ("Program Terminated Normally! \n");
     return 0;
}















