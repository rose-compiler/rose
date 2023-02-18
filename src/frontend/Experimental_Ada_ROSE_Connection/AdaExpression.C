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
      logKind("A_Parameter_Association", elem.ID);
      break;
    case A_Pragma_Argument_Association:
      logKind("A_Pragma_Argument_Association", elem.ID);
      break;
    case A_Generic_Association:
      logKind("A_Generic_Association", elem.ID);
      break;
    default:
      ADA_ASSERT(false);
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
    ADA_ASSERT(  formalName.Expression_Kind == An_Identifier
              || formalName.Expression_Kind == An_Operator_Symbol
              );

    logKind("An_Identifier", formalParm->ID);
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

      AdaCallBuilder(Element_ID targetid, std::vector<SgExpression*> arglist, bool useCallSyntax, AstContext astctx)
      : base(nullptr), tgtid(targetid), args(std::move(arglist)), callSyntax(useCallSyntax), ctx(astctx)
      {}

      void mkCall(SgExpression& n)
      {
        SgExprListExp& arglst = mkExprListExp(args);

        res = &mkFunctionCallExp(n, arglst, !callSyntax);
      }

#if OBSOLETE_CODE_FN
      SgAdaInheritedFunctionSymbol*
      inheritedFunctionSymbol(SgType* ty, SgFunctionSymbol& origSymbol)
      {
        const SgDeclarationStatement* tydcl = si::Ada::associatedDeclaration(ty);
        const bool                    supported = (  isSgTypedefDeclaration(tydcl)
                                                  || isSgClassDeclaration(tydcl)
                                                  || isSgEnumDeclaration(tydcl)
                                                  );

        if (!supported)
        {
          //~ logError() << "not a supported type derivation (i.e., extension record, derived type, derived enum)"
                     //~ << std::endl;
          return nullptr;
        }

        InheritedSymbolKey            key{origSymbol.get_declaration(), si::getDeclaredType(tydcl)};

        return findFirst(inheritedSymbols(), key);
      }

      SgFunctionSymbol&
      functionSymbol( const std::vector<si::Ada::PrimitiveParameterDesc>& primitiveArgs,
                      const SgExprListExp& args,
                      SgFunctionSymbol& implSymbol
                    )
      {
        using PrimitiveParmIterator = std::vector<si::Ada::PrimitiveParameterDesc>::const_iterator;
        using ArgumentIterator      = SgExpressionPtrList::const_iterator;

        if (primitiveArgs.size() == 0)
          return implSymbol;

        const SgExpressionPtrList& arglst      = args.get_expressions();
        const size_t               posArgLimit = si::Ada::positionalArgumentLimit(args);
        PrimitiveParmIterator      aa          = primitiveArgs.begin();
        PrimitiveParmIterator      zz          = primitiveArgs.end();

        // check all positional arguments
        while ((aa != zz) && (aa->pos() < posArgLimit))
        {
          const SgExpression* arg = arglst.at(aa->pos());

          if (SgAdaInheritedFunctionSymbol* inhSym = inheritedFunctionSymbol(arg->get_type(), implSymbol))
          {
            //~ logError() << "inh fnsym" << std::endl;
            return *inhSym;
          }

          ++aa;
        }

        ROSE_ASSERT(posArgLimit <= arglst.size());
        ArgumentIterator firstNamed = arglst.begin() + posArgLimit;
        ArgumentIterator argLimit   = arglst.end();

        // check all named arguments
        while (aa != zz)
        {
          const std::string& parmName = SG_DEREF(aa->name()).get_name();
          auto               sameNamePred = [&parmName](const SgExpression* arg) -> bool
                                            {
                                              const SgActualArgumentExpression* actarg = isSgActualArgumentExpression(arg);

                                              ROSE_ASSERT(actarg);
                                              const std::string& argName = actarg->get_argument_name().getString();

                                              return boost::iequals(parmName, argName);
                                            };
          ArgumentIterator argpos   = std::find_if(firstNamed, argLimit, sameNamePred);

          ++aa;

          if (argpos == argLimit)
            continue;

          if (SgAdaInheritedFunctionSymbol* inhSym = inheritedFunctionSymbol((*argpos)->get_type(), implSymbol))
          {
            //~ logError() << "inh2 fnsym" << std::endl;
            return *inhSym;
          }
        }

        return implSymbol;
      }

      SgFunctionSymbol&
      functionSymbol(SgFunctionDeclaration& dcl, SgFunctionSymbol& fnsym, SgExprListExp& args)
      {
        auto primitiveArgs = si::Ada::primitiveParameterPositions(fnsym.get_declaration());

        return functionSymbol(primitiveArgs, args, fnsym);
      }
