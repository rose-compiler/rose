
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include "KLT/OpenCL/kernel.hpp"

#include "KLT/Core/generator.hpp"
#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-mapper.hpp"
#include "KLT/Core/iteration-mapper.hpp"

#include "KLT/OpenCL/mfb-klt.hpp"

//#include "build-main.hpp"

#include "sage3basic.h"

#include <cassert>

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
  std::list<SgVariableSymbol *> parameter_order;
  std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > inout_data_order;  
  loop_trees.read(argv[1], parameter_order, inout_data_order);

  ::MultiFileBuilder::KLT_Driver driver(project);
  KLT::Core::Generator<KLT::OpenCL::Kernel, ::MultiFileBuilder::KLT_Driver> generator(driver, std::string(argv[2]) + ".kernel");

  std::set<std::list<KLT::OpenCL::Kernel *> > kernel_lists;
  KLT::Core::CG_Config<KLT::OpenCL::Kernel> cg_config(
    new KLT::Core::LoopMapper<KLT::OpenCL::Kernel>(),
    new KLT::Core::IterationMapper<KLT::OpenCL::Kernel>(),
    new KLT::Core::DataFlow<KLT::OpenCL::Kernel>()
  );
  generator.generate(loop_trees, kernel_lists, cg_config);
/*
  std::list<KLT::Core::Kernel *>::const_iterator it_kernel_;
  std::vector<SgExpression *>::const_iterator it_expr;
  for (it_kernel_ = kernels_.begin(); it_kernel_ != kernels_.end(); it_kernel_++) {
    KLT::OpenCL::Kernel * kernel_ = dynamic_cast<KLT::OpenCL::Kernel *>(*kernels_.begin());
    assert(kernel_ != NULL);

    const std::set<KLT::OpenCL::Kernel::a_kernel *> & kernels = kernel_->getKernels();
    assert(kernels.size() == 1); // As long as KLT::OpenCL::Dummy_WorkSizeShaper is used it will hold

    KLT::OpenCL::Kernel::a_kernel * kernel = *kernels.begin();

    std::cout << "Kernel #" << kernel_->id << ":" << std::endl;
    std::cout << "   name = \"" << kernel->kernel_name << "\"" << std::endl;
    std::cout << "   nbr dims = " << kernel->number_dims << std::endl;
    std::cout << "   global work sizes = " << std::endl;
    for (unsigned int i = 0; i < kernel->number_dims; i++)
      std::cout << "      " << kernel->global_work_size[i]->unparseToString() << std::endl;
    if (kernel->have_local_work_size) {
      std::cout << "   local work sizes = " << std::endl;
      for (unsigned int i = 0; i < kernel->number_dims; i++) 
        std::cout << "      " << kernel->local_work_size[i]->unparseToString() << std::endl;
    }
    std::cout << std::endl;
  }
*/

  project->unparse();

  return 0;
}

