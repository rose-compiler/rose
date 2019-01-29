/** 
 * \file MFB/include/MFB/Sage/enum-declaration.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_ENUM_DECLARATION_HPP__
#define __MFB_SAGE_ENUM_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_enumdecl
 * @{
*/

template <>
class Sage<SgEnumDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgEnumSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
    };

    struct object_desc_t {
      std::string name;

      SgSymbol * parent;
      size_t file_id; // non-zero

      object_desc_t(
        std::string name_,
        SgSymbol * parent_,
        size_t file_id_
      );
    };

    struct build_result_t {
      SgEnumSymbol * symbol;
    };
};

template <>
Sage<SgEnumDeclaration>::build_result_t Driver<Sage>::build<SgEnumDeclaration>(const Sage<SgEnumDeclaration>::object_desc_t & desc);

template <>
Sage<SgEnumDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgEnumDeclaration>(const Sage<SgEnumDeclaration>::object_desc_t & desc);

template <>
void Driver<Sage>::createForwardDeclaration<SgEnumDeclaration>(Sage<SgEnumDeclaration>::symbol_t symbol, size_t target_file_id);

/** @} */

}

#endif /* __MFB_SAGE_ENUM_DECLARATION_HPP__ */
