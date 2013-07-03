
#ifndef __SCSM_SAGE_HPP__
#define __SCSM_SAGE_HPP__

#include "multi-file-builder.hpp"

#include <string>
#include <utility>

class SgType;

class SgFunctionDeclaration;
class SgFunctionParameterList;
class SgFunctionSymbol;
class SgFunctionDefinition;

class SgClassDeclaration;
class SgClassSymbol;
class SgClassDefinition;

class SgMemberFunctionDeclaration;
class SgMemberFunctionSymbol;

namespace MultiFileBuilder {

// Model: Sage

template <typename Object>
class Sage {
  public:
    typedef SgScopeStatement * scope_t;
};

// Driver Ctor for Sage

template <>
Driver<Sage>::Driver(const std::string & filename_, SgProject * project_);

// SgFunctionDeclaration

template <>
class Sage<SgFunctionDeclaration> {
  public:
    typedef SgFunctionSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
      SgScopeStatement * defn_scope;
    };

    struct object_desc_t {
      std::string name;
      SgType * return_type;
      SgFunctionParameterList * params;
    };

    struct build_result_t {
      SgFunctionSymbol * symbol;
      SgFunctionDefinition * definition;
    };
};

template <>
template <>
Sage<SgFunctionDeclaration>::build_result_t Driver<Sage>::build<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc);

template <>
template <>
Sage<SgFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc);

// SgClassDeclaration

template <>
class Sage<SgClassDeclaration> {
  public:
    typedef SgClassSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
    };

    struct object_desc_t {
      std::string name;
      unsigned long kind;
    };

    struct build_result_t {
      SgClassSymbol * symbol;
      SgClassDefinition * definition;
    };
};

template <>
template <>
Sage<SgClassDeclaration>::build_result_t Driver<Sage>::build<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc);

template <>
template <>
Sage<SgClassDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc);

// SgMemberFunctionDeclaration

template <>
class Sage<SgMemberFunctionDeclaration> {
  public:
    typedef SgMemberFunctionSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
      SgScopeStatement * defn_scope;
    };

    struct object_desc_t {
      std::string name;
      SgType * return_type;
      SgFunctionParameterList * params;
      SgClassSymbol * parent_class;
    };

    struct build_result_t {
      SgMemberFunctionSymbol * symbol;
      SgFunctionDefinition * definition;
    };
};

template <>
template <>
Sage<SgMemberFunctionDeclaration>::build_result_t Driver<Sage>::build<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc);

template <>
template <>
Sage<SgMemberFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc);

}

#endif /* __SCSM_SAGE_HPP__ */

