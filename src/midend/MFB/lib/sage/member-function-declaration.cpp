/** 
 * \file lib/sage/member-function-declaration.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MFB/Sage/member-function-declaration.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_mfuncdecl
 * @{
*/

bool ignore(const std::string & name);
bool ignore(SgScopeStatement * scope);

template <>
bool Driver<Sage>::resolveValidParent<SgMemberFunctionSymbol>(SgMemberFunctionSymbol * symbol) {
  SgClassSymbol * parent = NULL;
  
  if (p_valid_symbols.find(symbol) != p_valid_symbols.end()) return true;

  SgClassDefinition * class_scope = isSgClassDefinition(symbol->get_scope());
  if (class_scope != NULL) {
    SgClassDeclaration * parent_decl = class_scope->get_declaration();
    assert(parent_decl != NULL);
    parent = SageInterface::lookupClassSymbolInParentScopes(parent_decl->get_name(), parent_decl->get_scope());
    assert(parent != NULL);

    if (!resolveValidParent<SgClassSymbol>((SgClassSymbol *)parent)) return false;
    assert(p_valid_symbols.find(parent) != p_valid_symbols.end());
  }

  p_valid_symbols.insert(symbol);
  p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(symbol, parent));
  p_member_function_symbols.insert(symbol);

  return true;
}

template <>
void  Driver<Sage>::loadSymbols<SgMemberFunctionDeclaration>(unsigned file_id, SgSourceFile * file) {
  std::vector<SgMemberFunctionDeclaration *> member_function_decl = SageInterface::querySubTree<SgMemberFunctionDeclaration>(file);

  std::set<SgMemberFunctionSymbol *> member_function_symbols;
  std::vector<SgMemberFunctionDeclaration *>::const_iterator it_member_function_decl;
  for (it_member_function_decl = member_function_decl.begin(); it_member_function_decl != member_function_decl.end(); it_member_function_decl++) {
    SgMemberFunctionDeclaration * member_function_decl = *it_member_function_decl;

    if (ignore(member_function_decl->get_scope())) continue;
    if (ignore(member_function_decl->get_name().getString())) continue;

    SgMemberFunctionSymbol * member_function_sym = (SgMemberFunctionSymbol *)SageInterface::lookupFunctionSymbolInParentScopes(member_function_decl->get_name(), member_function_decl->get_scope());
    assert(member_function_sym != NULL);

    member_function_symbols.insert(member_function_sym);
  }

  std::set<SgMemberFunctionSymbol *>::iterator it;
  for (it = member_function_symbols.begin(); it != member_function_symbols.end(); it++)
    if (resolveValidParent<SgMemberFunctionSymbol>(*it)) {
      p_symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned>(*it, file_id));
//    std::cout << "   Method Symbol : " << (*it) << ", name = " << (*it)->get_name().getString() << ", scope = " << (*it)->get_scope() << "(" << (*it)->get_scope()->class_name() << ")" << std::endl;
    }
}

Sage<SgMemberFunctionDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  SgType * return_type_,
  SgFunctionParameterList * params_,
  SgClassSymbol * parent_class_,
  unsigned file_id_,
  bool is_static_,
  bool is_virtual_,
  bool is_constructor_,
  bool is_destructor_,
  bool create_definition_
) :
  name(name_),
  return_type(return_type_),
  params(params_),
  parent(parent_class_),
  file_id(file_id_),
  is_static(is_static_),
  is_virtual(is_virtual_),
  is_constructor(is_constructor_),
  is_destructor(is_destructor_),
  create_definition(create_definition_)
{}

