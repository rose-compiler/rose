
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include "KLT/Core/cg-config.hpp"

#include "KLT/Sequential/generator.hpp"

#include <cassert>

#include "sage3basic.h"

int main(int argc, char ** argv) {

  SgProject * project = new SgProject::SgProject();
  { // Add default command line to an empty project
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }

  assert(argc == 3);

  KLT::Core::LoopTrees loop_trees;
  
  loop_trees.read(argv[1]);

  KLT::Sequential::Generator generator(project, argv[2]);
  std::list<KLT::Core::Kernel *> kernels;
  KLT::Core::CG_Config cg_config;

  generator.generate(loop_trees, kernels, cg_config);

  // TODO

  project->unparse();

  return 0;
}

