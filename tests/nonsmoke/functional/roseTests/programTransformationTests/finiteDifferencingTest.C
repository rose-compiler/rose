#include "rose.h"
#include "finiteDifferencing.h"
#include "pre.h"

void  FixSgProject(SgProject&);

int main (int argc, char* argv[]) {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
  SgProject* sageProject = frontend(argc,argv);

  simpleIndexFiniteDifferencing(sageProject);

  FixSgProject(*sageProject);

  // partialRedundancyElimination(sageProject);

  // AstPDFGeneration().generateInputFiles(sageProject);

  // Generate the final C++ source code from the potentially modified SAGE AST
  sageProject->unparse();

  // What remains is to run the specified compiler (typically the C++ compiler) using 
  // the generated output file (unparsed and transformed application code) to generate
  // an object file.
  // int finalCombinedExitStatus = sageProject->compileOutput();

  // return exit code from complilation of generated (unparsed) code
  return 0;
}