template <>
Sage<SgMemberFunctionDeclaration>::build_result_t Driver<Sage>::build<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc) {
  Sage<SgMemberFunctionDeclaration>::build_result_t result;

  Sage<SgMemberFunctionDeclaration>::build_scopes_t scopes = getBuildScopes<SgMemberFunctionDeclaration>(desc);
  
  SgScopeStatement * decl_scope = scopes.decl_scope;
  SgScopeStatement * defn_scope = scopes.defn_scope;

  assert(isSgClassDefinition(decl_scope));
  assert(decl_scope != defn_scope);

  // Build Declaration
  SgMemberFunctionDeclaration * mfunc_decl = NULL;
  {
    mfunc_decl = SageBuilder::buildNondefiningMemberFunctionDeclaration(desc.name, desc.return_type, desc.params, decl_scope, NULL, 0, false, NULL);
    SageInterface::appendStatement(mfunc_decl, decl_scope);

    if (desc.is_constructor)
      mfunc_decl->get_specialFunctionModifier().setConstructor();

    assert(mfunc_decl->get_definition() == NULL);
    assert(mfunc_decl->get_associatedClassDeclaration() != NULL);
  }

  // Build Definition
  SgMemberFunctionDeclaration * mfunc_defn = NULL;
  {
    mfunc_defn = SageBuilder::buildDefiningMemberFunctionDeclaration(desc.name, desc.return_type, desc.params, decl_scope, NULL, 0, false, mfunc_decl, NULL);
    SageInterface::appendStatement(mfunc_defn, defn_scope);

    if (desc.is_constructor)
      mfunc_defn->get_specialFunctionModifier().setConstructor();

    assert(mfunc_defn->get_associatedClassDeclaration() != NULL);

    result.definition = mfunc_defn->get_definition();
    assert(result.definition != NULL);
  }

  { // Symbol handling
    result.symbol = isSgMemberFunctionSymbol(decl_scope->lookup_function_symbol(desc.name));
    assert(result.symbol != NULL);

    std::map<SgSymbol *, unsigned>::iterator it_symbol_to_file_id = p_symbol_to_file_id_map.find(desc.parent);
    assert(it_symbol_to_file_id != p_symbol_to_file_id_map.end());
    p_symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned long>(result.symbol, it_symbol_to_file_id->second));

    p_valid_symbols.insert(result.symbol);
    p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result.symbol, desc.parent));
    p_member_function_symbols.insert(result.symbol);
  }

#if PATCHING_SAGE_BUILDER_ISSUES
  // Decl and Defn
  { 
    mfunc_decl->set_definingDeclaration(mfunc_defn);
    mfunc_decl->set_firstNondefiningDeclaration(mfunc_decl);

    mfunc_defn->set_definingDeclaration(mfunc_defn);
    mfunc_defn->set_firstNondefiningDeclaration(mfunc_decl);
  }
#endif

  return result;
}

template <>
Sage<SgMemberFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc) {
  Sage<SgMemberFunctionDeclaration>::build_scopes_t result;

  assert(desc.parent != NULL);

  {
    SgClassDeclaration * class_decl = desc.parent->get_declaration();
    assert(class_decl != NULL);
    class_decl = isSgClassDeclaration(class_decl->get_definingDeclaration());
    assert(class_decl != NULL);
    SgClassDefinition * class_defn = isSgClassDefinition(class_decl->get_definition());
    assert(class_defn != NULL);
    result.decl_scope = class_defn;
  }

  if (desc.create_definition) {
    unsigned file_id = desc.file_id;

    if (file_id == 0) {
      std::map<SgSymbol *, unsigned>::iterator it_symbol_to_file_id = p_symbol_to_file_id_map.find(desc.parent);
      assert(it_symbol_to_file_id != p_symbol_to_file_id_map.end());

      file_id = it_symbol_to_file_id->second;
    }
    assert(file_id != 0);

    std::map<unsigned, SgSourceFile *>::iterator it_file = id_to_file_map.find(file_id);
    assert(it_file != id_to_file_map.end());
    SgSourceFile * defn_file = it_file->second;
    assert(defn_file != NULL);

    result.defn_scope = defn_file->get_globalScope();
  }
  else result.defn_scope = NULL;

  return result;
}

/** @} */

}

