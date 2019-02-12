
#include "ROSE/proposed/sage-interface.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/api.hpp"
#include "MFB/utils.hpp"

#include "ROSE/Analysis/Template/instantiation-flow.hpp"

#include "sage3basic.h"

#include <cassert>

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv+argc);
  std::set<std::string> nsp_filter_analysis;
  std::set<std::string> force_analysis;
  std::set<std::string> nsp_filter_graphviz;
  auto arg = args.begin();
  while (arg != args.end()) {
    if (arg->find("--namespace-filter-for-analysis=") == 0) {
      std::string nsp_name = arg->substr(32);
      if (nsp_name.substr(0,2) != "::")
        nsp_name = "::" + nsp_name;
      nsp_filter_analysis.insert(nsp_name);
      arg = args.erase(arg);
    } else if (arg->find("--namespace-filter-for-graphviz=") == 0) {
      std::string nsp_name = arg->substr(32);
      if (nsp_name.substr(0,2) != "::")
        nsp_name = "::" + nsp_name;
      nsp_filter_graphviz.insert(nsp_name);
      arg = args.erase(arg);
    } else if (arg->find("--force-analysis=") == 0) {
      std::string nsp_name = arg->substr(17);
      if (nsp_name.substr(0,2) != "::")
        nsp_name = "::" + nsp_name;
      force_analysis.insert(nsp_name);
      arg = args.erase(arg);
    } else {
      arg++;
    }
  }

  MFB::Driver<MFB::Sage> driver(new SgProject(argc,argv));

  auto files = driver.project->get_files();
  ROSE_ASSERT(files.size() == 1);

  std::string filename = files[0]->get_sourceFileNameWithoutPath();

  MFB::api_t * api = driver.getAPI();
  for (std::set<SgClassSymbol *>::const_iterator it = api->class_symbols.begin(); it != api->class_symbols.end(); it++) {
    SgClassSymbol * csym = *it;

    SgClassDeclaration * xdecl = csym->get_declaration();
    assert(xdecl != NULL);

    std::string xname = xdecl->get_qualified_name().getString();

    bool declaration_of_interrest = true;
    for (auto nsp_name: nsp_filter_analysis) {
      if (xname.find(nsp_name) == 0) {
        declaration_of_interrest = false;
      }
    }
    for (auto nsp_name: force_analysis) {
      if (xname.find(nsp_name) == 0) {
        declaration_of_interrest = true;
      }
    }

    if (declaration_of_interrest)
      ROSE::Analysis::Template::InstantiationFlow::insert(*it);
  }
  ROSE::Analysis::Template::InstantiationFlow::finalize();

  filename += ".dot";

  std::cout << "GraphViz filename: " << filename << std::endl;

  std::ofstream dot(filename);
  ROSE::Analysis::Template::InstantiationFlow::toGraphViz(dot, nsp_filter_graphviz);
  dot.close();

  ROSE::Analysis::Template::InstantiationFlow::clear();

  return 0;
}

