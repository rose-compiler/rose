#include "sage3basic.h"
#include "rose_config.h"

#include "fortran_flang_support.h"
#include "flang-external-builder-main.h"
#include "SageTreeBuilder.h"

using namespace Rose;

#include <iostream>
using std::cout;

int
experimental_fortran_main(int argc, char* argv[], SgSourceFile* sg_source_file) {
  int status{-1};

  if (SgProject::get_verbose() > 0) {
    cout << "\n";
    cout << "experimental_fortran_main: calling flang parser\n";
    std::cout << "--> argc=" << argc << " argv=";
    if (argc > 2) std::cout << argv[0] << ":" << argv[1] << ":" << argv[2] << std::endl;
  }

  SgGlobal* global_scope = Rose::builder::initialize_global_scope(sg_source_file);
  ROSE_ASSERT(global_scope && "fortran_flang_support: failed initialize_global_scope");

  status = flang_external_builder_main(argc, argv, sg_source_file);

  if (SgProject::get_verbose() > 0) {
    cout << "FINISHED parsing with status " << status << "\n";
  }

  return status;
}
