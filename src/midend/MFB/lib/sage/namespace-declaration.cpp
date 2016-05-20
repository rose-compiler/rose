
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace MFB {

bool ignore(const std::string & name);

template <>
bool Driver<Sage>::resolveValidParent<SgNamespaceSymbol>(SgNamespaceSymbol * symbol) {
  SgNamespaceSymbol * parent = NULL;
  
  if (p_valid_symbols.find(symbol) != p_valid_symbols.end()) return true;

  SgNamespaceDefinitionStatement * namespace_scope = isSgNamespaceDefinitionStatement(symbol->get_scope());
  if (namespace_scope != NULL) {
    SgNamespaceDeclarationStatement * parent_decl = namespace_scope->get_namespaceDeclaration();
    assert(parent_decl != NULL);
    parent = SageInterface::lookupNamespaceSymbolInParentScopes(parent_decl->get_name(), parent_decl->get_scope());
    assert(parent != NULL);

    if (!resolveValidParent<SgNamespaceSymbol>(parent)) return false;
    assert(p_valid_symbols.find(parent) != p_valid_symbols.end());
  }

  p_valid_symbols.insert(symbol);
  p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(symbol, parent));
  p_namespace_symbols.insert(symbol);

  return true;
}

template <>
void Driver<Sage>::loadSymbols<SgNamespaceDeclarationStatement>(size_t file_id, SgSourceFile * file) {
  std::vector<SgNamespaceDeclarationStatement *> namespace_decl = SageInterface::querySubTree<SgNamespaceDeclarationStatement>(file);

  std::set<SgNamespaceSymbol *> namespace_symbols;
  std::vector<SgNamespaceDeclarationStatement *>::const_iterator it_namespace_decl;
  for (it_namespace_decl = namespace_decl.begin(); it_namespace_decl != namespace_decl.end(); it_namespace_decl++) {
    SgNamespaceDeclarationStatement * namespace_decl = *it_namespace_decl;

    if (ignore(namespace_decl->get_name().getString())) continue;

    SgNamespaceSymbol * namespace_sym = SageInterface::lookupNamespaceSymbolInParentScopes(namespace_decl->get_name(), namespace_decl->get_scope());
    assert(namespace_sym != NULL);

    namespace_symbols.insert(namespace_sym);
  }

  std::set<SgNamespaceSymbol *>::iterator it;
  for (it = namespace_symbols.begin(); it != namespace_symbols.end(); it++)
    if (resolveValidParent<SgNamespaceSymbol>(*it)) {
#if VERBOSE
      std::cerr << "[Info] (MFB::Driver<Sage>::loadSymbols<SgNamespaceDeclarationStatement>) Add: " << (*it)->get_name().getString() << " from File #" << file_id << std::endl;
#endif
    }
}

Sage<SgNamespaceDeclarationStatement>::object_desc_t::object_desc_t(const std::string & name_, SgNamespaceSymbol * parent_, size_t file_id_) :
  name(name_),
  parent(parent_),
  file_id(file_id_)
{}

template <>
Sage<SgNamespaceDeclarationStatement>::build_result_t Driver<Sage>::build<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc) {
  Sage<SgNamespaceDeclarationStatement>::build_result_t result;

  Sage<SgNamespaceDeclarationStatement>::build_scopes_t scopes = getBuildScopes<SgNamespaceDeclarationStatement>(desc);

  SgNamespaceDeclarationStatement * namespace_header_decl = NULL;
  SgNamespaceSymbol * header_sym = NULL;
  if (scopes.header_scope != NULL) {
    namespace_header_decl = SageBuilder::buildNamespaceDeclaration(desc.name, scopes.header_scope);
    SageInterface::appendStatement(namespace_header_decl, scopes.header_scope);
    header_sym = scopes.header_scope->lookup_namespace_symbol(desc.name);
  }

  SgNamespaceDeclarationStatement * namespace_source_decl = NULL;
  SgNamespaceSymbol * source_sym = NULL;
  if (scopes.source_scope != NULL) {
    namespace_source_decl = SageBuilder::buildNamespaceDeclaration(desc.name, scopes.source_scope);
    SageInterface::appendStatement(namespace_source_decl, scopes.source_scope);
    source_sym = scopes.source_scope->lookup_namespace_symbol(desc.name);
  }

  assert(scopes.header_scope == NULL || header_sym != NULL);
  assert(scopes.source_scope == NULL || source_sym != NULL);
  assert(header_sym != NULL || source_sym != NULL);
  assert((header_sym == NULL || source_sym == NULL) || header_sym == source_sym);

  if (header_sym != NULL) result = header_sym;
  else result = source_sym;

  if (scopes.header_scope != NULL) Sage<SgNamespaceDeclarationStatement>::setDefinition(result, scopes.header_file, namespace_header_decl);
  if (scopes.source_scope != NULL) Sage<SgNamespaceDeclarationStatement>::setDefinition(result, scopes.source_file, namespace_source_decl);

  p_valid_symbols.insert(result);
  p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result, desc.parent));
  p_namespace_symbols.insert(result);

  return result;
}

