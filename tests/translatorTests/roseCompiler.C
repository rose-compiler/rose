// Example ROSE translator
// used for testing ROSE infrastructure
// Liao, 1/15/2009
// similar to identityTranslator, but is built within src 
// This provides a quick way to build src and install it for testing
// without building projects, tests directories
#include "rose.h"

int
main (int argc, char *argv[])
{
  SgProject *sageProject = frontend (argc, argv);
  AstTests::runAllTests (sageProject);
  return backend (sageProject);	// only backend error code is reported
}
