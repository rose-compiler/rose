
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
typedef ::DLX::TileK::language_t Dlang; // Directives Language

#include "DLX/KLT/annotations.hpp"
typedef ::DLX::KLT::Annotation<Dlang> Annotation;

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Language
typedef ::KLT::Language::C Klang; // Kernel Language

#include "MDCG/KLT/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "MDCG/TileK/model.hpp"
typedef ::MDCG::TileK::KernelDesc<Hlang, Klang> KernelDesc; // Model for Static Initializer

#include "MDCG/TileK/runtime.hpp"

#include "DLX/KLT/compiler.hpp" // Needs Annotation and Runtime to be defined

#include <cassert>

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

#ifdef TILEK_THREADS
  args.push_back("-DTILEK_THREADS");
#endif

  SgProject * project = new SgProject(args);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));


  DLX::KLT::compile<Dlang, Hlang, Klang, Runtime, KernelDesc>(project, std::string(KLT_RTL_INC_PATH), std::string(TILEK_RTL_INC_PATH), basename + "-kernel.c", basename + "-data.c");

  project->unparse();

  return 0;
}

