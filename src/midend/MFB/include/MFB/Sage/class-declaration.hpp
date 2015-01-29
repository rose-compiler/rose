/** 
 * \file MFB/include/MFB.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_CLASS_DECLARATION_HPP__
#define __MFB_SAGE_CLASS_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_classdecl
 * @{
*/

template <>
class Sage<SgClassDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgClassSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
    };

    struct object_desc_t {
      std::string name;
      unsigned long kind;

      SgSymbol * parent;
      unsigned file_id; // non-zero

      bool create_definition;

      object_desc_t(
        std::string name_,
        unsigned long kind_,
        SgSymbol * parent_,
        unsigned file_id_,
        bool create_definition_ = true
      );
    };

    struct build_result_t {
      SgClassSymbol * symbol;
      SgClassDefinition * definition;
    };
};

template <>
Sage<SgClassDeclaration>::build_result_t Driver<Sage>::build<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc);

template <>
Sage<SgClassDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc);

template <>
void Driver<Sage>::createForwardDeclaration<SgClassDeclaration>(Sage<SgClassDeclaration>::symbol_t symbol, unsigned target_file_id);

/** @} */

}

#endif /* __MFB_SAGE_CLASS_DECLARATION_HPP__ */
