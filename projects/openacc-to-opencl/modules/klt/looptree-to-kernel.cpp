
#include "KLT/loop-trees.hpp"
#include "KLT/data.hpp"

#include "KLT/iteration-mapper.hpp"
#include "KLT/loop-mapper.hpp"
#include "KLT/data-flow.hpp"
#include "KLT/cg-config.hpp"
#include "KLT/generator.hpp"
#include "KLT/kernel.hpp"
#include "KLT/mfb-klt.hpp"

#include "KLT/mfb-acc-ocl.hpp"

#include "KLT/dlx-openacc.hpp"

#include <cassert>

#include "sage3basic.h"


typedef ::KLT::Language::OpenCL Language;
typedef ::KLT::Runtime::OpenACC Runtime;
typedef ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

int main(int argc, char ** argv) {
  assert(argc == 4);

  DLX::OpenACC::language_t::init();

  LoopTrees loop_trees;

  loop_trees.read(argv[1]);

  SgProject * project = new SgProject::SgProject();
  { // Add default command line to an empty project
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
      arglist.push_back(std::string("-I") + argv[3]);
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }

  MFB::KLT_Driver driver(project);

  KLT::Runtime::OpenACC::loadAPI(driver, argv[3]);

  KLT::Generator<Annotation, Language, Runtime, MFB::KLT_Driver> generator(driver, std::string(argv[2]) + ".cl");

  std::set<std::list<Kernel *> > kernel_lists;

  KLT::CG_Config<Annotation, Language, Runtime> cg_config(
      new KLT::LoopMapper<Annotation, Language, Runtime>(),
      new KLT::IterationMapper<Annotation, Language, Runtime>(),
      new KLT::DataFlow<Annotation, Language, Runtime>()
  );
  generator.generate(loop_trees, kernel_lists, cg_config);

  project->unparse(); // Cannot call the backend directly because of OpenCL files. There is a warning when trying, just have to trace it.

  std::set<std::list<Kernel *> >::const_iterator it_kernel_list;
  std::list<Kernel *>::const_iterator it_kernel;
  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    for (it_kernel = it_kernel_list->begin(); it_kernel != it_kernel_list->end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      const std::vector<Kernel::a_kernel *> & kernels = kernel->getKernels();

      /// \todo print info about generated kernel
    }
  }

  return 0;
}

