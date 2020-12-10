#ifndef _F2CXX_CONVENIENCE_HPP

#define _F2CXX_CONVENIENCE_HPP 1

#include "rose.h"

#include "sageGeneric.h"

namespace f2cxx
{

  //
  // obtain iterators

  static inline
  SgInitializedNamePtrList::iterator
  begin(SgFunctionParameterList& n)
  {
    return n.get_args().begin();
  }

  static inline
  SgInitializedNamePtrList::iterator
  end(SgFunctionParameterList& n)
  {
    return n.get_args().end();
  }

  //
  // params family

  static inline
  SgInitializedNamePtrList& params(SgFunctionParameterList& n)
  {
    return n.get_args();
  }

  static inline
  SgInitializedNamePtrList& params(SgFunctionDeclaration& n)
  {
    return params(sg::deref(n.get_parameterList()));
  }

  static inline
  SgExpressionPtrList& params(SgExprListExp& args)
  {
    return args.get_expressions();
  }

  static inline
  SgExpressionPtrList& params(SgCallExpression& call)
  {
    return params(sg::deref(call.get_args()));
  }

  //
  // arg_at family

  static inline
  SgExpression* arg_at(SgExpressionPtrList& lst, size_t n)
  {
    return lst.at(n);
  }

  static inline
  SgExpression* arg_at(SgExprListExp* args, size_t n)
  {
    return arg_at(sg::deref(args).get_expressions(), n);
  }

  static inline
  SgExpression* arg_at(SgCallExpression* call, size_t n)
  {
    return arg_at(sg::deref(call).get_args(), n);
  }

  //
  // arg_count family

  static inline
  size_t arg_count(SgExpressionPtrList& lst)
  {
    return lst.size();
  }

  static inline
  size_t arg_count(SgExprListExp* args)
  {
    return arg_count(sg::deref(args).get_expressions());
  }

  static inline
  size_t arg_count(SgCallExpression* call)
  {
    return arg_count(sg::deref(call).get_args());
  }

  //
  // get_decl/get_defn families

  static inline
  SgInitializedName& get_decl(SgVariableSymbol& n)
  {
    return sg::deref(n.get_declaration());
  }

  static inline
  SgClassDeclaration& get_decl(SgClassSymbol& clsy)
  {
    return sg::deref(clsy.get_declaration());
  }

  static inline
  SgInitializedName& get_decl(SgVarRefExp& n)
  {
    return get_decl(sg::deref(n.get_symbol()));
  }

  static inline
  SgClassDefinition& get_defn(SgClassDeclaration& cdcl)
  {
    SgClassDeclaration* ddcl =
      sg::assert_sage_type<SgClassDeclaration>(cdcl.get_definingDeclaration());

    return sg::deref(ddcl->get_definition());
  }

  static inline
  SgClassDefinition& get_defn(SgClassSymbol& clsy)
  {
    return get_defn(get_decl(clsy));
  }

  static inline
  SgFunctionDefinition& get_defn(SgFunctionDeclaration& dcl)
  {
    return sg::deref(dcl.get_definition());
  }

  //
  // get_body family

  static inline
  SgBasicBlock& get_body(SgFunctionDefinition& def)
  {
    return sg::deref(def.get_body());
  }

  static inline
  SgBasicBlock& get_body(SgFunctionDeclaration& dcl)
  {
    return get_body(get_defn(dcl));
  }
}


#endif /* _F2CXX_CONVENIENCE_HPP */
