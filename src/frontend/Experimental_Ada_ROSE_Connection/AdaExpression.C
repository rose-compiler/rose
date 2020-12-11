#include "sage3basic.h"

#include <vector>
#include <boost/algorithm/string.hpp>

#include "sageGeneric.h"
#include "sageBuilder.h"

#include "AdaExpression.h"

#include "Ada_to_ROSE.h"
#include "AdaMaker.h"
#include "AdaStatement.h"
#include "AdaType.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"


namespace sb = SageBuilder;

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
    ROSE_ASSERT(elem.Element_Kind == An_Association);

    Association_Struct& assoc      = elem.The_Union.Association;
    ROSE_ASSERT(assoc.Association_Kind == A_Parameter_Association);
    logKind("A_Parameter_Association");

    SgExpression&       arg        = getExprID(assoc.Actual_Parameter, ctx);
    Element_Struct*     formalParm = retrieveAsOpt<Element_Struct>(elemMap(), assoc.Formal_Parameter);

    if (!formalParm) return arg;

    ROSE_ASSERT(formalParm->Element_Kind == An_Expression);

    Expression_Struct&  formalName = formalParm->The_Union.Expression;
    ROSE_ASSERT(formalName.Expression_Kind == An_Identifier);

    logKind("An_Identifier");
    return SG_DEREF(sb::buildActualArgumentExpression(formalName.Name_Image, &arg));
  }
}

namespace
{
  struct AdaCallBuilder : sg::DispatchHandler<SgExpression*>
  {
    typedef sg::DispatchHandler<SgExpression*> base;

    AdaCallBuilder(ElemIdRange params, AstContext astctx)
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

    void handle(SgNode& n)       { SG_UNEXPECTED_NODE(n); }

    // default
    void handle(SgExpression& n) { mkCall(n); }

    void handle(SgUnaryOp& n)
    {
      // computed target ?
      if (n.get_operand() != nullptr)
      {
        mkCall(n);
        return;
      }

      ROSE_ASSERT(range.size() == 1);
      std::vector<SgExpression*> args = computeArguments();

      ROSE_ASSERT(args.size() == 1);
      n.set_operand(args[0]);
      res = &n;
    }

    void handle(SgBinaryOp& n)
    {
      // lhs and rhs must be null or not-null
      ROSE_ASSERT((n.get_lhs_operand() == nullptr) == (n.get_rhs_operand() == nullptr));

      // computed target ?
      if (n.get_lhs_operand() != nullptr)
      {
        mkCall(n);
        return;
      }

      ROSE_ASSERT(range.size() == 2);
      std::vector<SgExpression*> args = computeArguments();

      ROSE_ASSERT(args.size() == 2);
      n.set_lhs_operand(args[0]);
      n.set_rhs_operand(args[1]);
      res = &n;
    }

    ElemIdRange range;
    AstContext  ctx;
  };

  struct ArrayAggregateCreator
  {
      ArrayAggregateCreator(bool namedInit, AstContext astctx)
      : namedElements(namedInit), ctx(astctx), elems()
      {}

      ArrayAggregateCreator(ArrayAggregateCreator&&)                 = default;
      ArrayAggregateCreator& operator=(ArrayAggregateCreator&&)      = default;

      // \todo the following copying functions should be removed post C++17
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
    ROSE_ASSERT(el.Element_Kind == An_Association);

    Association_Struct&        assoc = el.The_Union.Association;
    ROSE_ASSERT(assoc.Association_Kind == An_Array_Component_Association);
    logKind("An_Array_Component_Association");

    SgExpression&              init = getExprID(assoc.Component_Expression, ctx);
    SgExpression*              sgnode = &init;
    ElemIdRange                range = idRange(assoc.Array_Component_Choices);

    ROSE_ASSERT(namedElements || range.empty());

    if (namedElements)
    {
      std::vector<SgExpression*> expr = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});
      SgExprListExp&             explst = SG_DEREF(sb::buildExprListExp(expr));

