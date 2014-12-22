// Example ROSE translator
// used for testing ROSE infrastructure
// Liao, 1/15/2009
// similar to identityTranslator, but is built within src 
// This provides a quick way to build src and install it for testing
// without building projects, tests directories
#include "rose.h"
#include "transformationTracking.h"

#include <vector>
#include <string>

using namespace std;

int
main (int argc, char *argv[])
{
  bool id_dump = false;
  vector <string> argvList (argv, argv + argc);

  if (CommandlineProcessing::isOption (argvList,"-rose:unique_id","",true))
  {
    id_dump = true; 
  }

  //SgProject *sageProject = frontend (argc, argv);
  SgProject *sageProject = frontend (argvList);
  AstTests::runAllTests (sageProject);

  if (id_dump)
  {
    // Test unique ID assignment and dot graph generation
    TransformationTracking::registerAstSubtreeIds (sageProject);
    generateWholeGraphOfAST("wholeAST-v1");

    AstDOTGeneration astdotgen;
    astdotgen.generate(sageProject,AstDOTGeneration::TOPDOWNBOTTOMUP, "v1");
    //astdotgen.generate(sageProject,"myfile", AstDOTGeneration::TOPDOWNBOTTOMUP, "v1.");
    //astdotgen.generateWithinFile(sageProject, AstDOTGeneration::TOPDOWNBOTTOMUP);
  }

#if 0
  // simple DOT graph
  AstDOTGeneration astdotgen;
  astdotgen.generateInputFiles(sageProject);
#endif 
  return backend (sageProject);	// only backend error code is reported
}
