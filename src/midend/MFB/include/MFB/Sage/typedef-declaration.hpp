/** 
 * \file include/MFB/Sage/typedef-declaration.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_TYPEDEF_DECLARATION_HPP__
#define __MFB_SAGE_TYPEDEF_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_tddecl
 * @{
*/

template <>
class Sage<SgTypedefDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgTypedefSymbol * symbol_t;

    typedef SgScopeStatement * build_scopes_t;

    struct object_desc_t {
      std::string name;
      SgType * type;

      SgSymbol * parent;
      size_t file_id; // if parent is a class and it is a static: only used for definition scope (declaration scope depends on parent)

      object_desc_t(
        std::string name_,
        SgType * type_,
        SgSymbol * parent_ = NULL,
        size_t file_id_ = 0
      );
    };

    struct build_result_t {
      SgTypedefSymbol * symbol;
    };
};

template <>
Sage<SgTypedefDeclaration>::build_result_t Driver<Sage>::build<SgTypedefDeclaration>(const Sage<SgTypedefDeclaration>::object_desc_t & desc);

template <>
Sage<SgTypedefDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgTypedefDeclaration>(const Sage<SgTypedefDeclaration>::object_desc_t & desc);

template <>
void Driver<Sage>::createForwardDeclaration<SgTypedefDeclaration>(Sage<SgTypedefDeclaration>::symbol_t symbol, size_t target_file_id);

/** @} */

}

#endif /* __MFB_SAGE_TYPEDEF_DECLARATION_HPP__ */
