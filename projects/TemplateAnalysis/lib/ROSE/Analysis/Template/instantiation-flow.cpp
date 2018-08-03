
#include "ROSE/Analysis/Template/instantiation-flow.hpp"
#include "ROSE/Analysis/Template/instantiation.hpp"
#include "ROSE/Analysis/Template/constraints.hpp"
#include "ROSE/Analysis/Template/element.hpp"
#include "ROSE/Analysis/Template/relation.hpp"

namespace ROSE { namespace Analysis { namespace Template {

void InstantiationFlow::finalize() {
  for (auto it = Instantiation::all.begin(); it != Instantiation::all.end(); it++) {
    it->second->finalize();
  }
}

void InstantiationFlow::insert(SgClassSymbol * symbol) {
  TemplateInstantiation::build((SgSymbol *)symbol);
}

void InstantiationFlow::insert(SgNonrealSymbol * symbol) {
  NonrealInstantiation::build((SgSymbol *)symbol);
}

void InstantiationFlow::toGraphViz(std::ostream & out) {
  out << "digraph {" << std::endl;

  for (auto it = Element::all.begin(); it != Element::all.end(); it++) {
    it->second->toGraphViz(out);
  }

  for (auto it = Relation::all.begin(); it != Relation::all.end(); it++) {
    it->second->toGraphViz(out);
  }

  out << "}" << std::endl;
}

void InstantiationFlow::clear() {
  for (auto it = Element::all.begin(); it != Element::all.end(); it++) {
    delete it->second;
  }
  
  for (auto it = Relation::all.begin(); it != Relation::all.end(); it++) {
    delete it->second;
  }
}

} } }

