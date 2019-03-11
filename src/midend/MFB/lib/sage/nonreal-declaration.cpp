
#include "MFB/Sage/nonreal-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace MFB {

bool ignore(const std::string & name);
bool ignore(SgScopeStatement * scope);

template <>
bool Driver<Sage>::resolveValidParent<SgNonrealSymbol>(SgNonrealSymbol * symbol) {
  SgSymbol * parent = NULL;

  std::cout << "[Debug] (MFB::Driver<Sage>::resolveValidParent<SgNonrealDecl>) symbol = " << symbol << " (" << symbol->get_name().str() << ")" << std::endl;
  
  if (p_valid_symbols.find(symbol) != p_valid_symbols.end()) return true;

  SgNamespaceDefinitionStatement * namespace_scope = isSgNamespaceDefinitionStatement(symbol->get_scope());
  SgClassDefinition              * class_scope     = isSgClassDefinition             (symbol->get_scope());
  SgDeclarationScope             * nrscope         = isSgDeclarationScope            (symbol->get_scope());
  if (namespace_scope != NULL) {
    SgNamespaceDeclarationStatement * parent_decl = namespace_scope->get_namespaceDeclaration();
    assert(parent_decl != NULL);
    parent = SageInterface::lookupNamespaceSymbolInParentScopes(parent_decl->get_name(), parent_decl->get_scope());
    assert(parent != NULL);

    if (!resolveValidParent<SgNamespaceSymbol>((SgNamespaceSymbol *)parent)) return false;
    assert(p_valid_symbols.find(parent) != p_valid_symbols.end());
  }
  else if (class_scope != NULL) {
    SgClassDeclaration * parent_decl = class_scope->get_declaration();
    assert(parent_decl != NULL);
    parent = SageInterface::lookupClassSymbolInParentScopes(parent_decl->get_name(), parent_decl->get_scope());
    assert(parent != NULL);

    if (!resolveValidParent<SgClassSymbol>((SgClassSymbol *)parent)) return false;
    assert(p_valid_symbols.find(parent) != p_valid_symbols.end());
  }
  else if (nrscope != NULL) {
    SgDeclarationStatement * parent_decl = isSgDeclarationStatement(nrscope->get_parent());
    assert(parent_decl != NULL);

    parent = parent_decl->search_for_symbol_from_symbol_table();
    assert(parent != NULL);

    if (isSgClassSymbol(parent)) {
      if (!resolveValidParent<SgClassSymbol>((SgClassSymbol *)parent)) return false;
    } else {
      ROSE_ASSERT(isSgNonrealSymbol(parent));
      if (!resolveValidParent<SgNonrealSymbol>((SgNonrealSymbol *)parent)) return false;
    }
    assert(p_valid_symbols.find(parent) != p_valid_symbols.end());
  }

  p_valid_symbols.insert(symbol);
  p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(symbol, parent));
  p_nonreal_symbols.insert(symbol);

  return true;
}

template <>
void  Driver<Sage>::loadSymbols<SgNonrealDecl>(size_t file_id, SgSourceFile * file) {
  std::vector<SgNonrealDecl *> nonreal_decl = SageInterface::querySubTree<SgNonrealDecl>(file);
#if VERBOSE || 1
    std::cout << "[Debug] (MFB::Driver<Sage>::loadSymbols<SgNonrealDecl>) nonreal_decl.size() = " << nonreal_decl.size() << std::endl;
#endif

  std::set<SgNonrealSymbol *> nonreal_symbols;
  std::vector<SgNonrealDecl *>::const_iterator it_nonreal_decl;
  for (it_nonreal_decl = nonreal_decl.begin(); it_nonreal_decl != nonreal_decl.end(); it_nonreal_decl++) {
    SgNonrealDecl * nonreal_decl = *it_nonreal_decl;
#if VERBOSE || 1
    std::cout << "[Debug] (MFB::Driver<Sage>::loadSymbols<SgNonrealDecl>) nonreal_decl = " << nonreal_decl << " (" << nonreal_decl->get_name().str() << ")" << std::endl;
#endif

    SgScopeStatement * scope = nonreal_decl->get_scope();
    ROSE_ASSERT(scope != NULL);
    if (ignore(scope)) continue;

    if (ignore(nonreal_decl->get_name().getString())) continue;

    SgNonrealSymbol * nonreal_sym = isSgNonrealSymbol(nonreal_decl->search_for_symbol_from_symbol_table());
    ROSE_ASSERT(nonreal_sym != NULL);

    nonreal_symbols.insert(nonreal_sym);
  }

  std::set<SgNonrealSymbol *>::iterator it;
  for (it = nonreal_symbols.begin(); it != nonreal_symbols.end(); it++)
    if (resolveValidParent<SgNonrealSymbol>(*it)) {
      p_symbol_to_file_id_map[*it] = file_id;
#if VERBOSE
      std::cerr << "[Info] (MFB::Driver<Sage>::loadSymbols<SgNonrealDecl>) Add: " << (*it)->get_name().getString() << " from File #" << file_id << std::endl;
#endif
    }
}

Sage<SgNonrealDecl>::object_desc_t::object_desc_t(
  std::string name_,
  SgSymbol * parent_,
  size_t file_id_
) :
  name(name_),
  parent(parent_),
  file_id(file_id_)
{}


template <>
Sage<SgNonrealDecl>::build_result_t Driver<Sage>::build<SgNonrealDecl>(const Sage<SgNonrealDecl>::object_desc_t & desc) {
  ROSE_ASSERT(false);
}

template <>
Sage<SgNonrealDecl>::build_scopes_t Driver<Sage>::getBuildScopes<SgNonrealDecl>(const Sage<SgNonrealDecl>::object_desc_t & desc) {
  ROSE_ASSERT(false);
}

template <>
void Driver<Sage>::createForwardDeclaration<SgNonrealDecl>(Sage<SgNonrealDecl>::symbol_t symbol, size_t target_file_id) {
  ROSE_ASSERT(false);
}

}

