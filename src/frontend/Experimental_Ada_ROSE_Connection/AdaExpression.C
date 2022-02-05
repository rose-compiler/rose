#include "sage3basic.h"

#include <vector>
#include <boost/algorithm/string.hpp>

#include "sageGeneric.h"
#include "sageBuilder.h"
#include "sageInterfaceAda.h"

#include "AdaExpression.h"

#include "Ada_to_ROSE.h"
#include "AdaMaker.h"
#include "AdaStatement.h"
#include "AdaType.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"


namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Ada_ROSE_Translation
{

// \todo move to AdaType file and include that
SgType&
getDeclTypeID(Element_ID id, AstContext ctx);


namespace
{
  SgExpression&
  getArg(Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT(elem.Element_Kind == An_Association);

    Association_Struct& assoc      = elem.The_Union.Association;
    ADA_ASSERT(  assoc.Association_Kind == A_Parameter_Association
               || assoc.Association_Kind == A_Pragma_Argument_Association
               || assoc.Association_Kind == A_Generic_Association
               );

    switch (assoc.Association_Kind) {
    case A_Parameter_Association:
      logKind("A_Parameter_Association");
      break;
    case A_Pragma_Argument_Association:
      logKind("A_Pragma_Argument_Association");
      break;
    case A_Generic_Association:
      logKind("A_Generic_Association");
      break;
    default:
      ROSE_ABORT();
    }

    SgExpression&       arg        = getExprID(assoc.Actual_Parameter, ctx);
    Element_Struct*     formalParm = retrieveAsOpt(elemMap(), assoc.Formal_Parameter);

    /* unused fields (A_Parameter_Association)
       bool                   Is_Normalized
       bool                   Is_Defaulted_Association
    */

    if (!formalParm) return arg;

    ADA_ASSERT(formalParm->Element_Kind == An_Expression);

    Expression_Struct&  formalName = formalParm->The_Union.Expression;
    ADA_ASSERT(formalName.Expression_Kind == An_Identifier);

    logKind("An_Identifier");
    SgExpression&       namedArg = SG_DEREF(sb::buildActualArgumentExpression(formalName.Name_Image, &arg));

    attachSourceLocation(namedArg, elem, ctx);
    return namedArg;
  }
}

namespace
{
  struct AdaCallBuilder : sg::DispatchHandler<SgExpression*>
  {
      using base = sg::DispatchHandler<SgExpression*>;

      AdaCallBuilder(ElemIdRange params, bool useCallSyntax, AstContext astctx)
      : base(nullptr), range(params), ctx(astctx)
      {}

      ArgListCreator computeArguments()
      {
        return traverseIDs(range, elemMap(), ArgListCreator{ctx});
      }

      void mkCall(SgExpression& n)
      {
        SgExprListExp& arglst = computeArguments();

        res = sb::buildFunctionCallExp(&n, &arglst);
      }

      SgAdaInheritedFunctionSymbol*
      inheritedFunctionSymbol(SgType* ty, SgFunctionSymbol& origSymbol)
      {
        using key_t = std::pair<const SgFunctionDeclaration*, const SgTypedefType*>;

        const SgDeclarationStatement* tydcl = si::ada::baseDeclaration(ty);
        const SgTypedefDeclaration*   tydefDcl = isSgTypedefDeclaration(tydcl);

        if (tydefDcl == nullptr)
          return nullptr;

        key_t key{origSymbol.get_declaration(), tydefDcl->get_type()};

        return findNode(inheritedSymbols(), key);
      }

      SgFunctionSymbol&
      functionSymbol( const std::vector<si::ada::PrimitiveParameterDesc>& primitiveArgs,
                      const SgExprListExp& args,
                      SgFunctionSymbol& implSymbol
                    )
      {
        using PrimitiveParmIterator = std::vector<si::ada::PrimitiveParameterDesc>::const_iterator;
        using ArgumentIterator      = SgExpressionPtrList::const_iterator;

        if (primitiveArgs.size() == 0)
          return implSymbol;

        const SgExpressionPtrList& arglst      = args.get_expressions();
        const size_t               posArgLimit = si::ada::positionalArgumentLimit(args);
        PrimitiveParmIterator      aa          = primitiveArgs.begin();
        PrimitiveParmIterator      zz          = primitiveArgs.end();

        // check all positional arguments
        while ((aa != zz) && (aa->pos() < posArgLimit))
        {
          const SgExpression* arg = arglst.at(aa->pos());

          if (SgAdaInheritedFunctionSymbol* inhSym = inheritedFunctionSymbol(arg->get_type(), implSymbol))
            return *inhSym;

          ++aa;
        }

        ROSE_ASSERT(posArgLimit <= arglst.size());
        ArgumentIterator firstNamed = arglst.begin() + posArgLimit;
        ArgumentIterator argLimit   = arglst.end();

        // check all named arguments
        while (aa != zz)
        {
          const std::string parmName = SG_DEREF(aa->name()).get_name();
          auto              sameNamePred = [&parmName](const SgExpression* arg) -> bool
                                           {
                                             const SgActualArgumentExpression* actarg = isSgActualArgumentExpression(arg);

                                             ROSE_ASSERT(actarg);
                                             return parmName == std::string{actarg->get_argument_name()};
                                           };
          ArgumentIterator argpos   = std::find_if(firstNamed, argLimit, sameNamePred);

          ++aa;

          if (argpos == argLimit)
            continue;

          if (SgAdaInheritedFunctionSymbol* inhSym = inheritedFunctionSymbol((*argpos)->get_type(), implSymbol))
            return *inhSym;
        }

        return implSymbol;
      }

      SgFunctionSymbol&
      functionSymbol(SgFunctionDeclaration& dcl, SgFunctionSymbol& fnsym, SgExprListExp& args)
      {
        auto primitiveArgs = si::ada::primitiveParameterPositions(fnsym.get_declaration());

        return functionSymbol(primitiveArgs, args, fnsym);
      }

      void handle(SgNode& n)       { SG_UNEXPECTED_NODE(n); }

      // default
      void handle(SgExpression& n) { mkCall(n); }

      void handle(SgFunctionRefExp& n)
      {
        SgExprListExp& arglst = computeArguments();

        if (SgFunctionDeclaration* funDcl = n.getAssociatedFunctionDeclaration())
        {
          SgFunctionSymbol& origSym = SG_DEREF(n.get_symbol());
          SgFunctionSymbol& funSym  = functionSymbol(*funDcl, origSym, arglst);

          if (&origSym != &funSym)
            n.set_symbol(&funSym);
        }

        res = sb::buildFunctionCallExp(&n, &arglst);
      }

      void handle(SgUnaryOp& n)
      {
        // computed target ?
        if (n.get_operand() != nullptr)
        {
          mkCall(n);
          return;
        }

        ADA_ASSERT(range.size() == 1);
        std::vector<SgExpression*> args = computeArguments();

        ADA_ASSERT(args.size() == 1);
        n.set_operand(args[0]);
        res = &n;
      }

      void handle(SgBinaryOp& n)
      {
        // lhs and rhs must both be null or not-null
        ADA_ASSERT((n.get_lhs_operand() == nullptr) == (n.get_rhs_operand() == nullptr));

        // computed target ?
        if (n.get_lhs_operand() != nullptr)
        {
          mkCall(n);
          return;
        }

        ADA_ASSERT(range.size() == 2);
        std::vector<SgExpression*> args = computeArguments();

        ADA_ASSERT(args.size() == 2);
        n.set_lhs_operand(args[0]);
        n.set_rhs_operand(args[1]);
        res = &n;
      }

    private:
      ElemIdRange range;
      bool        callSyntax;
      AstContext  ctx;
  };

  struct ArrayAggregateCreator
  {
      ArrayAggregateCreator(bool namedInit, AstContext astctx)
      : namedElements(namedInit), ctx(astctx), elems()
      {}

      ArrayAggregateCreator(ArrayAggregateCreator&&)                 = default;
      ArrayAggregateCreator& operator=(ArrayAggregateCreator&&)      = default;

      // \todo the following copying functions should be deleted post C++17
      // @{
      ArrayAggregateCreator(const ArrayAggregateCreator&)            = default;
      ArrayAggregateCreator& operator=(const ArrayAggregateCreator&) = default;
      // @}

      void operator()(Element_Struct& el);

      /// result read-out
      operator std::vector<SgExpression*> () &&
      {
        return std::move(elems);
      }

    private:
      const bool                 namedElements;
      AstContext                 ctx;
      std::vector<SgExpression*> elems;

      ArrayAggregateCreator() = delete;
  };

  void ArrayAggregateCreator::operator()(Element_Struct& el)
  {
    ADA_ASSERT(el.Element_Kind == An_Association);

    Association_Struct&        assoc  = el.The_Union.Association;
    ADA_ASSERT(assoc.Association_Kind == An_Array_Component_Association);
    logKind("An_Array_Component_Association");

    SgExpression&              init   = getExprID(assoc.Component_Expression, ctx);
    SgExpression*              sgnode = &init;
    ElemIdRange                range  = idRange(assoc.Array_Component_Choices);

    ADA_ASSERT(namedElements || range.size() < 2);

    if (!range.empty())
    {
      std::vector<SgExpression*> exprs = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
      SgExprListExp&             choicelst = mkExprListExp(exprs);

      sgnode = &mkAdaNamedInitializer(choicelst, init);
      ADA_ASSERT(choicelst.get_parent());
    }

    ADA_ASSERT(sgnode);
    attachSourceLocation(*sgnode, el, ctx);
    elems.push_back(sgnode);
  }


  struct RecordAggregateCreator
  {
      explicit
      RecordAggregateCreator(AstContext astctx)
      : ctx(astctx), elems()
      {}

      RecordAggregateCreator(RecordAggregateCreator&&)                 = default;
      RecordAggregateCreator& operator=(RecordAggregateCreator&&)      = default;

      // \todo the following copying functions should be deleted post C++17
      // @{
      RecordAggregateCreator(const RecordAggregateCreator&)            = default;
      RecordAggregateCreator& operator=(const RecordAggregateCreator&) = default;
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

      RecordAggregateCreator() = delete;
  };

  void RecordAggregateCreator::operator()(Element_Struct& el)
  {
    ADA_ASSERT(el.Element_Kind == An_Association);

    Association_Struct&        assoc = el.The_Union.Association;
    ADA_ASSERT(assoc.Association_Kind == A_Record_Component_Association);
    logKind("A_Record_Component_Association");

    SgExpression&              init = getExprID(assoc.Component_Expression, ctx);
    SgExpression*              sgnode = &init;
    ElemIdRange                range = idRange(assoc.Record_Component_Choices);

    if (!range.empty())
    {
      std::vector<SgExpression*> exprs = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
      SgExprListExp&             choicelst = mkExprListExp(exprs);

      sgnode = &mkAdaNamedInitializer(choicelst, init);
      ADA_ASSERT(choicelst.get_parent());
    }

    attachSourceLocation(SG_DEREF(sgnode), el, ctx);
    elems.push_back(sgnode);
  }

  // wrapper uses homogeneous return types instead of covariant ones
  template <class R, R* (*mkexp) (SgExpression*, SgExpression*)>
  SgExpression* mk2_wrapper()
  {
    return mkexp(nullptr, nullptr);
  }

  // wrapper uses homogeneous return types instead of covariant ones
  template <class R, R* (*mkexp) (SgExpression*)>
  SgExpression* mk1_wrapper()
  {
    return mkexp(nullptr);
  }


  SgExpression&
  getOperator(Expression_Struct& expr, AstContext ctx)
  {
    using MkWrapperFn = std::function<SgExpression*()>;
    using OperatorMakerMap = std::map<Operator_Kinds, std::pair<const char*, MkWrapperFn> >;

    static const OperatorMakerMap makerMap =
    { { An_And_Operator,                  {"An_And_Operator",                  mk2_wrapper<SgBitAndOp,         sb::buildBitAndOp> }},
      { An_Or_Operator,                   {"An_Or_Operator",                   mk2_wrapper<SgBitOrOp,          sb::buildBitOrOp> }},
      { An_Xor_Operator,                  {"An_Xor_Operator",                  mk2_wrapper<SgBitXorOp,         sb::buildBitXorOp> }},
      { An_Equal_Operator,                {"An_Equal_Operator",                mk2_wrapper<SgEqualityOp,       sb::buildEqualityOp> }},
      { A_Not_Equal_Operator,             {"A_Not_Equal_Operator",             mk2_wrapper<SgNotEqualOp,       sb::buildNotEqualOp> }},
      { A_Less_Than_Operator,             {"A_Less_Than_Operator",             mk2_wrapper<SgLessThanOp,       sb::buildLessThanOp> }},
      { A_Less_Than_Or_Equal_Operator,    {"A_Less_Than_Or_Equal_Operator",    mk2_wrapper<SgLessOrEqualOp,    sb::buildLessOrEqualOp> }},
      { A_Greater_Than_Operator,          {"A_Greater_Than_Operator",          mk2_wrapper<SgGreaterThanOp,    sb::buildGreaterThanOp> }},
      { A_Greater_Than_Or_Equal_Operator, {"A_Greater_Than_Or_Equal_Operator", mk2_wrapper<SgGreaterOrEqualOp, sb::buildGreaterOrEqualOp> }},
      { A_Plus_Operator,                  {"A_Plus_Operator",                  mk2_wrapper<SgAddOp,            sb::buildAddOp> }},
      { A_Minus_Operator,                 {"A_Minus_Operator",                 mk2_wrapper<SgSubtractOp,       sb::buildSubtractOp> }},
      { A_Concatenate_Operator,           {"A_Concatenate_Operator",           mk2_wrapper<SgConcatenationOp,  sb::buildConcatenationOp> }},
      { A_Unary_Plus_Operator,            {"A_Unary_Plus_Operator",            mk1_wrapper<SgUnaryAddOp,       sb::buildUnaryAddOp> }},
      { A_Unary_Minus_Operator,           {"A_Unary_Minus_Operator",           mk1_wrapper<SgMinusOp,          sb::buildMinusOp> }},
      { A_Multiply_Operator,              {"A_Multiply_Operator",              mk2_wrapper<SgMultiplyOp,       sb::buildMultiplyOp> }},
      { A_Divide_Operator,                {"A_Divide_Operator",                mk2_wrapper<SgDivideOp,         sb::buildDivideOp> }},
      { A_Mod_Operator,                   {"A_Mod_Operator",                   mk2_wrapper<SgModOp,            sb::buildModOp> }},
      { A_Rem_Operator,                   {"A_Rem_Operator",                   mk2_wrapper<SgRemOp,            buildRemOp> }},
      { An_Exponentiate_Operator,         {"An_Exponentiate_Operator",         mk2_wrapper<SgExponentiationOp, sb::buildExponentiationOp> }},
      { An_Abs_Operator,                  {"An_Abs_Operator",                  mk1_wrapper<SgAbsOp,            buildAbsOp> }},
      { A_Not_Operator,                   {"A_Not_Operator",                   mk1_wrapper<SgNotOp,            sb::buildNotOp> }},
    };

    ADA_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    OperatorMakerMap::const_iterator pos = makerMap.find(expr.Operator_Kind);

    if (pos != makerMap.end())
    {
      logKind(pos->second.first);
      return SG_DEREF(pos->second.second());
    }

    ADA_ASSERT(expr.Operator_Kind != Not_An_Operator);

    /* unused fields:
         Defining_Name_ID      Corresponding_Name_Definition;
         Defining_Name_List    Corresponding_Name_Definition_List; // Only >1 if the expression in a pragma is ambiguous
         Element_ID            Corresponding_Name_Declaration; // Decl or stmt
         Defining_Name_ID      Corresponding_Generic_Element;
    */
    return SG_DEREF(sb::buildOpaqueVarRefExp(expr.Name_Image, &ctx.scope()));
  }


  /// converts enum values to SgExpressions
  /// \note currently True and False are handled separately, because
  ///       their definition in package Standard is not seen.
  SgExpression&
  getEnumLiteral(Expression_Struct& expr, AstContext ctx)
  {
    ADA_ASSERT(expr.Expression_Kind == An_Enumeration_Literal);

    SgExpression* res = NULL;

    if (SgInitializedName* enumitem = findFirst(asisVars(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
    {
      SgEnumType&        enumtype = SG_DEREF( isSgEnumType(enumitem->get_type()) );
      SgEnumDeclaration& enumdecl = SG_DEREF( isSgEnumDeclaration(enumtype.get_declaration()) );

      // res = sb::buildEnumVal_nfi(-1, &enumdecl, enumitem->get_name());
      res = &ctx.enumBuilder()(enumdecl, *enumitem);
    }
    else
    {
      std::string   enumstr{expr.Name_Image};

      boost::to_upper(enumstr);

      // \todo replace with actual enum values
      if (enumstr == "TRUE")
        res = sb::buildBoolValExp(1);
      else if (enumstr == "FALSE")
        res = sb::buildBoolValExp(0);
      else
      {
        logWarn() << "unable to find definition for enum val " << enumstr
                  << std::endl;

        res = sb::buildStringVal(enumstr);
      }
    }

    return SG_DEREF(res);
  }

  /// defines ROSE AST types for which we do not generate scope qualification
  struct RoseRequiresScopeQual : sg::DispatchHandler<bool>
  {
    void handle(const SgNode& n)               { SG_UNEXPECTED_NODE(n); }

    void handle(const SgDeclarationStatement&) { res = true; }
    void handle(const SgAdaTaskSpecDecl&)      { res = false; }
    void handle(const SgAdaProtectedSpecDecl&) { res = false; }
    void handle(const SgAdaPackageSpecDecl&)   { res = false; }
    void handle(const SgImportStatement&)      { res = false; }
    //~ void handle(const SgFunctionDeclaration&)  { res = false; }
    void handle(const SgAdaRenamingDecl&)      { res = false; }
  };


  /// tests whether ROSE represents the prefix expression
  ///   (e.g., true for objects, false for scope-qualification)
  bool roseRequiresPrefixID(Element_ID el, AstContext ctx)
  {
    Element_Struct&    elem = retrieveAs(elemMap(), el);
    ADA_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& expr = elem.The_Union.Expression;

    if (expr.Expression_Kind == An_Identifier)
    {
      const SgDeclarationStatement* dcl = getDecl_opt(expr, ctx);

      // \note getDecl_opt does not retrieve variable declarations
      //       => assuming a is a variable of record: a.x (the dcl for a would be nullptr)
      return dcl == nullptr || sg::dispatch(RoseRequiresScopeQual{}, dcl);
      //~ return dcl != nullptr && sg::dispatch(RoseRequiresScopeQual(), dcl);
    }

    if (expr.Expression_Kind == A_Selected_Component)
    {
      return    roseRequiresPrefixID(expr.Prefix, ctx)
             || roseRequiresPrefixID(expr.Selector, ctx);
    }

    if (expr.Expression_Kind == An_Indexed_Component)
    {
      // \todo should this always return true (like the cases below)?
      return roseRequiresPrefixID(expr.Prefix, ctx);
    }

    if (  (expr.Expression_Kind == An_Explicit_Dereference)
       || (expr.Expression_Kind == A_Function_Call)
       )
      return true;

    logWarn() << "roseRequiresPrefixID: untested expression-kind: "
              << expr.Expression_Kind
              << std::endl;
    ADA_ASSERT(!FAIL_ON_ERROR(ctx) && "untested expression-kind");
    return true;
  }

  struct ExprRefMaker : sg::DispatchHandler<SgExpression*>
  {
      using base = sg::DispatchHandler<SgExpression*>;

      explicit
      ExprRefMaker(AstContext astctx)
      : base(), ctx(astctx)
      {}

      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

      void handle(SgDeclarationStatement& n)
      {
        logError() << "ExprRefMaker: " << typeid(n).name() << std::endl;

        res = sb::buildIntVal();
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
      }

      // void handle(SgImportStatement& n)

      void handle(SgFunctionDeclaration& n)    { res = sb::buildFunctionRefExp(&n); }
      void handle(SgAdaRenamingDecl& n)        { res = &mkAdaRenamingRefExp(n); }
      void handle(SgAdaTaskSpecDecl& n)        { res = &mkAdaTaskRefExp(n); }
      void handle(SgAdaProtectedSpecDecl& n)   { res = &mkAdaProtectedRefExp(n); }
      void handle(SgAdaGenericDecl& n)         { res = &mkAdaUnitRefExp(n); }
      void handle(SgAdaGenericInstanceDecl& n) { res = &mkAdaUnitRefExp(n); }
      void handle(SgAdaPackageSpecDecl& n)     { res = &mkAdaUnitRefExp(n); }
      void handle(SgAdaTaskTypeDecl& n)        { res = sb::buildTypeExpression(n.get_type()); }
      void handle(SgAdaProtectedTypeDecl& n)   { res = sb::buildTypeExpression(n.get_type()); }

    private:
      AstContext ctx;
  };

  struct TypeRefMaker : sg::DispatchHandler<SgExpression*>
  {
      using base = sg::DispatchHandler<SgExpression*>;

      explicit
      TypeRefMaker(AstContext astctx)
      : base(), ctx(astctx)
      {}

      void set(SgType* ty);

      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

      void handle(SgDeclarationStatement& n)
      {
        logError() << "TypeRefMaker: " << typeid(n).name() << std::endl;

        set(sb::buildVoidType());
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
      }

      // void handle(SgImportStatement& n)

      void handle(SgClassDeclaration& n)   { set(n.get_type()); }
      void handle(SgTypedefDeclaration& n) { set(n.get_type()); }
      void handle(SgEnumDeclaration& n)    { set(n.get_type()); }
      void handle(SgAdaFormalTypeDecl& n)  { set(n.get_type()); }

    private:
      AstContext ctx;
  };

  void TypeRefMaker::set(SgType* ty)
  {
    ADA_ASSERT(ty);
    res = sb::buildTypeExpression(ty);
    ADA_ASSERT(res);
  }


  SgExprListExp&
  getRecordAggregate(Element_Struct& elem, Expression_Struct& expr, AstContext ctx)
  {
    ADA_ASSERT(  expr.Expression_Kind == A_Record_Aggregate
              || expr.Expression_Kind == An_Extension_Aggregate
              );

    ElemIdRange                range  = idRange(expr.Record_Component_Associations);
    std::vector<SgExpression*> components = traverseIDs(range, elemMap(), RecordAggregateCreator{ctx});
    SgExprListExp&             sgnode = mkExprListExp(components);

    attachSourceLocation(sgnode, elem, ctx);
    return sgnode;
  }

  SgExprListExp&
  getArrayAggregate(Element_Struct& elem, Expression_Struct& expr, AstContext ctx)
  {
    ADA_ASSERT(  expr.Expression_Kind == A_Named_Array_Aggregate
              || expr.Expression_Kind == A_Positional_Array_Aggregate
              );

    const bool namedAggregate = expr.Expression_Kind == A_Named_Array_Aggregate;

    logKind(namedAggregate ? "A_Named_Array_Aggregate" : "A_Positional_Array_Aggregate");

    ElemIdRange                range  = idRange(expr.Array_Component_Associations);
    std::vector<SgExpression*> components = traverseIDs(range, elemMap(), ArrayAggregateCreator{namedAggregate, ctx});
    SgExprListExp&             sgnode = mkExprListExp(components);
    attachSourceLocation(sgnode, elem, ctx);

    return sgnode;
  }


} // anonymous


SgAdaAttributeExp&
getAttributeExpr(Expression_Struct& expr, AstContext ctx)
{
  ADA_ASSERT(expr.Expression_Kind == An_Attribute_Reference);

  SgAdaAttributeExp* res = nullptr;
  NameData           name = getNameID(expr.Attribute_Designator_Identifier, ctx);
  SgExpression&      obj = getExprID(expr.Prefix, ctx);

  switch (expr.Attribute_Kind)
  {
    // attributes with optional expression list argument

    case A_First_Attribute:            // 3.5(12), 3.6.2(3), K(68), K(70)
    case A_Length_Attribute:           // 3.6.2(9), K(117)
    case An_Unknown_Attribute:          // Unknown to ASIS
    case An_Implementation_Defined_Attribute:  // Reference Manual, Annex M
      logInfo() << "untested attribute created: " << expr.Attribute_Kind
                << "  attr-name: " << name.fullName
                << std::endl;

    /* fall through */
    case A_Last_Attribute:            // 3.5(13), 3.6.2(5), K(102), K(104)
    case A_Range_Attribute:            // 3.5(14), 3.6.2(7), K(187), ú(189)
    {
      ElemIdRange                range = idRange(expr.Attribute_Designator_Expressions);
      std::vector<SgExpression*> exprs = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
      SgExprListExp&             args  = mkExprListExp(exprs);

      res = &mkAdaAttributeExp(obj, name.fullName, args);
      break;
    }

    // attributes with empty expression list argument

    case An_Access_Attribute:          // 3.10.2(24), 3.10.2(32), K(2), K(4)
    case An_Address_Attribute:         // 13.3(11), J.7.1(5), K(6)
    case An_Adjacent_Attribute:        // A.5.3(48), K(8)
    case An_Aft_Attribute:            // 3.5.10(5), K(12)
    case An_Alignment_Attribute:       // 13.3(23), K(14)
    case A_Base_Attribute:            // 3.5(15), K(17)
    case A_Bit_Order_Attribute:        // 13.5.3(4), K(19)
    case A_Body_Version_Attribute:      // E.3(4), K(21)
    case A_Callable_Attribute:         // 9.9(2), K(23)
    case A_Caller_Attribute:           // C.7.1(14), K(25)
    case A_Ceiling_Attribute:          // A.5.3(33), K(27)
    case A_Class_Attribute:            // 3.9(14), 7.3.1(9), K(31), K(34)
    case A_Component_Size_Attribute:    // 13.3(69), K(36)
    case A_Compose_Attribute:          // A.5.3(24), K(38)
    case A_Constrained_Attribute:      // 3.7.2(3), J.4(2), K(42)
    case A_Copy_Sign_Attribute:        // A.5.3(51), K(44)
    case A_Count_Attribute:            // 9.9(5), K(48)
    case A_Definite_Attribute:         // 12.5.1(23), K(50)
    case A_Delta_Attribute:            // 3.5.10(3), K(52)
    case A_Denorm_Attribute:           // A.5.3(9), K(54)
    case A_Digits_Attribute:           // 3.5.8(2), 3.5.10(7), K(56), K(58)
    case An_Exponent_Attribute:        // A.5.3(18), K(60)
    case An_External_Tag_Attribute:     // 13.3(75), K(64)
    case A_First_Bit_Attribute:        // 13.5.2(3), K(72)
    case A_Floor_Attribute:            // A.5.3(30), K(74)
    case A_Fore_Attribute:            // 3.5.10(4), K(78)
    case A_Fraction_Attribute:         // A.5.3(21), K(80)
    case An_Identity_Attribute:        // 11.4.1(9), C.7.1(12), K(84), K(86)
    case An_Image_Attribute:           // 3.5(35), K(88)
    case An_Input_Attribute:           // 13.13.2(22), 13.13.2(32), K(92), K(96)
    case A_Last_Bit_Attribute:         // 13.5.2(4), K(106)
    case A_Leading_Part_Attribute:      // A.5.3(54), K(108)
    case A_Machine_Attribute:          // A.5.3(60), K(119)
    case A_Machine_Emax_Attribute:      // A.5.3(8), K(123)
    case A_Machine_Emin_Attribute:      // A.5.3(7), K(125)
    case A_Machine_Mantissa_Attribute:  // A.5.3(6), K(127)
    case A_Machine_Overflows_Attribute: // A.5.3(12), A.5.4(4), K(129), K(131)
    case A_Machine_Radix_Attribute:     // A.5.3(2), A.5.4(2), K(133), K(135)
    case A_Machine_Rounds_Attribute:    // A.5.3(11), A.5.4(3), K(137), K(139)
    case A_Max_Attribute:             // 3.5(19), K(141)
    case A_Max_Size_In_Storage_Elements_Attribute: //   13.11.1(3), K(145)
    case A_Min_Attribute:             // 3.5(16), K(147)
    case A_Model_Attribute:            // A.5.3(68), G.2.2(7), K(151)
    case A_Model_Emin_Attribute:       // A.5.3(65), G.2.2(4), K(155)
    case A_Model_Epsilon_Attribute:     // A.5.3(66), K(157)
    case A_Model_Mantissa_Attribute:    // A.5.3(64), G.2.2(3), K(159)
    case A_Model_Small_Attribute:      // A.5.3(67), K(161)
    case A_Modulus_Attribute:          // 3.5.4(17), K(163)
    case An_Output_Attribute:          // 13.13.2(19), 13.13.2(29), K(165), K(169)
    case A_Partition_ID_Attribute:      // E.1(9), K(173)
    case A_Pos_Attribute:             // 3.5.5(2), K(175)
    case A_Position_Attribute:         // 13.5.2(2), K(179)
    case A_Pred_Attribute:            // 3.5(25), K(181)
    case A_Read_Attribute:            // 13.13.2(6), 13.13.2(14), K(191), K(195)
    case A_Remainder_Attribute:        // A.5.3(45), K(199)
    case A_Round_Attribute:            // 3.5.10(12), K(203)
    case A_Rounding_Attribute:         // A.5.3(36), K(207)
    case A_Safe_First_Attribute:       // A.5.3(71), G.2.2(5), K(211)
    case A_Safe_Last_Attribute:        // A.5.3(72), G.2.2(6), K(213)
    case A_Scale_Attribute:            // 3.5.10(11), K(215)
    case A_Scaling_Attribute:          // A.5.3(27), K(217)
    case A_Signed_Zeros_Attribute:      // A.5.3(13), K(221)
    case A_Size_Attribute:            // 13.3(40), 13.3(45), K(223), K(228)
    case A_Small_Attribute:            // 3.5.10(2), K(230)
    case A_Storage_Pool_Attribute:      // 13.11(13), K(232)
    case A_Storage_Size_Attribute:      // 13.3(60), 13.11(14), J.9(2), K(234),
                                       //                             K(236)
    case A_Succ_Attribute:            // 3.5(22), K(238)
    case A_Tag_Attribute:             // 3.9(16), 3.9(18), K(242), K(244)
    case A_Terminated_Attribute:       // 9.9(3), K(246)
    case A_Truncation_Attribute:       // A.5.3(42), K(248)
    case An_Unbiased_Rounding_Attribute: // A.5.3(39), K(252)
    case An_Unchecked_Access_Attribute:  // 13.10(3), H.4(18), K(256)
    case A_Val_Attribute:              // 3.5.5(5), K(258)
    case A_Valid_Attribute:            // 13.9.2(3), H(6), K(262)
    case A_Value_Attribute:            // 3.5(52), K(264)
    case A_Version_Attribute:           // E.3(3), K(268)
    case A_Wide_Image_Attribute:        // 3.5(28), K(270)
    case A_Wide_Value_Attribute:        // 3.5(40), K(274)
    case A_Wide_Width_Attribute:        // 3.5(38), K(278)
    case A_Width_Attribute:            // 3.5(39), K(280)
    case A_Write_Attribute:            // 13.13.2(3), 13.13.2(11), K(282), K(286)

    //  |A2005 start
    //  New Ada 2005 attributes. To be alphabetically ordered later
    case A_Machine_Rounding_Attribute:
    case A_Mod_Attribute:
    case A_Priority_Attribute:
    case A_Stream_Size_Attribute:
    case A_Wide_Wide_Image_Attribute:
    case A_Wide_Wide_Value_Attribute:
    case A_Wide_Wide_Width_Attribute:
    //  |A2005 end

    //  |A2012 start
    //  New Ada 2012 attributes. To be alphabetically ordered later
    case A_Max_Alignment_For_Allocation_Attribute:
    case An_Overlaps_Storage_Attribute:
    //  |A2012 end
      {
        logInfo() << "untested attribute created: " << expr.Attribute_Kind
                  << std::endl;

        res = &mkAdaAttributeExp(obj, name.fullName, mkExprListExp());
        break;
      }

    // failure kinds

    case Not_An_Attribute:             // An unexpected element
    default:
      {
        logError() << "unknown expression attribute: " << expr.Attribute_Kind
                   << std::endl;

        res = &mkAdaAttributeExp(obj, "ErrorAttr:" + name.fullName, mkExprListExp());
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
      }
  }

  return SG_DEREF(res);
}

SgAdaAttributeExp&
getAttributeExprID(Element_ID el, AstContext ctx)
{
  Element_Struct& elem = retrieveAs(elemMap(), el);

  ADA_ASSERT(elem.Element_Kind == An_Expression);
  SgAdaAttributeExp& sgnode = getAttributeExpr(elem.The_Union.Expression, ctx);

  attachSourceLocation(sgnode, elem, ctx);
  return sgnode;
}

namespace
{
  SgExprListExp& createExprListExpIfNeeded(SgExpression& exp)
  {
    SgExprListExp* res = isSgExprListExp(&exp);

    return (res == nullptr) ? mkExprListExp({&exp}) : *res;
  }


  SgExpression&
  getExprID_undecorated(Element_ID el, AstContext ctx);


  /// creates expressions from elements, but does not decorate
  ///   aggregates with SgAggregateInitializers
  SgExpression&
  getExpr_undecorated(Element_Struct& elem, AstContext ctx)
  {
    ADA_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& expr      = elem.The_Union.Expression;
    SgExpression*      res       = NULL;

    switch (expr.Expression_Kind)
    {
      case An_Identifier:                             // 4.1
        {
          logKind("An_Identifier");

          if (SgInitializedName* var = findFirst(asisVars(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
          {
            res = sb::buildVarRefExp(var, &ctx.scope());
          }
          else if (SgDeclarationStatement* dcl = getDecl_opt(expr, ctx))
          {
            res = sg::dispatch(ExprRefMaker{ctx}, dcl);
          }
          else if (SgInitializedName* exc = findFirst(asisExcps(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
          {
            res = &mkExceptionRef(*exc, ctx.scope());
          }
          else if (SgDeclarationStatement* tydcl = findFirst(asisTypes(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
          {
            res = sg::dispatch(TypeRefMaker{ctx}, tydcl);
          }
          else if (SgType* ty = findFirst(adaTypes(), AdaIdentifier{expr.Name_Image}))
          {
            res = sb::buildTypeExpression(ty);
          }
          else
          {
            // \todo check why the name remained unresolved
            res = &mkUnresolvedName(expr.Name_Image, ctx.scope());
          }

          /* unused fields: (Expression_Struct)
               ** depends on the branch
               Defining_Name_ID      Corresponding_Generic_Element;
          */
          break;
        }

      case A_Function_Call:                           // 4.1
        {
          logKind("A_Function_Call");

          logTrace() << "function call "
                     << expr.Is_Prefix_Notation << " "
                     << expr.Is_Prefix_Call
                     << std::endl;

          SgExpression&           target = getExprID(expr.Prefix, ctx);
          ElemIdRange             range  = idRange(expr.Function_Call_Parameters);

          // distinguish between operators and calls
          res = &createCall(target, range, expr.Is_Prefix_Call, ctx);

          /* unused fields:
             Expression_Struct
               Expression_ID         Prefix;
               bool                  Is_Prefix_Notation;
               bool                  Is_Generalized_Reference;
               bool                  Is_Dispatching_Call;
               bool                  Is_Call_On_Dispatching_Operation;
          */
          break;
        }

      case An_Integer_Literal:                        // 2.4
        {
          logKind("An_Integer_Literal");

          res = &mkAdaIntegerLiteral(expr.Value_Image);

          /* unused fields: (Expression_Struct)
               enum Attribute_Kinds  Attribute_Kind
          */
          break;
        }

      case A_Character_Literal:                       // 4.1
        {
          logKind("A_Character_Literal");
          res = &mkValue<SgCharVal>(expr.Name_Image);

          /* unused fields: (Expression_Struct)
               Defining_Name_ID      Corresponding_Name_Definition;
               Defining_Name_List    Corresponding_Name_Definition_List;
               Element_ID            Corresponding_Name_Declaration;
               Defining_Name_ID      Corresponding_Generic_Element
          */
          break;
        }

      case A_String_Literal:                          // 2.6
        {
          logKind("A_String_Literal");
          res = &mkValue<SgStringVal>(expr.Value_Image);
          break;
        }

      case A_Real_Literal:                            // 2.4.1
        {
          logKind("A_Real_Literal");
          res = &mkValue<SgLongDoubleVal>(expr.Value_Image);
          /* unused fields: (Expression_Struct)
               enum Attribute_Kinds  Attribute_Kind;
          */
          break;
        }

      case An_Operator_Symbol:                        // 4.1
        {
          logKind("An_Operator_Symbol");
          res = &getOperator(expr, ctx);
          /* unused fields:
             Defining_Name_ID      Corresponding_Name_Definition;
             Defining_Name_List    Corresponding_Name_Definition_List;
             Element_ID            Corresponding_Name_Declaration;
             Defining_Name_ID      Corresponding_Generic_Element;
          */
          break;
        }

      case An_Enumeration_Literal:                    // 4.1
        {
          logKind("An_Enumeration_Literal");
          res = &getEnumLiteral(expr, ctx);
          /* unused fields: (Expression_Struct)
             Defining_Name_ID      Corresponding_Name_Definition;
             Defining_Name_List    Corresponding_Name_Definition_List;
             Element_ID            Corresponding_Name_Declaration;
             Defining_Name_ID      Corresponding_Generic_Element;
          */
          break;
        }

      case An_Explicit_Dereference:                   // 4.1
        {
          logKind("An_Explicit_Dereference");

          SgExpression& exp = getExprID(expr.Prefix, ctx);

          // prefix calls are incorrectly unparsed as infix (check if data is avail in Asis)
          // in this case, parenthesis are missing.
          // if (isSgBinaryOp(&exp)) exp.set_need_paren(true);

          res = sb::buildPointerDerefExp(&exp);
          break;
        }

      case An_Indexed_Component:                      // 4.1.1
        {
          logKind("An_Indexed_Component");

          SgExpression&              prefix = getExprID(expr.Prefix, ctx);
          ElemIdRange                idxrange = idRange(expr.Index_Expressions);
          std::vector<SgExpression*> idxexpr = traverseIDs(idxrange, elemMap(), ExprSeqCreator{ctx});
          SgExpression&              indices = mkExprListExp(idxexpr);

          res = sb::buildPntrArrRefExp(&prefix, &indices);
          ADA_ASSERT(indices.get_parent());
          /* unused fields
             Declaration_ID        Corresponding_Called_Function; // An_Indexed_Component (Is_Generalized_Indexing == true) //ASIS 2012 // 4.1.1
             bool                  Is_Generalized_Indexing
          */
          break;
        }

      case A_Slice:                                   // 4.1.2
        {
          logKind("A_Slice");

          SgExpression&  prefix = getExprID(expr.Prefix, ctx);
          SgExpression&  range  = getDiscreteRangeID(expr.Slice_Range, ctx);
          SgExprListExp& index  = mkExprListExp({&range});

          // \todo consider introducing a ROSE IR node for array slices
          res = sb::buildPntrArrRefExp(&prefix, &index);
          /* unused fields
          */
          break;
        }

      case A_Selected_Component:                      // 4.1.3
        {
          logKind("A_Selected_Component");
          SgExpression& selector = getExprID(expr.Selector, ctx);

          // Check if the kind requires a prefix in ROSE,
          //   or if the prefix (scope qualification) is implied and
          //   generated by the backend.
          if (roseRequiresPrefixID(expr.Prefix, ctx))
          {
            SgExpression& prefix = getExprID(expr.Prefix, ctx);

            res = &mkSelectedComponent(prefix, selector);
          }
          else
          {
            res = &selector;
          }
          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case An_Attribute_Reference:
        {
          logKind("An_Attribute_Reference");

          res = &getAttributeExpr(expr, ctx);
          break;
        }

      case A_Positional_Array_Aggregate:              // 4.3
      case A_Named_Array_Aggregate:                   // 4.3
        {
          res = &getArrayAggregate(elem, expr, ctx);
          /*
          SgExprListExp& explst = getArrayAggregate(elem, expr, ctx);

          res = sb::buildAggregateInitializer(&explst);
          ADA_ASSERT(explst.get_parent());
          */
          break;
        }

      case A_Record_Aggregate:                        // 4.3
        {
          logKind("A_Record_Aggregate");

          res = &getRecordAggregate(elem, expr, ctx);
          break;
        }

      case An_Extension_Aggregate:                    // 4.3
        {
          logKind("An_Extension_Aggregate");

          SgExprListExp& elemlst   = getRecordAggregate(elem, expr, ctx);
          SgExpression&  parentexp = getExprID(expr.Extension_Aggregate_Expression, ctx);

          elemlst.prepend_expression(&mkAdaAncestorInitializer(parentexp));

          res = &elemlst;
          break;
        }


      case An_And_Then_Short_Circuit:                 // 4.4
        {
          logKind("An_And_Then_Short_Circuit");
          SgExpression& lhs = getExprID(expr.Short_Circuit_Operation_Left_Expression, ctx);
          SgExpression& rhs = getExprID(expr.Short_Circuit_Operation_Right_Expression, ctx);

          res = sb::buildAndOp(&lhs, &rhs);
          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case An_Or_Else_Short_Circuit:                  // 4.4
        {
          logKind("An_Or_Else_Short_Circuit");
          // \todo remove _opt once the asis connection fills in the list
          SgExpression& lhs = getExprID_opt(expr.Short_Circuit_Operation_Left_Expression, ctx);
          SgExpression& rhs = getExprID_opt(expr.Short_Circuit_Operation_Right_Expression, ctx);

          res = sb::buildOrOp(&lhs, &rhs);
          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case A_Parenthesized_Expression:                // 4.4
        {
          logKind("A_Parenthesized_Expression");

          // \todo remove _opt when the asis connection implements A_Parenthesized_Expression
          res = &getExprID_opt(expr.Expression_Parenthesized, ctx);
          res->set_need_paren(true);

          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case A_Null_Literal:                            // 4.4
        {
          logKind("A_Null_Literal");

          res = sb::buildNullptrValExp();
          break;
        }

      case An_In_Membership_Test:                     // 4.4  Ada 2012
      case A_Not_In_Membership_Test:                  // 4.4  Ada 2012
        {
          const bool inTest = expr.Expression_Kind == An_In_Membership_Test;

          logKind(inTest ? "An_In_Membership_Test" : "A_Not_In_Membership_Test");

          SgExpression&              test = getExprID(expr.Membership_Test_Expression, ctx);
          ElemIdRange                range = idRange(expr.Membership_Test_Choices);
          std::vector<SgExpression*> choices = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
          SgExpression&              choiceexp = mkChoiceExpIfNeeded(std::move(choices));

          res = inTest ? static_cast<SgExpression*>(sb::buildMembershipOp(&test, &choiceexp))
                       : sb::buildNonMembershipOp(&test, &choiceexp)
                       ;
          break;
        }

      case A_Qualified_Expression:                    // 4.7
      case A_Type_Conversion:                         // 4.6
        {
          const bool isConv = expr.Expression_Kind == A_Type_Conversion;

          logKind(isConv ? "A_Type_Conversion" : "A_Qualified_Expression");

          SgExpression& exp = getExprID(expr.Converted_Or_Qualified_Expression, ctx);
          SgType&       ty  = getDeclTypeID(expr.Converted_Or_Qualified_Subtype_Mark, ctx);

          res = isConv ? &mkCastExp(exp, ty)
                       : &mkQualifiedExp(exp, ty);

          /* unused fields: (Expression_Struct)
               Expression_ID         Predicate;
          */
          break;
        }

      case An_Allocation_From_Subtype:                // 4.8
        {
          logKind("An_Allocation_From_Subtype");

          SgType& ty = getDefinitionTypeID(expr.Allocator_Subtype_Indication, ctx);

          res = &mkNewExp(ty);

          /* unused fields
              Expression_ID         Subpool_Name;
           */
          break;
        }

      case An_Allocation_From_Qualified_Expression:   // 4.8
        {
          logKind("An_Allocation_From_Qualified_Expression");

          Element_Struct&    allocElem = retrieveAs(elemMap(), expr.Allocator_Qualified_Expression);
          ADA_ASSERT(allocElem.Element_Kind == An_Expression);

          Expression_Struct& allocExpr = allocElem.The_Union.Expression;
          ADA_ASSERT(allocExpr.Expression_Kind == A_Qualified_Expression);
          logKind("A_Qualified_Expression");

          SgType&            ty  = getDeclTypeID(allocExpr.Converted_Or_Qualified_Subtype_Mark, ctx);
          SgExpression&      arg = getExprID_undecorated(allocExpr.Converted_Or_Qualified_Expression, ctx);
          SgExprListExp&     inilst = createExprListExpIfNeeded(arg);

  /*
          Element_Struct&    initElem = retrieveAs(elemMap(), allocExpr.Converted_Or_Qualified_Expression);
          ADA_ASSERT(initElem.Element_Kind == An_Expression);
          Expression_Struct& initExpr = initElem.The_Union.Expression;

          SgExprListExp&     tyinit  = getAggregate(initElem, initExpr, ctx);
  */

          res = &mkNewExp(ty, &inilst);

          /* unused fields
            Expression_ID         Subpool_Name
          */
          break;
        }


      case A_Box_Expression:                          // Ada 2005 4.3.1(4): 4.3.3(3:6)
      case A_Raise_Expression:                        // 4.4 Ada 2012 (AI12-0022-1)

      case A_Case_Expression:                         // Ada 2012
      case An_If_Expression:                          // Ada 2012
      case A_For_All_Quantified_Expression:           // Ada 2012
      case A_For_Some_Quantified_Expression:          // Ada 2012
      case Not_An_Expression: /* break; */            // An unexpected element
      default:
        logWarn() << "unhandled expression: " << expr.Expression_Kind << std::endl;
        res = sb::buildIntVal();
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    attachSourceLocation(SG_DEREF(res), elem, ctx);
    return *res;
  }

  SgExpression&
  getExprID_undecorated(Element_ID el, AstContext ctx)
  {
    return getExpr_undecorated(retrieveAs(elemMap(), el), ctx);
  }
}

SgExpression&
getExpr(Element_Struct& elem, AstContext ctx)
{
  SgExpression*      res  = &getExpr_undecorated(elem, ctx);
  Expression_Struct& expr = elem.The_Union.Expression;

  switch (expr.Expression_Kind)
  {
    case A_Positional_Array_Aggregate:              // 4.3
    case A_Named_Array_Aggregate:                   // 4.3
    case A_Record_Aggregate:                        // 4.3
    case An_Extension_Aggregate:                    // 4.3
      {
        SgExprListExp* explst = isSgExprListExp(res);
        ADA_ASSERT(explst);

        res = sb::buildAggregateInitializer(explst);
        ADA_ASSERT(explst->get_parent());
        attachSourceLocation(SG_DEREF(res), elem, ctx);
        break;
      }

    default:;
  }

  return SG_DEREF(res);
}


SgExpression&
getExprID(Element_ID el, AstContext ctx)
{
  return getExpr(retrieveAs(elemMap(), el), ctx);
}

SgExpression&
getExprID_opt(Element_ID el, AstContext ctx)
{
  if (isInvalidId(el))
  {
    logWarn() << "uninitialized expression id " << el << std::endl;
    return mkNullExpression();
  }

  return el == 0 ? mkNullExpression()
                 : getExprID(el, ctx)
                 ;
}

namespace
{
  template <typename AsisDiscreteRangeStruct>
  SgExpression&
  getDiscreteRangeGeneric(Element_Struct& el, Definition_Struct& def, AsisDiscreteRangeStruct& range, AstContext ctx)
  {
    SgExpression* res = nullptr;

    switch (range.Discrete_Range_Kind)
    {
      case A_Discrete_Subtype_Indication:         // 3.6.1(6), 3.2.2
        {
          logKind("A_Discrete_Subtype_Indication");
          SgType* ty = &getDeclTypeID(range.Subtype_Mark, ctx);

          // \todo if there is no subtype constraint, shall we produce
          //       a subtype w/ NoConstraint, or leave the original type?
          if (range.Subtype_Constraint)
          {
            SgAdaTypeConstraint& constraint = getConstraintID(range.Subtype_Constraint, ctx);

            ty = &mkAdaSubtype(SG_DEREF(ty), constraint);
          }

          ADA_ASSERT(ty);
          res = sb::buildTypeExpression(ty);
          break;
        }

      case A_Discrete_Simple_Expression_Range:    // 3.6.1, 3.5
        {
          logKind("A_Discrete_Simple_Expression_Range");

          SgExpression& lb = getExprID(range.Lower_Bound, ctx);
          SgExpression& ub = getExprID(range.Upper_Bound, ctx);

          res = &mkRangeExp(lb, ub);
          break;
        }

      case A_Discrete_Range_Attribute_Reference:  // 3.6.1, 3.5
        {
          logKind("A_Discrete_Range_Attribute_Reference");

          res = &getExprID(range.Range_Attribute, ctx);
          break;
        }

      case Not_A_Discrete_Range:                  // An unexpected element
      default:
        logWarn() << "Unhandled range: " << range.Discrete_Range_Kind << std::endl;
        res = &mkRangeExp();
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    attachSourceLocation(SG_DEREF(res), el, ctx);
    return *res;
  }

  /// \private
  /// returns a range expression from the Asis definition \ref def
  SgExpression&
  getDiscreteRange(Element_Struct& el, Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == A_Discrete_Range);

    return getDiscreteRangeGeneric(el, def, def.The_Union.The_Discrete_Range, ctx);
  }

  SgExpression&
  getDiscreteRange(Element_Struct& el, AstContext ctx)
  {
    ADA_ASSERT(el.Element_Kind == A_Definition);

    return getDiscreteRange(el, el.The_Union.Definition, ctx);
  }

  SgExpression&
  getDiscreteSubtype(Element_Struct& el, Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == A_Discrete_Subtype_Definition);

    return getDiscreteRangeGeneric(el, def, def.The_Union.The_Discrete_Subtype_Definition, ctx);
  }

  SgExpression&
  getConstraintExpr(Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == A_Constraint);

    Constraint_Struct& constraint = def.The_Union.The_Constraint;

    if (constraint.Constraint_Kind == A_Range_Attribute_Reference)  // 3.5(2)
    {
      logKind("A_Range_Attribute_Reference");

      return getExprID(constraint.Range_Attribute, ctx);
    }


    ADA_ASSERT (constraint.Constraint_Kind == A_Simple_Expression_Range);
    logKind("A_Simple_Expression_Range");

    SgExpression& lb = getExprID(constraint.Lower_Bound, ctx);
    SgExpression& ub = getExprID(constraint.Upper_Bound, ctx);
    return mkRangeExp(lb, ub);
  }
}

void ExprSeqCreator::operator()(Element_Struct& el)
{
  SgExpression* res = nullptr;

  if (el.Element_Kind == An_Expression)
    res = &getExpr(el, ctx);
  else if (el.Element_Kind == A_Definition)
    res = &getDefinitionExpr(el, ctx);

  ADA_ASSERT(res);
  elems.push_back(res);
}

void ArgListCreator::operator()(Element_Struct& elem)
{
  args.push_back(&getArg(elem, ctx));
}


void RangeListCreator::operator()(Element_Struct& elem)
{
  lst.push_back(&getDiscreteRange(elem, ctx));
}


/// returns an expression from the Asis definition \ref def
SgExpression&
getDefinitionExpr(Element_Struct& el, AstContext ctx)
{
  ADA_ASSERT(el.Element_Kind == A_Definition);

  Definition_Struct& def = el.The_Union.Definition;
  SgExpression*      res = nullptr;

  switch (def.Definition_Kind)
  {
    case A_Discrete_Range:
      logKind("A_Discrete_Range");
      res = &getDiscreteRange(el, def, ctx);
      break;

    case A_Discrete_Subtype_Definition:
      logKind("A_Discrete_Subtype_Definition");
      res = &getDiscreteSubtype(el, def, ctx);
      break;

    case An_Others_Choice:
      logKind("An_Others_Choice");
      res = &mkAdaOthersExp();
      break;

    case A_Constraint:
      logKind("A_Constraint");
      res = &getConstraintExpr(def, ctx);
      break;

    default:
      logWarn() << "Unhandled definition expr: " << def.Definition_Kind << std::endl;
      res = sb::buildNullExpression();
      ADA_ASSERT(!FAIL_ON_ERROR(ctx));
  }

  attachSourceLocation(SG_DEREF(res), el, ctx);
  return *res;
}

SgExpression&
getDiscreteRangeID(Element_ID id, AstContext ctx)
{
  return getDiscreteRange(retrieveAs(elemMap(), id), ctx);
}

SgExpression&
getDefinitionExprID(Element_ID id, AstContext ctx)
{
  return getDefinitionExpr(retrieveAs(elemMap(), id), ctx);
}

SgExpression& createCall(SgExpression& target, ElemIdRange args, bool callSyntax, AstContext ctx)
{
  SgExpression* res = sg::dispatch(AdaCallBuilder{args, callSyntax, ctx}, &target);

  return SG_DEREF(res);
}

SgExpression&
getEnumRepresentationValue(Element_Struct& el, AstContext ctx)
{
  ADA_ASSERT(el.Element_Kind == A_Defining_Name);

  Defining_Name_Struct& def = el.The_Union.Defining_Name;
  ADA_ASSERT(  def.Defining_Name_Kind == A_Defining_Enumeration_Literal
            || def.Defining_Name_Kind == A_Defining_Character_Literal
            );

  return mkAdaIntegerLiteral(def.Representation_Value_Image);
}

} // namespace Ada_ROSE_Translation
