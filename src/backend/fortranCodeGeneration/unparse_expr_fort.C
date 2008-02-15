/* unparse_expr_fort.C
 * 
 * Code to unparse Sage/Fortran expression nodes.
 * 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rose.h>
#include "unparser_fort.h"


static VariantT 
GetOperatorVariant(SgExpression* expr);

static SgExpression* 
GetFirstOperand(SgExpression* expr);

static int 
GetPrecedence(VariantT variant);

static int 
GetAssociativity(VariantT variant);

static bool 
isSubroutineCall(SgFunctionCallExp* func_call);


static const char* ARRAY_IDX_OP = "(array-index)";

//----------------------------------------------------------------------------
//  UnparserFort::unparseExpression
//  
//  General unparse function for expressions. Routes work to the
//  appropriate helper function.
//----------------------------------------------------------------------------
void 
UnparserFort::unparseExpression(SgExpression* expr, SgUnparse_Info& info)
{
  ROSE_ASSERT(expr != NULL);

#if 0
  string sgnm = expr->sage_class_name();
  printf("Beg unparseExpression (%s, %p):\n", sgnm.c_str(), expr);
  cur << "\n! Beg of unparseExpression() " << stnm << "\n";
#endif
  
  // -------------------------------------------------------
  // unparse the expression
  // -------------------------------------------------------
  bool printParen = printStartParen(expr, info);

  // Output the left paren
  if (printParen) {
    // should not be an expresion list
    ROSE_ASSERT(isSgExprListExp(expr) == NULL);
    cur << "(";
  }
  
  switch (expr->variantT())
    {
    case V_SgExpressionRoot:     unparseExprRoot(expr, info); break;

      // function, intrinsic calls
    case V_SgFunctionCallExp:    unparseFuncCall(expr, info); break;
    case V_SgIntrinsicFn:        unparseIntrinsic(expr, info); break;

      // operators
    case V_SgUnaryOp:            unparseUnaryExpr (expr, info); break;
    case V_SgBinaryOp:           unparseBinaryExpr(expr, info); break;

    case V_SgAssignOp:           unparseAssnOp(expr, info); break;

    case V_SgNotOp:              unparseNotOp(expr, info); break;
    case V_SgAndOp:              unparseAndOp(expr, info); break;
    case V_SgOrOp:               unparseOrOp(expr, info); break;
      
    case V_SgEqualityOp:         unparseEqOp(expr, info); break;
    case V_SgNotEqualOp:         unparseNeOp(expr, info); break;
    case V_SgLessThanOp:         unparseLtOp(expr, info); break;
    case V_SgGreaterThanOp:      unparseGtOp(expr, info); break;
    case V_SgLessOrEqualOp:      unparseLeOp(expr, info); break;
    case V_SgGreaterOrEqualOp:   unparseGeOp(expr, info); break;
      
    case V_SgMinusOp:            unparseUnaryMinusOp(expr, info); break;
    case V_SgUnaryAddOp:         unparseUnaryAddOp(expr, info); break;
    case V_SgAddOp:              unparseAddOp(expr, info); break;
    case V_SgSubtractOp:         unparseSubtOp(expr, info); break;
    case V_SgMultiplyOp:         unparseMultOp(expr, info); break;
    case V_SgDivideOp:           unparseDivOp(expr, info); break;
    case V_SgIntegerDivideOp:    unparseIntDivOp(expr, info); break;
    case V_SgExponentiationOp:   unparseExpOp(expr, info); break;

      // intrinsics mapped to Sage operators
    case V_SgModOp:              unparseModOp(expr, info); break;
    case V_SgBitXorOp:           unparseBitXOrOp(expr, info); break;
    case V_SgBitAndOp:           unparseBitAndOp(expr, info); break;
    case V_SgBitOrOp:            unparseBitOrOp(expr, info); break;
    case V_SgLshiftOp:           unparseLShiftOp(expr, info); break;
    case V_SgRshiftOp:           unparseRShiftOp(expr, info); break;
    case V_SgBitComplementOp:    unparseBitCompOp(expr, info); break;

      // operators, other
    case V_SgPntrArrRefExp:      unparseArrayOp(expr, info); break;
    case V_SgDotExp:             unparseRecRef(expr, info); break;
    case V_SgCastExp:            unparseCastOp(expr, info); break;

      // FIXME: allocate/deallocate are statements in Fortran
    case V_SgNewExp:             unparseNewOp(expr, info); break;
    case V_SgDeleteExp:          unparseDeleteOp(expr, info); break;

      // FIXME: 
    case V_SgArrowExp:           unparsePointStOp(expr, info); break;
    case V_SgPointerDerefExp:    unparseDerefOp(expr, info); break;
    case V_SgAddressOfOp:        unparseAddrOp(expr, info); break;
    case V_SgRefExp:             unparseTypeRef(expr, info); break;

    case V_SgSubscriptExpression: unparseSubExpr(expr, info); break;
    case V_SgSubscriptColon:      unparseSubColon(expr, info); break;
    case V_SgSubscriptAsterisk:   unparseSubAsterick(expr, info); break;
      
      // initializers
    case V_SgInitializer:            unparseExprInit(expr, info); break;
    case V_SgAggregateInitializer:   unparseAggrInit(expr, info); break;
    case V_SgConstructorInitializer: unparseConInit(expr, info); break;
    case V_SgAssignInitializer:      unparseAssnInit(expr, info); break;

      // rename/only lists
    case V_SgUseRenameExpression:   unparseUseRename(expr, info); break;
    case V_SgUseOnlyExpression:     unparseUseOnly(expr, info); break;
      
      // IO
    case V_SgIOItemExpression:      unparseIOItemExpr(expr, info); break;
    case V_SgIOImpliedDo:           unparseIOImpliedDo(expr, info); break;

      // symbol references
    case V_SgVarRefExp:             unparseVarRef(expr, info); break;
    case V_SgFunctionRefExp:        unparseFuncRef(expr, info); break;
    case V_SgMemberFunctionRefExp:  unparseMFuncRef(expr, info); break;
    case V_SgClassNameRefExp:       unparseClassRef(expr, info); break;
      
      // constants
    case V_SgBoolValExp:         unparseBoolVal(expr, info); break;
    case V_SgCharVal:            unparseCharVal(expr, info); break;
    case V_SgUnsignedCharVal:    unparseUCharVal(expr, info); break;
    case V_SgWcharVal:           unparseWCharVal(expr, info); break;
    case V_SgStringVal:          unparseStringVal(expr, info); break;
    case V_SgShortVal:           unparseShortVal(expr, info); break;
    case V_SgUnsignedShortVal:   unparseUShortVal(expr, info); break;
    case V_SgEnumVal:            unparseEnumVal(expr, info); break;
    case V_SgIntVal:             unparseIntVal(expr, info); break;
    case V_SgUnsignedIntVal:     unparseUIntVal(expr, info); break;
    case V_SgLongIntVal:         unparseLongIntVal(expr, info); break;
    case V_SgUnsignedLongVal:    unparseULongIntVal(expr, info); break;
    case V_SgLongLongIntVal:     unparseLongLongIntVal(expr, info); break;
    case V_SgUnsignedLongLongIntVal: unparseULongLongIntVal(expr, info); break;
    case V_SgFloatVal:           unparseFLoatVal(expr, info); break;
    case V_SgDoubleVal:          unparseDblVal(expr, info); break;
    case V_SgLongDoubleVal:      unparseLongDblVal(expr, info); break;

    default:
      printf("UnparserFort::unparseExpression: Error: No handler for %s (variant: %d)\n",
	     expr->sage_class_name(), expr->variantT());
      ROSE_ASSERT(false);
      break;
    }
  
  // Output the right paren
  if (printParen) {
    cur << ")";
  }


#if 0
  printf ("End unparseExpression (%s, %p):\n", sgnm.c_str(), expr);
  cur << "\n! End of unparseExpression() " << stnm << "\n";
#endif
}


//----------------------------------------------------------------------------
//  UnparserFort::unparseExprRoot
//----------------------------------------------------------------------------

void 
UnparserFort::unparseExprRoot(SgExpression* expr, SgUnparse_Info& info) 
{
}


//----------------------------------------------------------------------------
//  UnparserFort::<function, intrinsic calls>
//----------------------------------------------------------------------------

void
UnparserFort::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran function/subroutine call
  SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
  ROSE_ASSERT(func_call != NULL);

  // Two cases: operator overloading (in-order unparsing) vs. the
  // typical pre-order function call syntax.
  // 
  // - Use in-order unparsing when we have a binary operator overloading
  //   function and the operator overloading option is turned off. 
  // - Use standard pre-order unparsing otherwise

#if 0  
  if (!opt.get_overload_opt() // FIXME:eraxxon unary overloading
      && isBinaryOperator(func_call->get_function())) {
    // -----------------------------------------------------
    // Unparse as in-order operator. 
    // Exception: dot expression, arrow expression
    // -----------------------------------------------------
    SgUnparse_Info ninfo(info);
    
    SgExpressionPtrList& list = func_call->get_args()->get_expressions();
    SgExpressionPtrList::iterator arg = list.begin();
    if (arg != list.end()) {
      ninfo.set_nested_expression();

      // unparse the lhs operand
      unparseExpression((*arg), ninfo);
      // unparse the operator
      unparseExpression(func_call->get_function(), info);
      // unparse the rhs operand
      arg++;
      unparseExpression((*arg), ninfo);
      ninfo.unset_nested_expression();
    }
  }
  else {
#endif

    // -----------------------------------------------------
    // Unparse as pre-order subroutine/function call. 
    // -----------------------------------------------------

    if (isSubroutineCall(func_call)) {
      cur << "CALL ";
    }

    // subroutine/function name
    unparseExpression(func_call->get_function(), info);

    // argument list
    SgUnparse_Info ninfo(info);
    cur << "(";
    if (func_call->get_args()) {
      SgExpressionPtrList& list = func_call->get_args()->get_expressions();
      SgExpressionPtrList::iterator arg = list.begin();
      while (arg != list.end()) {
	SgConstructorInitializer* con_init = isSgConstructorInitializer(*arg);
	unparseExpression((*arg), ninfo);
	arg++;
	if (arg != list.end()) {
	  cur << ","; 
	}
      }
    }
    cur << ")";

#if 0
  }
#endif
}

void
UnparserFort::unparseIntrinsic(SgExpression* expr, SgUnparse_Info& info) 
{
  SgIntrinsicFn* intrn = isSgIntrinsicFn(expr);
  ROSE_ASSERT(intrn != NULL);
  
  // intrinsic name
  cur << intrn->get_name().str();

  // argument list
  unparseExprList(intrn->get_args(), info);
}


//----------------------------------------------------------------------------
//  UnparserFort::<operators>
//----------------------------------------------------------------------------

void
UnparserFort::unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info) 
{
  SgUnaryOp* unary_op = isSgUnaryOp(expr);
  ROSE_ASSERT(unary_op != NULL);
  
  // A name-operator is an operator that is formed using the
  // .name. syntax (e.g., .gt.) as opposed to 'symbolic' characters
  // (e.g. >).
  const char* opstr = info.get_operator_name();
  bool nameOp = ((opstr[0] == '.') && (opstr[strlen(opstr)-1] == '.'));

  cur << info.get_operator_name();
  if (nameOp) {
    cur << " "; 
  }
  info.set_nested_expression();
  unparseExpression(unary_op->get_operand(), info);
  info.unset_nested_expression();
}

void
UnparserFort::unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info) 
{
  printDebugInfo("entering unparseBinaryExpr", TRUE);

  SgBinaryOp* binary_op = isSgBinaryOp(expr);
  ROSE_ASSERT(binary_op != NULL);

  info.set_nested_expression();
  
  if (strcmp(info.get_operator_name(), ARRAY_IDX_OP) == 0) {
    // Special case: array indicing
    unparseExpression(binary_op->get_lhs_operand(), info);

    SgExprListExp* subscripts = isSgExprListExp(binary_op->get_rhs_operand());
    ROSE_ASSERT(subscripts);
    unparseExprList(subscripts, info);
  } 
  else {
    unparseExpression(binary_op->get_lhs_operand(), info);
    cur << " " << info.get_operator_name() << " ";
    unparseExpression(binary_op->get_rhs_operand(), info);
  }

  info.unset_nested_expression();
}

void
UnparserFort::unparseUnaryOpr(SgExpression* expr, const char* op, 
			      SgUnparse_Info& info)
{
  SgUnparse_Info ninfo(info);
  ninfo.set_operator_name(op);
  unparseUnaryExpr(expr, ninfo);
}

void
UnparserFort::unparseBinaryOpr(SgExpression* expr, const char* op, 
			       SgUnparse_Info& info)
{
  SgUnparse_Info ninfo(info);
  ninfo.set_operator_name(op);
  unparseBinaryExpr(expr, ninfo);
}

void 
UnparserFort::unparseAssnOp(SgExpression* expr, SgUnparse_Info& info) 
{ 
  // Sage node corresponds to Fortran assignment
  unparseBinaryOpr(expr, "=", info); 
}


void 
UnparserFort::unparseNotOp(SgExpression* expr, SgUnparse_Info& info) 
{ 
  // Sage node corresponds to Fortran logical inversion operator
  unparseUnaryOpr(expr, ".NOT.", info);
}

void
UnparserFort::unparseAndOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran logical-and operator
  unparseBinaryOpr(expr, ".AND.", info);
}

void
UnparserFort::unparseOrOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran logical-or operator
  unparseBinaryOpr(expr, ".OR.", info);
}


void
UnparserFort::unparseEqOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran equals operator
  unparseBinaryOpr(expr, "==", info);
}

void
UnparserFort::unparseNeOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran not-equals operator
  unparseBinaryOpr(expr, "/=", info);
}

void
UnparserFort::unparseLtOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran less-than operator
  unparseBinaryOpr(expr, "<", info);
}

void
UnparserFort::unparseGtOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran greater-than operator
  unparseBinaryOpr(expr, ">", info);
}

void
UnparserFort::unparseLeOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran less-than-or-equals operator
  unparseBinaryOpr(expr, "<=", info);
}

void
UnparserFort::unparseGeOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran greater-than-or-equals operator
  unparseBinaryOpr(expr, ">=", info);
}


void
UnparserFort::unparseUnaryMinusOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran unary-minus operator
  unparseUnaryOpr(expr, "-", info);
}

void
UnparserFort::unparseUnaryAddOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran unary-plus operator
  unparseUnaryOpr(expr, "+", info);
}

void
UnparserFort::unparseAddOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran addition operator
  unparseBinaryOpr(expr, "+", info);
}

void
UnparserFort::unparseSubtOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran subtraction operator
  unparseBinaryOpr(expr, "-", info);
}

void
UnparserFort::unparseMultOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran multiplication operator
  unparseBinaryOpr(expr, "*", info);
}

void
UnparserFort::unparseDivOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran division operator
  unparseBinaryOpr(expr, "/", info);
}

void
UnparserFort::unparseIntDivOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran int-division operator
  unparseBinaryOpr(expr, "/", info);
}

void
UnparserFort::unparseExpOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran exponentiation operator
  unparseBinaryOpr(expr, "**", info);
}


//----------------------------------------------------------------------------
//  UnparserFort::<FIXME> (Intrinsics mapped to Sage nodes)
//----------------------------------------------------------------------------

void
UnparserFort::unparseModOp(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran mod intrinsic (remainder function)
  unparseBinaryOpr(expr, "MOD", info);
}

void
UnparserFort::unparseBitXOrOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran ieor intrinsic
  unparseBinaryOpr(expr, "IEOR", info);
}

void
UnparserFort::unparseBitAndOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran iand intrinsic
  unparseBinaryOpr(expr, "IAND", info);
}

void
UnparserFort::unparseBitOrOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran ior intrinsic
  unparseBinaryOpr(expr, "IOR", info);
}

void
UnparserFort::unparseLShiftOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran ishft(x,+) intrinsic
  // FIXME:eraxxon: need special case in unparseBinaryExpr
  unparseBinaryOpr(expr, "ISHFT(x,+)", info);
}

void
UnparserFort::unparseRShiftOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran ishft(x,-) intrinsic
  // FIXME:eraxxon: need special case in unparseBinaryExpr
  unparseBinaryOpr(expr, "ISHFT(x,-)", info);
}

void 
UnparserFort::unparseBitCompOp(SgExpression* expr, SgUnparse_Info& info) 
{ 
  // Sage node corresponds to Fortran not intrinsic
  unparseUnaryOpr(expr, "NOT", info);
}


//----------------------------------------------------------------------------
//  UnparserFort::<operators, other>
//----------------------------------------------------------------------------

void
UnparserFort::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran array indicing
  unparseBinaryOpr(expr, ARRAY_IDX_OP, info); 
}

void
UnparserFort::unparseRecRef(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran record selector
  unparseBinaryOpr(expr, "%",info);
}

void
UnparserFort::unparseCastOp(SgExpression* expr, SgUnparse_Info& info)
{
  // FIXME:eraxxon
  SgCastExp* cast_op = isSgCastExp(expr);
  ROSE_ASSERT(cast_op != NULL);

  SgUnparse_Info ninfo(info);
  ninfo.unset_PrintName();

  // Never unparse the declaration from within a cast expression
  ninfo.set_SkipDefinition();
  ninfo.unset_SkipBaseType();

  switch(cast_op->cast_type())
    {
    case SgCastExp::dynamic_cast_e:
      {
	// dynamic_cast <P *> (expr)
	cur << "dynamic_cast <";
	unparseType(cast_op->get_expression_type(), ninfo);
	cur << ">"; // paren are in operand_i
	break; 
      }
    case SgCastExp::reinterpret_cast_e:
      {
	// reinterpret_cast <P *> (expr)
	cur << "reinterpret_cast <";
	unparseType(cast_op->get_expression_type(), ninfo);
	cur << ">";
	break;
      }
    case SgCastExp::c_cast_e:
    case SgCastExp::const_cast_e:
      {
	// (P *) expr
	// check if the expression that we are casting is not a string
	if (cast_op->get_operand_i()->variant() != STRING_VAL)
	  {
	    // it is not a string, so we always cast
	    cur << "(";
	    unparseType(cast_op->get_expression_type(), ninfo);
	    cur << ")";
	  }
	break; 
      }
    }

  unparseExpression(cast_op->get_operand_i(), info); 
}


//----------------------------------------------------------------------------
//  UnparserFort::<FIXME>
//----------------------------------------------------------------------------

void
UnparserFort::unparseNewOp(SgExpression* expr, SgUnparse_Info& info)
{
  // FIXME:eraxxon
  SgNewExp* new_op = isSgNewExp(expr);
  ROSE_ASSERT(new_op != NULL);

  ROSE_ASSERT(false && "UnparserFort::unparseNewOp");

  cur << "new ";

  SgUnparse_Info ninfo(info);
  ninfo.unset_inVarDecl();
  if (new_op->get_placement_args()) {
    unparseExpression(new_op->get_placement_args(), ninfo);
  }
  
  ninfo.unset_PrintName();
  ninfo.set_SkipClassSpecifier();
  
  unparseType(new_op->get_type(), ninfo);
  
  if (new_op->get_constructor_args()) {
    unparseExpression(new_op->get_constructor_args(), ninfo);
  }
  
  if (new_op->get_builtin_args()) {
    unparseExpression(new_op->get_builtin_args(), ninfo);
  }
}

void
UnparserFort::unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info)
{
  // FIXME:eraxxon
  SgDeleteExp* delete_op = isSgDeleteExp(expr);
  ROSE_ASSERT(delete_op != NULL);

  ROSE_ASSERT(false && "UnparserFort::unparseDeleteOp");
  
  cur << "delete ";
  SgUnparse_Info ninfo(info);
  unparseExpression(delete_op->get_variable(), ninfo);
}


//----------------------------------------------------------------------------
//  UnparserFort::<FIXME>
//----------------------------------------------------------------------------

void
UnparserFort::unparsePointStOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node has no Fortran correspondence (unless semantics are twisted)
  ROSE_ASSERT(false && "UnparserFort::unparsePointStOp");
  unparseBinaryOpr(expr, "->", info);
}

void 
UnparserFort::unparseDerefOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node has no explicit Fortran correspondence
  // FIXME:eraxxon:
  // unparseUnaryOpr(expr, "FIXME*", info);
}

void
UnparserFort::unparseAddrOp(SgExpression* expr, SgUnparse_Info& info) 
{ 
  // Sage node has no explicit Fortran correspondence
  // FIXME:eraxxon:
  // unparseUnaryOpr(expr, "FIXME&", info);
}

void 
UnparserFort::unparseTypeRef(SgExpression* expr, SgUnparse_Info& info) 
{
  // FIXME:eraxxon
  SgRefExp* type_ref = isSgRefExp(expr);
  ROSE_ASSERT(type_ref != NULL);

  SgUnparse_Info ninfo(info);
  ninfo.unset_PrintName();
  
  unparseType(type_ref->get_type_name(), ninfo);
}

void 
UnparserFort::unparseSubExpr(SgExpression* expr, SgUnparse_Info& info) 
{
  SgSubscriptExpression* sub_expr = isSgSubscriptExpression(expr);
  ROSE_ASSERT(sub_expr != NULL);
  
  SgExpression* lb     = sub_expr->get_lowerBound();
  SgExpression* stride = sub_expr->get_stride();
  
  if (lb) {
    unparseExpression(lb, info);
    cur << ":";
  }
  
  unparseExpression(sub_expr->get_upperBound(), info);
  
  if (stride) {
    cur << ":";
    unparseExpression(stride, info);
  }
}

void 
UnparserFort::unparseSubColon(SgExpression* expr, SgUnparse_Info& info) 
{
  SgSubscriptColon* sub_col = isSgSubscriptColon(expr);
  ROSE_ASSERT(sub_col != NULL);
  
  unparseExpression(sub_col->get_lowerBound(), info);
  cur << ":";
  // FIXME: get_stride(): why do we need this?
}

void 
UnparserFort::unparseSubAsterick(SgExpression* expr, SgUnparse_Info& info) 
{
  SgSubscriptAsterisk* sub_ast = isSgSubscriptAsterisk(expr);
  ROSE_ASSERT(sub_ast != NULL);

  cur << "*";
}

//----------------------------------------------------------------------------
//  UnparserFort::<initializers>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseExprInit(SgExpression* expr, SgUnparse_Info& info) 
{
}

void 
UnparserFort::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
{
  // FIXME:eraxxon
  SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
  ROSE_ASSERT(aggr_init != NULL);
  
  SgUnparse_Info ninfo(info);
  cur << "{";
  
  SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();
  SgExpressionPtrList::iterator p = list.begin();
  while (p != list.end()) {
    unparseExpression((*p), ninfo);
    p++;
    if (p != list.end()) {  
      cur << ", ";  
    }
  }
  
  cur << "}";
}

void
UnparserFort::unparseConInit(SgExpression* expr, SgUnparse_Info& info)
{
  // FIXME:eraxxon
  SgConstructorInitializer* con_init = isSgConstructorInitializer(expr);
  ROSE_ASSERT(con_init != NULL);
  
  SgUnparse_Info ninfo(info);
  
  if (con_init->get_need_name() == true) {
    SgName nm;
    
    // DQ (12/4/2003): Added assertion (one of these is required!)
    ROSE_ASSERT (con_init->get_declaration() != NULL || con_init->get_class_decl() != NULL);
    
    if (con_init->get_declaration() != NULL) {
      if (con_init->get_need_qualifier())
	nm = con_init->get_declaration()->get_qualified_name();
      else
	nm = con_init->get_declaration()->get_name();
    }
    else {
      if (con_init->get_class_decl() != NULL) {
	if (con_init->get_need_qualifier()) 
	  nm = con_init->get_class_decl()->get_qualified_name();
	else
	  nm = con_init->get_class_decl()->get_name();
      }
    }
    
    ROSE_ASSERT ( nm.is_null() == false );
    cur << nm.str();
  }
  
  if (con_init->get_args()) {
    cur << "(";
    unparseExpression(con_init->get_args(), ninfo);
    cur << ")";
  }
  else {
    // DQ (5/29/2004) Skip this so that we can avoid unparsing "B b;" as "B b();" since
    // this is a problem for g++ if a reference is taken to "b" (see test2004_44.C).
    // Verify that P::P():B() {} will still unparse correctly, ... it does!
  }
}

void
UnparserFort::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
{
  // FIXME:eraxxon
  SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
  ROSE_ASSERT(assn_init != NULL);
  unparseExpression(assn_init->get_operand(), info);
}


//----------------------------------------------------------------------------
//  UnparserFort::<rename/only lists>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseUseRename(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran rename expression ('use a => a_old')
  SgUseRenameExpression* rename_expr = isSgUseRenameExpression(expr);
  ROSE_ASSERT(rename_expr != NULL);

  SgExpression* oldnm = rename_expr->get_oldname();
  SgExpression* newnm = rename_expr->get_newname();
  
  unparseExpression(newnm, info);
  cur << " => ";
  unparseExpression(oldnm, info);
}

void 
UnparserFort::unparseUseOnly(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran 'use, only' expression
  SgUseOnlyExpression* only_expr = isSgUseOnlyExpression(expr);
  ROSE_ASSERT(only_expr != NULL);

  SgExprListExp* lst = only_expr->get_access_list();
  cur << ", ONLY: ";
  unparseExprList(lst, info, false /*paren*/);
}


