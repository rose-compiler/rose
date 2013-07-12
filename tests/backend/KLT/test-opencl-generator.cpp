
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include "KLT/OpenCL/cg-config.hpp"

#include "KLT/OpenCL/generator.hpp"
#include "KLT/OpenCL/kernel.hpp"

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
  std::list<KLT::Core::Kernel *> kernels_;
  KLT::OpenCL::CG_Config cg_config(new KLT::Core::DataFlow(), new KLT::Core::Dummy_LoopSelector(), new KLT::OpenCL::Dummy_WorkSizeShaper());

  generator.generate(loop_trees, kernels_, cg_config);

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

  project->unparse();

  return 0;
}

