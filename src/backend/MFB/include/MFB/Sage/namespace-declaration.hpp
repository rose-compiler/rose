
#ifndef __MFB_SAGE_NAMESPACE_DECLARATION_HPP__
#define __MFB_SAGE_NAMESPACE_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MultiFileBuilder {

template <>
class Sage<SgNamespaceDeclarationStatement> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgNamespaceSymbol * symbol_t;

    typedef SgScopeStatement * build_scopes_t;

    struct object_desc_t {
      std::string name;
      SgNamespaceSymbol * parent;
    };

    typedef SgNamespaceSymbol * build_result_t;

  private:
    /// namespaces are "unified" over multiple files. We instantiate a namespace declaration only when needed. 
    static std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> > per_file_namespace_decl;

  public:
    static SgNamespaceDefinitionStatement * getDefinition(SgNamespaceSymbol * symbol, SgSourceFile * file);
/*
  public:
    static void init();
*/
};

template <>
Sage<SgNamespaceDeclarationStatement>::build_result_t Driver<Sage>::build<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc);

template <>
Sage<SgNamespaceDeclarationStatement>::build_scopes_t Driver<Sage>::getBuildScopes<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc);

}

#endif /* __MFB_SAGE_NAMESPACE_DECLARATION_HPP__ */
