#include "rose.h"

int main( int argc, char* argv[] ) {
  ROSE_INITIALIZE;
  std::vector<std::string> argvList(argv, argv + argc);

  SgProject* project = frontend(argvList);
  ROSE_ASSERT (project != NULL);

  std::string filename = SageInterface::generateProjectName(project);

  if (project->get_verbose() > 0) {
    printf ("Generating AST tree (%d nodes) in file %s.dot.\n", numberOfNodes(), filename.c_str());
  }

  AstDOTGeneration astdotgen;
//TransformationTracking::registerAstSubtreeIds(project);
  astdotgen.generateInputFiles(project);

  return 0;
}