//----------------------------------------------------------------------------
//  UnparserFort::<IO>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseIOItemExpr(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node represents Fortran IO
  SgIOItemExpression* ioitem_expr = isSgIOItemExpression(expr);
  ROSE_ASSERT(ioitem_expr != NULL);

  SgExpression* ioitem = ioitem_expr->get_io_item();

  if (isSgExprListExp(ioitem)) {
    unparseExprList(isSgExprListExp(ioitem), info, false /*paren*/);
  }
  else {
    unparseExpression(ioitem, info);
  }
}


void 
UnparserFort::unparseIOImpliedDo(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran implied do
  SgIOImpliedDo* ioitem_expr = isSgIOImpliedDo(expr);
  ROSE_ASSERT(ioitem_expr != NULL);

  SgExprListExp* obj_list = ioitem_expr->get_object_list();
  SgVarRefExp* var = ioitem_expr->get_do_var();
  SgExpression* lb = ioitem_expr->get_first_val();
  SgExpression* ub = ioitem_expr->get_last_val();
  SgExpression* step = ioitem_expr->get_increment();

  cur << "(";
  if (obj_list) {
    unparseExprList(obj_list, info, false /*paren*/);
  }
  unparseExpression(var, info);
  cur << " = ";
  unparseExpression(lb, info);
  cur << ", ";
  unparseExpression(ub, info);
  cur << ", ";
  unparseExpression(step, info);
  cur << ")";
}


