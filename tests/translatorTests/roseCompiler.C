// Example ROSE translator
// used for testing ROSE infrastructure
// Liao, 1/15/2009
// similar to identityTranslator, but is built within src 
// This provides a quick way to build src and install it for testing
// without building projects, tests directories
#include "rose.h"
#include "transformationTracking.h"

int
main (int argc, char *argv[])
{
  SgProject *sageProject = frontend (argc, argv);
  AstTests::runAllTests (sageProject);

#if 0
// Test unique ID assignment and dot graph generation
  TransformationTracking::registerAstSubtreeIds (sageProject);
  generateWholeGraphOfAST("wholeAST");

// simple DOT graph
  AstDOTGeneration astdotgen;
  astdotgen.generateInputFiles(sageProject);
#endif 
  return backend (sageProject);	// only backend error code is reported
}
