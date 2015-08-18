/** 
 * \file lib/sage/class-declaration.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_classdecl
 * @{
*/

bool ignore(const std::string & name);
bool ignore(SgScopeStatement * scope);

template <>
bool Driver<Sage>::resolveValidParent<SgClassSymbol>(SgClassSymbol * symbol) {
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
  p_class_symbols.insert(symbol);

  return true;
}

template <>
void  Driver<Sage>::loadSymbols<SgClassDeclaration>(size_t file_id, SgSourceFile * file) {
  std::vector<SgClassDeclaration *> class_decl = SageInterface::querySubTree<SgClassDeclaration>(file);

  std::set<SgClassSymbol *> class_symbols;
  std::vector<SgClassDeclaration *>::const_iterator it_class_decl;
  for (it_class_decl = class_decl.begin(); it_class_decl != class_decl.end(); it_class_decl++) {
    SgClassDeclaration * class_decl = *it_class_decl;

    if (class_decl->get_definition() == NULL) continue;

    Sg_File_Info * file_info = class_decl->get_file_info();
    assert(file_info != NULL);

    if (file_info->get_raw_filename() != file->getFileName()) continue;

    if (ignore(class_decl->get_scope())) continue;
    if (ignore(class_decl->get_name().getString())) continue;

    SgClassSymbol * class_sym = SageInterface::lookupClassSymbolInParentScopes(class_decl->get_name(), class_decl->get_scope());
    assert(class_sym != NULL);

    class_symbols.insert(class_sym);
  }

  std::set<SgClassSymbol *>::iterator it;
  for (it = class_symbols.begin(); it != class_symbols.end(); it++)
    if (resolveValidParent<SgClassSymbol>(*it)) {
      p_symbol_to_file_id_map[*it] = file_id;
      std::cerr << "[Info] (MFB::Driver<Sage>::loadSymbols<SgClassDeclaration>) Add: " << (*it)->get_name().getString() << " from File #" << file_id << std::endl;
    }
}

Sage<SgClassDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  size_t kind_,
  SgSymbol * parent_,
  size_t file_id_,
  bool create_definition_
) :
  name(name_),
  kind(kind_),
  parent(parent_),
  file_id(file_id_),
  create_definition(create_definition_)
{}

template <>
Sage<SgClassDeclaration>::build_result_t Driver<Sage>::build<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc) {
  Sage<SgClassDeclaration>::build_result_t result;

  assert(desc.file_id != 0);

  Sage<SgClassDeclaration>::build_scopes_t scopes = getBuildScopes<SgClassDeclaration>(desc);

  SgScopeStatement * decl_scope = scopes.decl_scope;

  // Decl
  SgClassDeclaration * class_decl = SageBuilder::buildNondefiningClassDeclaration_nfi(desc.name, (SgClassDeclaration::class_types)desc.kind, decl_scope, false, NULL);

  // Symbol handling
  {
    result.symbol = decl_scope->lookup_class_symbol(desc.name);
    assert(result.symbol != NULL);

    p_symbol_to_file_id_map.insert(std::pair<SgSymbol *, size_t>(result.symbol, desc.file_id));
    p_valid_symbols.insert(result.symbol);
    p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result.symbol, desc.parent));
    p_class_symbols.insert(result.symbol);
  }

  // Defining decl
  SgClassDeclaration * class_defn = NULL;
  {
    class_defn = SageBuilder::buildNondefiningClassDeclaration_nfi(desc.name, (SgClassDeclaration::class_types)desc.kind, decl_scope, false, NULL);
    SageInterface::appendStatement(class_defn, decl_scope);

    result.definition = SageBuilder::buildClassDefinition_nfi(class_defn, false);
    class_defn->set_definition(result.definition);
    class_defn->unsetForward(); 
  
    assert(class_decl->get_definition() == NULL);
    assert(class_defn->get_definition() != NULL);
    assert(!class_defn->isForward());
  }

  assert(class_decl->get_type() == class_defn->get_type());

#if PATCHING_SAGE_BUILDER_ISSUES
  { // connection between decl/defn
    class_decl->set_definingDeclaration(class_defn);
    class_decl->set_firstNondefiningDeclaration(class_decl);

    class_defn->set_definingDeclaration(class_defn);
    class_defn->set_firstNondefiningDeclaration(class_decl);
  }
#endif

  return result;
}

template <>
Sage<SgClassDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc) {
  Sage<SgClassDeclaration>::build_scopes_t result;

  SgClassSymbol * class_parent = isSgClassSymbol(desc.parent);
  SgNamespaceSymbol * namespace_parent = isSgNamespaceSymbol(desc.parent);

  assert(desc.parent == NULL || ((class_parent != NULL) xor (namespace_parent != NULL))); // if parent, it needs to be either a class or a namespace

  if (class_parent != NULL) {
    assert(desc.file_id == 0);

    assert(false); // NIY
  }
  else {
    assert(desc.file_id != 0);

    std::map<size_t, SgSourceFile *>::iterator it_file = id_to_file_map.find(desc.file_id);
    assert(it_file != id_to_file_map.end());
    SgSourceFile * decl_file = it_file->second;
    assert(decl_file != NULL);

    if (namespace_parent == NULL)
      result.decl_scope = decl_file->get_globalScope();
    else {
      result.decl_scope = Sage<SgNamespaceDeclarationStatement>::getDefinition(namespace_parent, decl_file);
    }
  }

  return result;
}

template <>
void Driver<Sage>::createForwardDeclaration<SgClassDeclaration>(Sage<SgClassDeclaration>::symbol_t symbol, size_t target_file_id) {
  std::map<size_t, SgSourceFile *>::iterator it_target_file = id_to_file_map.find(target_file_id);
  assert(it_target_file != id_to_file_map.end());
  SgSourceFile * target_file = it_target_file->second;
  assert(target_file != NULL);

  std::map<SgSymbol *, SgSymbol *>::iterator it_parent = p_parent_map.find(symbol);
  assert(it_parent != p_parent_map.end());

  SgClassDeclaration * orig_decl = symbol->get_declaration();
  SgScopeStatement * scope = NULL;
  SgScopeStatement * insert_scope = NULL;

  SgSymbol * parent = it_parent->second;
  if (parent == NULL) {
    scope = target_file->get_globalScope();
    insert_scope = scope;
  }
  else {
    assert(false); // TODO
  }

  SgClassDeclaration * class_decl = SageBuilder::buildNondefiningClassDeclaration_nfi(symbol->get_name(), orig_decl->get_class_type(), scope, false, NULL);
  SageInterface::prependStatement(class_decl, insert_scope);
}

/** @} */

}

