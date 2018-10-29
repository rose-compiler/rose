
#include "ROSE/proposed/sage-interface.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/api.hpp"
#include "MFB/utils.hpp"

#include "ROSE/Analysis/Template/instantiation-flow.hpp"

#include "sage3basic.h"

#include <cassert>

int main(int argc, char ** argv) {
  MFB::Driver<MFB::Sage> driver(new SgProject(argc,argv));

  auto files = driver.project->get_files();
  ROSE_ASSERT(files.size() == 1);

  std::string filename = files[0]->get_sourceFileNameWithoutPath();

  MFB::api_t * api = driver.getAPI();
  for (std::set<SgClassSymbol *>::const_iterator it = api->class_symbols.begin(); it != api->class_symbols.end(); it++) {
    ROSE::Analysis::Template::InstantiationFlow::insert(*it);
  }
  ROSE::Analysis::Template::InstantiationFlow::finalize();

  std::ofstream dot(filename + ".dot");
  ROSE::Analysis::Template::InstantiationFlow::toGraphViz(dot);
  dot.close();

  ROSE::Analysis::Template::InstantiationFlow::clear();

  return 0;
}

