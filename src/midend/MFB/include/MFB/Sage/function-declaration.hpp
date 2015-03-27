/** 
 * \file MFB/include/MFB.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_FUNCTION_DECLARATION_HPP__
#define __MFB_SAGE_FUNCTION_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_funcdecl
 * @{
*/

template <>
class Sage<SgFunctionDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgFunctionSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
      SgScopeStatement * defn_scope;
    };

    struct object_desc_t {
      std::string name;
      SgType * return_type;
      SgFunctionParameterList * params;

      SgNamespaceSymbol * parent;

      unsigned decl_file_id;
      unsigned defn_file_id;

      bool is_static;

      bool create_definition;

      object_desc_t(
        std::string name_,
        SgType * return_type_,
        SgFunctionParameterList * params_,
        SgNamespaceSymbol * parent_namespace_,
        unsigned decl_file_id_,
        unsigned defn_file_id_,
        bool is_static_ = false,
        bool create_definition_ = true
      );
    };

    struct build_result_t {
      SgFunctionSymbol * symbol;
      SgFunctionDefinition * definition;
    };
};

template <>
Sage<SgFunctionDeclaration>::build_result_t Driver<Sage>::build<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc);

template <>
Sage<SgFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc);

template <>
void Driver<Sage>::createForwardDeclaration<SgFunctionDeclaration>(Sage<SgFunctionDeclaration>::symbol_t symbol, unsigned target_file_id);

/** @} */

}

#endif /* __MFB_SAGE_FUNCTION_DECLARATION_HPP__ */

