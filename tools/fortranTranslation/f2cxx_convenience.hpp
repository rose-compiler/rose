#ifndef _F2CXX_CONVENIENCE_HPP

#define _F2CXX_CONVENIENCE_HPP 1

#include "rose.h"

#include "sageGeneric.h"

namespace f2cxx
{
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
    SgExprListExp& args = sg::deref(call.get_args());

    return args.get_expressions();
  }

  static inline
  SgBasicBlock* get_body(SgFunctionDeclaration& n)
  {
    SgFunctionDefinition* def = n.get_definition();

    if (def == NULL) return NULL;

    ROSE_ASSERT(def->get_body());
    return def->get_body();
  }

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

  // get_decl family

  static inline
  SgInitializedName& get_decl(SgVariableSymbol& n)
  {
    return sg::deref(n.get_declaration());
  }

  static inline
  SgInitializedName& get_decl(SgVarRefExp& n)
  {
    return get_decl(sg::deref(n.get_symbol()));
  }

  static inline
  SgClassDefinition& get_defn(SgClassDeclaration& cdcl)
  {
    return sg::deref(cdcl.get_definition());
  }
}


#endif /* _F2CXX_CONVENIENCE_HPP */
