/** 
 * \file include/MFB/Sage/nonreal-declaration.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_NONREAL_DECLARATION_HPP__
#define __MFB_SAGE_NONREAL_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_nrdecl
 * @{
*/

template <>
class Sage<SgNonrealDecl> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgNonrealSymbol * symbol_t;

    typedef SgScopeStatement * build_scopes_t;

    struct object_desc_t {
      std::string name;

      SgSymbol * parent;
      size_t file_id; // if parent is a class and it is a static: only used for definition scope (declaration scope depends on parent)

      object_desc_t(
        std::string name_,
        SgSymbol * parent_ = NULL,
        size_t file_id_ = 0
      );
    };

    struct build_result_t {
      SgNonrealSymbol * symbol;
    };
};

template <>
Sage<SgNonrealDecl>::build_result_t Driver<Sage>::build<SgNonrealDecl>(const Sage<SgNonrealDecl>::object_desc_t & desc);

template <>
Sage<SgNonrealDecl>::build_scopes_t Driver<Sage>::getBuildScopes<SgNonrealDecl>(const Sage<SgNonrealDecl>::object_desc_t & desc);

template <>
void Driver<Sage>::createForwardDeclaration<SgNonrealDecl>(Sage<SgNonrealDecl>::symbol_t symbol, size_t target_file_id);

/** @} */

}

#endif /* __MFB_SAGE_NONREAL_DECLARATION_HPP__ */
