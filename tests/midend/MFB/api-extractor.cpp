
#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/api.hpp"

#include "sage3basic.h"

int main(int argc, char ** argv) {
  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
  project->set_originalCommandLineArgumentList (arglist);

  MFB::Driver<MFB::Sage> driver(project);

  // Load a pair header and source files
  unsigned long loaded_file_id = driver.loadPairOfFiles(argv[1], "", "");

  MFB::api_t * api = driver.getAPI(loaded_file_id);

  MFB::dump_api(api);

  return 0;
}

