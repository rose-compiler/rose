
#ifndef __MFB_SAGE_VARIABLE_DECLARATION_HPP__
#define __MFB_SAGE_VARIABLE_DECLARATION_HPP__

#include "MFB/Sage/driver.hpp"

namespace MultiFileBuilder {

template <>
class Sage<SgVariableDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgVariableSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * scope;
    };

    struct object_desc_t {
      std::string name;
      SgType * type;
      SgInitializer * initializer;

      bool is_static;

      SgSymbol * parent;
      unsigned long file_id; // if parent is a class and it is a static: only used for definition scope (declaration scope depends on parent)

      bool create_definition; // for static field (valid if is_static && isSgClassSymbol(parent))
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
void Driver<Sage>::createForwardDeclaration<SgVariableDeclaration>(Sage<SgVariableDeclaration>::symbol_t symbol, SgSourceFile * target_file);

}

#endif /* __MFB_SAGE_VARIABLE_DECLARATION_HPP__ */
