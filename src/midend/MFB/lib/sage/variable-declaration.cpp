/** 
 * \file lib/sage/variable-declaration.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MFB/Sage/variable-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_vardecl
 * @{
*/

bool ignore(const std::string & name);
bool ignore(SgScopeStatement * scope);

template <>
bool Driver<Sage>::resolveValidParent<SgVariableSymbol>(SgVariableSymbol * symbol) {
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
  p_variable_symbols.insert(symbol);

  return true;
}

template <>
void  Driver<Sage>::loadSymbolsFromPair<SgVariableDeclaration>(unsigned long file_id, SgSourceFile * header_file, SgSourceFile * source_file) {
  std::vector<SgVariableDeclaration *> header_variable_decl = SageInterface::querySubTree<SgVariableDeclaration>(header_file);
  std::vector<SgVariableDeclaration *> source_variable_decl = SageInterface::querySubTree<SgVariableDeclaration>(source_file);

  std::set<SgVariableSymbol *> variable_symbols;
  std::vector<SgVariableDeclaration *>::const_iterator it_variable_decl;
  for (it_variable_decl = header_variable_decl.begin(); it_variable_decl != header_variable_decl.end(); it_variable_decl++) {
    SgVariableDeclaration * variable_decl = *it_variable_decl;
    assert(variable_decl->get_variables().size() == 1);

    if (ignore(variable_decl->get_scope())) continue;

    SgInitializedName * init_name = variable_decl->get_variables()[0];

    if (ignore(init_name->get_name().getString())) continue;

    SgVariableSymbol * variable_sym = SageInterface::lookupVariableSymbolInParentScopes(init_name->get_name(), variable_decl->get_scope());
    assert(variable_sym != NULL);

    variable_symbols.insert(variable_sym);
  }
  for (it_variable_decl = source_variable_decl.begin(); it_variable_decl != source_variable_decl.end(); it_variable_decl++) {
    SgVariableDeclaration * variable_decl = *it_variable_decl;
    assert(variable_decl->get_variables().size() == 1);

    if (ignore(variable_decl->get_scope())) continue;

    SgInitializedName * init_name = variable_decl->get_variables()[0];

    if (ignore(init_name->get_name().getString())) continue;

    SgVariableSymbol * variable_sym = SageInterface::lookupVariableSymbolInParentScopes(init_name->get_name(), variable_decl->get_scope());
    assert(variable_sym != NULL);

    variable_symbols.insert(variable_sym);
  }

  std::set<SgVariableSymbol *>::iterator it;
  for (it = variable_symbols.begin(); it != variable_symbols.end(); it++)
    if (resolveValidParent<SgVariableSymbol>(*it)) {
      p_symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned long>(*it, file_id));
//    std::cout << " Variable Symbol : " << (*it) << ", name = " << (*it)->get_name().getString() << ", scope = " << (*it)->get_scope() << "(" << (*it)->get_scope()->class_name() << ")" << std::endl;
    }
}

Sage<SgVariableDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  SgType * type_,
  SgInitializer * initializer_,
  SgSymbol * parent_,
  unsigned long file_id_,
  bool is_static_,
  bool create_definition_
) :
  name(name_),
  type(type_),
  initializer(initializer_),
  parent(parent_),
  file_id(file_id_),
  is_static(is_static_),
  create_definition(create_definition_)
{}


template <>
Sage<SgVariableDeclaration>::build_result_t Driver<Sage>::build<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc) {
  Sage<SgVariableDeclaration>::build_result_t result;

  SgScopeStatement * decl_scope = getBuildScopes<SgVariableDeclaration>(desc);

  SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(desc.name, desc.type, desc.initializer, decl_scope);
  SageInterface::appendStatement(var_decl, decl_scope);

  result.symbol = decl_scope->lookup_variable_symbol(desc.name);
  assert(result.symbol != NULL);

  result.definition = var_decl->get_variables()[0];
  assert(result.definition != NULL);

  if (isSgClassSymbol(desc.parent) != NULL) {
    std::map<SgSymbol *, unsigned long>::iterator it_sym_to_file = p_symbol_to_file_id_map.find(desc.parent);
    assert(it_sym_to_file != p_symbol_to_file_id_map.end());
    p_symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned long>(result.symbol, it_sym_to_file->second));
  }
  else
    p_symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned long>(result.symbol, desc.file_id));

  p_valid_symbols.insert(result.symbol);
  p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result.symbol, desc.parent));
  p_variable_symbols.insert(result.symbol);

  return result;
}

template <>
Sage<SgVariableDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc) {
  Sage<SgVariableDeclaration>::build_scopes_t result = NULL;

  SgClassSymbol * class_symbol = isSgClassSymbol(desc.parent);
  SgNamespaceSymbol * namespace_symbol = isSgNamespaceSymbol(desc.parent);

  SgSourceFile * file = NULL;
  if (desc.file_id != 0) {
    std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >::const_iterator it_file_pair_map = file_pair_map.find(desc.file_id);
    std::map<unsigned long, SgSourceFile *>::const_iterator it_standalone_source_file_map = standalone_source_file_map.find(desc.file_id);
    assert((it_file_pair_map != file_pair_map.end()) xor (it_standalone_source_file_map != standalone_source_file_map.end()));
    if (it_file_pair_map != file_pair_map.end())
      file = it_file_pair_map->second.first;
    if (it_standalone_source_file_map != standalone_source_file_map.end())
      file = it_standalone_source_file_map->second;
    assert(file != NULL);
  }

  if (desc.parent == NULL)
    result = file->get_globalScope();
  else if (namespace_symbol != NULL)
    result = Sage<SgNamespaceDeclarationStatement>::getDefinition(namespace_symbol, file);
  else if (class_symbol != NULL)
    result = ((SgClassDeclaration *)class_symbol->get_declaration()->get_definingDeclaration())->get_definition();
  else assert(false); 

  assert(result != NULL);

  return result;
}

template <>
void Driver<Sage>::createForwardDeclaration<SgVariableDeclaration>(Sage<SgVariableDeclaration>::symbol_t symbol, SgSourceFile * target_file) {
  assert(false);
}

/** @} */

}

