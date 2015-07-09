
#include "sage3basic.h"

#include "DLX/KLT/compiler.hpp"

#include "DLX/TileK/language.hpp" // DLX::TileK::language_t
#include "KLT/Language/none.hpp"  // KLT::Language::None
#include "KLT/Runtime/tilek.hpp"  // KLT::Runtime::TileK
#include "MDCG/TileK/model.hpp"   // MDCG::TileK::KernelDesc

#include "MFB/KLT/tilek.hpp" // Specialize "template <class Kernel> MFB::KLT" for "KLT::Kernel<DLX::KLT::Annotation<DLX::TileK::language_t>, KLT::Language::None, KLT::Runtime::TileK>"

#include <cassert>

int main(int argc, char ** argv) {
  SgProject * project = new SgProject::SgProject(argc, argv);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

  DLX::KLT::compile<DLX::TileK::language_t, KLT::Language::None, KLT::Runtime::TileK, MDCG::TileK::KernelDesc>(project, std::string(TILEK_INC_PATH) + "/TileK", basename + "-kernel.c", basename + "-data.c");

  project->unparse();

  return 0;
}

