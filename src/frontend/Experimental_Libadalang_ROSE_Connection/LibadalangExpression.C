#include "sage3basic.h"

#include <vector>
#include <boost/algorithm/string.hpp>

#include "sageGeneric.h"
#include "sageBuilder.h"
#include "sageInterfaceAda.h"

#include "LibadalangExpression.h"

#include "Libadalang_to_ROSE.h"
#include "AdaMaker.h"
#include "LibadalangStatement.h"
#include "LibadalangType.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Libadalang_ROSE_Translation
{

namespace
{
  SgExpression&
  getArg(ada_base_entity* lal_element, AstContext ctx)
  {
    //Get the kind
    ada_node_kind_enum kind = ada_node_kind(lal_element);

    ada_text kind_name;
    ada_kind_name(kind, &kind_name);
    std::string kind_name_string = ada_text_to_locale_string(&kind_name);
    ada_destroy_text(&kind_name);
    logKind(kind_name_string.c_str(), kind);

    ada_base_entity actual_parameter;
    ada_base_entity formal_parameter;
    bool has_formal = false;

    //If this is an assoc, get the actual parameter
    if(kind == ada_param_assoc){
      ada_param_assoc_f_r_expr(lal_element, &actual_parameter);
      ada_param_assoc_f_designator(lal_element, &formal_parameter);
      has_formal = true;
    } else if(kind == ada_pragma_argument_assoc){
      ada_pragma_argument_assoc_f_expr(lal_element, &actual_parameter);
      ada_pragma_argument_assoc_f_id(lal_element, &formal_parameter);
      has_formal = true; //TODO Test this
    } else { //TODO What of the other assocs?
      actual_parameter = *lal_element;
    }

    SgExpression&       arg        = getExpr(&actual_parameter, ctx);

    if(!has_formal || ada_node_is_null(&formal_parameter)) return arg;

    /*ADA_ASSERT(formalParm->Element_Kind == An_Expression);

    ADA_ASSERT(  formalName.Expression_Kind == An_Identifier
              || formalName.Expression_Kind == An_Operator_Symbol
              );*/

    //Get the name of this node
    std::string element_name = canonical_text_as_string(&formal_parameter);

    //logKind("An_Identifier", formalParm->ID);
    SgExpression&       namedArg = SG_DEREF(sb::buildActualArgumentExpression_nfi(element_name, &arg));

    attachSourceLocation(namedArg, lal_element, ctx);
    return namedArg;
  }
} //End unnamed namespace

SgAdaAttributeExp&
getAttributeExpr(ada_base_entity* lal_element, AstContext ctx, ada_base_entity* argRangeSuppl)
{

  SgAdaAttributeExp* res = nullptr;

  //Get the name of the attribute
  ada_base_entity lal_attribute;
  ada_attribute_ref_f_attribute(lal_element, &lal_attribute);
  std::string name = canonical_text_as_string(&lal_attribute);

  //Get the prefix
  ada_base_entity lal_prefix;
  ada_attribute_ref_f_prefix(lal_element, &lal_prefix);
  SgExpression&      obj = getExpr(&lal_prefix, ctx);

    // attributes with optional expression list argument
    /*case A_First_Attribute:            // 3.5(12), 3.6.2(3), K(68), K(70)
    case A_Length_Attribute:           // 3.6.2(9), K(117)
    case An_Unknown_Attribute:          // Unknown to ASIS
    case An_Implementation_Defined_Attribute:  // Reference Manual, Annex M
    case A_Last_Attribute:            // 3.5(13), 3.6.2(5), K(102), K(104)
    case A_Range_Attribute:            // 3.5(14), 3.6.2(7), K(187), ú(189)*/
  if(name == "")
    {
      /*ElemIdRange                range = idRange(expr.Attribute_Designator_Expressions);
      ADA_ASSERT(argRangeSuppl.empty() || range.empty());

      std::vector<SgExpression*> exprs;

      if (range.empty())
        exprs = traverseIDs(argRangeSuppl, elemMap(), ArgListCreator{ctx});
      else
        exprs = traverseIDs(range, elemMap(), ExprSeqCreator{ctx});

      res = &mkAdaAttributeExp(obj, name.fullName, mkExprListExp(exprs));*/ //TODO
    }

    // attributes with empty expression list argument

    /*case An_Access_Attribute:          // 3.10.2(24), 3.10.2(32), K(2), K(4)
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
    //  |A2012 end*/
    else if(name == "access")
      {
        logInfo() << "untested attribute created: " << name
                  << std::endl;
        std::vector<SgExpression*> exprs;
        if(argRangeSuppl != nullptr && !ada_node_is_null(argRangeSuppl)){
          //Call getArg on each child node
          int count = ada_node_children_count(argRangeSuppl);
          for(int i = 0; i < count; i++){
            ada_base_entity lal_arg;
            ada_node_child(argRangeSuppl, i, &lal_arg);
            exprs.push_back(&getArg(&lal_arg, ctx));
          }
        }
        SgExprListExp&             args  = mkExprListExp(exprs);

        res = &mkAdaAttributeExp(obj, name, args);
      }

    // failure kinds
    else
      {
        logError() << "unknown expression attribute: " << name
                   << std::endl;

        res = &mkAdaAttributeExp(obj, "ErrorAttr: " + name, mkExprListExp());
      }

  return SG_DEREF(res);
}

namespace
{
  struct AdaCallBuilder : sg::DispatchHandler<SgExpression*>
  {
      using base = sg::DispatchHandler<SgExpression*>;

      AdaCallBuilder( ada_base_entity* targetid,
                      std::vector<SgExpression*> arglist,
                      bool usePrefixCallSyntax,
                      bool useObjectCallSyntax,
                      AstContext astctx
                    )
      : base(nullptr),
        tgtid(targetid),
        args(std::move(arglist)),
        prefixCallSyntax(usePrefixCallSyntax),
        objectCallSyntax(useObjectCallSyntax),
        ctx(astctx)
      {}

      void mkCall(SgExpression& n)
      {
        SgExprListExp& arglst = mkExprListExp(args);

        res = &mkFunctionCallExp(n, arglst, !prefixCallSyntax, objectCallSyntax);
      }

      void handle(SgNode& n)       { SG_UNEXPECTED_NODE(n); }

      // default
      void handle(SgExpression& n) { mkCall(n); }

      // same as mkCall
      // void handle(SgFunctionRefExp& n)
      // {
      //   SgExprListExp& arglst = mkExprListExp(args);
      //   res = &mkFunctionCallExp(n, arglst, !prefixCallSyntax, objectCallSyntax);
      // }

      void handle(SgUnaryOp& n)
      {
        // computed target ?
        if (n.get_operand() != nullptr)
        {
          mkCall(n);
          return;
        }

        //ADA_ASSERT(args.size() == 1);
        n.set_operand(args[0]);
        res = &n;
      }

      void handle(SgBinaryOp& n)
      {
        // lhs and rhs must both be null or not-null
        //ADA_ASSERT((n.get_lhs_operand() == nullptr) == (n.get_rhs_operand() == nullptr));

        // computed target ?
        if (n.get_lhs_operand() != nullptr)
        {
          mkCall(n);
          return;
        }

        //ADA_ASSERT(args.size() == 2);
        n.set_lhs_operand(args[0]);
        n.set_rhs_operand(args[1]);
        res = &n;
      }

    private:
      ada_base_entity*           tgtid;
      std::vector<SgExpression*> args;
      bool                       prefixCallSyntax;
      bool                       objectCallSyntax;
      AstContext                 ctx;
  };

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

        res = sb::buildIntVal_nfi();
        //ADA_ASSERT(!FAIL_ON_ERROR(ctx));
      }

      // void handle(SgImportStatement& n)

      void handle(SgFunctionDeclaration& n)    { res = &mkFunctionRefExp(n); }
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
      }

      // void handle(SgImportStatement& n)

      void handle(SgClassDeclaration& n)         { set(n.get_type()); }
      void handle(SgTypedefDeclaration& n)       { set(n.get_type()); }
      void handle(SgEnumDeclaration& n)          { set(n.get_type()); }
      void handle(SgAdaFormalTypeDecl& n)        { set(n.get_type()); }
      void handle(SgAdaTaskTypeDecl& n)          { set(n.get_type()); }
      void handle(SgAdaProtectedTypeDecl& n)     { set(n.get_type()); }
      void handle(SgAdaDiscriminatedTypeDecl& n) { set(n.get_type()); }

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
  /// \note surviving use cases:
  ///       * pragma inline("+")
  ///       * generic instantiations is instance(integer, "+")
  ///       * ??
  SgExpression&
  getOperator_fallback(ada_base_entity* lal_expr, bool unary, AstContext ctx)
  {
    using MkWrapperFn = std::function<SgExpression*()>;
    using OperatorMakerMap = std::map<ada_node_kind_enum, std::pair<const char*, MkWrapperFn> >;

    //Get the kind of this node
    ada_node_kind_enum kind = ada_node_kind(lal_expr);

    logTrace() << "In getOperator_fallback for node of kind " << kind << std::endl;

    //Get the name of this node
    std::string expr_name = canonical_text_as_string(lal_expr);

    static const OperatorMakerMap makerMap =
    { { ada_op_and,     {"ada_op_and",     mk2_wrapper<SgBitAndOp,         sb::buildBitAndOp> }},
      { ada_op_or,      {"ada_op_or",      mk2_wrapper<SgBitOrOp,          sb::buildBitOrOp> }},
      { ada_op_xor,     {"ada_op_xor",     mk2_wrapper<SgBitXorOp,         sb::buildBitXorOp> }},
      { ada_op_eq,      {"ada_op_eq",      mk2_wrapper<SgEqualityOp,       sb::buildEqualityOp> }},
      { ada_op_neq,     {"ada_op_neq",     mk2_wrapper<SgNotEqualOp,       sb::buildNotEqualOp> }},
      { ada_op_lt,      {"ada_op_lt",      mk2_wrapper<SgLessThanOp,       sb::buildLessThanOp> }},
      { ada_op_lte,     {"ada_op_lte",     mk2_wrapper<SgLessOrEqualOp,    sb::buildLessOrEqualOp> }},
      { ada_op_gt,      {"ada_op_gt",      mk2_wrapper<SgGreaterThanOp,    sb::buildGreaterThanOp> }},
      { ada_op_gte,     {"ada_op_gte",     mk2_wrapper<SgGreaterOrEqualOp, sb::buildGreaterOrEqualOp> }},
      { ada_op_plus,    {"ada_op_plus",    mk2_wrapper<SgAddOp,            sb::buildAddOp> }},
      { ada_op_minus,   {"ada_op_minus",   mk2_wrapper<SgSubtractOp,       sb::buildSubtractOp> }},
      { ada_op_concat,  {"ada_op_concat",  mk2_wrapper<SgConcatenationOp,  sb::buildConcatenationOp> }},
      { ada_op_mult,    {"ada_op_mult",    mk2_wrapper<SgMultiplyOp,       sb::buildMultiplyOp> }},
      { ada_op_div,     {"ada_op_div",     mk2_wrapper<SgDivideOp,         sb::buildDivideOp> }},
      { ada_op_mod,     {"ada_op_mod",     mk2_wrapper<SgModOp,            sb::buildModOp> }},
      { ada_op_rem,     {"ada_op_rem",     mk2_wrapper<SgRemOp,            sb::buildRemOp> }},
      { ada_op_pow,     {"ada_op_pow",     mk2_wrapper<SgExponentiationOp, sb::buildExponentiationOp> }},
      { ada_op_abs,     {"ada_op_abs",     mk1_wrapper<SgAbsOp,            sb::buildAbsOp> }},
      { ada_op_not,     {"ada_op_not",     mk1_wrapper<SgNotOp,            sb::buildNotOp> }},
    };

    //Libadalang doesn't inherently treat unary +/- different from binary +/-, so we need a different map for them
    static const OperatorMakerMap unaryMakerMap =
    {
      { ada_op_plus,    {"ada_op_plus",    mk1_wrapper<SgUnaryAddOp,       sb::buildUnaryAddOp> }},
      { ada_op_minus,   {"ada_op_minus",   mk1_wrapper<SgMinusOp,          sb::buildMinusOp> }},
    };

    //TODO What about ada_op_and_then: ada_op_double_dot: ada_op_in: ada_op_not_in: ada_op_or_else:

    //ADA_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    if(unary){
      OperatorMakerMap::const_iterator pos = unaryMakerMap.find(kind);

      if (pos != unaryMakerMap.end())
      {
        logKind(pos->second.first, kind);

        SgExpression* res = pos->second.second();

        operatorExprs().push_back(res);
        return SG_DEREF(res);
      }
    }

    OperatorMakerMap::const_iterator pos = makerMap.find(kind);

    if (pos != makerMap.end())
    {
      logKind(pos->second.first, kind);

      SgExpression* res = pos->second.second();

      operatorExprs().push_back(res);
      return SG_DEREF(res);
    }

    //ADA_ASSERT(expr.Operator_Kind != Not_An_Operator);

    /* unused fields:
         Defining_Name_ID      Corresponding_Name_Definition;
         Defining_Name_List    Corresponding_Name_Definition_List; // Only >1 if the expression in a pragma is ambiguous
         Element_ID            Corresponding_Name_Declaration; // Decl or stmt
         Defining_Name_ID      Corresponding_Generic_Element;
    */
    return SG_DEREF(sb::buildOpaqueVarRefExp(expr_name, &ctx.scope()));
  }

  bool anonAccessType(const OperatorCallSupplement& suppl, const AstContext&)
  {
    const OperatorCallSupplement::ArgDescList& argtypes = suppl.args();
    SgAdaAccessType*                           argty    = isSgAdaAccessType(argtypes.front().type());

    return argty && argty->get_is_anonymous();
  }


  SgType& boolType(AstContext ctx)
  {
    ada_base_entity* lal_root = ctx.unit_root();
    ada_base_entity ada_bool;
    ada_ada_node_p_bool_type(lal_root, &ada_bool);
    int hash = hash_node(&ada_bool);
    return SG_DEREF(adaTypes().at(hash));
  }

  OperatorCallSupplement
  createSupplement(SgTypePtrList types, SgType* result)
  {
    static constexpr int MAX_PARAMS = 2;
    static const std::string parmNames[MAX_PARAMS] = { "Left", "Right" };

    OperatorCallSupplement::ArgDescList args;
    std::size_t                         parmNameIdx = MAX_PARAMS - types.size() - 1;

    for (SgType* ty : types)
    {
      args.emplace_back(parmNames[++parmNameIdx], ty);
    }

    return { std::move(args), result };
  }


  /// Takes available operator information that was computed from argument types and computes the signature of
  /// a generatable operator.
  /// \param  name operator name (w/o si::Ada::roseOperatorPrefix)
  /// \param  suppl the available argument type information
  /// \param  domArgPos the dominant argument position
  /// \param  ctx the translator context
  /// \return an adjusted operator call supplement
  /// \note
  ///   since we rely on a solid frontend, we may not need to check all the requirements
  OperatorCallSupplement
  operatorSignature(AdaIdentifier name, const OperatorCallSupplement& suppl, std::size_t domArgPos, AstContext ctx)
  {
    // see https://www.adaic.com/resources/add_content/standards/05rm/html/RM-4-5-2.html
    // see https://www.adaic.com/resources/add_content/standards/05rm/html/RM-4-4.html

    if (name == "&")
    {
      SgType* resTy = nullptr;
      SgType& lhsTy = SG_DEREF( suppl.args().front().type() );

      if (SgArrayType* lhsArrTy = si::Ada::getArrayTypeInfo(lhsTy).type())
        resTy = lhsArrTy;
      else if (SgArrayType* rhsArrTy = si::Ada::getArrayTypeInfo(suppl.args().back().type()).type())
        resTy = rhsArrTy;
      else
      {
        SgType&        positive = SG_DEREF(adaTypesByName().at(AdaIdentifier{"POSITIVE"}));
        SgExprListExp& idx = mkExprListExp({&mkTypeExpression(positive)});

        resTy = &mkArrayType(lhsTy, idx, true /* unconstrained */);
      }

      //ADA_ASSERT(resTy != nullptr);
      return { suppl.args(), resTy };
    }

    SgType* const domTy = suppl.args().at(domArgPos).type();

    if (name == "=")
    {
      // ADA_ASSERT(!anonAccessType(suppl, ctx) && "test case sought"); // catch test case

      // requires (  nonLimitedArgumentType(domTy, ctx)
      //          && equalArgumentTypes(suppl, ctx)     // will be adjusted
      //          && resultTypeIsBool(suppl, ctx)       // will be adjusted
      //          )
      return createSupplement( {domTy, domTy}, &boolType(ctx) );
    }

    if (name == "/=" && !anonAccessType(suppl, ctx))
    {
      //ADA_ASSERT(!anonAccessType(suppl, ctx) && "test case sought"); // catch test case

      // requires (  ( nonLimitedArgumentType(suppl, ctx) || hasEqualityOperator(suppl, ctx) )
      //          && equalArgumentTypes(suppl, ctx) // will be adjusted
      //          && resultTypeIsBool(suppl, ctx)   // will be adjusted
      //          );
      return createSupplement( {domTy, domTy}, &boolType(ctx) );
    }


    if ((name == "<") || (name == "<=") || (name == ">") || (name == ">="))
    {
      bool req = (  (si::Ada::isScalarType(domTy) || si::Ada::isDiscreteArrayType(domTy))
                 //~ && equalArgumentTypes(suppl, ctx) // will be adjusted
                 //~ && resultTypeIsBool(suppl, ctx)   // will be adjusted
                 );

      if (!req)
      {
        if (true)
          logWarn() << "(sca: " << si::Ada::isScalarType(domTy)
                    << " | dsc: " << si::Ada::isDiscreteArrayType(domTy)
                    << " -> : " << req
                    << std::endl;

        return OperatorCallSupplement{};
      }

      return createSupplement( {domTy, domTy}, &boolType(ctx) );
    }

    if (name == "AND" || name == "OR" || name == "XOR")
    {
      // requires (  equalArgumentTypes(suppl, ctx) // will be adjusted
      //          && (si::Ada::isBooleanType(domTy) || si::Ada::isModularType(domTy) || (isBoolArray(domTy)))
      //          )
      return createSupplement( {domTy, domTy}, domTy );
    }

    if ( (  name == "+"   || name == "-"   || name == "*"
         || name == "/"   || name == "MOD" || name == "REM"
         || name == "ABS" || name == "NOT" || name == "**"
         )
       )
    {
      if (suppl.args().size() == 1)
         return createSupplement( { domTy }, domTy );

      return createSupplement( {domTy,domTy}, domTy );
    }

    return OperatorCallSupplement{};
  }


  bool equalParameterTypes(const SgTypePtrList& lhs, const OperatorCallSupplement::ArgDescList& rhs)
  {
    auto typeEquality = [](const SgType* lty, const ArgDesc& rty) -> bool
                        {
                          return lty == rty.type();
                        };

    return (  (lhs.size() == rhs.size())
           && std::equal(lhs.begin(), lhs.end(), rhs.begin(), typeEquality)
           );
  }

  SgFunctionDeclaration*
  findExistingOperator(AdaIdentifier name, SgScopeStatement& scope, const OperatorCallSupplement& suppl)
  {
    using OpMap = map_t<OperatorKey, std::vector<OperatorDesc> >;

    OpMap const&          opMap = operatorSupport();
    OpMap::const_iterator pos = opMap.find({&scope, name});

    if (pos == opMap.end()) return nullptr;

    auto sameSignature =
            [&suppl](const OperatorDesc& desc) -> bool
            {
              const SgFunctionDeclaration& cand   = SG_DEREF( desc.function() );
              const SgFunctionType&        candTy = SG_DEREF( cand.get_type() );

              return (  (candTy.get_return_type() == suppl.result())
                     && equalParameterTypes(candTy.get_arguments(), suppl.args())
                     );
            };

    const std::vector<OperatorDesc>& opers = pos->second;
    auto  veclim = opers.end();
    auto  vecpos = std::find_if(pos->second.begin(), veclim, sameSignature);

    if (vecpos == veclim) return nullptr;

    return vecpos->function();
    return nullptr;
  }

  void sortByArgumentName(OperatorCallSupplement::ArgDescList& lst)
  {
    // assumes arguments are named Left and Right
    // i.e., sorts Left before Right
    // \todo what if the call is written as
    //       "+"(Left => 2, 3) ?

    if (lst.size() < 2) return;
    //ADA_ASSERT(lst.size() == 2);

    if (lst.at(1).name() < lst.at(0).name())
      std::swap(lst.at(0), lst.at(1));
  }

  SgTypePtrList extractTypes(const OperatorCallSupplement::ArgDescList& lst)
  {
    SgTypePtrList res;
    auto          typeExtractor = [](const ArgDesc& desc) { return desc.type(); };

    std::transform(lst.begin(), lst.end(), std::back_inserter(res), typeExtractor);
    return res;
  }

  // if \ref id is valid, lookup scope from translation context
  //   otherwise use si::Ada::operatorScope to find an appropriate scope
  si::Ada::OperatorScopeInfo
  operatorScope(const AdaIdentifier& name, SgTypePtrList argTypes, int id, AstContext ctx)
  {
    /*if (id < 1)*/ return si::Ada::operatorScope(name, std::move(argTypes));

    /*SgScopeStatement&        scope = queryScopeOf(id, ctx);
    si::Ada::DominantArgInfo dom = si::Ada::operatorArgumentWithNamedRootIfAvail(argTypes);

    return { &scope, dom.pos() };*/ //TODO How does id work????
  }

  bool pragmaProcessing(const AstContext& ctx)
  {
    return isSgPragmaDeclaration(ctx.pragmaAspectAnchor());
  }

  SgExpression*
  generateOperator(AdaIdentifier name, OperatorCallSupplement suppl, AstContext ctx)
  {
    if (!suppl.args_valid())
    {
      if (!pragmaProcessing(ctx))
        logWarn() << "suppl.args() is null" << std::endl;

      return nullptr;
    }

    sortByArgumentName(suppl.args());

    si::Ada::OperatorScopeInfo scopeinfo = operatorScope(name, extractTypes(suppl.args()), suppl.scopeId(), ctx);
    SgScopeStatement&          scope     = SG_DEREF(scopeinfo.scope());

    suppl = operatorSignature(name, std::move(suppl), scopeinfo.argpos(), ctx);

    // \todo add support for other operators
    if (!suppl.valid())
    {
      logWarn() << "oper " << name << " not generatable" << std::endl;
      return nullptr;
    }

    if (SgFunctionDeclaration* fndcl = findExistingOperator(name, scope, suppl))
      return &mkFunctionRefExp(*fndcl);

    std::string            opname   = si::Ada::roseOperatorPrefix + name;

    auto                   complete =
       [&suppl](SgFunctionParameterList& fnParmList, SgScopeStatement& scope)->void
       {
         SgTypeModifier defaultInMode;

         defaultInMode.setDefault();

         for (const ArgDesc& parmDesc : suppl.args())
         {
           // \todo use parmDesc.name() instead of parmNames
           SgType&                  parmType = SG_DEREF(parmDesc.type());
           const std::string&       parmName = parmDesc.name();
           SgInitializedName&       parmDecl = mkInitializedName(parmName, parmType, nullptr);
           SgInitializedNamePtrList parmList = {&parmDecl};
           /* SgVariableDeclaration&   pvDecl   =*/ mkParameter(parmList, defaultInMode, scope);

           parmDecl.set_parent(&fnParmList);
           fnParmList.get_args().push_back(&parmDecl);
         }
       };

    SgFunctionDeclaration& opdcl = mkProcedureDecl_nondef(opname, scope, *suppl.result(), complete);

    operatorSupport()[{&scope, name}].emplace_back(&opdcl, OperatorDesc::COMPILER_GENERATED);
    return &mkFunctionRefExp(opdcl);
  }

  SgExpression&
  getOperator(ada_base_entity* lal_expr, OperatorCallSupplement suppl, bool unary, AstContext ctx)
  {
    // FYI https://en.wikibooks.org/wiki/Ada_Programming/All_Operators
    //ADA_ASSERT(expr.Expression_Kind == An_Operator_Symbol);

    ada_node_kind_enum kind = ada_node_kind(lal_expr);

    std::string expr_name;
    if(kind != ada_string_literal){ //Strings won't work for this section, need to use a different method
      //Get the hash of the first corresponding declaration
      ada_base_entity corresponding_decl; 
      ada_expr_p_first_corresponding_decl(lal_expr, &corresponding_decl);
      bool decl_exists = true;
      int decl_hash = 0;
      if(ada_node_is_null(&corresponding_decl)){
        //If we don't have a p_first_corresponding_decl, set a flag
        decl_exists = false;
      } else {
        decl_hash = hash_node(&corresponding_decl);
      }

      //Get the name of this expr
      expr_name = canonical_text_as_string(lal_expr);
      logInfo() << "In getOperator, expr_name is " << expr_name << std::endl;

      // PP 11/18/22
      // UNCLEAR_LINK_1
      // under some unclear circumstances a provided = operator and a generated /= may have the
      //   same Corresponding_Name_Declaration, but different Corresponding_Name_Definition.
      //   => just use the Corresponding_Name_Definition
      // ROSE regression tests: dbase.ads, dbase.adb, dbase_test.adb
      // was: if (SgDeclarationStatement* dcl = findFirst(asisDecls(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration))
      /*if (SgDeclarationStatement* dcl = findFirst(libadalangDecls(), expr.Corresponding_Name_Definition))
      {
        SgExpression* res = sg::dispatch(ExprRefMaker{ctx}, dcl);

        return SG_DEREF(res);
      }*/ //TODO Does Libadlang do def pointers?

      // PP 08/03/23
      // UNCLEAR_LINK_2
      // under some unclear circumstances ASIS does not link a callee (i.e., A_PLUS_OPERATOR representing a unary call)
      // to its available definition, but only to its declaration (A_UNARY_PLUS_OPERATOR).
      // ACATS test: c87b04b
      // => to resolve the issue, look up the declaration by expr.Corresponding_Name_Declaration;
      //    to avoid the case described by UNCLEAR_LINK_1, test if the operator declaration has
      //    the same name as used for the call.
      if(decl_exists){
        if (SgDeclarationStatement* dcl = findFirst(libadalangDecls(), decl_hash))
        {
          const std::string dclname = si::Ada::convertRoseOperatorNameToAdaName(si::get_name(dcl));
          const bool        sameOperatorName = boost::iequals(dclname, expr_name);
          logInfo() << "dclname is " << dclname <<", lal_expr name is " << expr_name << std::endl;

          if (sameOperatorName)
          {
            SgExpression* res = sg::dispatch(ExprRefMaker{ctx}, dcl);

            return SG_DEREF(res);
          }
        }
      }
    } else {
      ada_text_type denoted_value;
      ada_string_literal_p_denoted_value(lal_expr, &denoted_value);
      expr_name = "\"" + dot_ada_text_type_to_string(denoted_value) + "\"";
    }

    const char*                         expr_name_c = expr_name.c_str();
    const std::size_t                   len = strlen(expr_name_c);
    if((len > 2) && (expr_name_c[0] == '"') && (expr_name_c[len-1] == '"')){
      // do not use leading and trailing '"'
      AdaIdentifier                       fnname{expr_name_c+1, int(len)-2};

      // try to generate the operator
      if (SgExpression* res = generateOperator(fnname, std::move(suppl), ctx))
        return *res;
    }

    if (!pragmaProcessing(ctx))
      logWarn() << "Using first version generator as fallback to model operator " << expr_name
                << std::endl;

    /* unused fields:
       Defining_Name_List    Corresponding_Name_Definition_List;
       Defining_Name_ID      Corresponding_Generic_Element;
    */
    return getOperator_fallback(lal_expr, unary, ctx);
  }

  SgExpression&
  getBooleanEnumItem(AdaIdentifier boolid)
  {
    SgEnumType&               enty  = SG_DEREF( isSgEnumType(adaTypesByName().at(AdaIdentifier{"BOOLEAN"})) );
    SgEnumDeclaration&        endcl = SG_DEREF( isSgEnumDeclaration(enty.get_declaration()) );
    SgInitializedNamePtrList& enlst = endcl.get_enumerators();

    auto lim = enlst.end();
    auto pos = std::find_if( enlst.begin(), lim,
                             [id = std::move(boolid)](const SgInitializedName* itm)->bool
                             {
                               return id == AdaIdentifier{itm->get_name().getString()};
                             }
                           );

    return mkEnumeratorRef(endcl, **pos);
  }

  auto
  refFromWithinFunction(const SgFunctionDeclaration& ref, const SgScopeStatement& refedFrom) -> bool
  {
    const SgFunctionDeclaration* fun = sg::ancestor<const SgFunctionDeclaration>(&refedFrom);

    while (  (fun != nullptr)
          && (fun->get_firstNondefiningDeclaration() != ref.get_firstNondefiningDeclaration())
          )
    {
      fun = sg::ancestor<const SgFunctionDeclaration>(fun);
    }

    return fun != nullptr;
  }

  /// defines ROSE AST types for which we do not generate scope qualification
  struct RoseRequiresScopeQual : sg::DispatchHandler<bool>
  {
      using base = sg::DispatchHandler<bool>;

      RoseRequiresScopeQual(bool prfx, AstContext astctx)
      : base(), fromPrefix(prfx), ctx(astctx)
      {}

      void handle(const SgNode& n)                 { SG_UNEXPECTED_NODE(n); }

      // scope qual requried for
      void handle(const SgDeclarationStatement&)   { res = true; } // default for declarations
      void handle(const SgInitializedName&)        { res = true; }
      void handle(const SgAdaEntryDecl&)           { res = true; } // overrides SgFunctionDeclaration

      // no scope qual needed for
      void handle(const SgAdaTaskSpecDecl&)        { res = false; }
      void handle(const SgAdaProtectedSpecDecl&)   { res = false; }
      void handle(const SgAdaPackageSpecDecl&)     { res = false; }
      void handle(const SgImportStatement&)        { res = false; }
      void handle(const SgBasicBlock&)             { res = false; }
      void handle(const SgAdaGenericInstanceDecl&) { res = false; }

      void handle(const SgFunctionDeclaration& n)
      {
        // ASIS_FUNCTION_REF_ISSUE_1
        // acats test: c41306c.adb
        // this is to work around a representation issue in Asis
        //   where the reference to a returned component
        //   is just represented as identifier.
        //   e.g., F.x       -- F is expected to be a function call as in
        //         F(true).x --   but in Asis it is just an identifier reference.
        //         to distinguish between scoperefs for overload distinction
        //         which we do not want in ROSE and true calls,
        //         we check if the reference is from within the same function.
        //         This method is not perfect. To resolve this issue completely,
        //         we would need more context...
        // The AST fix is added by mkDotExp.
        res = fromPrefix && !refFromWithinFunction(n, ctx.scope());
      }

      // dependent on underlying data
      void handle(const SgAdaRenamingDecl& n)
      {
        res = si::Ada::isObjectRenaming(n);
      }

    private:
      bool       fromPrefix;
      AstContext ctx;
  };


  /// tests whether ROSE represents the prefix expression
  ///   (e.g., true for objects, false for scope-qualification)
  bool roseRequiresPrefix(ada_base_entity* lal_element, bool fromPrefix, AstContext ctx)
  {
    //Get the kind
    ada_node_kind_enum kind = ada_node_kind(lal_element);

    if(kind == ada_identifier)
    {
      const SgNode* astnode = queryCorrespondingAstNode(lal_element, ctx);

      logTrace() << "ada_identifier?" << std::endl;

      std::string ident = canonical_text_as_string(lal_element);

      if(!astnode)
        logTrace() << "Identifier '" << ident << "' has no corresponding node in ROSE."
                   << std::endl;

      return astnode == nullptr || sg::dispatch(RoseRequiresScopeQual{fromPrefix, ctx}, astnode);
    }

    if(kind == ada_dotted_name)
    {
      logTrace() << "ada_dotted_name?" << std::endl;

      ada_base_entity lal_prefix, lal_suffix;
      ada_dotted_name_f_prefix(lal_element, &lal_prefix);
      ada_dotted_name_f_suffix(lal_element, &lal_suffix);
      return    roseRequiresPrefix(&lal_prefix, true, ctx)
             || roseRequiresPrefix(&lal_suffix, fromPrefix, ctx);
    }

    /*if (expr.Expression_Kind == An_Indexed_Component)
    {
      logTrace() << "An_Indexed_Component?" << std::endl;
      // \todo should this always return true (like the cases below)?
      return roseRequiresPrefix(expr.Prefix, fromPrefix, ctx);
    }*/ //TODO What is this node kind in lal?

    if(kind == ada_explicit_deref)
    {

      logTrace() << "ada_explicit_deref?" << std::endl;
      return true;
    }

    if(kind == ada_call_expr)
    {
      logTrace() << "ada_call_expr?" << std::endl;
      return true;
    }

    logFlaw() << "roseRequiresPrefix: untested expression-kind: "
              << kind
              << std::endl;
    return true;
  }

  SgExprListExp&
  getRecordAggregate(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_base_entity lal_assocs;
    ada_base_aggregate_f_assocs(lal_element, &lal_assocs);
    int count = ada_node_children_count(&lal_assocs);
    std::vector<SgExpression*> components;

    for(int i = 0; i < count; i++){
      ada_base_entity lal_aggregate_assoc;
      if(ada_node_child(&lal_assocs, i, &lal_aggregate_assoc) != 0){
        //Process this aggregate assoc node and add it to components
        ada_base_entity lal_r_expr;
        ada_aggregate_assoc_f_r_expr(&lal_aggregate_assoc, &lal_r_expr);

        ada_base_entity lal_designators;
        ada_aggregate_assoc_f_designators(&lal_aggregate_assoc, &lal_designators);

        SgExpression&              init = getExpr(&lal_r_expr, ctx);
        SgExpression*              sgnode = &init;
        int                        num_designators = ada_node_children_count(&lal_designators);

        if(!ada_node_is_null(&lal_designators) && num_designators > 0)
        {
          std::vector<SgExpression*> exprs;

          for(int j = 0; j < num_designators; j++){
            ada_base_entity lal_designator;
            if(ada_node_child(&lal_designators, j, &lal_designator) != 0){
              SgExpression* res = &getExpr(&lal_designator, ctx); //TODO What if this is a definition?
              exprs.push_back(res);
            }
          }
          SgExprListExp& choicelst = mkExprListExp(exprs);

          sgnode = &mkAdaNamedInitializer(choicelst, init);
        }
        attachSourceLocation(SG_DEREF(sgnode), &lal_aggregate_assoc, ctx);
        components.push_back(sgnode);
      }
    }

    SgExprListExp& sgnode = mkExprListExp(components);

    attachSourceLocation(sgnode, lal_element, ctx);
    return sgnode;
  }

} //end unnamed namespace