      sgnode = &mkAdaNamedInitializer(explst, init);
    }

    ROSE_ASSERT(sgnode);
    attachSourceLocation(*sgnode, el, ctx);
    elems.push_back(sgnode);
  }


  typedef SgExpression* (*mk_wrapper_fun)();

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

  template <class R, R* (*mkexp) (SgExpression*)>
  SgExpression* mk_rem_wrapper()
  {
    return mkexp(nullptr);
  }

  SgExpression&
  getOperator(Expression_Struct& expr, AstContext ctx)
  {
    typedef std::map<Operator_Kinds, std::pair<const char*, mk_wrapper_fun> > binary_maker_map_t;

    static const binary_maker_map_t binary_maker_map =
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

    ROSE_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    binary_maker_map_t::const_iterator pos = binary_maker_map.find(expr.Operator_Kind);

    if (pos != binary_maker_map.end())
    {
      logKind(pos->second.first);
      return SG_DEREF(pos->second.second());
    }

    ROSE_ASSERT(expr.Operator_Kind != Not_An_Operator);

    /* unused fields:
         Defining_Name_ID      Corresponding_Name_Definition;
         Defining_Name_List    Corresponding_Name_Definition_List; // Only >1 if the expression in a pragma is ambiguous
         Element_ID            Corresponding_Name_Declaration; // Decl or stmt
         Defining_Name_ID      Corresponding_Generic_Element;
    */
    return SG_DEREF(sb::buildOpaqueVarRefExp(expr.Name_Image, &ctx.scope()));
  }


  /// converts enum values to SgExpressions
  /// \todo currently only True and False are handled
  ///       revisit when Asis representation is complete
  SgExpression&
  getEnumLiteral(Expression_Struct& expr, AstContext ctx)
  {
    ROSE_ASSERT(expr.Expression_Kind == An_Enumeration_Literal);

    Element_Struct* typedcl = retrieveAsOpt<Element_Struct>(elemMap(), expr.Corresponding_Expression_Type_Definition);

    ROSE_ASSERT (!typedcl);

    std::string   enumstr{expr.Name_Image};
    SgExpression* res = NULL;

    boost::to_upper(enumstr);

    // \todo replace with actual enum values
    if (enumstr == "TRUE")
      res = sb::buildBoolValExp(1);
    else if (enumstr == "FALSE")
      res = sb::buildBoolValExp(0);
    else
      res = sb::buildStringVal(enumstr);

    return SG_DEREF( res );
  }

  /// defines ROSE AST types for which we do not generate scope qualification
  struct RoseRequiresScopeQual : sg::DispatchHandler<bool>
  {
    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }

    void handle(SgDeclarationStatement&) { res = true; }
    void handle(SgAdaTaskSpecDecl&)      { res = false; }
    void handle(SgAdaPackageSpecDecl&)   { res = false; }
  };


  /// tests whether ROSE represents the prefix expression
  ///   (e.g., true for objects, false for scope-qualification)
  bool roseRequiresPrefixID(Element_ID el, AstContext ctx)
  {
    Element_Struct&    elem = retrieveAs<Element_Struct>(elemMap(), el);
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& expr = elem.The_Union.Expression;

    if (expr.Expression_Kind == An_Identifier)
    {
    /// \todo dcl == nullptr should be an error (as soon as the Asis AST
    ///       is generated completely.
    SgDeclarationStatement* dcl = getDecl_opt(expr, ctx);

      return dcl == nullptr || sg::dispatch(RoseRequiresScopeQual(), dcl);
    }

    if (expr.Expression_Kind == A_Selected_Component)
    {
      /// \todo dcl == nullptr should be an error (as soon as the Asis AST
      ///       is generated completely.
      return    roseRequiresPrefixID(expr.Prefix, ctx)
             || roseRequiresPrefixID(expr.Selector, ctx);
    }

    ROSE_ASSERT(!FAIL_ON_ERROR);
    logWarn() << "roseRequiresPrefixID: untested expression-kind: "
              << expr.Expression_Kind
              << std::endl;
    return true;
  }

  struct ExprRefMaker : sg::DispatchHandler<SgExpression*>
  {
    void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

    void handle(SgDeclarationStatement& n)
    {
      logError() << "ExprRefMaker: " << typeid(n).name() << std::endl;

      res = sb::buildIntVal();
      ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    // void handle(SgImportStatement& n)

    void handle(SgFunctionDeclaration& n) { res = sb::buildFunctionRefExp(&n); }
    void handle(SgAdaTaskSpecDecl& n)     { res = &mkAdaTaskRefExp(n); }
  };

  struct TypeRefMaker : sg::DispatchHandler<SgExpression*>
  {
    void set(SgType* ty);

    void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

    void handle(SgDeclarationStatement& n)
    {
      logError() << "TypeRefMaker: " << typeid(n).name() << std::endl;

      set(sb::buildVoidType());
      ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    // void handle(SgImportStatement& n)

    void handle(SgTypedefDeclaration& n) { set(n.get_type()); }
  };

  void TypeRefMaker::set(SgType* ty)
  {
    ROSE_ASSERT(ty);
    res = sb::buildTypeExpression(ty);
    ROSE_ASSERT(res);
  }

} // anonymous




SgExpression&
getExpr(Element_Struct& elem, AstContext ctx)
{
  ROSE_ASSERT(elem.Element_Kind == An_Expression);

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
          res = sg::dispatch(ExprRefMaker{}, dcl);
        }
        else if (SgInitializedName* exc = findFirst(asisExcps(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
        {
          res = &mkExceptionRef(*exc, ctx.scope());
        }
        else if (SgDeclarationStatement* tydcl = findFirst(asisTypes(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
        {
          res = sg::dispatch(TypeRefMaker{}, tydcl);
        }
        else if (SgType* ty = findFirst(adaTypes(), AdaIdentifier(expr.Name_Image)))
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
        res = sg::dispatch(AdaCallBuilder(range, ctx), &target);

        /* unused fields:
           Expression_Struct
             Expression_ID         Prefix;
             bool                  Is_Prefix_Call;
             bool                  Is_Generalized_Reference;
             bool                  Is_Dispatching_Call;
             bool                  Is_Call_On_Dispatching_Operation;
        */
        break;
      }

    case An_Integer_Literal:                        // 2.4
      {
        logKind("An_Integer_Literal");
        res = &mkValue<SgIntVal>(expr.Value_Image);
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

    case An_Indexed_Component:                      // 4.1.1
      {
        logKind("An_Indexed_Component");

        SgExpression&              prefix = getExprID(expr.Prefix, ctx);
        ElemIdRange                idxrange = idRange(expr.Index_Expressions);
        std::vector<SgExpression*> idxexpr = traverseIDs(idxrange, elemMap(), ExprSeqCreator{ctx});
        SgExpression&              indices = SG_DEREF(idxexpr.size() < 2 ? idxexpr.at(0)
                                                                         : sb::buildExprListExp(idxexpr)
                                                     );

        res = sb::buildPntrArrRefExp(&prefix, &indices);
        /* unused fields
           Declaration_ID        Corresponding_Called_Function; // An_Indexed_Component (Is_Generalized_Indexing == true) //ASIS 2012 // 4.1.1
           bool                  Is_Generalized_Indexing
        */
        break;
      }

    case A_Slice:                                   // 4.1.2
      {
        logKind("A_Slice");

        SgExpression&              prefix = getExprID(expr.Prefix, ctx);
        SgExpression&              range  = getDiscreteRangeID(expr.Slice_Range, ctx);

        // \todo consider introducing a ROSE IR node for array slices
        res = sb::buildPntrArrRefExp(&prefix, &range);
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

    case A_Positional_Array_Aggregate:              // 4.3
    case A_Named_Array_Aggregate:                   // 4.3
      {
        const bool namedAggregate = expr.Expression_Kind == A_Named_Array_Aggregate;

        logKind(namedAggregate ? "A_Named_Array_Aggregate" : "A_Positional_Array_Aggregate");

        ElemIdRange                range  = idRange(expr.Array_Component_Associations);
        std::vector<SgExpression*> components = traverseIDs(range, elemMap(), ArrayAggregateCreator{namedAggregate, ctx});
        SgExprListExp&             explst = SG_DEREF(sb::buildExprListExp(components));

        attachSourceLocation(explst, elem, ctx);
        res = sb::buildAggregateInitializer(&explst);
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
        SgExpression&              choiceexp = mkChoiceExp(std::move(choices));

        res = inTest ? static_cast<SgExpression*>(sb::buildMembershipOp(&test, &choiceexp))
                     : sb::buildNonMembershipOp(&test, &choiceexp)
                     ;
        break;
      }

    case A_Type_Conversion:                         // 4.6
      {
        logKind("A_Type_Conversion");

        SgExpression& exp = getExprID(expr.Converted_Or_Qualified_Expression, ctx);
        SgType&       ty  = getDeclTypeID(expr.Converted_Or_Qualified_Subtype_Mark, ctx);

        res = sb::buildCastExp(&exp, &ty);
        /* unused fields: (Expression_Struct)
             Expression_ID         Predicate;
        */
        break;
      }

    case A_Box_Expression:                          // Ada 2005 4.3.1(4): 4.3.3(3:6)

    case An_Explicit_Dereference:                   // 4.1

    case A_Record_Aggregate:                        // 4.3
    case An_Extension_Aggregate:                    // 4.3

    case A_Raise_Expression:                        // 4.4 Ada 2012 (AI12-0022-1)

    case A_Qualified_Expression:                    // 4.7
    case An_Allocation_From_Subtype:                // 4.8
    case An_Allocation_From_Qualified_Expression:   // 4.8

    case A_Case_Expression:                         // Ada 2012
    case An_If_Expression:                          // Ada 2012
    case A_For_All_Quantified_Expression:           // Ada 2012
    case A_For_Some_Quantified_Expression:          // Ada 2012
    case Not_An_Expression: /* break; */            // An unexpected element
    default:
      logWarn() << "unhandled expression: " << expr.Expression_Kind << std::endl;
      res = sb::buildIntVal();
      ROSE_ASSERT(!FAIL_ON_ERROR);
  }

  attachSourceLocation(SG_DEREF(res), elem, ctx);
  return *res;
}


SgExpression&
getExprID(Element_ID el, AstContext ctx)
{
  return getExpr(retrieveAs<Element_Struct>(elemMap(), el), ctx);
}

SgExpression&
getExprID_opt(Element_ID el, AstContext ctx)
{
  if (isInvalidId(el))
  {
    logWarn() << "uninitalized expression id " << el << std::endl;
    return SG_DEREF( sb::buildNullExpression() );
  }

  return el == 0 ? SG_DEREF( sb::buildNullExpression() )
                 : getExprID(el, ctx)
                 ;
}

namespace
{
  template <typename AsisDiscreteRangeStruct>
  SgExpression&
  getDiscreteRangeGeneric(Definition_Struct& def, AsisDiscreteRangeStruct& range, AstContext ctx)
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

          ROSE_ASSERT(ty);
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
          /*
          logKind("A_Discrete_Range_Attribute_Reference");
          SgExpression& expr = getExprID(range.Prefix, ctx);
          std::string   attr = getAttributeNameID(range.Attribute_Designator_Identifier, ctx);

          res = mkAdaExprAttr(expr, range, "range_type");
          break ;
          */
        }

      case Not_A_Discrete_Range:                  // An unexpected element
      default:
        logWarn() << "Unhandled range: " << range.Discrete_Range_Kind << std::endl;
        res = &mkRangeExp();
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    return SG_DEREF(res);
  }

  /// \private
  /// returns a range expression from the Asis definition \ref def
  SgExpression&
  getDiscreteRange(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == A_Discrete_Range);

    return getDiscreteRangeGeneric(def, def.The_Union.The_Discrete_Range, ctx);
  }

  SgExpression&
  getDiscreteRange(Element_Struct& el, AstContext ctx)
  {
    ROSE_ASSERT(el.Element_Kind == A_Definition);

    return getDiscreteRange(el.The_Union.Definition, ctx);
  }

  SgExpression&
  getDiscreteSubtype(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == A_Discrete_Subtype_Definition);

    return getDiscreteRangeGeneric(def, def.The_Union.The_Discrete_Subtype_Definition, ctx);
  }

  SgExpression&
  getConstraintExpr(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == A_Constraint);

    Constraint_Struct& constraint = def.The_Union.The_Constraint;

    ROSE_ASSERT (constraint.Constraint_Kind == A_Simple_Expression_Range);
    logKind("A_Simple_Expression_Range");

    SgExpression& lb = getExprID(constraint.Lower_Bound, ctx);
    SgExpression& ub = getExprID(constraint.Upper_Bound, ctx);
    return mkRangeExp(lb, ub);
  }

  /// \private
  /// returns an expression from the Asis definition \ref def
  SgExpression&
  getDefinitionExpr(Element_Struct& el, AstContext ctx)
  {
    ROSE_ASSERT(el.Element_Kind == A_Definition);

    Definition_Struct& def = el.The_Union.Definition;
    SgExpression*      res = nullptr;

    switch (def.Definition_Kind)
    {
      case A_Discrete_Range:
        logKind("A_Discrete_Range");
        res = &getDiscreteRange(def, ctx);
        break;

      case A_Discrete_Subtype_Definition:
        logKind("A_Discrete_Subtype_Definition");
        res = &getDiscreteSubtype(def, ctx);
        break;

      case An_Others_Choice:
        logKind("An_Others_Choice");
        res = &mkOthersExp();
        break;

      case A_Constraint:
        logKind("A_Constraint");
        res = &getConstraintExpr(def, ctx);
        break;

      default:
        logWarn() << "Unhandled definition expr: " << def.Definition_Kind << std::endl;
        res = sb::buildNullExpression();
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    attachSourceLocation(SG_DEREF(res), el, ctx);
    return *res;
  }
}

void ExprSeqCreator::operator()(Element_Struct& el)
{
  SgExpression* res = nullptr;

  if (el.Element_Kind == An_Expression)
    res = &getExpr(el, ctx);
  else if (el.Element_Kind == A_Definition)
    res = &getDefinitionExpr(el, ctx);

  ROSE_ASSERT(res);
  elems.push_back(res);
}

void ArgListCreator::operator()(Element_Struct& elem)
{
  args.push_back(&getArg(elem, ctx));
}


void RangeListCreator::operator()(Element_Struct& elem)
{
  SgExpression& rngexp = getDiscreteRange(elem, ctx);
  SgRangeExp*   range  = isSgRangeExp(&rngexp);
  ROSE_ASSERT(range);

  lst.push_back(range);
}


SgExpression&
getDiscreteRangeID(Element_ID id, AstContext ctx)
{
  return getDiscreteRange(retrieveAs<Element_Struct>(elemMap(), id), ctx);
}

} // namespace Ada_ROSE_Translation
