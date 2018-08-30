
#include "MFB/Sage/enum-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace MFB {

bool ignore(const std::string & name);
bool ignore(SgScopeStatement * scope);

template <>
bool Driver<Sage>::resolveValidParent<SgEnumSymbol>(SgEnumSymbol * symbol) {
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
  p_enum_symbols.insert(symbol);

  return true;
}

template <>
void  Driver<Sage>::loadSymbols<SgEnumDeclaration>(size_t file_id, SgSourceFile * file) {
  std::vector<SgEnumDeclaration *> enum_decls = SageInterface::querySubTree<SgEnumDeclaration>(file);

  std::set<SgEnumSymbol *> enum_symbols;
  std::vector<SgEnumDeclaration *>::const_iterator it_enum_decl;
  for (it_enum_decl = enum_decls.begin(); it_enum_decl != enum_decls.end(); it_enum_decl++) {
    SgEnumDeclaration * enum_decl = *it_enum_decl;

    Sg_File_Info * file_info = enum_decl->get_file_info();
    assert(file_info != NULL);

    if (file_info->get_raw_filename() != file->getFileName()) continue;

    if (ignore(enum_decl->get_scope())) continue;
    if (ignore(enum_decl->get_name().getString())) continue;

    SgEnumSymbol * enum_sym = SageInterface::lookupEnumSymbolInParentScopes(enum_decl->get_name(), enum_decl->get_scope());
    assert(enum_sym != NULL);

    enum_symbols.insert(enum_sym);
  }

  std::set<SgEnumSymbol *>::iterator it;
  for (it = enum_symbols.begin(); it != enum_symbols.end(); it++)
    if (resolveValidParent<SgEnumSymbol>(*it)) {
      p_symbol_to_file_id_map[*it] = file_id;
#if VERBOSE
      std::cerr << "[Info] (MFB::Driver<Sage>::loadSymbols<SgEnumDeclaration>) Add: " << (*it)->get_name().getString() << " from File #" << file_id << std::endl;
#endif
    }
}

Sage<SgEnumDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  SgSymbol * parent_,
  size_t file_id_
) :
  name(name_),
  parent(parent_),
  file_id(file_id_)
{}

template <>
Sage<SgEnumDeclaration>::build_result_t Driver<Sage>::build<SgEnumDeclaration>(const Sage<SgEnumDeclaration>::object_desc_t & desc) {
  ROSE_ASSERT(false);
}

template <>
Sage<SgEnumDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgEnumDeclaration>(const Sage<SgEnumDeclaration>::object_desc_t & desc) {
  ROSE_ASSERT(false);
}

template <>
void Driver<Sage>::createForwardDeclaration<SgEnumDeclaration>(Sage<SgEnumDeclaration>::symbol_t symbol, size_t target_file_id) {
  ROSE_ASSERT(false);
}

/** @} */

}