namespace{
  SgExprListExp& createExprListExpIfNeeded(SgExpression& exp)
  {
    SgExprListExp* res = isSgExprListExp(&exp);

    return (res == nullptr) ? mkExprListExp({&exp}) : *res;
  }

  SgScopeStatement& scopeForUnresolvedNames(AstContext ctx)
  {
    SgDeclarationStatement* dcl = ctx.pragmaAspectAnchor();

    // if there is no anchor, then just return the scope
    if (dcl == nullptr) return ctx.scope();

    // create a SgDeclarationScope for the anchor node if needed
    if (dcl->get_declarationScope() == nullptr)
    {
      SgDeclarationScope& dclscope = mkDeclarationScope(ctx.scope());

      sg::linkParentChild(*dcl, dclscope, &SgDeclarationStatement::set_declarationScope);
    }

    // return the declaration scope of the anchor
    return SG_DEREF(dcl->get_declarationScope());
  }

  /// creates expressions from elements, but does not decorate
  ///   aggregates with SgAggregateInitializers
  SgExpression&
  getExpr_undecorated(ada_base_entity* lal_element, AstContext ctx, OperatorCallSupplement suppl = {}, bool unary = false)
  {
    //Get the kind
    ada_node_kind_enum kind = ada_node_kind(lal_element);

    ada_text kind_name;
    ada_kind_name(kind, &kind_name);
    std::string kind_name_string = ada_text_to_locale_string(&kind_name);
    ada_destroy_text(&kind_name);

    SgExpression*      res       = NULL;

    switch (kind)
    {
      case ada_int_literal:                        // 2.4
        {
          logKind("ada_int_literal", kind);

          /*ada_big_integer denoted_value; //TODO This way strips the formatting, but will work after lal_2021
          ada_text value_text;

          //Get the value of this node
          ada_int_literal_p_denoted_value(lal_element, &denoted_value);
          ada_big_integer_text(denoted_value, &value_text);
          std::string denoted_text = ada_text_to_locale_string(&value_text);
          ada_destroy_text(&value_text);*/

          //Get the value of this node
          ada_symbol_type canonical_text_symbol;
          ada_text canonical_text;
          ada_single_tok_node_p_canonical_text(lal_element, &canonical_text_symbol);
          ada_symbol_text(&canonical_text_symbol, &canonical_text);
          std::string denoted_text = ada_text_to_locale_string(&canonical_text);
          ada_destroy_text(&canonical_text);

          res = &mkAdaIntegerLiteral(denoted_text.c_str());
          break;
        }

      case ada_char_literal:                       // 4.1
        {
          logKind("ada_char_literal", kind);

          //Get the value of this char
          uint32_t lal_denoted_value;
          ada_char_literal_p_denoted_value(lal_element, &lal_denoted_value);
          std::string denoted_char = "\'";
          denoted_char += char(lal_denoted_value);
          denoted_char += '\'';

          res = &mkValue<SgCharVal>(denoted_char.c_str());
          break;
        }

      case ada_string_literal:                          // 2.6
        {
          logKind("ada_string_literal", kind);
          //Get the value of this string
          ada_text_type denoted_value;
          ada_string_literal_p_denoted_value(lal_element, &denoted_value);
          std::string denoted_text = dot_ada_text_type_to_string(denoted_value);
          if(denoted_text == "+" || denoted_text == "-" || denoted_text == "*"){ //If the string is for an op, call getOperator
            res = &getOperator(lal_element, suppl, unary, ctx);
            break;
          }
          SgStringVal& sgnode = mkLocatedNode<SgStringVal>(denoted_text);
          sgnode.set_stringDelimiter('"');
          res = &sgnode;
          break;
        }

      case ada_real_literal:                            // 2.4.1
        {
          logKind("ada_real_literal", kind);
          //Get the value
          std::string value_image = canonical_text_as_string(lal_element); //TODO this won't work after lal 2021
          const char* c_value_image = value_image.c_str();

          res = &mkValue<SgLongDoubleVal>(c_value_image);

          break;
        }

      case ada_identifier:                             // 4.1
        {
          // \todo use the queryCorrespondingAstNode function and the
          //       generate the expression based on that result.
          logKind("ada_identifier", kind);

          //Get the text of this identifier
          const std::string name = canonical_text_as_string(lal_element);

          //Check if this is an enum value instead of a variable
          //Get the expression type & check for ada_enum_type_def
          ada_base_entity lal_expr_type;
          ada_node_kind_enum lal_expr_type_kind;
          ada_expr_p_expression_type(lal_element, &lal_expr_type);
          if(!ada_node_is_null(&lal_expr_type)){
            ada_type_decl_f_type_def(&lal_expr_type, &lal_expr_type);
            lal_expr_type_kind = ada_node_kind(&lal_expr_type);
            while(lal_expr_type_kind == ada_derived_type_def){
              ada_derived_type_def_f_subtype_indication(&lal_expr_type, &lal_expr_type);
              ada_type_expr_p_designated_type_decl(&lal_expr_type, &lal_expr_type);
              ada_type_decl_f_type_def(&lal_expr_type, &lal_expr_type);
              lal_expr_type_kind = ada_node_kind(&lal_expr_type);
            }
          }

          if(!ada_node_is_null(&lal_expr_type) && lal_expr_type_kind == ada_enum_type_def){
            logInfo() << "identifier " << name << " is being treated as an enum value (p_expr_type_def = ada_enum_type_def).\n";

            //Get the hash for the decl
            ada_base_entity lal_first_corresponding_decl;
            ada_expr_p_first_corresponding_decl(lal_element, &lal_first_corresponding_decl);
            int decl_hash = hash_node(&lal_first_corresponding_decl);

            if (SgInitializedName* enumitem = findFirst(libadalangVars(), decl_hash))
            {
              SgEnumType&        enumtype = SG_DEREF( isSgEnumType(enumitem->get_type()) );
              SgEnumDeclaration& enumdecl = SG_DEREF( isSgEnumDeclaration(enumtype.get_declaration()) );

              res = &mkEnumeratorRef(enumdecl, *enumitem);
            }
            else
            {
              logFlaw() << "unable to find definition for enum val " << name
                        << std::endl;

              SgStringVal& strval = SG_DEREF(sb::buildStringVal(name));

              strval.set_stringDelimiter(' ');
              res = &strval;
            }
            break;
          }

          //Find the definition of this identifier and get its hash
          int hash;

          //Bool for if we found the corresponding decl
          bool found_decl = true;

          ada_base_entity corresponding_decl; //lal doesn't give definition directly, so go from the decl
          ada_expr_p_first_corresponding_decl(lal_element, &corresponding_decl);

          if(ada_node_is_null(&corresponding_decl)){
            logInfo() << "Identifier has no first_corresponding_decl, trying referenced_decl instead.\n";
            //If first_corresponding_decl isn't there, try referenced_decl
            ada_name_p_referenced_decl(lal_element, 1, &corresponding_decl);
          }

          //Get the kind of the decl
          ada_node_kind_enum decl_kind = ada_node_kind(&corresponding_decl);
          if(decl_kind >= ada_abstract_state_decl && decl_kind <= ada_single_task_decl){ //62 - 121
            ada_ada_node_array defining_name_list;
            ada_basic_decl_p_defining_names(&corresponding_decl, &defining_name_list);
          
            //Find the correct decl in the defining name list
            for(int i = 0; i < defining_name_list->n; i++){
              ada_base_entity defining_name = defining_name_list->items[i];
              ada_base_entity name_identifier;
              ada_defining_name_f_name(&defining_name, &name_identifier);
              const std::string test_name = canonical_text_as_string(&name_identifier);
              if(name == test_name){
                hash = hash_node(&defining_name);
                ada_node_kind_enum def_kind = ada_node_kind(&defining_name);
                logInfo() << "Found definition for ada_identifier " << name << ", kind = " << def_kind << std::endl;
                break;
              }
            }
          } else if(decl_kind == ada_entry_decl){
            hash = hash_node(&corresponding_decl);
          } else {
            logError() << "Could not get corresponding decl for identifier! decl_kind = " << decl_kind << "\n";
            found_decl = false;
          }

          if(!found_decl){
            //If we haven't found a decl, don't check the maps
            SgScopeStatement& scope = scopeForUnresolvedNames(ctx);
            if (&scope == &ctx.scope())
            {
              // issue warning for unresolved names outside pragmas and aspects
              logWarn() << "ADDING unresolved name: " << name
                        << " (no decl found)" << std::endl;
            }
            res = &mkUnresolvedName(name, scope);
            break;
          }

          if(SgInitializedName* var = findFirst(libadalangVars(), hash))
          {
            res = sb::buildVarRefExp(var, &ctx.scope());
          }
          else if(SgDeclarationStatement* dcl = queryDecl(lal_element, ctx))
          {
            res = sg::dispatch(ExprRefMaker{ctx}, dcl);
          }
          else if(SgInitializedName* exc = findFirst(libadalangExcps(), hash))
          {
            res = &mkExceptionRef(*exc, ctx.scope());
          }
          else if(SgDeclarationStatement* tydcl = findFirst(libadalangTypes(), hash))
          {
            res = sg::dispatch(TypeRefMaker{ctx}, tydcl);
          }
          else if (SgType* ty = findFirst(adaTypes(), hash))
          {
            res = &mkTypeExpression(*ty);
          }
          /*else if (SgInitializedName* fld = queryByNameInDeclarationID(adaIdent, expr.Corresponding_Name_Declaration, ctx))
          {
            res = sb::buildVarRefExp(fld, &ctx.scope());
          }*/
          else if (SgInitializedName* var = findFirst(adaVars(), hash))
          {
            res = sb::buildVarRefExp(var, &ctx.scope());
          }
          else if (SgInitializedName* exc = findFirst(adaExcps(), hash))
          {
            res = &mkExceptionRef(*exc, ctx.scope());
          }
/*
          else if (SgInitializedName* dsc = getRefFromDeclarationContext(expr, adaIdent, ctx))
          {
            res = sb::buildVarRefExp(dsc, &ctx.scope());
          }
*/
          else
          {
            SgScopeStatement& scope = scopeForUnresolvedNames(ctx);

            if (&scope == &ctx.scope())
            {
              // issue warning for unresolved names outside pragmas and aspects
              logWarn() << "ADDING unresolved name: " << name
                        << std::endl;
            }

            res = &mkUnresolvedName(name, scope);
          }
          break;
        }

      case ada_explicit_deref:                   // 4.1
        {
          logKind("ada_explicit_deref", kind);

          //Get the prefix
          ada_base_entity lal_prefix;
          ada_explicit_deref_f_prefix(lal_element, &lal_prefix);

          SgExpression& exp = getExpr(&lal_prefix, ctx);

          res = &mkPointerDerefExp(exp);
          break;
        }

      case ada_bin_op:
      case ada_un_op:
      case ada_relation_op:
      case ada_call_expr:                           // 4.1
        {
          logKind("A_Function_Call", kind);

          //If this is an ada_call_expr, we might want to treat it like a_type_conversion instead
          if(kind == ada_call_expr){
            //Get the f_name, and check if it is a type
            ada_base_entity lal_name;
            ada_call_expr_f_name(lal_element, &lal_name);
            ada_node_kind_enum name_kind = ada_node_kind(&lal_name);
            if(name_kind == ada_identifier){
              ada_base_entity lal_decl;
              ada_expr_p_first_corresponding_decl(&lal_name, &lal_decl);
              ada_node_kind_enum decl_kind = ada_node_kind(&lal_decl);
              if(decl_kind == ada_type_decl){
                logInfo() << "This ada_call_expr corresponds to a_type_conversion.\n";
                const bool isConv = true; //TODO A_Qualified_Expression?

                //Get the expr
                ada_base_entity lal_expr;
                ada_call_expr_f_suffix(lal_element, &lal_expr);
                ada_node_child(&lal_expr, 0, &lal_expr);
                ada_param_assoc_f_r_expr(&lal_expr, &lal_expr);

                SgExpression& exp = getExpr(&lal_expr, ctx);
                SgType&       ty  = getDeclType(&lal_name, ctx);

                res = isConv ? &mkCastExp(exp, ty)
                             : &mkQualifiedExp(exp, ty);
                // fix-up aggregate type
                if (SgAggregateInitializer* aggrexp = isSgAggregateInitializer(&exp))
                {
                  //ADA_ASSERT(isSgTypeUnknown(aggrexp->get_type()));
                  aggrexp->set_expression_type(&ty);
                }
                break;
              }
            }
          }
          logInfo() << "Not a_type_conversion.\n";

          std::vector<ada_base_entity*>  params;
          ada_base_entity                prefix;
          std::vector<ada_base_entity>   param_backend;
          bool                           operatorCallSyntax;
          bool                           objectCallSyntax;

          //Based on the kind, fill in the prefix & params
          if(kind == ada_bin_op || kind == ada_relation_op){
            param_backend.resize(2);
            ada_bin_op_f_op(lal_element, &prefix);
            ada_bin_op_f_left(lal_element, &param_backend.at(0));
            params.push_back(&param_backend.at(0));
            ada_bin_op_f_right(lal_element, &param_backend.at(1));
            params.push_back(&param_backend.at(1));
            operatorCallSyntax = false;
            objectCallSyntax = false;
          } else if(kind == ada_un_op){
            param_backend.resize(1);
            ada_un_op_f_op(lal_element, &prefix);
            ada_un_op_f_expr(lal_element, &param_backend.at(0));
            params.push_back(&param_backend.at(0));
            operatorCallSyntax = false;
            objectCallSyntax = false;
          } else if(kind == ada_call_expr){
            ada_call_expr_f_name(lal_element, &prefix);
            ada_base_entity param_list;
            ada_call_expr_f_suffix(lal_element, &param_list);
            int count = ada_node_children_count(&param_list);
            params.resize(count);
            param_backend.resize(count);
            for (int i = 0; i < count; ++i)
            {
              if(ada_node_child(&param_list, i, &param_backend.at(i)) == 0){
                logError() << "Error while getting a child in getExpr_undecorated.\n";
              }

              if(!ada_node_is_null(&param_backend.at(i))){
                int temp_hash = hash_node(&param_backend.at(i));
                ada_node_kind_enum temp_kind = ada_node_kind(&param_backend.at(i));
                logInfo() << "Adding " << &param_backend.at(i) << "(hash = " << temp_hash << "), (kind = " << temp_kind << ") to params.\n";
                params.at(i) = &param_backend.at(i);
              }
            }
            operatorCallSyntax = true;
            objectCallSyntax = false;
          }

          // PP (04/22/22) if the callee is an Ada Attribute then integrate
          //               the arguments into the Ada attribute expression directly.
          //               Note sure if it is good to deviate from the Asis representation
          //               but some arguments have no underlying functiom declaration.
          // \todo Consider adding an optional function reference to the SgAdaAttribute rep.
          if(false /*queryExprKindID(expr.Prefix) == An_Attribute_Reference TODO How to convert this?*/){
            //res = &getAttributeExprID(expr.Prefix, ctx, range);
          } else {
            res = &createCall(&prefix, params, operatorCallSyntax, objectCallSyntax, ctx);
          }
          break;
        }

      case ada_op_concat:
      case ada_op_plus:                        // 4.1 TODO Add more ops
      case ada_op_minus:
      case ada_op_eq:
      case ada_op_gt:
      case ada_op_gte:
      case ada_op_in:
      case ada_op_lt:
      case ada_op_lte:
      case ada_op_neq:
      case ada_op_mult:
      case ada_op_div:
      case ada_op_pow:
        {
          logKind("An_Operator_Symbol", kind);

          res = &getOperator(lal_element, suppl, unary, ctx);
          break;
        }

      case ada_dotted_name:                      // 4.1.3
        {
          logKind("ada_dotted_name", kind);

          ada_base_entity lal_prefix, lal_suffix;
          ada_dotted_name_f_prefix(lal_element, &lal_prefix);
          ada_dotted_name_f_suffix(lal_element, &lal_suffix);
          int prefix_hash = hash_node(&lal_prefix);

          suppl.scopeId(prefix_hash);

          SgExpression& selector = getExpr(&lal_suffix, ctx, std::move(suppl));
          const bool    enumval = isSgEnumVal(&selector) != nullptr;

          // Check if the kind requires a prefix in ROSE,
          //   or if the prefix (scope qualification) is implied and
          //   generated by the backend.
          if(!enumval && roseRequiresPrefix(&lal_prefix, true, ctx))
          {
            SgExpression& prefix = getExpr(&lal_prefix, ctx);

            res = &mkDotExp(prefix, selector);
          }
          else
          {
            res = &selector;
          }
          break;
        }

      case ada_attribute_ref:
        {
          logKind("ada_attribute_ref", kind);

          res = &getAttributeExpr(lal_element, ctx);
          break;
        }

      case ada_aggregate:                        // 4.3
        {
          logKind("ada_aggregate", kind);

          res = &getRecordAggregate(lal_element, ctx);
          break;
        }

      case ada_paren_expr:                // 4.4
        {
          logKind("ada_paren_expr", kind);

          ada_base_entity lal_expr;
          ada_paren_expr_f_expr(lal_element, &lal_expr);

          res = &getExpr(&lal_expr, ctx);
          res->set_need_paren(true);

          break;
        }

      case ada_qual_expr:                    // 4.7
//      case A_Type_Conversion:                         // 4.6
        {
          const bool isConv = (kind != ada_qual_expr);

          logKind(isConv ? "A_Type_Conversion" : "ada_qual_expr", kind);

          //Get the id for the expression
          ada_base_entity lal_prefix;
          ada_qual_expr_f_prefix(lal_element, &lal_prefix);

          //Get the suffix for the expression
          ada_base_entity lal_suffix;
          ada_qual_expr_f_suffix(lal_element, &lal_suffix);
          ada_node_kind_enum lal_suffix_kind = ada_node_kind(&lal_suffix);
          if(lal_suffix_kind == ada_paren_expr){
            //lal sometimes (always?) has a paren expr here, but ASIS doesn't. Skip the parens if it exists.
            ada_paren_expr_f_expr(&lal_suffix, &lal_suffix);
          }

          SgExpression& exp = getExpr(&lal_suffix, ctx);
          SgType&       ty  = getDeclType(&lal_prefix, ctx);

          res = isConv ? &mkCastExp(exp, ty)
                       : &mkQualifiedExp(exp, ty);

          // fix-up aggregate type
          if (SgAggregateInitializer* aggrexp = isSgAggregateInitializer(&exp))
          {
            aggrexp->set_expression_type(&ty);
          }

          break;
        }

      case ada_allocator:   // 4.8
        {
          logKind("ada_allocator", kind);

          //Get the expression
          ada_base_entity lal_expr;
          ada_allocator_f_type_or_expr(lal_element, &lal_expr);
          ada_node_kind_enum lal_expr_kind = ada_node_kind(&lal_expr);

          //lal_expr can either be a qual_expr or a subtype_indication
          if(lal_expr_kind == ada_qual_expr){
            logKind("An_Allocation_From_Qualified_Expression", lal_expr_kind);

            //Get the id for the expression
            ada_base_entity lal_prefix;
            ada_qual_expr_f_prefix(&lal_expr, &lal_prefix);

            //Get the suffix for the expression
            ada_base_entity lal_suffix;
            ada_qual_expr_f_suffix(&lal_expr, &lal_suffix);
            ada_node_kind_enum lal_suffix_kind = ada_node_kind(&lal_suffix);
            if(lal_suffix_kind == ada_paren_expr){
              //lal sometimes (always?) has a paren expr here, but ASIS doesn't. Skip the parens if it exists.
              ada_paren_expr_f_expr(&lal_suffix, &lal_suffix);
            }

            SgType&            ty  = getDeclType(&lal_prefix, ctx);
            SgExpression&      arg = getExpr_undecorated(&lal_suffix, ctx);
            SgExprListExp&     inilst = createExprListExpIfNeeded(arg);

            res = &mkNewExp(ty, &inilst);
          } else {
            logKind("An_Allocation_From_Subtype", lal_expr_kind);

            SgType& ty = getDefinitionType(&lal_expr, ctx);

            res = &mkNewExp(ty);
          }

          break;
        }

      case ada_others_designator:
        {
          logKind("ada_others_designator", kind);
          res = &mkAdaOthersExp();
          break;
        }

      case ada_box_expr:                          // Ada 2005 4.3.1(4): 4.3.3(3:6)
        {
          logKind("ada_box_expr", kind);

          res = &mkAdaBoxExp();
          break;
        }

      default:
        logFlaw() << "Unhandled expression: " << kind_name_string << std::endl;
        res = sb::buildIntVal();
        //ADA_ASSERT(!FAIL_ON_ERROR(ctx));
    }

    attachSourceLocation(SG_DEREF(res), lal_element, ctx);
    return *res;
  }
} //End unnamed namespace

