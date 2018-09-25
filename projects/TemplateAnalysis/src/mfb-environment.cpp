
#include "ROSE/proposed/sage-interface.hpp"
#include "ROSE/proposed/mfb-json.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/api.hpp"
#include "MFB/utils.hpp"

#include "ROSE/Analysis/Template/instantiation-flow.hpp"

#include "sage3basic.h"

#include <cassert>

int main(int argc, char ** argv) {
  ROSE::proposed::MFB::Environment mfbenv(argc,argv);

  ::MFB::api_t * api = mfbenv.driver->getAPI();
  for (std::set<SgClassSymbol *>::const_iterator it = api->class_symbols.begin(); it != api->class_symbols.end(); it++) {
    ROSE::Analysis::Template::InstantiationFlow::insert(*it);
  }
  ROSE::Analysis::Template::InstantiationFlow::finalize();

  std::ofstream dot(mfbenv.config["resdir"].get<std::string>() + "templinstflow.dot");
  ROSE::Analysis::Template::InstantiationFlow::toGraphViz(dot);
  dot.close();

  system(("zgrviewer -f " + mfbenv.config["resdir"].get<std::string>() + "templinstflow.dot").c_str());

#if 0
  std::cerr << "Queries:" << std::endl;
  size_t qcnt = 0;
  for (nlohmann::json::iterator it = mfbenv.config["queries"].begin(); it != mfbenv.config["queries"].end(); it ++) {
    assert((*it).find("template") != (*it).end());
    assert((*it)["template"].is_string());
    assert((*it).find("arguments") != (*it).end());
    assert((*it)["arguments"].is_array());

    std::string query = (*it)["template"].get<std::string>();
    std::cerr << " -- query[" << qcnt++ << "] = " << query << std::endl;

    SgSymbol * sym = ROSE::proposed::SageInterface::getSymbolFromString(query, mfbenv.driver->project->get_globalScopeAcrossFiles());
    assert(sym != NULL);

    std::cerr << "     -- sym = " << std::hex << sym << " (" << sym->class_name() << "): " << sym->get_name() << std::endl;

    TemplateInstantiation * assoc_instantiation = TemplateInstantiation::build(sym, instflow);

    // TODO report
  }

  std::cerr << "All queries have been processed" << std::endl;
#endif

  ROSE::Analysis::Template::InstantiationFlow::clear();

  return 0;
}

