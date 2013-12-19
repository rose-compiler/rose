
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include "DLX/OpenACC/language.hpp"

#include <cassert>


int main(int argc, char ** argv) {
  assert(argc == 3);

  KLT::Core::LoopTrees<
      DLX::KLT_Data   <DLX::OpenACC::language_t>,
      DLX::KLT_Region <DLX::OpenACC::language_t>,
      DLX::KLT_Loop   <DLX::OpenACC::language_t>
  > loop_trees;

  std::list<SgVariableSymbol *> parameter_order;
  std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > inout_data_order;

  loop_trees.read(argv[1], parameter_order, inout_data_order);

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

  KLT::Core::CG_Config< KLT::Language::OpenCL , KLT::Runtime::OpenACC > cg_config(
      new KLT::Core::LoopMapper      < KLT::Language::OpenCL , KLT::Runtime::OpenACC >(),
      new KLT::Core::IterationMapper < KLT::Language::OpenCL , KLT::Runtime::OpenACC >(),
      new KLT::Core::DataFlow        < KLT::Language::OpenCL , KLT::Runtime::OpenACC >()
  );
  generator.generate(loop_trees, kernel_lists, cg_config);

  project->unparse();

  return 0;
}