SgExpression&
getExpr(ada_base_entity* lal_element, AstContext ctx, OperatorCallSupplement suppl, bool unary)
{
  //Check the kind
  ada_node_kind_enum kind = ada_node_kind(lal_element);
  
  ada_text kind_name;
  ada_kind_name(kind, &kind_name);
  std::string kind_name_string = ada_text_to_locale_string(&kind_name);
  logTrace() << "getExpr called on a " << kind_name_string << std::endl;
  ada_destroy_text(&kind_name);

  SgExpression*      res   = &getExpr_undecorated(lal_element, ctx, std::move(suppl), unary);

  switch(kind)
  {
    case ada_aggregate:              // 4.3
    /*case A_Named_Array_Aggregate:                   // 4.3 TODO: Are there more aggregate nodes to worry about?
    case A_Record_Aggregate:                        // 4.3
    case An_Extension_Aggregate:                    // 4.3*/
      {
        SgExprListExp* explst = isSgExprListExp(res);

        res = &mkAggregateInitializer(SG_DEREF(explst));
        attachSourceLocation(SG_DEREF(res), lal_element, ctx);
        break;
      }

    default:;
  }

  return SG_DEREF(res);
}

SgExpression&
getExpr_opt(ada_base_entity* lal_expr, AstContext ctx, OperatorCallSupplement suppl)
{
  if(lal_expr == nullptr)
  {
    logFlaw() << "uninitialized expression id " << lal_expr << std::endl;
    return mkNullExpression();
  }

  return ada_node_is_null(lal_expr) ? mkNullExpression()
                                    : getExpr(lal_expr, ctx, std::move(suppl))
                                    ;
}

  SgExpression&
  getDiscreteRangeGeneric(ada_base_entity* lal_element, AstContext ctx)
  {
    ada_node_kind_enum kind = ada_node_kind(lal_element);
    SgExpression* res = nullptr;

    switch(kind)
    {
      /*case A_Discrete_Subtype_Indication:         // 3.6.1(6), 3.2.2
        {
          logKind("A_Discrete_Subtype_Indication", el.ID);

          SgType& ty = getDiscreteSubtypeID(range.Subtype_Mark, range.Subtype_Constraint, ctx);

          res = &mkTypeExpression(ty);
          break;
        }*/

      case ada_bin_op:    // 3.6.1, 3.5
        {
          logKind("A_Discrete_Simple_Expression_Range", kind);

          //Get the bounds
          ada_base_entity lal_lower_bound, lal_upper_bound;
          ada_bin_op_f_left(lal_element, &lal_lower_bound);
          ada_bin_op_f_right(lal_element, &lal_upper_bound);

          SgExpression& lb = getExpr(&lal_lower_bound, ctx);
          SgExpression& ub = getExpr(&lal_upper_bound, ctx);

          res = &mkRangeExp(lb, ub);
          break;
        }

      /*case A_Discrete_Range_Attribute_Reference:  // 3.6.1, 3.5
        {
          logKind("A_Discrete_Range_Attribute_Reference", el.ID);

          res = &getExprID(range.Range_Attribute, ctx);
          break;
        }*/

      default:
        logFlaw() << "Unhandled range: " << kind << " in getDiscreteRangeGeneric" << std::endl;
        res = &mkRangeExp();
    }

    attachSourceLocation(SG_DEREF(res), lal_element, ctx);
    return *res;
  }

  /*/// \private
  /// returns a range expression from the Asis definition \ref def
  SgExpression&
  getDiscreteRange(Element_Struct& el, Definition_Struct& def, AstContext ctx)
  {
    ADA_ASSERT(def.Definition_Kind == A_Discrete_Range);

    return getDiscreteRangeGeneric(el, def.The_Union.The_Discrete_Range, ctx);
  }*/

  SgExpression&
  getDiscreteRange(ada_base_entity* lal_element, AstContext ctx)
  {
    //Run a bunch of checks to see if this is a node that should go to getDiscreteRangeGeneric
    bool needs_generic = false;

    ada_node_kind_enum kind = ada_node_kind(lal_element);

    if(kind == ada_bin_op){
      ada_base_entity lal_op;
      ada_bin_op_f_op(lal_element, &lal_op);
      ada_node_kind_enum lal_op_kind = ada_node_kind(&lal_op);
      if(lal_op_kind == ada_op_double_dot){
        needs_generic = true;
      }
    }

    if(needs_generic){
      return getDiscreteRangeGeneric(lal_element, ctx);
    }

    return getExpr(lal_element, ctx);
  }