#endif /* OBSOLETE_CODE_FN */

      void handle(SgNode& n)       { SG_UNEXPECTED_NODE(n); }

      // default
      void handle(SgExpression& n) { mkCall(n); }

      void handle(SgFunctionRefExp& n)
      {
        SgExprListExp& arglst = mkExprListExp(args);

#if OBSOLETE_CODE_FN
        // the disambiguation has been moved to a post-processing phase (Ada_to_ROSE.C)
        // where both the arguments and return context can be taken into account.

        if (SgFunctionDeclaration* funDcl = n.getAssociatedFunctionDeclaration())
        {
          SgFunctionSymbol& origSym = SG_DEREF(n.get_symbol());
          SgFunctionSymbol& funSym  = functionSymbol(*funDcl, origSym, arglst);

          if (&origSym != &funSym)
            n.set_symbol(&funSym);
        }
        else
        {
          //~ logError() << "w/o fndcl" << std::endl;
        }
#endif /* OBSOLETE_CODE_FN */

        res = &mkFunctionCallExp(n, arglst, !callSyntax);
      }

      void handle(SgUnaryOp& n)
      {
        // computed target ?
        if (n.get_operand() != nullptr)
        {
          mkCall(n);
          return;
        }

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

        ADA_ASSERT(args.size() == 2);
        n.set_lhs_operand(args[0]);
        n.set_rhs_operand(args[1]);
        res = &n;
      }

    private:
      Element_ID                 tgtid;
      std::vector<SgExpression*> args;
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
    logKind("An_Array_Component_Association", el.ID);

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
    logKind("A_Record_Component_Association", el.ID);

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
      void handle(SgAdaGenericInstanceDecl& n) { res = &mkAdaUnitRefExp(n); }
      void handle(SgAdaPackageSpecDecl& n)     { res = &mkAdaUnitRefExp(n); }
      void handle(SgAdaTaskTypeDecl& n)        { res = &mkTypeExpression(SG_DEREF(n.get_type())); }
      void handle(SgAdaProtectedTypeDecl& n)   { res = &mkTypeExpression(SG_DEREF(n.get_type())); }

      // \todo should we reference the underlying declaration instead of the generic??
      void handle(SgAdaGenericDecl& n)         { res = &mkAdaUnitRefExp(n); }

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

        set(&mkTypeUnknown());
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
      }

      // void handle(SgImportStatement& n)

      void handle(SgClassDeclaration& n)   { set(n.get_type()); }
      void handle(SgTypedefDeclaration& n) { set(n.get_type()); }
      void handle(SgEnumDeclaration& n)    { set(n.get_type()); }
      void handle(SgAdaFormalTypeDecl& n)  { set(n.get_type()); }
      void handle(SgAdaTaskTypeDecl& n)      { set(n.get_type()); }
      void handle(SgAdaProtectedTypeDecl& n) { set(n.get_type()); }

    private:
      AstContext ctx;
  };

  void TypeRefMaker::set(SgType* ty)
  {
    res = &mkTypeExpression(SG_DEREF(ty));
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

  /// old operator call, currently serves as fallback
  /// \todo remove from code base
  SgExpression&
  getOperator_fallback(Expression_Struct& expr, AstContext ctx)
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
      { A_Rem_Operator,                   {"A_Rem_Operator",                   mk2_wrapper<SgRemOp,            sb::buildRemOp> }},
      { An_Exponentiate_Operator,         {"An_Exponentiate_Operator",         mk2_wrapper<SgExponentiationOp, sb::buildExponentiationOp> }},
      { An_Abs_Operator,                  {"An_Abs_Operator",                  mk1_wrapper<SgAbsOp,            sb::buildAbsOp> }},
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

  SgFunctionDeclaration*
  disambiguateOperators( const AdaIdentifier& dbgname,
                         std::vector<SgFunctionDeclaration*>& cands,
                         OperatorCallSupplement suppl
                       )
  {
    if ((suppl.args() == nullptr) || (suppl.args()->size() == 0))
    {
      logWarn() << "unable to disambiguate operator " << dbgname << " w/o arguments "
                << (suppl.args() ? int(suppl.args()->size()) : -1)
                << std::endl;
      return nullptr;
    }

    std::vector<SgFunctionDeclaration*> res;

    std::copy_if( cands.begin(), cands.end(),
                  std::back_inserter(res),
                  [&suppl](SgFunctionDeclaration* fn) -> bool
                  {
                    ADA_ASSERT(fn);
                    const std::size_t numParams = fn->get_args().size();

                    if (numParams != suppl.args()->size()) return false;

                    bool        res = true;
                    std::size_t i   = 0;

                    while (res && (i < numParams))
                    {
                      SgInitializedName& parm = SG_DEREF(fn->get_args().at(i));
                      SgType*            argRootTy = si::Ada::typeRoot(suppl.args()->at(i)).typerep();

                      // \todo consider to replace the simple type check with a proper overload resolution
                      res = si::Ada::typeRoot(parm.get_type()).typerep() == argRootTy;

                      if (false && !res)
                        logWarn() << i << ". parm/arg: "
                                  << si::Ada::typeRoot(parm.get_type()).typerep_ref().class_name() << " / "
                                  << (argRootTy ? argRootTy->class_name() : std::string{"<null>"})
                                  << std::endl;

                      ++i;
                    }

                    return res;
                  }
                );

    if (res.size() != 1)
    {
      logWarn() << "unable to disambiguate operator " << dbgname << ": " << res.size()
                << " viable candidates found."
                << std::endl;
    }

    return res.size() != 1 ? nullptr : res.front();
  }

  bool anonAccessType(OperatorCallSupplement suppl, const AstContext&)
  {
    SgTypePtrList*   argtypes = suppl.args();
    ADA_ASSERT(argtypes);

    SgAdaAccessType* argty = isSgAdaAccessType(argtypes->front());

    return argty && argty->get_is_anonymous();
  }

  bool equalArgumentTypes(OperatorCallSupplement suppl, const AstContext&)
  {
    SgTypePtrList* argtypes = suppl.args();
    ADA_ASSERT(argtypes && argtypes->size() == 2);

    // \todo do we need to find root types?
    SgType*        lhsty = argtypes->front();
    SgType*        rhsty = argtypes->back();

    return lhsty == rhsty;
  }

  bool resultTypeIsBool(OperatorCallSupplement suppl, const AstContext&)
  {
    SgType const* rootty = si::Ada::typeRoot(suppl.result()).typerep();

    //~ res = res->stripTypedefsAndModifiers();
    return isSgTypeBool(rootty) != nullptr;
  }

  bool nonLimitedArgumentType(OperatorCallSupplement suppl, const AstContext&)
  {
    SgTypePtrList* argtypes = suppl.args();
    ADA_ASSERT(argtypes);

    SgType*        argty = argtypes->front();
    ADA_ASSERT(argty);

    SgDeclarationStatement* dcl = si::Ada::associatedDeclaration(argty);

    return !(dcl && dcl->get_declarationModifier().isAdaLimited());
  }

  bool hasEqualityOperator(OperatorCallSupplement, const AstContext&)
  {
    return false;
  }

  bool isScalarType(OperatorCallSupplement suppl, const AstContext&)
  {
    SgTypePtrList* argtypes = suppl.args();
    ADA_ASSERT(argtypes);

    SgType const*  argty = si::Ada::typeRoot(argtypes->front()).typerep();

    if (!argty) return false;

    return (  si::Ada::isModularType(*argty)
           || si::Ada::isIntegerType(*argty)
           || si::Ada::isFloatingPointType(*argty)
           || si::Ada::isDiscreteType(*argty)
           || si::Ada::isFixedType(*argty)
           || si::Ada::isDecimalFixedType(*argty)
           || isSgEnumType(argty)
           );
  }

  bool isDiscreteArrayType(OperatorCallSupplement suppl, const AstContext&)
  {
    SgTypePtrList const* argtypes = suppl.args();
    ADA_ASSERT(argtypes);

    SgType const*        argty = si::Ada::typeRoot(argtypes->front()).typerep();
    SgArrayType const*   arrty = isSgArrayType(argty);

    if (!arrty) return false;

    SgExprListExp const* idx = arrty->get_dim_info();
    if (!idx || (idx->get_expressions().size() != 1)) return false;

    SgType const* elmty = si::Ada::typeRoot(arrty->get_base_type()).typerep();

    if (elmty == nullptr) return false;

    return (  si::Ada::isModularType(*elmty)
           || si::Ada::isIntegerType(*elmty)
           //~ || si::Ada::isFloatingPointType(*elmty)
           || si::Ada::isDiscreteType(*elmty)
           //~ || si::Ada::isFixedType(*elmty)
           //~ || si::Ada::isDecimalFixedType(*elmty)
           || isSgEnumType(elmty)
           );
  }

  bool isGeneratableOperator(AdaIdentifier name, OperatorCallSupplement suppl, AstContext ctx)
  {
    // this function returns true if an operator can be generated.
    // \todo it is unclear whether the ROSE frontend needs to validate based on argument and return types
    //       as defined by the Ada Standard, since the frontend should have already done it.
    //       Currently, the validation checking is implemented for a few relational operators but
    //       left as todo item for others (12/16/22).

    // imprecise .. see below
    if ((name == "=" || name == "/="))
      return true;

    // see https://www.adaic.com/resources/add_content/standards/05rm/html/RM-4-5-2.html
    // see https://www.adaic.com/resources/add_content/standards/05rm/html/RM-4-4.html
    if (name == "=" && !anonAccessType(suppl, ctx))
    {
      return (  nonLimitedArgumentType(suppl, ctx)
             && equalArgumentTypes(suppl, ctx)
             && resultTypeIsBool(suppl, ctx)
             );
    }

    if (name == "/=" && !anonAccessType(suppl, ctx))
    {
      return (  ( nonLimitedArgumentType(suppl, ctx) || hasEqualityOperator(suppl, ctx) )
             && equalArgumentTypes(suppl, ctx)
             && resultTypeIsBool(suppl, ctx)
             );
    }

    if ((name == "=" || name == "/=") && anonAccessType(suppl, ctx))
    {
      // catch these cases in testing...
      ADA_ASSERT(false);
      return true;
    }

    if ((name == "<") || (name == "<=") || (name == ">") || (name == ">="))
    {
      return (  (isScalarType(suppl, ctx) || isDiscreteArrayType(suppl, ctx))
             && equalArgumentTypes(suppl, ctx)
             && resultTypeIsBool(suppl, ctx)
             );
    }

    if (name == "and" || name == "or" || name == "xor")
    {
      return equalArgumentTypes(suppl, ctx);
      //~ (  (argIsBoolean() || argIsModularType() || (argIsBoolArray()))
             //~ && equalArgumentTypes(suppl, ctx)
             //~ );
    }

    if ( (  name == "+"   || name == "-"   || name == "*"
         || name == "/"   || name == "mod" || name == "rem"
         || name == "abs" || name == "not"
         || name == "&"
         || name == "**"
         )
       )
    {
      // \todo check whether thie types match
      return true;
    }

    return false;
  }

  bool isRelationalOperator(AdaIdentifier name)
  {
    return (  name == "="
           || name == "/="
           || name == "<"
           || name == "<="
           || name == ">"
           || name == ">="
           );
  }

  void setDefaultReturnType(AdaIdentifier name, OperatorCallSupplement& suppl)
  {
    if (suppl.result())
      return;

    if (isRelationalOperator(name))
    {
      suppl.result(sb::buildBoolType());
      return;
    }

    if (suppl.args() == nullptr)
      return;

    if (  name == "+"
       || name == "-"
       || name == "*"
       || name == "/"
       || name == "mod"
       || name == "rem"
       || name == "abs"
       || name == "not"
       || name == "and"
       || name == "or"
       || name == "xor"
       || name == "**"
       )
    {
      suppl.result(suppl.args()->front());
      return;
    }

    if (name == "&")
    {
      // \todo return an open array type based on the array as the first or second argument.
      //       just picking the first type is wrong, because it may be bounded; or not be an array at all.
      logWarn() << "gen arg types .. [incomplete]" << std::endl;
      suppl.result(suppl.args()->front());
      return;
    }
  }

  // cmp to declareOp in SgType.C
  SgExpression*
  generateOperator(AdaIdentifier name, Expression_Struct& expr, OperatorCallSupplement suppl, AstContext ctx)
  {
    ADA_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    setDefaultReturnType(name, suppl);

    if ((suppl.args() == nullptr) || (suppl.result() == nullptr))
    {
      logWarn() << "suppl is null" << std::endl;
      return nullptr;
    }

    const SgType*          ty     = suppl.args()->front();
    SgScopeStatement*      scope  = si::Ada::operatorScope(ty, isRelationalOperator(name));

    if (scope == nullptr)
    {
      logWarn() << "unable to get scope of type declaration: "
                << (ty ? typeid(*ty).name() : std::string{"<null>"}) << std::flush
                << ": " << ty->unparseToString()
                << std::endl;
      return nullptr;
    }

    // \todo add support for other operators
    if (!isGeneratableOperator(name, suppl, ctx))
    {
      logWarn() << "oper " << name << " not generatable" << std::endl;
      return nullptr;
    }

    std::string            opname = si::Ada::roseOperatorPrefix + name;

    auto                   complete =
       [&suppl](SgFunctionParameterList& fnParmList, SgScopeStatement& scope)->void
       {
         static constexpr int MAX_PARAMS = 2;
         static const std::string parmNames[MAX_PARAMS] = { "Left", "Right" };

         int            parmNameIdx = MAX_PARAMS - suppl.args()->size() - 1;
         SgTypeModifier defaultInMode;

         defaultInMode.setDefault();

         ADA_ASSERT(suppl.args()->size() <= MAX_PARAMS);
         for (SgType* parmType : *suppl.args())
         {
           const std::string&       parmName = parmNames[++parmNameIdx];
           SgInitializedName&       parmDecl = mkInitializedName(parmName, SG_DEREF(parmType), nullptr);
           SgInitializedNamePtrList parmList = {&parmDecl};
           /* SgVariableDeclaration&   pvDecl   =*/ mkParameter(parmList, defaultInMode, scope);

           parmDecl.set_parent(&fnParmList);
           fnParmList.get_args().push_back(&parmDecl);
         }
       };

    SgFunctionDeclaration& opdcl = mkProcedureDecl_nondef(opname, *scope, *suppl.result(), complete);

    operatorSupport()[{scope, name}].emplace_back(&opdcl, OperatorDesc::COMPILER_GENERATED);
    return sb::buildFunctionRefExp(&opdcl);
  }

  void
  addToOverloadSet( map_t<OperatorKey, std::vector<OperatorDesc> >::const_iterator pos,
                    map_t<OperatorKey, std::vector<OperatorDesc> >::const_iterator lim,
                    std::vector<SgFunctionDeclaration*>& vec
                  )
  {
    if (pos == lim) return;

    std::transform( pos->second.begin(), pos->second.end(),
                    std::back_inserter(vec),
                    [](const OperatorDesc& desc) -> SgFunctionDeclaration*
                    {
                      return desc.function();
                    }
                  );
  }

  std::vector<SgFunctionDeclaration*>
  genOverloadSet(AdaIdentifier fnname, OperatorCallSupplement suppl, AstContext ctx)
  {
    std::vector<SgFunctionDeclaration*>                   res;
    map_t<OperatorKey, std::vector<OperatorDesc> > const& opMap = operatorSupport();

    addToOverloadSet(opMap.find({si::Ada::pkgStandardScope(), fnname}), opMap.end(), res);

    for (SgScopeStatement* scope = &ctx.scope(); scope && (!isSgGlobal(scope)); scope = si::getEnclosingScope(scope))
      addToOverloadSet(opMap.find({scope, fnname}), opMap.end(), res);

    // \todo add all "use" and "use type"
    return res;
  }

  SgExpression&
  getOperator(Expression_Struct& expr, OperatorCallSupplement suppl, AstContext ctx)
  {
    // FYI https://en.wikibooks.org/wiki/Ada_Programming/All_Operators

    ADA_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    // PP 11/18/22
    // under some unclear circumstances a provided = operator and a generated /= may have the
    //   same Corresponding_Name_Declaration, but different Corresponding_Name_Definition.
    //   => just use the Corresponding_Name_Definition
    // ROSE regression tests: dbase.ads, dbase.adb, dbase_test.adb
    // was: if (SgDeclarationStatement* dcl = findFirst(asisDecls(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
    if (SgDeclarationStatement* dcl = findFirst(asisDecls(), expr.Corresponding_Name_Definition))
    {
      SgExpression* res = sg::dispatch(ExprRefMaker{ctx}, dcl);

      //logWarn() << "ok1" << std::endl;
      return SG_DEREF(res);
    }

    int len = strlen(expr.Name_Image);
    ADA_ASSERT((len > 2) && (expr.Name_Image[0] == '"') && (expr.Name_Image[len-1] == '"'));

    // do not use leading and trailing '"'
    AdaIdentifier fnname{expr.Name_Image+1, len-2};

    std::vector<SgFunctionDeclaration*> overloadSet = genOverloadSet(fnname, suppl, ctx);

    if (!overloadSet.empty())
    {
      if (SgFunctionDeclaration* fundcl = disambiguateOperators(fnname, overloadSet, suppl))
      {
        //~ logWarn() << "ok2" << std::endl;
        return SG_DEREF(sb::buildFunctionRefExp(fundcl));
      }
    }
    else
    {
      logError() << "Operator name not registered: '" << expr.Name_Image << "'"
                 << std::endl;
    }

    // try to generate the operator
    if (SgExpression* res = generateOperator(fnname, expr, suppl, ctx))
    {
      return *res;
    }

    logWarn() << "Using first version generator as fallback to model operator " << expr.Name_Image
              << std::endl;

    /* unused fields:
       Defining_Name_List    Corresponding_Name_Definition_List;
       Defining_Name_ID      Corresponding_Generic_Element;
    */
    return getOperator_fallback(expr, ctx);
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

      res = &mkEnumeratorRef(enumdecl, *enumitem);
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

        SgStringVal& strval = SG_DEREF(sb::buildStringVal(enumstr));

        strval.set_stringDelimiter(' ');
        res = &strval;
      }
    }

    return SG_DEREF(res);
  }

  /// defines ROSE AST types for which we do not generate scope qualification
  struct RoseRequiresScopeQual : sg::DispatchHandler<bool>
  {
    void handle(const SgNode& n)                 { SG_UNEXPECTED_NODE(n); }

    // scope qual requried for
    void handle(const SgDeclarationStatement&)   { res = true; } // default for declarations
    void handle(const SgInitializedName&)        { res = true; }

    // no scope qual needed for
    void handle(const SgAdaTaskSpecDecl&)        { res = false; }
    void handle(const SgAdaProtectedSpecDecl&)   { res = false; }
    void handle(const SgAdaPackageSpecDecl&)     { res = false; }
    void handle(const SgImportStatement&)        { res = false; }
    void handle(const SgBasicBlock&)             { res = false; }
    void handle(const SgAdaGenericInstanceDecl&) { res = false; }
    //~ void handle(const SgFunctionDeclaration&)  { res = false; }

    // dependent on underlying data
    void handle(const SgAdaRenamingDecl& n)
    {
      res = si::Ada::isObjectRenaming(n);
    }
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
      const SgNode* astnode = queryCorrespondingAstNode(expr, ctx);

      logTrace() << "An_Identifier?" << std::endl;

      if (!astnode)
        logTrace() << "Identifier '" << expr.Name_Image << "' has no corresponding node in ROSE."
                   << std::endl;

      return astnode == nullptr || sg::dispatch(RoseRequiresScopeQual{}, astnode);
    }

    if (expr.Expression_Kind == A_Selected_Component)
    {
      logTrace() << "A_Selected_Component?" << std::endl;
      return    roseRequiresPrefixID(expr.Prefix, ctx)
             || roseRequiresPrefixID(expr.Selector, ctx);
    }

    if (expr.Expression_Kind == An_Indexed_Component)
    {
      logTrace() << "An_Indexed_Component?" << std::endl;
      // \todo should this always return true (like the cases below)?
      return roseRequiresPrefixID(expr.Prefix, ctx);
    }

    if (  (expr.Expression_Kind == An_Explicit_Dereference)
       || (expr.Expression_Kind == A_Function_Call)
       )
    {

      logTrace() << "A_Function_Call/An_Explicit_Dereference?" << std::endl;
      return true;
    }

    if (expr.Expression_Kind == A_Type_Conversion)
    {
      logTrace() << "A_Type_Conversion" << std::endl;
      return true;
    }

    logWarn() << "roseRequiresPrefixID: untested expression-kind: "
              << expr.Expression_Kind
              << std::endl;
    ADA_ASSERT(!FAIL_ON_ERROR(ctx) && "untested expression-kind");
    return true;
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
getAttributeExpr(Expression_Struct& expr, AstContext ctx, ElemIdRange argRangeSuppl)
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
    case A_Last_Attribute:            // 3.5(13), 3.6.2(5), K(102), K(104)
    case A_Range_Attribute:            // 3.5(14), 3.6.2(7), K(187), ú(189)
    {
      ElemIdRange                range = idRange(expr.Attribute_Designator_Expressions);
      ADA_ASSERT(argRangeSuppl.empty() || range.empty());

      std::vector<SgExpression*> exprs;

      if (range.empty())
        exprs = traverseIDs(argRangeSuppl, elemMap(), ArgListCreator{ctx});
      else
        exprs = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});

      res = &mkAdaAttributeExp(obj, name.fullName, mkExprListExp(exprs));
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
        std::vector<SgExpression*> exprs = traverseIDs(argRangeSuppl, elemMap(), ArgListCreator{ctx});
        SgExprListExp&             args  = mkExprListExp(exprs);

        res = &mkAdaAttributeExp(obj, name.fullName, args);
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
getAttributeExprID(Element_ID el, AstContext ctx, ElemIdRange argRangeSuppl)
{
  Element_Struct& elem = retrieveAs(elemMap(), el);

  ADA_ASSERT(elem.Element_Kind == An_Expression);
  SgAdaAttributeExp& sgnode = getAttributeExpr(elem.The_Union.Expression, ctx, argRangeSuppl);

  attachSourceLocation(sgnode, elem, ctx);
  return sgnode;
}

