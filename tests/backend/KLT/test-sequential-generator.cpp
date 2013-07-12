
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include "KLT/Core/cg-config.hpp"

#include "KLT/Sequential/generator.hpp"
#include "KLT/Sequential/kernel.hpp"

#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-selector.hpp"

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
  KLT::Core::CG_Config cg_config(new KLT::Core::DataFlow(), new KLT::Core::Dummy_LoopSelector());

  generator.generate(loop_trees, kernels, cg_config);

  std::list<KLT::Core::Kernel *>::const_iterator it_kernel;
  std::vector<SgExpression *>::const_iterator it_expr;
  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    KLT::Sequential::Kernel * kernel = dynamic_cast<KLT::Sequential::Kernel *>(*it_kernel);
    assert(kernel != NULL);

    // TODO
  }
  project->unparse();

  return 0;
}

