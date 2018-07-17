
#include "MFB/Sage/typedef-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace MFB {

bool ignore(const std::string & name);
bool ignore(SgScopeStatement * scope);

template <>
bool Driver<Sage>::resolveValidParent<SgTypedefSymbol>(SgTypedefSymbol * symbol) {
  SgSymbol * parent = NULL;
  
  if (p_valid_symbols.find(symbol) != p_valid_symbols.end()) return true;

  SgNamespaceDefinitionStatement * namespace_scope = isSgNamespaceDefinitionStatement(symbol->get_scope());
  SgClassDefinition              * class_scope     = isSgClassDefinition             (symbol->get_scope());
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

  p_valid_symbols.insert(symbol);
  p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(symbol, parent));
  p_typedef_symbols.insert(symbol);

  return true;
}

template <>
void  Driver<Sage>::loadSymbols<SgTypedefDeclaration>(size_t file_id, SgSourceFile * file) {
  std::vector<SgTypedefDeclaration *> typedef_decl = SageInterface::querySubTree<SgTypedefDeclaration>(file);

  std::set<SgTypedefSymbol *> typedef_symbols;
  std::vector<SgTypedefDeclaration *>::const_iterator it_typedef_decl;
  for (it_typedef_decl = typedef_decl.begin(); it_typedef_decl != typedef_decl.end(); it_typedef_decl++) {
    SgTypedefDeclaration * typedef_decl = *it_typedef_decl;
#if VERBOSE
    std::cout << "[Debug] (MFB::Driver<Sage>::loadSymbols<SgTypedefDeclaration>) typedef_decl = " << typedef_decl << " (" << typedef_decl->get_name().str() << ")" << std::endl;
#endif

#if 0
    SgTemplateTypedefDeclaration * tpltd_decl = isSgTemplateTypedefDeclaration(typedef_decl);
#endif

    SgScopeStatement * scope = typedef_decl->get_scope();
    ROSE_ASSERT(scope != NULL);
    if (ignore(scope)) continue;

    if (ignore(typedef_decl->get_name().getString())) continue;

    SgTypedefSymbol * typedef_sym = NULL;
#if 0
    if (tplvar_decl == NULL) {
#endif
      typedef_sym = SageInterface::lookupTypedefSymbolInParentScopes(typedef_decl->get_name(), scope);
      ROSE_ASSERT(typedef_sym != NULL);
#if 0
    } else {
      typedef_sym = SageInterface::lookupTemplateTypedefSymbolInParentScopes(typedef_decl->get_name(), &(tplvar_decl->get_templateParameters()), &(tplvar_decl->get_templateSpecializationArguments()), scope);
      ROSE_ASSERT(typedef_sym != NULL);
      ROSE_ASSERT(isSgTemplateTypedefSymbol(typedef_sym));
    }
#endif

    typedef_symbols.insert(typedef_sym);
  }

  std::set<SgTypedefSymbol *>::iterator it;
  for (it = typedef_symbols.begin(); it != typedef_symbols.end(); it++)
    if (resolveValidParent<SgTypedefSymbol>(*it)) {
      p_symbol_to_file_id_map[*it] = file_id;
#if VERBOSE
      std::cerr << "[Info] (MFB::Driver<Sage>::loadSymbols<SgTypedefDeclaration>) Add: " << (*it)->get_name().getString() << " from File #" << file_id << std::endl;
#endif
    }
}

Sage<SgTypedefDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  SgType * type_,
  SgSymbol * parent_,
  size_t file_id_
) :
  name(name_),
  type(type_),
  parent(parent_),
  file_id(file_id_)
{}


template <>
Sage<SgTypedefDeclaration>::build_result_t Driver<Sage>::build<SgTypedefDeclaration>(const Sage<SgTypedefDeclaration>::object_desc_t & desc) {
  ROSE_ASSERT(false);
}

template <>
Sage<SgTypedefDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgTypedefDeclaration>(const Sage<SgTypedefDeclaration>::object_desc_t & desc) {
  ROSE_ASSERT(false);
}

template <>
void Driver<Sage>::createForwardDeclaration<SgTypedefDeclaration>(Sage<SgTypedefDeclaration>::symbol_t symbol, size_t target_file_id) {
  ROSE_ASSERT(false);
}

}

