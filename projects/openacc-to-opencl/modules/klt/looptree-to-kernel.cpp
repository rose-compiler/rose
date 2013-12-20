
#include "KLT/loop-trees.hpp"
#include "KLT/data.hpp"

/*
#include "KLT/iteration-mapper.hpp"
#include "KLT/loop-mapper.hpp"
#include "KLT/data-flow.hpp"
#include "KLT/cg-config.hpp"
#include "KLT/generator.hpp"
#include "KLT/kernel.hpp"
#include "KLT/mfb-klt.hpp"
*/

#include "DLX/OpenACC/language.hpp"

#include <cassert>

//#include "sage3basic.h"

namespace DLX {
  template <class Lang> class KLT_Data {};
  template <class Lang> class KLT_Region {};
  template <class Lang> class KLT_Loop {};
}

int main(int argc, char ** argv) {
  assert(argc == 3);

  KLT::LoopTrees<
      DLX::KLT_Data   <DLX::OpenACC::language_t>,
      DLX::KLT_Region <DLX::OpenACC::language_t>,
      DLX::KLT_Loop   <DLX::OpenACC::language_t>
  > loop_trees;

  loop_trees.read(argv[1]);
/*
  SgProject * project = new SgProject::SgProject();
  { // Add default command line to an empty project
    std::vector<std::string> arglist;
      arglist.push_back("c++");
      arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
      arglist.push_back("-c");
    project->set_originalCommandLineArgumentList (arglist);
  }

  MFB::KLT_Driver driver(project);
  KLT::Generator<KLT::Language::OpenCL, KLT::Runtime::OpenACC, ::MFB::KLT_Driver> generator(driver, std::string(argv[2]));

  std::set<std::list<KLT::Kernel<KLT::Language::OpenCL, KLT::Runtime::OpenACC> *> > kernel_lists;

  KLT::CG_Config<
      DLX::KLT_Data   <DLX::OpenACC::language_t>,
      DLX::KLT_Region <DLX::OpenACC::language_t>,
      DLX::KLT_Loop   <DLX::OpenACC::language_t>,
      KLT::Language::OpenCL,
      KLT::Runtime::OpenACC
  > cg_config(
      new KLT::LoopMapper<
          DLX::KLT_Data   <DLX::OpenACC::language_t>,
          DLX::KLT_Region <DLX::OpenACC::language_t>,
          DLX::KLT_Loop   <DLX::OpenACC::language_t>,
          KLT::Language::OpenCL,
          KLT::Runtime::OpenACC
      >(),
      new KLT::IterationMapper<
          DLX::KLT_Data   <DLX::OpenACC::language_t>,
          DLX::KLT_Region <DLX::OpenACC::language_t>,
          DLX::KLT_Loop   <DLX::OpenACC::language_t>,
          KLT::Language::OpenCL,
          KLT::Runtime::OpenACC
      >(),
      new KLT::DataFlow<
          DLX::KLT_Data   <DLX::OpenACC::language_t>,
          DLX::KLT_Region <DLX::OpenACC::language_t>,
          DLX::KLT_Loop   <DLX::OpenACC::language_t>,
          KLT::Language::OpenCL,
          KLT::Runtime::OpenACC
      >()
  );
  generator.generate(loop_trees, kernel_lists, cg_config);

  project->unparse();
*/
  return 0;
}