namespace
{
  // \todo cmp. AdaStatement.C
  struct IfExprCreator
  {
      IfExprCreator(SgConditionalExp& sgnode, AstContext astctx)
      : ifExpr(&sgnode), ctx(astctx)
      {}

      void commonBranch(Path_Struct& path, void (SgConditionalExp::*branchSetter)(SgExpression*))
      {
        SgExpression& thenExpr = getExprID(path.Dependent_Expression, ctx);

        sg::linkParentChild(SG_DEREF(ifExpr), thenExpr, branchSetter);
      }

      void conditionedBranch(Path_Struct& path)
      {
        SgExpression& condExpr = getExprID(path.Condition_Expression, ctx);

        sg::linkParentChild(SG_DEREF(ifExpr), condExpr, &SgConditionalExp::set_conditional_exp);
        commonBranch(path, &SgConditionalExp::set_true_exp);
      }

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case An_If_Expression_Path:
            {
              logKind("An_If_Expression_Path", elem.ID);
              ADA_ASSERT(ifExpr);
              conditionedBranch(path);
              break;
            }

          case An_Elsif_Expression_Path:
            {
              logKind("An_Elsif_Expression_Path", elem.ID);
              ADA_ASSERT(ifExpr);

              SgConditionalExp& cascadingIf = mkIfExpr();

              sg::linkParentChild( SG_DEREF(ifExpr),
                                   static_cast<SgExpression&>(cascadingIf),
                                   &SgConditionalExp::set_false_exp
                                 );
              ifExpr = &cascadingIf;
              conditionedBranch(path);
              break;
            }

