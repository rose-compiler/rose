
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include "KLT/Sequential/kernel.hpp"

#include "KLT/Core/generator.hpp"
#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-mapper.hpp"
#include "KLT/Core/iteration-mapper.hpp"

#include "KLT/Sequential/mfb-klt.hpp"

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

  ::MultiFileBuilder::KLT_Driver driver(project);
  KLT::Core::Generator<KLT::Sequential::Kernel, ::MultiFileBuilder::KLT_Driver> generator(driver, argv[2]);

  std::set<std::list<KLT::Sequential::Kernel *> > kernel_lists;
  KLT::Core::CG_Config<KLT::Sequential::Kernel> cg_config(
    new KLT::Core::LoopMapper<KLT::Sequential::Kernel>(),
    new KLT::Core::IterationMapper<KLT::Sequential::Kernel>(),
    new KLT::Core::DataFlow<KLT::Sequential::Kernel>()
  );
  generator.generate(loop_trees, kernel_lists, cg_config);

/*
  std::list<KLT::Core::Kernel *>::const_iterator it_kernel;
  std::vector<SgExpression *>::const_iterator it_expr;
  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    KLT::Sequential::Kernel * kernel = dynamic_cast<KLT::Sequential::Kernel *>(*it_kernel);
    assert(kernel != NULL);

    // TODO
  }
*/

  project->unparse();

  return 0;
}

