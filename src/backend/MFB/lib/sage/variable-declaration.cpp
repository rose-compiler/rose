
#include "MFB/Sage/variable-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MultiFileBuilder {

template <>
Sage<SgVariableDeclaration>::build_result_t Driver<Sage>::build<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc) {
  Sage<SgVariableDeclaration>::build_result_t result;

  Sage<SgVariableDeclaration>::build_scopes_t scopes = getBuildScopes<SgVariableDeclaration>(desc);

  SgScopeStatement * decl_scope = scopes.scope;

  SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(desc.name, desc.type, desc.initializer, decl_scope);

  result.symbol = decl_scope->lookup_variable_symbol(desc.name);
  assert(result.symbol != NULL);

  result.definition = var_decl->get_variables()[0];
  assert(result.definition != NULL);

  return result;
}

template <>
Sage<SgVariableDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc) {
  Sage<SgVariableDeclaration>::build_scopes_t result;

  assert(false);
/*
  if (desc.parent == NULL) {
    assert(header_files.size() == 1);
    result.scope = (*header_files.begin())->get_globalScope();
  }
  else {
    SgClassSymbol * class_symbol = isSgClassSymbol(desc.parent);
    SgNamespaceSymbol * namespace_symbol = isSgNamespaceSymbol(desc.parent);

    assert(class_symbol != NULL xor namespace_symbol != NULL);

    if (class_symbol != NULL) {
      result.scope = ((SgClassDeclaration *)class_symbol->get_declaration()->get_definingDeclaration())->get_definition();
    }
    if (namespace_symbol != NULL) {
      assert(false);
    }
    
    assert(result.scope != NULL);
  }
*/
  return result;
}

template <>
void Driver<Sage>::createForwardDeclaration<SgVariableDeclaration>(Sage<SgVariableDeclaration>::symbol_t symbol, SgSourceFile * target_file) {
  assert(false);
}

}
