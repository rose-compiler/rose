/** 
 * \file MFB/include/MFB.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_MEMBER_FUNCTION_DECLARATION_HPP__
#define __MFB_SAGE_MEMBER_FUNCTION_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_mfuncdecl
 * @{
*/

template <>
class Sage<SgMemberFunctionDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgMemberFunctionSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
      SgScopeStatement * defn_scope;
    };

    struct object_desc_t {
      std::string name;
      SgType * return_type;
      SgFunctionParameterList * params;

      SgClassSymbol * parent;
      size_t file_id; // only used for definition scope (declaration scope depends on parent)

      bool is_static;
      bool is_virtual;
      bool is_constructor;
      bool is_destructor;

      bool create_definition;

      object_desc_t(
        std::string name_,
        SgType * return_type_,
        SgFunctionParameterList * params_,
        SgClassSymbol * parent_class_,
        size_t file_id_ = 0,
        bool is_static_ = false,
        bool is_virtual_ = false,
        bool is_constructor_ = false,
        bool is_destructor_ = false,
        bool create_definition_ = true
      );
    };

    struct build_result_t {
      SgMemberFunctionSymbol * symbol;
      SgFunctionDefinition * definition;
    };
};

template <>
Sage<SgMemberFunctionDeclaration>::build_result_t Driver<Sage>::build<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc);

template <>
Sage<SgMemberFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc);

/** @} */

}

#endif /* __MFB_SAGE_MEMBER_FUNCTION_DECLARATION_HPP__ */