          case An_Else_Expression_Path:
            {
              logKind("An_Else_Expression_Path", elem.ID);
              ADA_ASSERT(ifExpr);
              commonBranch(path, &SgConditionalExp::set_false_exp);
              break;
            }

          default:
            ADA_ASSERT(false);
        }
      }

    private:
      SgConditionalExp* ifExpr;
      AstContext        ctx;

      IfExprCreator() = delete;
  };



  SgExprListExp& createExprListExpIfNeeded(SgExpression& exp)
  {
    SgExprListExp* res = isSgExprListExp(&exp);

    return (res == nullptr) ? mkExprListExp({&exp}) : *res;
  }

  Expression_Kinds queryExprKindID(Element_ID id)
  {
    Element_Struct& elem = retrieveAs(elemMap(), id);

    ADA_ASSERT(elem.Element_Kind == An_Expression);
    return elem.The_Union.Expression.Expression_Kind;
  }

  SgInitializedName*
  queryByNameInDeclarationID(const AdaIdentifier& name, Declaration_ID id)
  {
    if (id <= 0)
      return nullptr;

    Element_Struct* elem = retrieveAsOpt(elemMap(), id); // \todo try the non_Opt version
    if (elem == nullptr || (elem->Element_Kind != A_Declaration))
      return nullptr;

    Declaration_Struct& decl = elem->The_Union.Declaration;
    if (decl.Declaration_Kind != A_Component_Declaration)
      return nullptr;

    ElemIdRange range = idRange(decl.Names);

    for (Element_ID_Ptr pos = range.first; pos != range.second; ++pos)
    {
      if (Element_Struct* el = retrieveAsOpt(elemMap(), *pos))
      {
        if (el->Element_Kind == A_Defining_Name)
        {
          Defining_Name_Struct& def = el->The_Union.Defining_Name;

          if (name == AdaIdentifier(def.Defining_Name_Image))
            return findFirst(asisVars(), el->ID);
        }
      }
    }

    return nullptr;
  }


  SgExpression&
  getExprID_undecorated(Element_ID el, AstContext ctx, OperatorCallSupplement suppl = {});

  /// creates expressions from elements, but does not decorate
  ///   aggregates with SgAggregateInitializers
  SgExpression&
  getExpr_undecorated(Element_Struct& elem, AstContext ctx, OperatorCallSupplement suppl = {})
  {
    ADA_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& expr      = elem.The_Union.Expression;
    SgExpression*      res       = NULL;

    switch (expr.Expression_Kind)
    {
      case An_Identifier:                             // 4.1
        {
          // \todo use the queryCorrespondingAstNode function and the
          //       generate the expression based on that result.
          logKind("An_Identifier", elem.ID);

          if (SgInitializedName* var = findFirst(asisVars(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
          {
            res = sb::buildVarRefExp(var, &ctx.scope());
          }
          else if (SgDeclarationStatement* dcl = queryDecl(expr, ctx))
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
          else
          {
            AdaIdentifier adaIdent{expr.Name_Image};

            // after there was no matching declaration, try to look up declarations in the standard package by name
            if (SgType* ty = findFirst(adaTypes(), adaIdent))
            {
              res = &mkTypeExpression(*ty);
            }
            else if (SgInitializedName* fld = queryByNameInDeclarationID(adaIdent, expr.Corresponding_Name_Declaration))
            {
              res = sb::buildVarRefExp(fld, &ctx.scope());
            }
            else if (SgInitializedName* var = findFirst(adaVars(), adaIdent))
            {
              res = sb::buildVarRefExp(var, &ctx.scope());
            }
            else if (SgInitializedName* exc = findFirst(adaExcps(), adaIdent))
            {
              res = &mkExceptionRef(*exc, ctx.scope());
            }
            else
            {
              // \todo check why the name remained unresolved
              res = &mkUnresolvedName(expr.Name_Image, ctx.scope());
            }
          }

          /* unused fields: (Expression_Struct)
               ** depends on the branch
               Defining_Name_ID      Corresponding_Generic_Element;
          */
          break;
        }

      case A_Function_Call:                           // 4.1
        {
          logKind("A_Function_Call", elem.ID);

          logTrace() << "function call "
                     << expr.Is_Prefix_Notation << " "
                     << expr.Is_Prefix_Call
                     << std::endl;

          ElemIdRange             range  = idRange(expr.Function_Call_Parameters);

          // PP (04/22/22) if the callee is an Ada Attribute then integrate
          //               the arguments into the Ada attribute expression directly.
          //               Note sure if it is good to deviate from the Asis representation
          //               but some arguments have no underlying functiom declaration.
          // \todo Consider adding an optional function reference to the SgAdaAttribute rep.
          if (queryExprKindID(expr.Prefix) == An_Attribute_Reference)
            res = &getAttributeExprID(expr.Prefix, ctx, range);
          else
            res = &createCall(expr.Prefix, range, expr.Is_Prefix_Call, ctx);

          /* unused fields:
             Expression_Struct
               bool                  Is_Prefix_Notation;
               bool                  Is_Generalized_Reference;
               bool                  Is_Dispatching_Call;
               bool                  Is_Call_On_Dispatching_Operation;
          */
          break;
        }

      case An_Integer_Literal:                        // 2.4
        {
          logKind("An_Integer_Literal", elem.ID);

          res = &mkAdaIntegerLiteral(expr.Value_Image);

          /* unused fields: (Expression_Struct)
               enum Attribute_Kinds  Attribute_Kind
          */
          break;
        }

      case A_Character_Literal:                       // 4.1
        {
          logKind("A_Character_Literal", elem.ID);
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
          logKind("A_String_Literal", elem.ID);
          res = &mkValue<SgStringVal>(expr.Value_Image);
          break;
        }

      case A_Real_Literal:                            // 2.4.1
        {
          logKind("A_Real_Literal", elem.ID);
          res = &mkValue<SgLongDoubleVal>(expr.Value_Image);
          /* unused fields: (Expression_Struct)
               enum Attribute_Kinds  Attribute_Kind;
          */
          break;
        }

      case An_Operator_Symbol:                        // 4.1
        {
          logKind("An_Operator_Symbol", elem.ID);
          res = &getOperator(expr, suppl, ctx);
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
          logKind("An_Enumeration_Literal", elem.ID);
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
          logKind("An_Explicit_Dereference", elem.ID);

          SgExpression& exp = getExprID(expr.Prefix, ctx);

          // prefix calls are incorrectly unparsed as infix (check if data is avail in Asis)
          // in this case, parenthesis are missing.
          // if (isSgBinaryOp(&exp)) exp.set_need_paren(true);

          res = sb::buildPointerDerefExp(&exp);
          break;
        }

      case An_Indexed_Component:                      // 4.1.1
        {
          logKind("An_Indexed_Component", elem.ID);

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
          logKind("A_Slice", elem.ID);

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
          logKind("A_Selected_Component", elem.ID);
          SgExpression& selector = getExprID(expr.Selector, ctx, suppl);
          const bool    enumval = isSgEnumVal(&selector) != nullptr;

          // Check if the kind requires a prefix in ROSE,
          //   or if the prefix (scope qualification) is implied and
          //   generated by the backend.
          if (!enumval && roseRequiresPrefixID(expr.Prefix, ctx))
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
          logKind("An_Attribute_Reference", elem.ID);

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
          logKind("A_Record_Aggregate", elem.ID);

          res = &getRecordAggregate(elem, expr, ctx);
          break;
        }

      case An_Extension_Aggregate:                    // 4.3
        {
          logKind("An_Extension_Aggregate", elem.ID);

          SgExprListExp& elemlst   = getRecordAggregate(elem, expr, ctx);
          SgExpression&  parentexp = getExprID(expr.Extension_Aggregate_Expression, ctx);

          elemlst.prepend_expression(&mkAdaAncestorInitializer(parentexp));

          res = &elemlst;
          break;
        }


      case An_And_Then_Short_Circuit:                 // 4.4
        {
          logKind("An_And_Then_Short_Circuit", elem.ID);
          SgExpression& lhs = getExprID(expr.Short_Circuit_Operation_Left_Expression, ctx);
          SgExpression& rhs = getExprID(expr.Short_Circuit_Operation_Right_Expression, ctx);

          res = sb::buildAndOp(&lhs, &rhs);
          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case An_Or_Else_Short_Circuit:                  // 4.4
        {
          logKind("An_Or_Else_Short_Circuit", elem.ID);
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
          logKind("A_Parenthesized_Expression", elem.ID);

          // \todo remove _opt when the asis connection implements A_Parenthesized_Expression
          res = &getExprID_opt(expr.Expression_Parenthesized, ctx);
          res->set_need_paren(true);

          /* unused fields: (Expression_Struct)
          */
          break;
        }

      case A_Null_Literal:                            // 4.4
        {
          logKind("A_Null_Literal", elem.ID);

          res = sb::buildNullptrValExp();
          break;
        }

      case An_In_Membership_Test:                     // 4.4  Ada 2012
      case A_Not_In_Membership_Test:                  // 4.4  Ada 2012
        {
          const bool inTest = expr.Expression_Kind == An_In_Membership_Test;

          logKind(inTest ? "An_In_Membership_Test" : "A_Not_In_Membership_Test", elem.ID);

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

          logKind(isConv ? "A_Type_Conversion" : "A_Qualified_Expression", elem.ID);

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
          logKind("An_Allocation_From_Subtype", elem.ID);

          SgType& ty = getDefinitionTypeID(expr.Allocator_Subtype_Indication, ctx);

          res = &mkNewExp(ty);

          /* unused fields
              Expression_ID         Subpool_Name;
           */
          break;
        }

      case An_Allocation_From_Qualified_Expression:   // 4.8
        {
          logKind("An_Allocation_From_Qualified_Expression", elem.ID);

          Element_Struct&    allocElem = retrieveAs(elemMap(), expr.Allocator_Qualified_Expression);
          ADA_ASSERT(allocElem.Element_Kind == An_Expression);

          Expression_Struct& allocExpr = allocElem.The_Union.Expression;
          ADA_ASSERT(allocExpr.Expression_Kind == A_Qualified_Expression);
          logKind("A_Qualified_Expression", allocElem.ID);

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
        {
          logKind("A_Box_Expression", elem.ID);

          res = &mkAdaBoxExp();
          break;
        }

      case An_If_Expression:                          // Ada 2012
        {
          logKind("An_If_Expression", elem.ID);

          SgConditionalExp& sgnode = mkIfExpr();
          ElemIdRange       range  = idRange(expr.Expression_Paths);

          traverseIDs(range, elemMap(), IfExprCreator{sgnode, ctx});
          res = &sgnode;
          /* unused fields:
          */
          break;

        }

      case A_Raise_Expression:                        // 4.4 Ada 2012 (AI12-0022-1)
      case A_Case_Expression:                         // Ada 2012
      case A_For_All_Quantified_Expression:           // Ada 2012
      case A_For_Some_Quantified_Expression:          // Ada 2012
      case Not_An_Expression: /* break; */            // An unexpected element
      default:
        logWarn() << "unhandled expression: " << expr.Expression_Kind << "   id: " << elem.ID << std::endl;
        res = sb::buildIntVal();
        ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    attachSourceLocation(SG_DEREF(res), elem, ctx);
    return *res;
  }

  SgExpression&
  getExprID_undecorated(Element_ID el, AstContext ctx, OperatorCallSupplement suppl)
  {
    return getExpr_undecorated(retrieveAs(elemMap(), el), ctx, suppl);
  }
}

SgExpression&
getExpr(Element_Struct& elem, AstContext ctx, OperatorCallSupplement suppl)
{
  SgExpression*      res  = &getExpr_undecorated(elem, ctx, suppl);
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
getExprID(Element_ID el, AstContext ctx, OperatorCallSupplement suppl)
{
  return getExpr(retrieveAs(elemMap(), el), ctx, suppl);
}

SgExpression&
getExprID_opt(Element_ID el, AstContext ctx, OperatorCallSupplement suppl)
{
  if (isInvalidId(el))
  {
    logWarn() << "uninitialized expression id " << el << std::endl;
    return mkNullExpression();
  }

  return el == 0 ? mkNullExpression()
                 : getExprID(el, ctx, suppl)
                 ;
}

namespace
{
  template <typename AsisDiscreteRangeStruct>
  SgExpression&
  getDiscreteRangeGeneric(Element_Struct& el, AsisDiscreteRangeStruct& range, AstContext ctx)
  {
    SgExpression* res = nullptr;

    switch (range.Discrete_Range_Kind)
    {
      case A_Discrete_Subtype_Indication:         // 3.6.1(6), 3.2.2
        {
          logKind("A_Discrete_Subtype_Indication", el.ID);

          SgType& ty = getDiscreteSubtypeID(range.Subtype_Mark, range.Subtype_Constraint, ctx);

          res = &mkTypeExpression(ty);
          break;
        }

      case A_Discrete_Simple_Expression_Range:    // 3.6.1, 3.5
        {
          logKind("A_Discrete_Simple_Expression_Range", el.ID);

          SgExpression& lb = getExprID(range.Lower_Bound, ctx);
          SgExpression& ub = getExprID(range.Upper_Bound, ctx);

          res = &mkRangeExp(lb, ub);
          break;
        }

      case A_Discrete_Range_Attribute_Reference:  // 3.6.1, 3.5
        {
          logKind("A_Discrete_Range_Attribute_Reference", el.ID);

          res = &getExprID(range.Range_Attribute, ctx);
          break;
        }

      case Not_A_Discrete_Range:                  // An unexpected element
      default:
        logWarn() << "Unhandled range: " << range.Discrete_Range_Kind << "  id: " << el.ID << std::endl;
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

    return getDiscreteRangeGeneric(el, def.The_Union.The_Discrete_Range, ctx);
  }

  SgExpression&
  getDiscreteRange(Element_Struct& el, AstContext ctx)
  {
    if (el.Element_Kind == A_Definition)
      return getDiscreteRange(el, el.The_Union.Definition, ctx);

    logError() << "unexpected range kind: " << el.Element_Kind << std::endl;
    return mkUnresolvedName("UNKNOWN_RANGE", ctx.scope());

#if 0
    ADA_ASSERT(el.Element_Kind == A_Definition);

    return getDiscreteRange(el, el.The_Union.Definition, ctx);
#endif /* 0 */
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

SgExpression&
getDiscreteSubtypeExpr(Element_Struct& el, Definition_Struct& def, AstContext ctx)
{
  ADA_ASSERT(def.Definition_Kind == A_Discrete_Subtype_Definition);

  return getDiscreteRangeGeneric(el, def.The_Union.The_Discrete_Subtype_Definition, ctx);
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
      logKind("A_Discrete_Range", el.ID);
      res = &getDiscreteRange(el, def, ctx);
      break;

    case A_Discrete_Subtype_Definition:
      logKind("A_Discrete_Subtype_Definition", el.ID);
      res = &getDiscreteSubtypeExpr(el, def, ctx);
      break;

    case An_Others_Choice:
      logKind("An_Others_Choice", el.ID);
      res = &mkAdaOthersExp();
      break;

    case A_Constraint:
      logKind("A_Constraint", el.ID);
      res = &getConstraintExpr(def, ctx);
      break;

    default:
      logWarn() << "Unhandled definition expr: " << def.Definition_Kind << "  id: " << el.ID << std::endl;
      res = &mkNullExpression();
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

SgExpression& createCall(Element_ID tgtid, ElemIdRange args, bool callSyntax, AstContext ctx)
{
  logInfo() << "use prefix call syntax: " << callSyntax << std::endl;

  // Create the arguments first. They may be needed to disambiguate operator calls
  std::vector<SgExpression*> arglist = traverseIDs(args, elemMap(), ArgListCreator{ctx});
  auto                       typeExtractor = [](SgExpression* exp)->SgType*
                                             {
                                               return si::Ada::typeOfExpr(exp).typerep();
                                             };

  SgTypePtrList              typlist;

  typlist.reserve(arglist.size());
  std::transform(arglist.begin(), arglist.end(), std::back_inserter(typlist), typeExtractor);

  SgExpression& tgt = getExprID(tgtid, ctx, OperatorCallSupplement{&typlist, nullptr /* unknown return type */});
  SgExpression* res = sg::dispatch(AdaCallBuilder{tgtid, std::move(arglist), callSyntax, ctx}, &tgt);

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

SgNode*
queryBuiltIn(AdaIdentifier adaIdent)
{
  static constexpr bool findFirstMatch = false /* syntactic sugar, always false */;

  SgNode* res = nullptr;

  findFirstMatch
  || (res = findFirst(adaTypes(), adaIdent))
  || (res = findFirst(adaPkgs(),  adaIdent))
  || (res = findFirst(adaVars(),  adaIdent))
  || (res = findFirst(adaExcps(), adaIdent))
  ;

  return res;
}

SgNode*
queryCorrespondingAstNode(Expression_Struct& expr, AstContext ctx)
{
  static constexpr bool findFirstMatch = false /* syntactic sugar, always false */;

  ADA_ASSERT(expr.Expression_Kind == An_Identifier);

  SgNode* res = nullptr;

  findFirstMatch
  || (res = findFirst(asisVars(),   expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
  || (res = findFirst(asisDecls(),  expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
  || (res = findFirst(asisExcps(),  expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
  || (res = findFirst(asisTypes(),  expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
  || (res = findFirst(asisBlocks(), expr.Corresponding_Name_Declaration))
  || (res = queryBuiltIn(expr.Name_Image))
  ;

  return res;
}

SgNode*
queryCorrespondingAstNode(Element_Struct& elem, AstContext ctx)
{
  ADA_ASSERT(elem.Element_Kind == An_Expression);

  return queryCorrespondingAstNode(elem.The_Union.Expression, ctx);
}

SgNode*
queryCorrespondingAstNodeID(Element_ID id, AstContext ctx)
{
  return queryCorrespondingAstNode(retrieveAs(elemMap(), id), ctx);
}

} // namespace Ada_ROSE_Translation
