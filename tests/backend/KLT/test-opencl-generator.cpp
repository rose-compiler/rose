
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include "KLT/OpenCL/cg-config.hpp"

#include "KLT/OpenCL/generator.hpp"

#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-selector.hpp"
#include "KLT/OpenCL/work-size-shaper.hpp"

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

  KLT::OpenCL::Generator generator(project, argv[2]);
  std::list<KLT::Core::Kernel *> kernels;
  KLT::OpenCL::CG_Config cg_config(new KLT::Core::DataFlow(), new KLT::Core::Dummy_LoopSelector(), new KLT::OpenCL::Dummy_WorkSizeShaper());

  generator.generate(loop_trees, kernels, cg_config);

  // TODO

  project->unparse();

  return 0;
}

