
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include "KLT/Sequential/kernel.hpp"

#include "KLT/Core/generator.hpp"
#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-mapper.hpp"
#include "KLT/Core/iteration-mapper.hpp"

#include "KLT/Sequential/mfb-klt.hpp"

#include "build-main.hpp"

#include "sage3basic.h"

#include <cassert>

int main(int argc, char ** argv) {
  SgProject * project = new SgProject::SgProject();
  { // Add default command line to an empty project
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }

  assert(argc == 3);

  KLT::Core::LoopTrees loop_trees;
  std::list<SgVariableSymbol *> parameter_order;
  std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > inout_data_order;
  loop_trees.read(argv[1], parameter_order, inout_data_order);

  MultiFileBuilder::KLT_Driver driver(project);
  KLT::Core::Generator<KLT::Sequential::Kernel, MultiFileBuilder::KLT_Driver> generator(driver, std::string(argv[2]) + ".kernel");

  std::set<std::list<KLT::Sequential::Kernel *> > kernel_lists;
  KLT::Core::CG_Config<KLT::Sequential::Kernel> cg_config(
    new KLT::Core::LoopMapper<KLT::Sequential::Kernel>(),
    new KLT::Core::IterationMapper<KLT::Sequential::Kernel>(),
    new KLT::Core::DataFlow<KLT::Sequential::Kernel>()
  );
  generator.generate(loop_trees, kernel_lists, cg_config);

  assert(kernel_lists.size() == 1); // FIXME should be true for some time (versionning due to the loop distribution in the Loop-Mapper)

  createMain(driver, std::string(argv[2]) + ".main", loop_trees, *kernel_lists.begin(), parameter_order, inout_data_order);

  project->unparse();

  return 0;
}

