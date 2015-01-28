/** 
 * \file include/MFB/Sage/variable-declaration.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_VARIABLE_DECLARATION_HPP__
#define __MFB_SAGE_VARIABLE_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_vardecl
 * @{
*/

template <>
class Sage<SgVariableDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgVariableSymbol * symbol_t;

    typedef SgScopeStatement * build_scopes_t;

    struct object_desc_t {
      std::string name;
      SgType * type;
      SgInitializer * initializer;

      SgSymbol * parent;
      unsigned file_id; // if parent is a class and it is a static: only used for definition scope (declaration scope depends on parent)

      bool is_static;

      bool create_definition; // for static field (valid if is_static && isSgClassSymbol(parent))

      object_desc_t(
        std::string name_,
        SgType * type_,
        SgInitializer * initializer_ = NULL,
        SgSymbol * parent_ = NULL,
        unsigned file_id_ = 0,
        bool is_static_ = false,
        bool create_definition_ = true
      );
    };

    struct build_result_t {
      SgVariableSymbol * symbol;
      SgInitializedName * definition;
    };
};

template <>
Sage<SgVariableDeclaration>::build_result_t Driver<Sage>::build<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc);

template <>
Sage<SgVariableDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc);

template <>
void Driver<Sage>::createForwardDeclaration<SgVariableDeclaration>(Sage<SgVariableDeclaration>::symbol_t symbol, unsigned target_file_id);

/** @} */

}

#endif /* __MFB_SAGE_VARIABLE_DECLARATION_HPP__ */
