
#include "MFB/Sage/member-function-declaration.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MultiFileBuilder {

Sage<SgMemberFunctionDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  SgType * return_type_,
  SgFunctionParameterList * params_,
  SgClassSymbol * parent_class_,
  unsigned long file_id_,
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

    std::map<SgSymbol *, unsigned long>::iterator it_symbol_to_file_id = symbol_to_file_id_map.find(desc.parent);
    assert(it_symbol_to_file_id != symbol_to_file_id_map.end());

    symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned long>(result.symbol, it_symbol_to_file_id->second));

    parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result.symbol, desc.parent));
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
    unsigned long file_id = desc.file_id;

    if (file_id == 0) {
      std::map<SgSymbol *, unsigned long>::iterator it_symbol_to_file_id = symbol_to_file_id_map.find(desc.parent);
      assert(it_symbol_to_file_id != symbol_to_file_id_map.end());

      file_id = it_symbol_to_file_id->second;
    }
    assert(file_id != 0);

    std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >::iterator it_file_pair = file_pair_map.find(file_id);
    std::map<unsigned long, SgSourceFile *>::iterator it_standalone_source_file = standalone_source_file_map.find(file_id);

    assert((it_file_pair != file_pair_map.end()) xor (it_standalone_source_file != standalone_source_file_map.end()));

    SgSourceFile * defn_file = NULL;
    if (it_file_pair != file_pair_map.end())
      defn_file = it_file_pair->second.second; // the source file
    else if (it_standalone_source_file != standalone_source_file_map.end())
      defn_file = it_standalone_source_file->second; // decl local to the source file
    else assert(false);
    assert(defn_file != NULL);

    result.defn_scope = defn_file->get_globalScope();
  }
  else result.defn_scope = NULL;

  return result;
}

}
