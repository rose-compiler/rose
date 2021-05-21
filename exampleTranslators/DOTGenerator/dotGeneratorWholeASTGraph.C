#include "rose.h"
#include "wholeAST.h"
#include <vector>

using namespace Rose::Diagnostics;

int main( int argc, char* argv[] ) {
  ROSE_INITIALIZE;
  std::vector<std::string> argvList(argv, argv + argc);

  // -DSKIP_ROSE_BUILTIN_DECLARATIONS needs to be added
  argvList.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");

  SgProject* project = frontend(argvList);
  ROSE_ASSERT (project != NULL);

  std::string filename = SageInterface::generateProjectName(project);

  if (project->get_verbose() > 0) {
    mlog[INFO] << "Generating AST tree (" << numberOfNodes() << " nodes) in file " << filename << ".dot.\n";
  }

  generateWholeGraphOfAST(filename, new CustomMemoryPoolDOTGeneration::s_Filter_Flags(argvList));

  return 0;
}