//----------------------------------------------------------------------------
//  UnparserFort::<symbol references>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran variable reference
  SgVarRefExp* var_ref = isSgVarRefExp(expr);
  ROSE_ASSERT(var_ref != NULL);
  ROSE_ASSERT(var_ref->get_symbol() != NULL);
  
  SgInitializedName* decl = var_ref->get_symbol()->get_declaration();
  SgVariableDeclaration* vd = isSgVariableDeclaration(decl->get_declaration());

  if (false /*vd != NULL*/) {
    // FIXME:eraxxon: how to handle renamed module vars?
    SgClassDefinition* cdef = isSgClassDefinition(vd->get_parent());
    if (cdef != NULL) {
      SgClassDeclaration* cdecl = isSgClassDeclaration(cdef->get_declaration());
      if (cdecl != NULL 
	  && vd->get_declarationModifier().get_storageModifier().isStatic()) {
	cur <<  cdecl->get_qualified_name().str() <<  "::";
      }
    }
  }
  
  cur << var_ref->get_symbol()->get_name().str();
}

void 
UnparserFort::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran function reference
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  ROSE_ASSERT(func_ref != NULL);
  string func_name = func_ref->get_symbol()->get_name().str();
  cur << func_name;
}

void
UnparserFort::unparseMFuncRef(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node has no Fortran correspondence (unless semantics are twisted)
  // FIXME:eraxxon

  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  ROSE_ASSERT(mfunc_ref != NULL);
  
  SgMemberFunctionDeclaration* mfd  = mfunc_ref->get_symbol_i()->get_declaration();
  SgClassDefinition*           cdef = mfd->get_scope();

  ROSE_ASSERT (cdef != NULL);
  SgClassDeclaration* cdecl = cdef->get_declaration();

  // qualified name is always outputed except when the p_need_qualifier is
  // set to 0 (when the naming class is identical to the selection class, and
  // and when we aren't suppressing the virtual function mechanism).  
  ROSE_ASSERT(cdecl != NULL);
  ROSE_ASSERT(cdecl->get_parent() != NULL);

  bool print_colons = FALSE;
  if (mfunc_ref->get_need_qualifier()) {
    cur <<  cdecl->get_qualified_name().str() << "::"; 
    print_colons = TRUE;
  }
  
  // comments about the logic below can be found above in the unparseFuncRef function.
  char* func_name = strdup( mfunc_ref->get_symbol()->get_name().str() );
  int diff = 0;

  // check that this an operator overloading function and that colons were not printed
  if (!opt.get_overload_opt() && !strncmp(func_name, "operator", 8) && !print_colons) {
    // the length difference between "operator" and function
    diff = strlen(func_name) - strlen("operator"); 
    if (diff > 0) {
      // get the substring after "operator"
      func_name = strchr(func_name, func_name[8]);
    }
  }

  if (strcmp(func_name,"[]") == 0) {
    // [DT] 3/30/2000 -- Don't unparse anything here.  The square brackets will
    //      be handled from unparseFuncCall().
    //
    //      May want to handle overloaded operator() the same way.
  } 
  else {
    if (strcmp(func_name, "()")) {
      cur <<  func_name;
    }
  }
}

