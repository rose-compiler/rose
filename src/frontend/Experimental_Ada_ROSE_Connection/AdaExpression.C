#include "sage3basic.h"

#include <vector>

#include "sageGeneric.h"
#include "sageBuilder.h"

#include "AdaExpression.h"

#include "Ada_to_ROSE.h"
#include "AdaMaker.h"
#include "AdaStatement.h"

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

    SgExpression&       arg        = getExprID(assoc.Actual_Parameter, ctx);
    Element_Struct*     formalParm = retrieveAsOpt<Element_Struct>(elemMap(), assoc.Formal_Parameter);

    if (!formalParm) return arg;

    ROSE_ASSERT(formalParm->Element_Kind == An_Expression);

    Expression_Struct&  formalName = formalParm->The_Union.Expression;
    ROSE_ASSERT(formalName.Expression_Kind == An_Identifier);

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

  typedef SgExpression* (*mk_wrapper_fun)();

  // homogeneous return types instead of covariant ones
  template <class R, R* (*mkexp) (SgExpression*, SgExpression*)>
  SgExpression* mk2_wrapper()
  {
    return mkexp(nullptr, nullptr);
  }

  // homogeneous return types instead of covariant ones
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

/*
    SgExpression* mkCall(SgExpression* callee, SgExpression* args)
    {
      SgExprListExp* lst = SG_ASSERT_TYPE(SgExprListExp, args);

      return sb::buildFunctionCallExp(callee, lst);
    }
*/

  SgExpression&
  getOperator(Expression_Struct& expr, AstContext ctx)
  {
    typedef std::map<Operator_Kinds, mk_wrapper_fun> binary_maker_map_t;

    static const binary_maker_map_t binary_maker_map
                     = { { An_And_Operator,                  mk2_wrapper<SgBitAndOp,         sb::buildBitAndOp> },         /* break; */
                         { An_Or_Operator,                   mk2_wrapper<SgBitOrOp,          sb::buildBitOrOp> },          /* break; */
                         { An_Xor_Operator,                  mk2_wrapper<SgBitXorOp,         sb::buildBitXorOp> },         /* break; */
                         { An_Equal_Operator,                mk2_wrapper<SgEqualityOp,       sb::buildEqualityOp> },       /* break; */
                         { A_Not_Equal_Operator,             mk2_wrapper<SgNotEqualOp,       sb::buildNotEqualOp> },       /* break; */
                         { A_Less_Than_Operator,             mk2_wrapper<SgLessThanOp,       sb::buildLessThanOp> },       /* break; */
                         { A_Less_Than_Or_Equal_Operator,    mk2_wrapper<SgLessOrEqualOp,    sb::buildLessOrEqualOp> },    /* break; */
                         { A_Greater_Than_Operator,          mk2_wrapper<SgGreaterThanOp,    sb::buildGreaterThanOp> },    /* break; */
                         { A_Greater_Than_Or_Equal_Operator, mk2_wrapper<SgGreaterOrEqualOp, sb::buildGreaterOrEqualOp> }, /* break; */
                         { A_Plus_Operator,                  mk2_wrapper<SgAddOp,            sb::buildAddOp> },            /* break; */
                         { A_Minus_Operator,                 mk2_wrapper<SgSubtractOp,       sb::buildSubtractOp> },       /* break; */
                         { A_Concatenate_Operator,           mk2_wrapper<SgConcatenationOp,  sb::buildConcatenationOp> },  /* break; */
                         { A_Unary_Plus_Operator,            mk1_wrapper<SgUnaryAddOp,       sb::buildUnaryAddOp> },       /* break; */
                         { A_Unary_Minus_Operator,           mk1_wrapper<SgMinusOp,          sb::buildMinusOp> },          /* break; */
                         { A_Multiply_Operator,              mk2_wrapper<SgMultiplyOp,       sb::buildMultiplyOp> },       /* break; */
                         { A_Divide_Operator,                mk2_wrapper<SgDivideOp,         sb::buildDivideOp> },         /* break; */
                         { A_Mod_Operator,                   mk2_wrapper<SgModOp,            sb::buildModOp> },            /* break; */
                         { A_Rem_Operator,                   mk2_wrapper<SgRemOp,            buildRemOp> },                /* break; */
                         { An_Exponentiate_Operator,         mk2_wrapper<SgPowerOp,          sb::buildPowerOp> },          /* break; */
                         { An_Abs_Operator,                  mk1_wrapper<SgAbsOp,            buildAbsOp> },                /* break; */
                         { A_Not_Operator,                   mk1_wrapper<SgNotOp,            sb::buildNotOp> },            /* break; */
                       };

    ROSE_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    binary_maker_map_t::const_iterator pos = binary_maker_map.find(expr.Operator_Kind);

    if (pos != binary_maker_map.end())
      return SG_DEREF(pos->second());

    ROSE_ASSERT(expr.Operator_Kind != Not_An_Operator); /* break; */

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

    if (enumstr == "True")
      res = sb::buildBoolValExp(1);
    else if (enumstr == "False")
      res = sb::buildBoolValExp(0);

    return SG_DEREF( res );
  }

} // anonymous