/// returns an expression from the libadalang definition \ref lal_element
SgExpression&
getDefinitionExpr(ada_base_entity* lal_element, AstContext ctx)
{
  ada_node_kind_enum kind = ada_node_kind(lal_element);
  SgExpression*      res = nullptr;

  switch(kind)
  {
    case ada_bin_op:
    {
      logKind("A_Discrete_Range", kind);

      //Get the bounds
      ada_base_entity lal_lower_bound, lal_upper_bound;
      ada_bin_op_f_left(lal_element, &lal_lower_bound);
      ada_bin_op_f_right(lal_element, &lal_upper_bound);

      SgExpression& lb = getExpr(&lal_lower_bound, ctx);
      SgExpression& ub = getExpr(&lal_upper_bound, ctx);

      res = &mkRangeExp(lb, ub);
      break;
    }
    case ada_identifier: //TODO Does this only correspond to A_Discrete_Subtype_Definition?
    {
      logKind("A_Discrete_Subtype_Definition", kind);

      ada_base_entity* lal_constraint = nullptr; //TODO Where should this come from?
      SgType& ty = getDiscreteSubtype(lal_element, lal_constraint, ctx);

      res = &mkTypeExpression(ty);
      break;
    }

    case ada_subtype_indication:
    {
      logKind("A_Discrete_Subtype_Definition", kind);

      ada_base_entity lal_identifier;
      ada_subtype_indication_f_name(lal_element, &lal_identifier);

      ada_base_entity lal_constraint;
      ada_subtype_indication_f_constraint(lal_element, &lal_constraint);

      SgType& ty = getDiscreteSubtype(&lal_identifier, &lal_constraint, ctx);

      res = &mkTypeExpression(ty);
      break;
    }

    case ada_others_designator:
    {
      logKind("ada_others_designator", kind);
      res = &mkAdaOthersExp();
      break;
    }

    /*case A_Constraint:
      logKind("A_Constraint", el.ID);
      res = &getConstraintExpr(def, ctx);
      break;*/

    default:
      logFlaw() << "Unhandled definition expr: " << kind << " in getDefinitionExpr" << std::endl;
      res = &mkNullExpression();
  }

  attachSourceLocation(SG_DEREF(res), lal_element, ctx);
  return *res;
}

