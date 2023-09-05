

#ifndef _ADA_EXPRESSION
#define _ADA_EXPRESSION 1

/// Contains functions and classes for converting Ada expressions
///   from Asis to ROSE.


#include "Ada_to_ROSE.h"
#include "AdaMaker.h"
#include "a_nodes.h"

namespace Ada_ROSE_Translation
{

  struct ArgDesc : std::tuple<std::string, SgType*>
  {
    using base = std::tuple<std::string, SgType*>;
    using base::base;

    const std::string& name() const { return std::get<0>(*this); }
    SgType*            type() const { return std::get<1>(*this); }
  };

  struct OperatorCallSupplement : std::tuple< std::vector<ArgDesc>, SgType*, Element_ID>
  {
    using ArgDescList = std::vector<ArgDesc>;

    using base = std::tuple<ArgDescList, SgType*, Element_ID>;

    // explicit
    OperatorCallSupplement(ArgDescList arglst = {}, SgType* resty = nullptr, Element_ID prefix = -1)
    : base(std::move(arglst), resty, prefix)
    {}

    const ArgDescList& args() const { return std::get<0>(*this); }
    ArgDescList&       args()       { return std::get<0>(*this); }

    SgType* result() const        { return std::get<1>(*this); }
    void    result(SgType* resty) { std::get<1>(*this) = resty; }

    Element_ID scopeId() const            { return std::get<2>(*this); }
    void       scopeId(Element_ID prefix) { std::get<2>(*this) = prefix; }

    bool args_valid() const { return args().size() > 0; }
    bool valid()      const { return args_valid() && result() != nullptr; }
  };

  /// returns the ROSE representation of the Asis expression \ref elem
  SgExpression&
  getExpr(Element_Struct& elem, AstContext ctx, OperatorCallSupplement suppl = {});

  /// returns the ROSE representation of the Asis Element_ID \ref el
  SgExpression&
  getExprID(Element_ID el, AstContext ctx, OperatorCallSupplement suppl = {});

  /// returns the ROSE representation of a valid Asis Element_ID \ref el.
  ///   if el is not a valid exprression, an SgNullExpression is returned
  SgExpression&
  getExprID_opt(Element_ID el, AstContext ctx, OperatorCallSupplement suppl = {});

  /// returns a range expression for the element \ref id.
  /// \pre id identifies a Discrete_Range definition
  SgExpression&
  getDiscreteRangeID(Element_ID id, AstContext ctx);

  /// returns an expression from the Asis definition \ref el
  SgExpression&
  getDefinitionExpr(Element_Struct& el, AstContext ctx);

  /// returns an expression for the Asis definition
  /// \pre id identifies a Asis definition
  SgExpression&
  getDefinitionExprID(Element_ID id, AstContext ctx);

  /// returns an expression for a discrete subtype.
  /// \details
  ///    if this is converted to a type, the getDefinitionType (SgType.C) should be used instead.
  SgExpression&
  getDiscreteSubtypeExpr(Element_Struct& el, Definition_Struct& def, AstContext ctx);

  /// returns an expression for attribute defined in expr
  /// \param expr          the representative Asis struct
  /// \param ctx           the translation context
  /// \param argRangeSuppl an optional argument to inject the arguments from a function call
  SgAdaAttributeExp&
  getAttributeExpr(Expression_Struct& expr, AstContext ctx, ElemIdRange argRangeSuppl = {});

  /// returns an expression for an Asis element ID \ref id.
  SgAdaAttributeExp&
  getAttributeExprID(Element_ID id, AstContext ctx, ElemIdRange argRangeSuppl = {});

  /// returns an expression for the representation value of an enumerator
  SgExpression&
  getEnumRepresentationValue(Element_Struct& el, AstContext ctx);


  /// creates a call to subroutine expression \ref target, and passes params as arguments.
  /// \param tgtid the ASIS Id of the called target (function)
  /// \param params a list of ASIS Ids representing the arguments
  /// \param prefixCallSyntax true if an operator uses call syntax (i.e., "+"(1,2) instead of 1 + 2)
  /// \param objectCallSyntax true if a call uses OOP style syntax (i.e., x.init instead of init(x))
  /// \param ctx the translation context
  /// \return an expression representing the call
  SgExpression& createCall(Element_ID tgtid, ElemIdRange params, bool prefixCallSyntax, bool objectCallSyntax, AstContext ctx);

  /// queries the corresponding ROSE AST node for a built-in identifer
  SgNode*
  queryBuiltIn(AdaIdentifier adaIdent);

  /// queries the corresponding ROSE AST node in ROSE for a given Asis representation.
  /// @{
  SgNode*
  queryCorrespondingAstNode(Expression_Struct& asis, AstContext ctx);

  SgNode*
  queryCorrespondingAstNode(Element_Struct& asis, AstContext ctx);

  SgNode*
  queryCorrespondingAstNodeID(Element_ID asis, AstContext ctx);
  /// @}


  /// creates a sequence of SgExpressions from a sequence of Asis elements
  ///   (eiter expression or definition).
  struct ExprSeqCreator
  {
      explicit
      ExprSeqCreator(AstContext astctx)
      : ctx(astctx), elems()
      {}

      ExprSeqCreator(ExprSeqCreator&&)                 = default;
      ExprSeqCreator& operator=(ExprSeqCreator&&)      = default;

      // \todo the following copying functions should be removed post C++17
      // @{
      ExprSeqCreator(const ExprSeqCreator&)            = default;
      ExprSeqCreator& operator=(const ExprSeqCreator&) = default;
      // @}

      void operator()(Element_Struct& el);

      /// result read-out
      operator std::vector<SgExpression*> () &&
      {
        return std::move(elems);
      }

    private:
      AstContext                 ctx;
      std::vector<SgExpression*> elems;

      ExprSeqCreator() = delete;
  };

  /// creates an SgExprListExp object from a sequence of Asis expressions
  struct ArgListCreator
  {
      explicit
      ArgListCreator(AstContext astctx)
      : ctx(astctx), args()
      {}

      ArgListCreator(ArgListCreator&&)                 = default;
      ArgListCreator& operator=(ArgListCreator&&)      = default;

      // \todo the following copying functions should be removed post C++17
      // @{
      ArgListCreator(const ArgListCreator&)            = default;
      ArgListCreator& operator=(const ArgListCreator&) = default;
      // @}

      void operator()(Element_Struct& elem);

      /// result read-out
      operator SgExprListExp& ()
      {
        return mkExprListExp(args);
      }

      operator std::vector<SgExpression*> () &&
      {
        return std::move(args);
      }

    private:
      AstContext                 ctx;
      std::vector<SgExpression*> args;

      ArgListCreator() = delete;
  };

  /// creates a sequence of SgRangeExp objects from a Discrete_Range sequence
  struct RangeListCreator
  {
      explicit
      RangeListCreator(AstContext astctx)
      : ctx(astctx), lst()
      {}

      RangeListCreator(RangeListCreator&&)                 = default;
      RangeListCreator& operator=(RangeListCreator&&)      = default;

      // \todo the following copying functions should be removed post C++17
      // @{
      RangeListCreator(const RangeListCreator&)            = default;
      RangeListCreator& operator=(const RangeListCreator&) = default;
      // @}

      void operator()(Element_Struct& elem);

      /// result read-out
      operator SgExpressionPtrList () &&
      {
        return std::move(lst);
      }

    private:
      AstContext          ctx;
      SgExpressionPtrList lst;

      RangeListCreator() = delete;
  };
}


#endif /* _ADA_EXPRESSION */