SgExpression&
getExpr(Element_Struct& elem, AstContext ctx)
{
  ROSE_ASSERT(elem.Element_Kind == An_Expression);

  bool               withParen = false;
  Expression_Struct& expr      = elem.The_Union.Expression;
  SgExpression*      res       = NULL;

  switch (expr.Expression_Kind)
  {
    case An_Identifier:                             // 4.1
      {
        if (SgInitializedName* var = findFirst(asisVars(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
        {
          res = sb::buildVarRefExp(var, &ctx.scope());
        }
        else if (SgDeclarationStatement* dcl = getDecl_opt(expr, ctx))
        {
          SgFunctionDeclaration* fundcl = isSgFunctionDeclaration(dcl);
          ROSE_ASSERT(fundcl);

          res = sb::buildFunctionRefExp(fundcl);
        }
        else if (SgInitializedName* exc = findFirst(asisExcps(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
        {
          res = &mkExceptionRef(*exc, ctx.scope());
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
        res = &mkValue<SgIntVal>(expr.Value_Image);
        /* unused fields: (Expression_Struct)
             enum Attribute_Kinds  Attribute_Kind
        */
        break;
      }

    case A_Character_Literal:                       // 4.1
      {
        res = &mkValue<SgCharVal>(expr.Name_Image);
        /* unused fields: (Expression_Struct)
             Defining_Name_ID      Corresponding_Name_Definition;
             Defining_Name_List    Corresponding_Name_Definition_List;
             Element_ID            Corresponding_Name_Declaration;
             Defining_Name_ID      Corresponding_Generic_Element
        */
        break;
      }

    case A_Real_Literal:                            // 2.4.1
      {
        res = &mkValue<SgLongDoubleVal>(expr.Value_Image);
        /* unused fields: (Expression_Struct)
             enum Attribute_Kinds  Attribute_Kind;
        */
        break;
      }

    case An_Operator_Symbol:                        // 4.1
      {
        res = &getOperator(expr, ctx);
        /* unused fields:
           char                 *Name_Image;
           Defining_Name_ID      Corresponding_Name_Definition;
           Defining_Name_List    Corresponding_Name_Definition_List;
           Element_ID            Corresponding_Name_Declaration;
           Defining_Name_ID      Corresponding_Generic_Element;
        */
        break;
      }

    case An_Enumeration_Literal:                    // 4.1
      {
        res = &getEnumLiteral(expr, ctx);
        /* unused fields: (Expression_Struct)
           Defining_Name_ID      Corresponding_Name_Definition;
           Defining_Name_List    Corresponding_Name_Definition_List;
           Element_ID            Corresponding_Name_Declaration;
           Defining_Name_ID      Corresponding_Generic_Element;
        */
        break;
      }

    case A_Selected_Component:                      // 4.1.3
      {
        SgExpression& prefix   = getExprID(expr.Prefix, ctx);
        SgExpression& selector = getExprID(expr.Selector, ctx);

        res = &mkSelectedComponent(prefix, selector);
        /* unused fields: (Expression_Struct)
        */
        break;
      }

    case An_And_Then_Short_Circuit:                 // 4.4
      {
        SgExpression& lhs = getExprID(expr.Short_Circuit_Operation_Left_Expression, ctx);
        SgExpression& rhs = getExprID(expr.Short_Circuit_Operation_Right_Expression, ctx);

        res = sb::buildAndOp(&lhs, &rhs);
        /* unused fields: (Expression_Struct)
        */
        break;
      }

    case An_Or_Else_Short_Circuit:                  // 4.4
      {
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
        withParen = true;

        // \todo remove _opt when the asis connection implements A_Parenthesized_Expression
        res = &getExprID_opt(expr.Expression_Parenthesized, ctx);

        /* unused fields: (Expression_Struct)
        */
        break;
      }

    case A_Type_Conversion:                         // 4.6
      {
        SgExpression& exp = getExprID(expr.Converted_Or_Qualified_Expression, ctx);
        SgType&       ty  = getDeclTypeID(expr.Converted_Or_Qualified_Subtype_Mark, ctx);

        res = sb::buildCastExp(&exp, &ty);
        /* unused fields: (Expression_Struct)
             Expression_ID         Predicate;
        */
        break;
      }

    case A_Box_Expression:                          // Ada 2005 4.3.1(4): 4.3.3(3:6)
    case A_String_Literal:                          // 2.6

    case An_Explicit_Dereference:                   // 4.1

    case An_Indexed_Component:                      // 4.1.1
    case A_Slice:                                   // 4.1.2
    case An_Attribute_Reference:                    // 4.1.4  -> Attribute_Kinds
    case A_Record_Aggregate:                        // 4.3
    case An_Extension_Aggregate:                    // 4.3
    case A_Positional_Array_Aggregate:              // 4.3
    case A_Named_Array_Aggregate:                   // 4.3

    case An_In_Membership_Test:                     // 4.4  Ada 2012
    case A_Not_In_Membership_Test:                  // 4.4  Ada 2012

    case A_Null_Literal:                            // 4.4
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

  attachSourceLocation(SG_DEREF(res), elem);
  res->set_need_paren(withParen);
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
  if (isInvaldId(el))
  {
    logWarn() << "unintialized expression id " << el << std::endl;
    return SG_DEREF( sb::buildNullExpression() );
  }

  return el == 0 ? SG_DEREF( sb::buildNullExpression() )
                 : getExprID(el, ctx)
                 ;
}

namespace
{
  /// \private
  /// returns a range expression from the Asis definition \ref def
  SgRangeExp&
  getDiscreteRange(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == A_Discrete_Range);

    SgRangeExp*            res = nullptr;
    Discrete_Range_Struct& range = def.The_Union.The_Discrete_Range;

    switch (range.Discrete_Range_Kind)
    {
      case A_Discrete_Simple_Expression_Range:    // 3.6.1, 3.5
        {
          SgExpression& lb = getExprID(range.Lower_Bound, ctx);
          SgExpression& ub = getExprID(range.Upper_Bound, ctx);

          res = &mkRangeExp(lb, ub);
          break;
        }

      case A_Discrete_Subtype_Indication:         // 3.6.1(6), 3.2.2
      case A_Discrete_Range_Attribute_Reference:  // 3.6.1, 3.5
      case Not_A_Discrete_Range:                  // An unexpected element
      default:
        logWarn() << "Unhandled range: " << range.Discrete_Range_Kind << std::endl;
        res = &mkRangeExp();
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    return SG_DEREF(res);
  }

  /// \private
  /// returns an expression from the Asis definition \ref def
  SgExpression&
  getDefinitionExpr(Definition_Struct& def, AstContext ctx)
  {
    SgExpression* res = nullptr;

    switch (def.Definition_Kind)
    {
      case A_Discrete_Range:
        res = &getDiscreteRange(def, ctx);
        break;

      case An_Others_Choice:
        res = &mkOthersExp();
        break;

      default:
        logWarn() << "Unhandled definition: " << def.Definition_Kind << std::endl;
        res = sb::buildNullExpression();
        ROSE_ASSERT(!FAIL_ON_ERROR);
    }

    return SG_DEREF(res);
  }
}

  void ExprSeqCreator::operator()(Element_Struct& el)
  {
    SgExpression* res = nullptr;

    if (el.Element_Kind == An_Expression)
      res = &getExpr(el, ctx);
    else if (el.Element_Kind == A_Definition)
      res = &getDefinitionExpr(el.The_Union.Definition, ctx);

    ROSE_ASSERT(res);
    elems.push_back(res);
  }

  void ArgListCreator::operator()(Element_Struct& elem)
  {
    args.push_back(&getArg(elem, ctx));
  }

}