OperatorCallSupplement::ArgDescList
createArgDescList(const SgExpressionPtrList& args)
{
  OperatorCallSupplement::ArgDescList res;
  auto argDescExtractor = [](SgExpression* exp) -> ArgDesc
                          {
                            std::string optArgName;

                            if (SgActualArgumentExpression* act = isSgActualArgumentExpression(exp))
                              optArgName = act->get_argument_name();

                            if (false)
                            {
                              SgType* ty = si::Ada::typeOfExpr(exp).typerep();

                              logTrace() << "argDescExtractor: " << exp->unparseToString() << " " << si::Ada::typeOfExpr(exp).typerep()
                                         << " " << (ty ? typeid(*ty).name() : std::string{"!"})
                                         << std::endl;
                            }

                            return { optArgName, si::Ada::typeOfExpr(exp).typerep() };
                          };

  res.reserve(args.size());
  std::transform(args.begin(), args.end(), std::back_inserter(res), argDescExtractor);

  return res;
}

SgExpression& createCall(ada_base_entity* lal_prefix, std::vector<ada_base_entity*> lal_params, bool operatorCallSyntax, bool objectCallSyntax, AstContext ctx)
{
  // Create the arguments first. They may be needed to disambiguate operator calls
  std::vector<SgExpression*> arglist;
  for(ada_base_entity* param : lal_params){
    arglist.push_back(&getArg(param, ctx));
  }

  bool unary = (lal_params.size() == 1);

  SgExpression& tgt = getExpr(lal_prefix, ctx, OperatorCallSupplement(createArgDescList(arglist), nullptr /* unknown return type */), unary);
  SgExpression* res = sg::dispatch(AdaCallBuilder{lal_prefix, std::move(arglist), operatorCallSyntax, objectCallSyntax, ctx}, &tgt);

  return SG_DEREF(res);
}

