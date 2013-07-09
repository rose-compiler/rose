
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MultiFileBuilder {

template <>
Sage<SgNamespaceDeclarationStatement>::build_result_t Driver<Sage>::build<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc) {
  Sage<SgNamespaceDeclarationStatement>::build_result_t result;

  SgScopeStatement * decl_scope = getBuildScopes<SgNamespaceDeclarationStatement>(desc);

  SgNamespaceDeclarationStatement * namespace_decl = SageBuilder::buildNamespaceDeclaration(desc.name, decl_scope);
  SageInterface::appendStatement(namespace_decl, decl_scope);

  result = decl_scope->lookup_namespace_symbol(desc.name);
  assert(result != NULL);

  parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result, desc.parent));

  return result;
}

template <>
Sage<SgNamespaceDeclarationStatement>::build_scopes_t Driver<Sage>::getBuildScopes<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc) {
  Sage<SgNamespaceDeclarationStatement>::build_scopes_t result = NULL;

  if (desc.parent != NULL)
    result = Sage<SgNamespaceDeclarationStatement>::getDefinition(desc.parent, NULL); // get definition for the project global scope
  else
    result = project->get_globalScopeAcrossFiles();

  assert(result != NULL);

  return result;
}

std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> > Sage<SgNamespaceDeclarationStatement>::per_file_namespace_decl;

SgNamespaceDefinitionStatement * Sage<SgNamespaceDeclarationStatement>::getDefinition(SgNamespaceSymbol * symbol, SgSourceFile * file) {
  SgNamespaceDefinitionStatement * result = NULL;

  std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> >::iterator it_per_file_namespace_decl = per_file_namespace_decl.find(symbol);
  assert(it_per_file_namespace_decl != per_file_namespace_decl.end());

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

/*
void Sage<SgNamespaceDeclarationStatement>::init() {
  
}
*/

}
