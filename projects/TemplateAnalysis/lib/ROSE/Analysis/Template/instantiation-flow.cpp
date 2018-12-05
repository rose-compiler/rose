
#include "ROSE/Analysis/Template/instantiation-flow.hpp"
#include "ROSE/Analysis/Template/instantiation.hpp"
#include "ROSE/Analysis/Template/constraints.hpp"
#include "ROSE/Analysis/Template/element.hpp"
#include "ROSE/Analysis/Template/relation.hpp"

#include "sage3basic.h"

namespace ROSE { namespace Analysis { namespace Template {

bool declared_in_namespace(SgNode * n, const std::set<std::string> & nsp_filter) {
  std::string name;

  SgClassSymbol * csym = isSgClassSymbol(n);
  SgTypedefSymbol * tdsym = isSgTypedefSymbol(n);
  SgVariableSymbol * vsym = isSgVariableSymbol(n);
  SgNonrealSymbol * nrsym = isSgNonrealSymbol(n);

  SgClassType * ctype = isSgClassType(n);
  SgModifierType * mtype = isSgModifierType(n);
  SgTypedefType * tdtype = isSgTypedefType(n);
  SgPointerType * ptype = isSgPointerType(n);
  SgNonrealType * nrtype = isSgNonrealType(n);
  SgReferenceType * rtype = isSgReferenceType(n);
  SgFunctionType * ftype = isSgFunctionType(n);

  if (csym) {
    SgClassDeclaration * decl = csym->get_declaration();
    assert(decl != NULL);
    name = decl->get_qualified_name().getString();
  } else if (tdsym) {
    SgTypedefDeclaration * decl = tdsym->get_declaration();
    assert(decl != NULL);
    name = decl->get_qualified_name().getString();
  } else if (vsym) {
    // SgInitializedName * get_declaration ()
  } else if (nrsym) {
    SgNonrealDecl * decl = isSgNonrealDecl(nrsym->get_declaration());
    assert(decl != NULL);
    name = decl->get_qualified_name().getString();
  } else if (ctype) {
    SgClassDeclaration * decl = isSgClassDeclaration(ctype->get_declaration());
    assert(decl != NULL);
    name = decl->get_qualified_name().getString();
  } else if (tdtype) {
    SgTypedefDeclaration * decl = isSgTypedefDeclaration(tdtype->get_declaration());
    assert(decl != NULL);
    name = decl->get_qualified_name().getString();
  } else if (mtype) {
    // 
  } else if (nrtype) {
    SgNonrealDecl * decl = isSgNonrealDecl(nrtype->get_declaration());
    assert(decl != NULL);
    name = decl->get_qualified_name().getString();
  } else if (ptype) {
    // 
  } else if (rtype) {
    // 
  } else if (ftype) {
    // nope!!!
  }

//std::cout << "###" <<  n->class_name() << std::endl;

  for (auto nsp_name: nsp_filter) {
    if (name.find(nsp_name) == 0) {
      return true;
    }
  }
  return false;
}

void InstantiationFlow::finalize() {
  for (auto it = Element::all.begin(); it != Element::all.end(); it++) {
    it->second->finalize();
  }
  for (auto it = Relation::all.begin(); it != Relation::all.end(); it++) {
    it->second->finalize();
  }
}

void InstantiationFlow::insert(SgClassSymbol * symbol) {
  TemplateInstantiation::build((SgSymbol *)symbol);
}

void InstantiationFlow::insert(SgNonrealSymbol * symbol) {
  NonrealInstantiation::build((SgSymbol *)symbol);
}

void InstantiationFlow::toGraphViz(std::ostream & out, const std::set<std::string> & nsp_filter) {
  out << "digraph {" << std::endl;

  std::set<Element*> selected;
  std::set<Element*> has_edge;

  for (auto it = Element::all.begin(); it != Element::all.end(); it++) {
    if (!declared_in_namespace(it->first, nsp_filter)) {
      selected.insert(it->second);
    }
  }

  for (auto it = Relation::all.begin(); it != Relation::all.end(); it++) {
    if (selected.count(it->first.first) == 1 && selected.count(it->first.second) == 1) {
      it->second->toGraphViz(out);
      has_edge.insert(it->first.first);
      has_edge.insert(it->first.second);
    }
  }

  for (auto it = selected.begin(); it != selected.end(); it++) {
    if (has_edge.count(*it)) {
      (*it)->toGraphViz(out);
    }
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