SgExpression&
getEnumRepresentationValue(ada_base_entity* lal_element, int enum_position, AstContext ctx)
{
  std::string enum_position_string = std::to_string(enum_position);
  const char* enum_position_c_string = enum_position_string.c_str();
  SgExpression& sgnode = mkAdaIntegerLiteral(enum_position_c_string);

  attachSourceLocation(sgnode, lal_element, ctx);
  return sgnode;
}

SgNode*
queryBuiltIn(int hash)
{
  static constexpr bool findFirstMatch = false /* syntactic sugar, always false */;

  SgNode* res = nullptr;

  findFirstMatch
  || (res = findFirst(adaTypes(), hash))
  || (res = findFirst(adaPkgs(),  hash))
  || (res = findFirst(adaVars(),  hash))
  //|| (res = findFirst(adaExcps(), hash))
  ;

  return res;
}

SgNode*
queryCorrespondingAstNode(ada_base_entity* lal_identifier, AstContext ctx)
{
  static constexpr bool findFirstMatch = false /* syntactic sugar, always false */;

  //Check the kind
  ada_node_kind_enum kind = ada_node_kind(lal_identifier);
  if(kind != ada_identifier){
    logError() << "queryCorrespondingAstNode called on non-ada_identifier " << kind << std::endl;
    return nullptr;
  }

  //Get the text of this identifier
  const std::string name = canonical_text_as_string(lal_identifier);

  //Find the definition of this identifier and get its hash
  int hash;

  ada_base_entity corresponding_decl; //lal doesn't give definition directly, so go from the decl
  ada_expr_p_first_corresponding_decl(lal_identifier, &corresponding_decl);
  ada_ada_node_array defining_name_list;
  ada_basic_decl_p_defining_names(&corresponding_decl, &defining_name_list);
          
  //Find the correct decl in the defining name list
  for(int i = 0; i < defining_name_list->n; i++){
    ada_base_entity defining_name = defining_name_list->items[i];
    ada_base_entity name_identifier;
    ada_defining_name_f_name(&defining_name, &name_identifier);
    const std::string test_name = canonical_text_as_string(&name_identifier);
    if(name == test_name){
      hash = hash_node(&defining_name);
      logInfo() << "Found definition for ada_identifier " << name << std::endl;
      break;
    }
  }

  SgNode* res = nullptr;

  findFirstMatch
  || (res = findFirst(libadalangVars(),   hash))
  || (res = findFirst(libadalangDecls(),  hash))
  || (res = findFirst(libadalangTypes(),  hash))
  || (res = findFirst(libadalangExcps(),  hash))
  || (res = findFirst(libadalangBlocks(), hash))
  || (res = queryBuiltIn(hash))
  ;

  return res;
}

} //end Libadalang_ROSE_Translation
