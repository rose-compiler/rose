#include "rose.h"

#include "astStructRecognition.h"

using namespace std;

// This is used to generate unique names of global variables.
// An alternative is to use the address, but that is not as 
// elegant (though it contains more information).
// int globalVariableCounter = 0;

int
main ( int argc, char* argv[] )
   {
  // Build the abstract syntax tree
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 1
     printf ("Generate the dot output of the SAGE III AST \n");
     generateDOT ( *project );
     printf ("DONE: Generate the dot output of the SAGE III AST \n");
     generateAstGraph(project, 4000);
     printf ("DONE: generateAstGraph() \n");
#endif

  // Step 1: Add an analysis attribute to represent the global scope (on the SgAsmInterpretation object).
     addScopeAnalysisAttributes(project);

  // Verify that the global variable is initialized properly.
     ROSE_ASSERT(globalScopeAttribute != NULL);

  // Step 2:
  // Note that I think we should detect the virtual function tables and THEN process the remaining functions
  // not processed as virtual functions.
  // New approach to the detection of specific instructions (will be extended to support regular expressions).
     detectVirtualTableLoad(project);

  // Step 3: Add analysis attributes to each function in the binary
     addFunctionAnalysisAttributes(project);

  // Generate the section list boundaries so that we can classify memory references.
  // vector<SgAsmGenericSection*> sectionList = generateSectionList(project);

     detectVariableInitialization(project);

  // printf ("Exiting after detectStructureFieldInitialization(project); \n");
  // ROSE_ASSERT(false);

  // This program only does analysis, so it need not call the backend to generate code.
  // return 0;

     printf ("Building the source code for the C++ class structure. \n");
     buildSourceCode(project);

     printf ("Generating the source code, compiling it, and regenerating the original binary executable. \n");
     return backend(project);
   }



