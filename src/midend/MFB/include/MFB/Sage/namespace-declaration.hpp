/** 
 * \file MFB/include/MFB.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_NAMESPACE_DECLARATION_HPP__
#define __MFB_SAGE_NAMESPACE_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_nscpdecl
 * @{
*/

template <>
class Sage<SgNamespaceDeclarationStatement> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgNamespaceSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * header_scope;
      SgScopeStatement * source_scope;
      SgSourceFile * header_file;
      SgSourceFile * source_file;
    };

    struct object_desc_t {
      std::string name;
      SgNamespaceSymbol * parent;
      size_t file_id;

      object_desc_t(const std::string & name_, SgNamespaceSymbol * parent_, size_t file_id_);
    };

    typedef SgNamespaceSymbol * build_result_t;

  private:
    /// namespaces are "unified" over multiple files. We instantiate a namespace declaration only when needed. 
    static std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> > s_per_file_namespace_decl;

  public:
    static SgNamespaceDefinitionStatement * getDefinition(SgNamespaceSymbol * symbol, SgSourceFile * file);
    static SgNamespaceDeclarationStatement * getDeclaration(SgNamespaceSymbol * symbol, SgSourceFile * file);
    static void setDefinition(SgNamespaceSymbol * symbol, SgSourceFile * file, SgNamespaceDeclarationStatement * decl);
/*
  public:
    static void init();
*/
};

template <>
Sage<SgNamespaceDeclarationStatement>::build_result_t Driver<Sage>::build<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc);

template <>
Sage<SgNamespaceDeclarationStatement>::build_scopes_t Driver<Sage>::getBuildScopes<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc);

/** @} */

}

#endif /* __MFB_SAGE_NAMESPACE_DECLARATION_HPP__ */