void 
UnparserFort::unparseClassRef(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node has no Fortran correspondence (unless semantics are twisted)
  // FIXME:eraxxon
  SgClassNameRefExp* classname_ref = isSgClassNameRefExp(expr);
  ROSE_ASSERT(classname_ref != NULL);
  cur << classname_ref->get_symbol()->get_declaration()->get_name().str();
}


//----------------------------------------------------------------------------
//  void UnparserFort::<constants>
//----------------------------------------------------------------------------

void
UnparserFort::unparseBoolVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran logical constant
  SgBoolValExp* bool_val = isSgBoolValExp(expr);
  ROSE_ASSERT(bool_val != NULL);  

  if (bool_val->get_value() == true) {
    cur <<  ".TRUE.";
  }
  else {
    cur <<  ".FALSE.";
  }
}

void 
UnparserFort::unparseCharVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran character constant
  SgCharVal* char_val = isSgCharVal(expr);
  ROSE_ASSERT(char_val != NULL);
  cur << char_val->get_value();
}

void 
UnparserFort::unparseUCharVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedCharVal* uchar_val = isSgUnsignedCharVal(expr);
  ROSE_ASSERT(uchar_val != NULL);
  cur << uchar_val->get_value();
}

void 
UnparserFort::unparseWCharVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node has no Fortran correspondence
  SgWcharVal* wchar_val = isSgWcharVal(expr);
  ROSE_ASSERT(wchar_val != NULL);
  ROSE_ASSERT(false && "UnparserFort::unparseWCharVal");
  cur << (int) wchar_val->get_value();
}

