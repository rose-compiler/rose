
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
#include "DLX/klt-annotations.hpp"

#include <cassert>

namespace DLX {

template <>
bool KLT_Region <OpenACC::language_t>::matchLabel() {
  AstFromString::afs_skip_whitespace();
  return AstFromString::afs_match_substr("openacc");
}

template <>
void KLT_Region <OpenACC::language_t>::parse(std::vector<DLX::KLT_Region <OpenACC::language_t> > & container) {
  if (AstFromString::afs_match_additive_expression()) {
    SgExpression * exp = isSgExpression(AstFromString::c_parsed_node);
    assert(exp != NULL);
    /// \todo save into container
  }
  else if (AstFromString::afs_match_identifier()) {
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);
    /// \todo save into container
  }
  else assert(false);
}

template <>
bool KLT_Data <OpenACC::language_t>::matchLabel() {
  AstFromString::afs_skip_whitespace();
  return AstFromString::afs_match_substr("openacc");
}

template <>
void KLT_Data <OpenACC::language_t>::parse(std::vector<DLX::KLT_Data <OpenACC::language_t> > & container) {
  if (AstFromString::afs_match_substr("copy")) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("copyin")) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("copyout")) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("create")) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("present_or_copy")) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("present_or_copyin")) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("present_or_copyout")) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("present_or_create")) {
    /// \todo
  }
  else assert(false);
}

template <>
bool KLT_Loop <OpenACC::language_t>::matchLabel() {
  AstFromString::afs_skip_whitespace();
  return AstFromString::afs_match_substr("openacc");
}

template <>
void KLT_Loop <OpenACC::language_t>::parse(std::vector<DLX::KLT_Loop <OpenACC::language_t> > & container) {
  if (AstFromString::afs_match_substr("gang",  false)) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("worker",  false)) {
    /// \todo
  }
  else if (AstFromString::afs_match_substr("vector",  false)) {
    /// \todo
  }
  else assert(false);
}

}

//#include "sage3basic.h"

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

