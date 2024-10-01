#ifndef _LIBADALANG_EXPRESSION
#define _LIBADALANG_EXPRESSION 1

/// Contains functions and classes for converting Ada expressions
///   from Libadalang to ROSE.


#include "Libadalang_to_ROSE.h"
#include "AdaMaker.h"
#include "libadalang.h"

namespace Libadalang_ROSE_Translation
{

  struct ArgDesc : std::tuple<std::string, SgType*>
  {
    using base = std::tuple<std::string, SgType*>;
    using base::base;

    const std::string& name() const { return std::get<0>(*this); }
    SgType*            type() const { return std::get<1>(*this); }
  };

  struct OperatorCallSupplement : std::tuple< std::vector<ArgDesc>, SgType*, int>
  {
    using ArgDescList = std::vector<ArgDesc>;

    using base = std::tuple<ArgDescList, SgType*, int>;

    // explicit
    OperatorCallSupplement(ArgDescList arglst = {}, SgType* resty = nullptr, int prefix = -1)
    : base(std::move(arglst), resty, prefix)
    {}

    const ArgDescList& args() const { return std::get<0>(*this); }
    ArgDescList&       args()       { return std::get<0>(*this); }

    SgType* result() const        { return std::get<1>(*this); }
    void    result(SgType* resty) { std::get<1>(*this) = resty; }

    int        scopeId() const     { return std::get<2>(*this); }
    void       scopeId(int prefix) { std::get<2>(*this) = prefix; }

    bool args_valid() const { return args().size() > 0; }
    bool valid()      const { return args_valid() && result() != nullptr; }
  };

  /// returns an expression for attribute defined in expr
  /// \param lal_element   the representative libadalang node
  /// \param ctx           the translation context
  /// \param argRangeSuppl an optional argument to inject the arguments from a function call
  SgAdaAttributeExp&
  getAttributeExpr(ada_base_entity* lal_element, AstContext ctx, ada_base_entity* argRangeSuppl = nullptr);

  /// returns the ROSE representation of the Libadalang expression \ref elem
  SgExpression&
  getExpr(ada_base_entity* lal_element, AstContext ctx, OperatorCallSupplement suppl = {}, bool unary = false);

  SgExpression&
  getExpr_opt(ada_base_entity* lal_expr, AstContext ctx, OperatorCallSupplement suppl = {});

  /// returns a range expression for the element \ref lal_element.
  /// \pre lal_element identifies a Discrete_Range definition
  SgExpression&
  getDiscreteRange(ada_base_entity* lal_element, AstContext ctx);

  /// returns an expression from the libadalang definition \ref lal_element
  SgExpression&
  getDefinitionExpr(ada_base_entity* lal_element, AstContext ctx);

  /// creates a call to subroutine expression \ref target, and passes params as arguments.
  /// \param tgtid the ASIS Id of the called target (function)
  /// \param params a list of ASIS Ids representing the arguments
  /// \param prefixCallSyntax true if an operator uses call syntax (i.e., "+"(1,2) instead of 1 + 2)
  /// \param objectCallSyntax true if a call uses OOP style syntax (i.e., x.init instead of init(x))
  /// \param ctx the translation context
  /// \return an expression representing the call
  SgExpression& createCall(ada_base_entity* lal_prefix, std::vector<ada_base_entity*> lal_params, bool prefixCallSyntax, bool objectCallSyntax, AstContext ctx);

  SgExpression&
  getEnumRepresentationValue(ada_base_entity* lal_element, int enum_position, AstContext ctx);

  /// queries the corresponding ROSE AST node for a built-in identifer
  SgNode*
  queryBuiltIn(int hash);

  /// queries the corresponding ROSE AST node in ROSE for a given Libadalang representation.
  SgNode*
  queryCorrespondingAstNode(ada_base_entity* lal_identifier);

  /// Handles a single argument provided in a function call
  SgExpression&
  getArg(ada_base_entity* lal_element, AstContext ctx);

} //end Libadalang_ROSE_Translation


#endif /* _ADA_EXPRESSION */