void
UnparserFort::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran string constant
  SgStringVal* str_val = isSgStringVal(expr);
  ROSE_ASSERT(str_val != NULL);
  ROSE_ASSERT(str_val->get_value() != NULL);      
  
  string str = string("\"") + str_val->get_value() + string("\"");
  cur << str;
}

void 
UnparserFort::unparseShortVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgShortVal* short_val = isSgShortVal(expr);
  ROSE_ASSERT(short_val != NULL);
  cur << short_val->get_value();
}

void 
UnparserFort::unparseUShortVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedShortVal* ushort_val = isSgUnsignedShortVal(expr);
  ROSE_ASSERT(ushort_val != NULL);
  cur << ushort_val->get_value();
}

void
UnparserFort::unparseEnumVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node has no Fortran correspondence
  SgEnumVal* enum_val = isSgEnumVal(expr);
  ROSE_ASSERT(enum_val != NULL);
  
  // FIXME:eraxxon [could be Fortran parameter]
  ROSE_ASSERT(false && "UnparserFort::unparseEnumVal");

  if (info.inEnumDecl()) {
    cur << enum_val->get_value();
  }
  else {
    SgClassDefinition* classdefn = NULL;
    if (enum_val->get_declaration() && (classdefn = isSgClassDefinition(enum_val->get_declaration()->get_parent()))) {
      cur << classdefn->get_qualified_name().str() <<  "::";
    }
    
    ROSE_ASSERT (enum_val->get_name().str() != NULL);
    cur << enum_val->get_name().str();
  }
}