template <>
Sage<SgNamespaceDeclarationStatement>::build_scopes_t Driver<Sage>::getBuildScopes<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc) {
  Sage<SgNamespaceDeclarationStatement>::build_scopes_t result;
    result.header_scope = NULL;
    result.source_scope = NULL;

  SgSourceFile * header_file = NULL;
  SgSourceFile * source_file = NULL;

  assert(desc.file_id != 0);

  std::map<size_t, SgSourceFile *>::iterator it_file = id_to_file_map.find(desc.file_id);
  assert(it_file != id_to_file_map.end());
  SgSourceFile * file = it_file->second;
  assert(file != NULL);

  if (file != NULL) result.header_file = file;

  if (desc.parent != NULL) {
    if (header_file != NULL) result.header_scope = Sage<SgNamespaceDeclarationStatement>::getDefinition(desc.parent, file);
    if (source_file != NULL) result.source_scope = Sage<SgNamespaceDeclarationStatement>::getDefinition(desc.parent, file);
  }
  else {
    if (header_file != NULL) result.header_scope = file->get_globalScope();
    if (source_file != NULL) result.source_scope = file->get_globalScope();
  }

  assert(result.source_scope != NULL);

  return result;
}

std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> > Sage<SgNamespaceDeclarationStatement>::s_per_file_namespace_decl;

SgNamespaceDefinitionStatement * Sage<SgNamespaceDeclarationStatement>::getDefinition(SgNamespaceSymbol * symbol, SgSourceFile * file) {
  SgNamespaceDefinitionStatement * result = NULL;

  std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> >::iterator it_per_file_namespace_decl = s_per_file_namespace_decl.find(symbol);
  assert(it_per_file_namespace_decl != s_per_file_namespace_decl.end());

  SgNamespaceDeclarationStatement * namespace_decl = NULL;

  std::map<SgSourceFile *, SgNamespaceDeclarationStatement *>::iterator it_decl_to_file = it_per_file_namespace_decl->second.find(file);
  if (it_decl_to_file != it_per_file_namespace_decl->second.end())
    namespace_decl = it_decl_to_file->second;
  else if (file != NULL) {
    it_decl_to_file = it_per_file_namespace_decl->second.find(NULL);
    assert(it_decl_to_file != it_per_file_namespace_decl->second.end());

    assert(false); // TODO build decl and defn for the requested file
  }
  else assert(false);

  assert(namespace_decl != NULL);

  result = namespace_decl->get_definition();

  assert(result != NULL);

  return result;
}

SgNamespaceDeclarationStatement * Sage<SgNamespaceDeclarationStatement>::getDeclaration(SgNamespaceSymbol * symbol, SgSourceFile * file) {
  SgNamespaceDeclarationStatement * namespace_decl = NULL;

  std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> >::iterator it_per_file_namespace_decl = s_per_file_namespace_decl.find(symbol);
  assert(it_per_file_namespace_decl != s_per_file_namespace_decl.end());

  std::map<SgSourceFile *, SgNamespaceDeclarationStatement *>::iterator it_decl_to_file = it_per_file_namespace_decl->second.find(file);
  if (it_decl_to_file != it_per_file_namespace_decl->second.end())
    namespace_decl = it_decl_to_file->second;
  else if (file != NULL) {
    it_decl_to_file = it_per_file_namespace_decl->second.find(NULL);
    assert(it_decl_to_file != it_per_file_namespace_decl->second.end());

    assert(false); // TODO build decl and defn for the requested file
  }
  else assert(false);

  assert(namespace_decl != NULL);

  return namespace_decl;
}

void Sage<SgNamespaceDeclarationStatement>::setDefinition(SgNamespaceSymbol * symbol, SgSourceFile * file, SgNamespaceDeclarationStatement * decl) {
  std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> >::iterator it_per_file_namespace_decl = s_per_file_namespace_decl.find(symbol);
  if (it_per_file_namespace_decl == s_per_file_namespace_decl.end())
    it_per_file_namespace_decl = s_per_file_namespace_decl.insert(
      std::pair<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> >(symbol, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *>())
    ).first;
  assert(it_per_file_namespace_decl != s_per_file_namespace_decl.end());

  std::map<SgSourceFile *, SgNamespaceDeclarationStatement *>::iterator it_src_to_decl = it_per_file_namespace_decl->second.find(file);
  assert(it_src_to_decl == it_per_file_namespace_decl->second.end()); // sjould not be present

  it_per_file_namespace_decl->second.insert(std::pair<SgSourceFile *, SgNamespaceDeclarationStatement *>(file, decl));
}

}