void 
UnparserFort::unparseIntVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran integer constant
  SgIntVal* int_val = isSgIntVal(expr);
  ROSE_ASSERT(int_val != NULL);
  cur << int_val->get_value();
}

void
UnparserFort::unparseUIntVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedIntVal* uint_val = isSgUnsignedIntVal(expr);
  ROSE_ASSERT(uint_val != NULL);
  cur << uint_val->get_value();
}

void 
UnparserFort::unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgLongIntVal* longint_val = isSgLongIntVal(expr);
  ROSE_ASSERT(longint_val != NULL);
  cur << longint_val->get_value();
}

void 
UnparserFort::unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedLongVal* ulongint_val = isSgUnsignedLongVal(expr);
  ROSE_ASSERT(ulongint_val != NULL);
  cur << ulongint_val->get_value();
}

void 
UnparserFort::unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgLongLongIntVal* longlongint_val = isSgLongLongIntVal(expr);
  ROSE_ASSERT(longlongint_val != NULL);
  cur << longlongint_val->get_value();
}

void 
UnparserFort::unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedLongLongIntVal* ulonglongint_val = isSgUnsignedLongLongIntVal(expr);
  ROSE_ASSERT(ulonglongint_val != NULL);
  cur << ulonglongint_val->get_value();
}

void 
UnparserFort::unparseFLoatVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran real constant
  SgFloatVal* float_val = isSgFloatVal(expr);
  ROSE_ASSERT(float_val != NULL);
  cur << float_val->get_value();
}

void
UnparserFort::unparseDblVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran real constant
  SgDoubleVal* dbl_val = isSgDoubleVal(expr);
  ROSE_ASSERT(dbl_val != NULL);
  cur << dbl_val->get_value(); 
}

void 
UnparserFort::unparseLongDblVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran real constant
  SgLongDoubleVal* longdbl_val = isSgLongDoubleVal(expr);
  ROSE_ASSERT(longdbl_val != NULL);
  cur << longdbl_val->get_value();
}


//----------------------------------------------------------------------------
//  helpers
//----------------------------------------------------------------------------

bool 
UnparserFort::unparseExprList(SgExprListExp* expr, SgUnparse_Info& info, 
			      bool paren) 
{
  ROSE_ASSERT(expr);
  info.set_nested_expression();

  if (paren) {
    cur << "(";
  }
  SgExpressionPtrList::iterator it = expr->get_expressions().begin();
  while (it != expr->get_expressions().end()) {
    unparseExpression(*it, info);
    it++;
    if (it != expr->get_expressions().end()) {
      cur << ","; 
    }
  }
  if (paren) {
    cur << ")";
  }

  info.unset_nested_expression();
}

//----------------------------------------------------------------------------
//  void UnparserFort::printStartParen
//  
//  Auxiliary function that determines whether "(" should been printed
//  for a given expression node. 
//----------------------------------------------------------------------------
bool 
UnparserFort::printStartParen(SgExpression* expr, SgUnparse_Info& info) 
{
  SgExpression* parentExpr = isSgExpression(expr->get_parent());
     
  if (!parentExpr || parentExpr->variantT() == V_SgExpressionRoot) {
    return false;
  }

  switch (expr->variantT()) {

      // expr-list
    case V_SgExprListExp:

      // subscripts
    case V_SgSubscriptExpression:
    case V_SgSubscriptColon:
    case V_SgSubscriptAsterisk:

      // IO
    case V_SgIOItemExpression:
    case V_SgIOImpliedDo:

      // symbol references
    case V_SgVarRefExp: 
    case V_SgFunctionRefExp: 
    case V_SgMemberFunctionRefExp: 
    case V_SgClassNameRefExp: 
         return false;
      
      // constants
    case V_SgBoolValExp:
      return false;

    case V_SgCharVal:
      if (isSgCharVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgUnsignedCharVal:
      return false;

    case V_SgWcharVal:
      if (isSgWcharVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgStringVal:
      if (isSgStringVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgShortVal:
      if (isSgShortVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgUnsignedShortVal:
    case V_SgEnumVal:
      return false;

    case V_SgIntVal:
      if (isSgIntVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgUnsignedIntVal:
      return false;

    case V_SgLongIntVal:
      if (isSgLongIntVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgUnsignedLongVal:
      return false;

    case V_SgLongLongIntVal:
      if (isSgLongLongIntVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgUnsignedLongLongIntVal:
      return false;

    case V_SgFloatVal:
      if (isSgFloatVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgDoubleVal:
      if (isSgDoubleVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;

    case V_SgLongDoubleVal:
      if (isSgLongDoubleVal(expr)->get_value() < 0) {
        return true;
      }
      else return false;
      
    default: {
      VariantT parentVariant = GetOperatorVariant(parentExpr);
      SgExpression* first = GetFirstOperand(parentExpr);
      if (parentVariant == V_SgPntrArrRefExp && first != expr) {
	return false;
      }

      int parentPrecedence = GetPrecedence(parentVariant);
      if (parentPrecedence == 0) {
	return true;
      }
      
      VariantT exprVariant = GetOperatorVariant(expr);
      int exprPrecedence = GetPrecedence(exprVariant);
      if (exprPrecedence > parentPrecedence) {
	return false;
      } 
      else if (exprPrecedence == parentPrecedence) {
	if (first == 0) {
	  return true; 
	}
	int assoc = GetAssociativity(parentVariant);
	if (assoc > 0 && first != expr) {
	  return false;
	}
	if (assoc < 0 && first == expr) {
	  return false;
	}
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------
//  void UnparserFort::isOperator
//  
//  General function to test if this expression is an unary or binary operator
//  overloading function
//----------------------------------------------------------------------------
bool 
UnparserFort::isOperator(SgExpression* expr) 
{
  ROSE_ASSERT(expr != NULL);
  
  if (isBinaryOperator(expr) || isUnaryOperator(expr)) {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
//  void UnparserFort::isUnaryOperator
//  
//  Function to test if this expression is an unary operator
//  overloading function
//----------------------------------------------------------------------------
bool 
UnparserFort::isUnaryOperator(SgExpression* expr) 
{
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  string fname;
  if (func_ref) {
    fname = func_ref->get_symbol()->get_name().str();
  }
  else if (mfunc_ref) {
    fname = mfunc_ref->get_symbol()->get_name().str();
  } 
  else {
    return false;
  }
  
  if (isUnaryOperatorPlus(mfunc_ref) || 
      isUnaryOperatorMinus(mfunc_ref) ||
      fname == "operator!") {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
//  void UnparserFort::isBinaryOperator
//  
//  Function to test if this expression is a binary operator
//  overloading function
//----------------------------------------------------------------------------
bool 
UnparserFort::isBinaryOperator(SgExpression* expr) 
{

  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

  string fname;
  if (func_ref) {
    fname = func_ref->get_symbol()->get_name().str();
  }
  else if (mfunc_ref) {
    fname = mfunc_ref->get_symbol()->get_name().str();
  } 
  else {
    return false;
  }
  
  if (fname.find("operator", 0) == 0) {
    if (fname == "operator=" ||
	
	fname == "operator||" ||
	fname == "operator&&" ||
	
	fname == "operator==" ||
	fname == "operator!=" ||
	fname == "operator<" ||
	fname == "operator>" ||
	fname == "operator<=" ||
	fname == "operator>=" ||
	
	fname == "operator+" ||
	fname == "operator-" ||
	fname == "operator*" ||
	fname == "operator/" ||
	
	fname == "operator**") {
      return true;
    }
    else {
      printf("isBinaryOperator: Error: no handler for '%s'\n", fname.c_str());
      ROSE_ASSERT(false);
    }
  }
  return false;
}

//----------------------------------------------------------------------------
//  void UnparserFort::isUnaryOperatorPlus
//  
//  Auxiliary function to test if this expression is an unary
//  operator+ overloading function
//----------------------------------------------------------------------------
bool 
UnparserFort::isUnaryOperatorPlus(SgExpression* expr) 
{
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  if (mfunc_ref) {
    SgMemberFunctionSymbol* mfunc_sym = mfunc_ref->get_symbol();
    if (mfunc_sym) {
      SgMemberFunctionDeclaration* mfunc_decl = mfunc_sym->get_declaration();
      if (mfunc_decl) {
	SgName func_name = mfunc_decl->get_name();
	if (strcmp(func_name.str(), "operator+") == 0) {
	  SgInitializedNamePtrList argList = mfunc_decl->get_args();
	  if (argList.size() == 0) {
	    return true;
	  }
	}
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------
//  void UnparserFort::isUnaryOperatorMinus
//  
//  Auxiliary function to test if this expression is an unary
//  operator- overloading function
//----------------------------------------------------------------------------
bool 
UnparserFort::isUnaryOperatorMinus(SgExpression* expr) 
{
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  if (mfunc_ref) {
    SgMemberFunctionSymbol* mfunc_sym = mfunc_ref->get_symbol();
    if (mfunc_sym) {
      SgMemberFunctionDeclaration* mfunc_decl = mfunc_sym->get_declaration();
      if (mfunc_decl) {
	SgName func_name = mfunc_decl->get_name();
	if (strcmp(func_name.str(), "operator-") == 0) {
	  SgInitializedNamePtrList argList = mfunc_decl->get_args();
	  if (argList.size() == 0) {
	    return true;
	  }
	}
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------
//  void GetOperatorVariant
//  
//  Function that returns the expression variant of overloaded operators
//----------------------------------------------------------------------------

static VariantT 
GetOperatorVariant(SgExpression* expr) 
{
  SgFunctionCallExp* fcall = isSgFunctionCallExp(expr);
  if (!fcall) {
    return expr->variantT();
  }
 
  // -------------------------------------------------------
  // we have a function call, must check for overloading
  // -------------------------------------------------------
  string fname;
  
  SgExpression* func = fcall->get_function();
  if (func->variantT() == V_SgFunctionRefExp) {
    fname = isSgFunctionRefExp(func)->get_symbol()->get_name().str();
  }
  
  if (!fname.empty() && (fname.find("operator", 0) == 0)) {
    
    if (fname == "operator=")       { return V_SgAssignOp; }
    
    else if (fname == "operator!")  { return V_SgNotOp; }
    else if (fname == "operator||") { return V_SgOrOp; }
    else if (fname == "operator&&") { return V_SgAndOp; }

    else if (fname == "operator==") { return V_SgEqualityOp; }
    else if (fname == "operator!=") { return V_SgNotEqualOp; }
    else if (fname == "operator<")  { return V_SgLessThanOp; }
    else if (fname == "operator>")  { return V_SgGreaterThanOp; }
    else if (fname == "operator<=") { return V_SgLessOrEqualOp; }
    else if (fname == "operator>=") { return V_SgGreaterOrEqualOp; }

    else if (fname == "operator+")  { return V_SgAddOp; }
    else if (fname == "operator-")  { return V_SgSubtractOp; }
    else if (fname == "operator*")  { return V_SgMultiplyOp; }
    else if (fname == "operator/")  { return V_SgDivideOp; }
    else if (fname == "operator**") { return V_SgExponentiationOp; }

    else if (fname == "operator.")  { return V_SgDotExp; }
    
    else {
      printf("GetOperatorVariant: Error: no handler for '%s'\n", 
	     fname.c_str());
      ROSE_ASSERT(false);
    }
  }
  else {
    return V_SgFunctionCallExp;
  }
}

static SgExpression* 
GetFirstOperand(SgExpression* expr) 
{
  SgFunctionCallExp* fcall = isSgFunctionCallExp(expr);
  if (fcall) {
    return fcall->get_function();
  }
  else {
    SgUnaryOp *op1 = isSgUnaryOp(expr);
    if (op1) {
      return op1->get_operand();
    }
    else {
      SgBinaryOp *op2 = isSgBinaryOp(expr);
      if (op2) {
	return op2->get_lhs_operand();
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
//  int GetPrecedence
//  
//  returns the precedence (1-17) of the expression variants, 
//  such that 17 has the highest precedence and 1 has the lowest precedence. 
//----------------------------------------------------------------------------
static int 
GetPrecedence(VariantT variant) 
{
  switch (variant) {
    case V_SgExprListExp: 
    case V_SgCommaOpExp: return 1;
    case V_SgAssignOp: return 2;
    case V_SgConditionalExp: return 3;
    case V_SgOrOp: return 4; 
    case V_SgAndOp: return 5; 
    case V_SgEqualityOp: return 9;  
    case V_SgNotEqualOp: return 9;
    case V_SgLessThanOp: return 10;
    case V_SgGreaterThanOp: return 10;
    case V_SgLessOrEqualOp:  return 10; 
    case V_SgGreaterOrEqualOp:  return 10; 
    case V_SgLshiftOp:  return 11;
    case V_SgRshiftOp:  return 11;
    case V_SgAddOp: return 12; 
    case V_SgSubtractOp: return 12; 
    case V_SgMultiplyOp: return 13; 
    case V_SgIntegerDivideOp:
    case V_SgDivideOp: return 13; 
    case V_SgExponentiationOp: return 13; 
    case V_SgNotOp: return 15;
    case V_SgFunctionCallExp: return 16;
    case V_SgPntrArrRefExp: return 16;
    case V_SgArrowExp: return 16;
    case V_SgDotExp: return 16;
  }
  return 0;
}

//----------------------------------------------------------------------------
//  GetAssociativity
//
//  Function that returns the associativity of the expression variants,
//  -1: left associative; 1: right associative; 0 : not associative/unknown
//----------------------------------------------------------------------------
static int 
GetAssociativity(VariantT variant) 
{
  switch (variant) {
    case V_SgAssignOp: return 1;
    case V_SgFunctionCallExp: return -1;
    case V_SgConditionalExp: return 1;

    case V_SgNotOp: return 1;
    case V_SgAndOp: return -1;
    case V_SgOrOp: return -1;

    case V_SgEqualityOp: return -1;
    case V_SgNotEqualOp: return -1;
    case V_SgLessThanOp: return -1;
    case V_SgGreaterThanOp: return -1;
    case V_SgLessOrEqualOp:  return -1;
    case V_SgGreaterOrEqualOp:  return -1;

    case V_SgAddOp: return -1;
    case V_SgSubtractOp: return -1;
    case V_SgMultiplyOp: return -1;
    case V_SgDivideOp: return -1;
    case V_SgIntegerDivideOp:
    case V_SgExponentiationOp:

    case V_SgPntrArrRefExp: return -1;
    case V_SgDotExp: return -1;
  }
  return 0;
}

static bool
isSubroutineCall(SgFunctionCallExp* fcall)
{
  // Returns true if this is a subroutine call (as opposed to a function call)

  SgFunctionRefExp* funcref = isSgFunctionRefExp(fcall->get_function());
  ROSE_ASSERT(funcref); 

  SgFunctionType* ftype = funcref->get_function_type();
  SgType* rtype = ftype->get_return_type();

  // Note: 'rtype' should be equivalent to 'fcall->get_type()'

  switch (rtype->variantT()) {
  case V_SgTypeVoid:
  case V_SgTypeGlobalVoid:
    return true;

  default:
    return false;
  }
}
