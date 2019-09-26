/* unparseFortran_expressionsrt.C
 * 
 * Code to unparse Sage/Fortran expression nodes.
 * 
 */
#include "sage3basic.h"
#include "unparser.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace Rose;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_HIDDEN_LIST_DATA 0
#define OUTPUT_DEBUGGING_INFORMATION 0

// static VariantT GetOperatorVariant(SgExpression* expr);
// static SgExpression* GetFirstOperand(SgExpression* expr);
// static int GetPrecedence(VariantT variant);
// static int GetAssociativity(VariantT variant);
// static bool isSubroutineCall(SgFunctionCallExp* func_call);

// DQ (8/14/2007): This appears to a temporary fix to generate just moderatly good looking code!
// static const char* ARRAY_IDX_OP = "(array-index)";
// const char* ARRAY_IDX_OP = "(array-index)";


void
FortranCodeGeneration_locatedNode::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info)
   {
  // This is the Fortran specific expression code generation

#if 0
     printf ("In FortranCodeGeneration_locatedNode::unparseLanguageSpecificExpression ( expr = %p = %s ) language = %s \n",expr,expr->class_name().c_str(),languageName().c_str());
#endif

     switch (expr->variantT())
        {
       // case V_SgExpressionRoot:     unparseExprRoot(expr, info); break;

       // function, intrinsic calls
          case V_SgFunctionCallExp:    unparseFuncCall(expr, info); break;

       // DQ (11/24/2007): This is removed, all function calls are equal, some are marked as being intrisic...
       // case V_SgIntrinsicFn:        unparseIntrinsic(expr, info); break;

       // operators
          case V_SgUnaryOp:            unparseUnaryExpr (expr, info); break;
          case V_SgBinaryOp:           unparseBinaryExpr(expr, info); break;

          case V_SgAssignOp:           unparseAssnOp(expr, info); break;
          case V_SgPointerAssignOp:    unparsePointerAssnOp(expr, info); break;

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

       // DQ (12/14/2007): Support for Fortran string concatenation
          case V_SgConcatenationOp:    unparseConcatenationOp(expr, info); break;

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

          case V_SgSubscriptExpression: unparseSubscriptExpr(expr, info); break;

       // DQ (11/18/2007): Added support for ":" in declarations
       // case V_SgColon:               unparseColon(expr, info); break;
       // case V_SgSubscriptAsterisk:   unparseSubAsterick(expr, info); break;
          case V_SgColonShapeExp:       unparseColonShapeExp(expr, info); break;
          case V_SgAsteriskShapeExp:    unparseAsteriskShapeExp(expr, info); break;

       // initializers
       // case V_SgInitializer:            unparseExprInit(expr, info); break;
          case V_SgAggregateInitializer:   unparseAggrInit(expr, info); break;
          case V_SgConstructorInitializer: unparseConInit(expr, info); break;
          case V_SgAssignInitializer:      unparseAssnInit(expr, info); break;

       // rename/only lists
       // case V_SgUseRenameExpression:   unparseUseRename(expr, info); break;
       // case V_SgUseOnlyExpression:     unparseUseOnly(expr, info); break;

       // IO
          case V_SgIOItemExpression:      unparseIOItemExpr(expr, info); break;
          case V_SgImpliedDo:             unparseImpliedDo(expr, info); break;

       // symbol references
          case V_SgVarRefExp:             unparseVarRef(expr, info); break;
          case V_SgFunctionRefExp:        unparseFuncRef(expr, info); break;
          case V_SgMemberFunctionRefExp:  unparseMFuncRef(expr, info); break;
          case V_SgClassNameRefExp:       unparseClassRef(expr, info); break;

       // DQ (9/29/2007): This is defined in the base class
          case V_SgNullExpression:        unparseNullExpression(expr, info); break;

       // DQ (11/24/2007): Support for unparsing the IR node which must be post-processed into either an array reference or a function call.
          case V_SgUnknownArrayOrFunctionReference: unparseUnknownArrayOrFunctionReference(expr, info); break;

       // DQ (12/1/2007): This sort of value has be be handled special for Fortran
          case V_SgBoolValExp:         unparseBoolVal(expr, info); break;

          case V_SgLabelRefExp:        unparseLabelRefExp(expr, info); break;

          case V_SgActualArgumentExpression:  unparseActualArgumentExpression(expr, info); break;

       // DQ (10/10/2008): Added support for unser defined unary and binary operators.
          case V_SgUserDefinedUnaryOp:        unparseUserDefinedUnaryOp (expr, info); break;
          case V_SgUserDefinedBinaryOp:       unparseUserDefinedBinaryOp(expr, info); break;
          case V_SgCAFCoExpression:           unparseCoArrayExpression(expr, info); break;

#if 0
       // DQ (8/15/2007): These are handled in the base class
       // constants
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
#endif
          default:
             {
               printf("FortranCodeGeneration_locatedNode::unparseExpression: Error: No handler for %s (variant: %d)\n",expr->sage_class_name(), expr->variantT());
               ROSE_ASSERT(false);
               break;
             }
        }
   }

void
FortranCodeGeneration_locatedNode::unparseActualArgumentExpression(SgExpression* expr, SgUnparse_Info& info)
   {
     SgActualArgumentExpression* actualArgumentExpression = isSgActualArgumentExpression(expr);

     curprint(actualArgumentExpression->get_argument_name());

#if 1
  // DQ (2/2/2011): Now we don't want to support the use of SgActualArgumentExpression 
  // to hide a alternative return argument.  So the name should never be "*". Now we
  // use a newer implementation with SgLabelRefExp instead (and a new SgTypeLabel IR node).
     ROSE_ASSERT(actualArgumentExpression->get_argument_name() != "*");
     curprint("=");
#else
  // DQ (1/30/2011): If the name is "*" then this is an "alternative return label".
  // Note that we might want this to appear more explicitly as a specialized IR 
  // node in the future.
     if (actualArgumentExpression->get_argument_name() != "*")
        {
          curprint("=");
        }
#endif

     unparseExpression(actualArgumentExpression->get_expression(),info);
   }


void
FortranCodeGeneration_locatedNode::unparseLabelRefExp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLabelRefExp* labelRefExp = isSgLabelRefExp(expr);
     ROSE_ASSERT(labelRefExp != NULL);

     SgLabelSymbol* labelSymbol = labelRefExp->get_symbol();
     ROSE_ASSERT(labelSymbol != NULL);

     int numericLabel = labelSymbol->get_numeric_label_value();
     ROSE_ASSERT(numericLabel >= 0);

     string numericLabelString = StringUtility::numberToString(numericLabel);

#if 0
  // This solution was going to use type checking, but is more complex so I have selected a simpler approach (initially for now).
     bool isFunctionCallArgumentForAlternativeReturnType = labelRefExp->isFunctionCallArgumentForAlternativeReturnType();
     if (isFunctionCallArgumentForAlternativeReturnType == true)
        {
          curprint("*");
        }
#else
  // DQ (2/2/2011): We can't do this since it will effect where lables are unparse in the OPEN statement (and likely other I/O statements).
  // After some email with Scott this is required to be handled via a special case so since in all other case the SgLabelRefExp shuld
  // have a IOStatement as a parent, we will look for where the parent is part of an expression list.  This could be improved later.
  // the best way to handle this would be to do the type checking, and I'm OK with that approach.  The backup plan is to embed names
  // into (what) that would trigger these to be treated as alternative return arguments.
  // We could also check if the enclosing statement is an IO statement.  So there are a number of options here.
  // curprint("*");

     SgStatement* tmp_statement = SageInterface::getEnclosingStatement(labelRefExp);
     ROSE_ASSERT(tmp_statement != NULL);

  // Check for either a SgIOStatement or a SgReturnStatement (not the special case we are looking for)
     if (isSgIOStatement(tmp_statement) == NULL && isSgReturnStmt(tmp_statement) == NULL)
        {
       // Output "*" if this is NOT a SgIOStatement (OK since I think that only functions in a function CALL statement can be used with alternative IO, is this true?
          curprint("*");

       // Instead of the numericLabelString, we output the index into the array of arguments with type == SgLabelSymbol taken from the function declaration's parameter list.
          curprint(numericLabelString);
        }
       else
        {
          if (isSgReturnStmt(tmp_statement) != NULL)
             {
            // This is a return statement, but we have to check if it is associated with a function that has SgTypeLabel parameters.
            // bool functionHasAlternativeArgumentParameters = true;

               size_t alternativeReturnValue = 0;

            // This is always a valid value (but not be correct)... just testing for now...
            // We have to correlate this SgLabelRefExp with the SgLabelSymbol of the correct parameter.

            // Note that this code is similar (copyied from) to R1236 c_action_return_stmt() in the ROSE Fortran support.
               SgFunctionDefinition* functionDefinition = SageInterface::getEnclosingFunctionDefinition(tmp_statement, /* includingSelf= */ true);
               ROSE_ASSERT(functionDefinition != NULL);

               SgFunctionDeclaration* functionDeclaration = functionDefinition->get_declaration();
               ROSE_ASSERT(functionDeclaration != NULL);

               SgInitializedNamePtrList & args = functionDeclaration->get_args();
               ROSE_ASSERT(alternativeReturnValue < args.size());

            // The Fortran world starts at one (not zero)!
               size_t counter = 1;

               for (size_t i = 0; i < args.size(); i++)
                  {
                     SgType* argumentType = args[i]->get_type();
                     SgTypeLabel* labelType = isSgTypeLabel(argumentType);
                     if (labelType != NULL)
                        {
                       // Search the argument list for a matching symbol.
                          SgSymbol* tmp_symbol = args[i]->get_symbol_from_symbol_table();
                          if (tmp_symbol == labelSymbol && alternativeReturnValue < 1)
                             {
                            // We have a match.
                               alternativeReturnValue = counter;
                             }

#if 0
                          if (counter == alternativeReturnValue)
                             {
                               argumentInitializedName = args[i];
                             }
#endif
                          counter++;
                        }
                  }

            // This is using Fortran world numbering (which starts a one, not zero).
            // curprint("1");
               ROSE_ASSERT(alternativeReturnValue > 0);
               ROSE_ASSERT(alternativeReturnValue <= args.size());
               curprint(StringUtility::numberToString(alternativeReturnValue));
             }
            else
             {
            // This is the most common case.
                curprint(numericLabelString);
             }
        }
#endif

   }

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::unparseExprRoot
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparseExprRoot(SgExpression* expr, SgUnparse_Info& info) 
   {
  // This IR nodes should not exist in a ROSE generated AST.
   }

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<function, intrinsic calls>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info)
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
  if (!opt.get_overload_opt() // FIXME: unary overloading
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
      curprint("CALL ");
    }

    // subroutine/function name
    unparseExpression(func_call->get_function(), info);

    // argument list
    SgUnparse_Info ninfo(info);
    curprint("(");
    if (func_call->get_args()) {
      SgExpressionPtrList& list = func_call->get_args()->get_expressions();
      SgExpressionPtrList::iterator arg = list.begin();
      while (arg != list.end()) {
//      SgConstructorInitializer* con_init = isSgConstructorInitializer(*arg);
        unparseExpression((*arg), ninfo);
        arg++;
        if (arg != list.end()) {
          curprint(",");
        }
      }
    }
    curprint(")");

#if 0
  }
#endif
}

#if 0
// DQ (11/24/2007): Removed this IR node.
void
FortranCodeGeneration_locatedNode::unparseIntrinsic(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgIntrinsicFn* intrn = isSgIntrinsicFn(expr);
     ROSE_ASSERT(intrn != NULL);
  
  // intrinsic name
     curprint(intrn->get_name().str());

  // argument list
  // unparseExprList(intrn->get_args(), info);
     UnparseLanguageIndependentConstructs::unparseExprList(intrn->get_args(), info);
   }
#endif

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<operators>
//----------------------------------------------------------------------------

#if 0
// DQ (8/14/2007): This function is defined in the langauge independent base class

void
FortranCodeGeneration_locatedNode::unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info) 
{
  SgUnaryOp* unary_op = isSgUnaryOp(expr);
  ROSE_ASSERT(unary_op != NULL);
  
  // A name-operator is an operator that is formed using the
  // .name. syntax (e.g., .gt.) as opposed to 'symbolic' characters
  // (e.g. >).
  const char* opstr = info.get_operator_name();
  bool nameOp = ((opstr[0] == '.') && (opstr[strlen(opstr)-1] == '.'));

  curprint(info.get_operator_name());
  if (nameOp) {
    curprint(" ");
  }
  info.set_nested_expression();
  unparseExpression(unary_op->get_operand(), info);
  info.unset_nested_expression();
}
#endif

#if 0
// DQ (8/14/2007): This function is defined in the langauge independent base class

void
FortranCodeGeneration_locatedNode::unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
     printDebugInfo("entering unparseBinaryExpr", true);

     SgBinaryOp* binary_op = isSgBinaryOp(expr);
     ROSE_ASSERT(binary_op != NULL);

     info.set_nested_expression();
  
     if (strcmp(info.get_operator_name(), ARRAY_IDX_OP) == 0)
        {
       // Special case: array indicing
          unparseExpression(binary_op->get_lhs_operand(), info);

          SgExprListExp* subscripts = isSgExprListExp(binary_op->get_rhs_operand());
          ROSE_ASSERT(subscripts);
          unparseExprList(subscripts, info);
        }
       else
        {
          unparseExpression(binary_op->get_lhs_operand(), info);
          curprint(" "); 
          curprint(info.get_operator_name());
          curprint(" ");
          unparseExpression(binary_op->get_rhs_operand(), info);
        }

     info.unset_nested_expression();
   }
#endif

void
FortranCodeGeneration_locatedNode::unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);
     ninfo.set_operator_name(op);
     unparseUnaryExpr(expr, ninfo);
   }

void
FortranCodeGeneration_locatedNode::unparseBinaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);
     ninfo.set_operator_name(op);
     unparseBinaryExpr(expr, ninfo);
   }

void 
FortranCodeGeneration_locatedNode::unparseAssnOp(SgExpression* expr, SgUnparse_Info& info) 
   {
     unparseBinaryOperator(expr, "=", info); 
   }


void 
FortranCodeGeneration_locatedNode::unparsePointerAssnOp(SgExpression* expr, SgUnparse_Info& info) 
   {
     unparseBinaryOperator(expr, "=>", info); 
   }


void 
FortranCodeGeneration_locatedNode::unparseNotOp(SgExpression* expr, SgUnparse_Info& info) 
   { 
     unparseUnaryOperator(expr, ".NOT.", info);
   }

void
FortranCodeGeneration_locatedNode::unparseAndOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran logical-and operator
     unparseBinaryOperator(expr, ".AND.", info);
   }

void
FortranCodeGeneration_locatedNode::unparseOrOp(SgExpression* expr, SgUnparse_Info& info)
   { 
  // Sage node corresponds to Fortran logical-or operator
     unparseBinaryOperator(expr, ".OR.", info);
   }


// DQ (8/6/2010): Output the logical operator when the operands are logical (SgBoolType)
// the type of the expression is not enough to test, we have to test the lhs and rhs type.
bool
outputLogicalOperator(SgExpression* expr)
   {
     bool outputLogicalOperator = false;

     SgBinaryOp* binaryOp = isSgBinaryOp(expr);
     if (binaryOp != NULL)
        {
          SgExpression* lhs = binaryOp->get_lhs_operand();
          SgExpression* rhs = binaryOp->get_rhs_operand();
          SgType* lhs_type = lhs->get_type();
          SgType* rhs_type = rhs->get_type();
          if (isSgTypeBool(lhs_type) != NULL)
             {
               if (isSgTypeBool(rhs_type) == NULL)
               {
                 printf ("Error: outputLogicalOperator(). Found a boolean lhs operand paired with a non-boolean rhs operand for SgExpression:%s\n",expr->class_name().c_str());
                 ROSE_ASSERT(isSgTypeBool(rhs_type) != NULL);
               }
               outputLogicalOperator = true;
             }
        }
       else
        {
          printf ("Error: this function only needs to handle binary operators. ");
          ROSE_ASSERT(false);
        }

     return outputLogicalOperator;
   }

void
FortranCodeGeneration_locatedNode::unparseEqOp(SgExpression* expr, SgUnparse_Info& info)
   { 
  // Sage node corresponds to Fortran equals operator
     ROSE_ASSERT(expr != NULL);
  // printf ("In FortranCodeGeneration_locatedNode::unparseEqOp(): type = %s ",expr->get_type()->class_name().c_str());

     
  // SgTypeBool* logicalType = isSgTypeBool(expr->get_type());
  // if (logicalType != NULL)
     if (outputLogicalOperator(expr) == true)
        {
          unparseBinaryOperator(expr, ".EQV.", info);
        }
       else
        {
          unparseBinaryOperator(expr, ".EQ.", info);
        }
   }

void
FortranCodeGeneration_locatedNode::unparseNeOp(SgExpression* expr, SgUnparse_Info& info)
   { 
     ROSE_ASSERT(expr != NULL);
  // Sage node corresponds to Fortran not-equals operator
  // unparseBinaryOperator(expr, "/=", info);

  // SgTypeBool* logicalType = isSgTypeBool(expr->get_type());
  // if (logicalType != NULL)
     if (outputLogicalOperator(expr) == true)
        {
          unparseBinaryOperator(expr, ".NEQV.", info);
        }
       else
        {
          unparseBinaryOperator(expr, ".NE.", info);
        }
   }

void
FortranCodeGeneration_locatedNode::unparseLtOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran less-than operator
  unparseBinaryOperator(expr, "<", info);
}

void
FortranCodeGeneration_locatedNode::unparseGtOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran greater-than operator
  unparseBinaryOperator(expr, ">", info);
}

void
FortranCodeGeneration_locatedNode::unparseLeOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran less-than-or-equals operator
  unparseBinaryOperator(expr, "<=", info);
}

void
FortranCodeGeneration_locatedNode::unparseGeOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran greater-than-or-equals operator
  unparseBinaryOperator(expr, ">=", info);
}


void
FortranCodeGeneration_locatedNode::unparseUnaryMinusOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran unary-minus operator
  unparseUnaryOperator(expr, "-", info);
}

void
FortranCodeGeneration_locatedNode::unparseUnaryAddOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran unary-plus operator
  unparseUnaryOperator(expr, "+", info);
}

void
FortranCodeGeneration_locatedNode::unparseAddOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran addition operator
  unparseBinaryOperator(expr, "+", info);
}

void
FortranCodeGeneration_locatedNode::unparseSubtOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran subtraction operator
  unparseBinaryOperator(expr, "-", info);
}

void
FortranCodeGeneration_locatedNode::unparseMultOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran multiplication operator
  unparseBinaryOperator(expr, "*", info);
}

void
FortranCodeGeneration_locatedNode::unparseDivOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran division operator
  unparseBinaryOperator(expr, "/", info);
}

void
FortranCodeGeneration_locatedNode::unparseIntDivOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran int-division operator
  unparseBinaryOperator(expr, "/", info);
}

void
FortranCodeGeneration_locatedNode::unparseExpOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran exponentiation operator
  unparseBinaryOperator(expr, "**", info);
}


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<FIXME> (Intrinsics mapped to Sage nodes)
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseModOp(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran mod intrinsic (remainder function)
  unparseBinaryOperator(expr, "MOD", info);
}

void
FortranCodeGeneration_locatedNode::unparseBitXOrOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran ieor intrinsic
  unparseBinaryOperator(expr, "IEOR", info);
}

void
FortranCodeGeneration_locatedNode::unparseBitAndOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran iand intrinsic
  unparseBinaryOperator(expr, "IAND", info);
}

void
FortranCodeGeneration_locatedNode::unparseBitOrOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran ior intrinsic
  unparseBinaryOperator(expr, "IOR", info);
}

void
FortranCodeGeneration_locatedNode::unparseLShiftOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran ishft(x,+) intrinsic
  // FIXME:eraxxon: need special case in unparseBinaryExpr
  unparseBinaryOperator(expr, "ISHFT(x,+)", info);
}

void
FortranCodeGeneration_locatedNode::unparseRShiftOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran ishft(x,-) intrinsic
  // FIXME:eraxxon: need special case in unparseBinaryExpr
  unparseBinaryOperator(expr, "ISHFT(x,-)", info);
}

void 
FortranCodeGeneration_locatedNode::unparseBitCompOp(SgExpression* expr, SgUnparse_Info& info) 
{ 
  // Sage node corresponds to Fortran not intrinsic
  unparseUnaryOperator(expr, "NOT", info);
}

void
FortranCodeGeneration_locatedNode::unparseConcatenationOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node corresponds to Fortran addition operator
  unparseBinaryOperator(expr, "//", info);
}

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<operators, other>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
   { 
  // Sage node corresponds to Fortran array indicing
  // unparseBinaryOperator(expr, ARRAY_IDX_OP, info);
  // unparseBinaryOperator(expr, "", info);
     SgPntrArrRefExp* arrayRefExp = isSgPntrArrRefExp(expr);

     unparseExpression(arrayRefExp->get_lhs_operand(),info);

     SgUnparse_Info ninfo(info);
     ninfo.set_SkipParen();

     curprint("(");
     unparseExpression(arrayRefExp->get_rhs_operand(),ninfo);
     curprint(")");
   }

void
FortranCodeGeneration_locatedNode::unparseRecRef(SgExpression* expr, SgUnparse_Info& info)
{ 

 // FMZ (7/16/2009): 
 //     cannot treat the operator "%" in same way with C/C++ modulo operator
 //     for example: X%(Y(1,2)) is not legal fortran expression
#if 0
  // Sage node corresponds to Fortran record selector
  unparseBinaryOperator(expr, "%",info);
#else
     SgDotExp* dotExpr = isSgDotExp(expr);
     unparseExpression(dotExpr->get_lhs_operand(),info);
     curprint("%");
     SgPntrArrRefExp* arrayRefExp=isSgPntrArrRefExp(dotExpr->get_rhs_operand());
     if (arrayRefExp != NULL) {
         unparseExpression(arrayRefExp->get_lhs_operand(),info);
         curprint("(");
         unparseExpression(arrayRefExp->get_rhs_operand(),info);
         curprint(")");
      } else 
         unparseExpression(dotExpr->get_rhs_operand(),info);
#endif
}

void
FortranCodeGeneration_locatedNode::unparseCastOp(SgExpression* expr, SgUnparse_Info& info)
   {

  // DQ (8/16/2007): Allow SgCast operators to work since we wnat to test the unparser using C code
  // and we will later want to add cast operators to the Fortran AST to explicitly mark implicit casts
  // in fortran (marked as compiler generated).
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
  // ROSE_ASSERT(false);

     SgCastExp* cast_op = isSgCastExp(expr);
     ROSE_ASSERT(cast_op != NULL);

     SgUnparse_Info ninfo(info);
     ninfo.unset_PrintName();

  // Never unparse the declaration from within a cast expression
     ninfo.set_SkipDefinition();
     ninfo.unset_SkipBaseType();

     switch(cast_op->cast_type())
        {
          case SgCastExp::e_unknown:
             {
               printf ("SgCastExp::e_unknown found \n");
               ROSE_ASSERT(false);
               break; 
             }

          case SgCastExp::e_default:
             {
               printf ("SgCastExp::e_default found \n");
               ROSE_ASSERT(false);
               break; 
             }

          case SgCastExp::e_dynamic_cast:
             {
            // dynamic_cast <P *> (expr)
               printf ("SgCastExp::e_dynamic_cast found (Now defined in Fortran) \n");
               ROSE_ASSERT(false);
               break;
             }
          case SgCastExp::e_static_cast:
             {
            // static_cast <P *> (expr)
               printf ("SgCastExp::e_static_cast found (Now defined in Fortran) \n");
               ROSE_ASSERT(false);
               break;
             }
          case SgCastExp::e_reinterpret_cast:
             {
            // reinterpret_cast <P *> (expr)
               printf ("SgCastExp::e_reinterpret_cast found (Now defined in Fortran) \n");
               ROSE_ASSERT(false);
               break;
             }

          case SgCastExp::e_C_style_cast:
          case SgCastExp::e_const_cast:
             {
            // (P *) expr
            // check if the expression that we are casting is not a string
            // if (cast_op->get_operand()->variant() != STRING_VAL)
               if (cast_op->get_operand()->variantT() != V_SgStringVal)
                  {
                 // it is not a string, so we always cast
                    curprint("(");
                 // unp->u_type->unparseType(cast_op->get_type(), ninfo);
                    unp->u_fortran_type->unparseType(cast_op->get_type(), ninfo);
                    curprint(")");
                  }
               break;
             }

     // DQ (3/28/2017): Adding this here to eliminate warning from Clang.
          case SgCastExp::e_safe_cast:
             {
               printf ("SgCastExp::e_safe_cast found (Not defined in Fortran) \n");
               ROSE_ASSERT(false);
               break;
             }

     // DQ (3/28/2017): Adding this here to eliminate warning from Clang.
        case SgCastExp::e_last_cast:
             {
               printf ("SgCastExp::e_last_cast is an error (end of enum list) \n");
               ROSE_ASSERT(false);
               break;
             }

     // DQ (3/28/2017): Adding this here to eliminate warning from Clang.
          default:
             {
            // DQ (3/28/2017): There should be no other cases, but make the default an error (just in case).
               printf ("SgCastExp::e_last_cast is an error (end of enum list) \n");
               ROSE_ASSERT(false);
               break;
             }
        }

     unparseExpression(cast_op->get_operand(), info); 
   }


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<FIXME>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseNewOp(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ASSERT(false);

#if 0
  // FIXME:eraxxon
  SgNewExp* new_op = isSgNewExp(expr);
  ROSE_ASSERT(new_op != NULL);

  ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparseNewOp");

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
#endif
   }

void
FortranCodeGeneration_locatedNode::unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ASSERT(false);

#if 0
  // FIXME:eraxxon
  SgDeleteExp* delete_op = isSgDeleteExp(expr);
  ROSE_ASSERT(delete_op != NULL);

  ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparseDeleteOp");
  
  cur << "delete ";
  SgUnparse_Info ninfo(info);
  unparseExpression(delete_op->get_variable(), ninfo);
#endif
   }


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<FIXME>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparsePointStOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node has no Fortran correspondence (unless semantics are twisted)
  ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparsePointStOp");
  unparseBinaryOperator(expr, "->", info);
}

void 
FortranCodeGeneration_locatedNode::unparseDerefOp(SgExpression* expr, SgUnparse_Info& info)
{ 
  // Sage node has no explicit Fortran correspondence
  // FIXME:eraxxon:
  // unparseUnaryOperator(expr, "FIXME*", info);
}

void
FortranCodeGeneration_locatedNode::unparseAddrOp(SgExpression* expr, SgUnparse_Info& info) 
{ 
  // Sage node has no explicit Fortran correspondence
  // FIXME:eraxxon:
  // unparseUnaryOperator(expr, "FIXME&", info);
}

void 
FortranCodeGeneration_locatedNode::unparseTypeRef(SgExpression* expr, SgUnparse_Info& info) 
   {
  // FIXME:eraxxon
     SgRefExp* type_ref = isSgRefExp(expr);
     ROSE_ASSERT(type_ref != NULL);

     SgUnparse_Info ninfo(info);
     ninfo.unset_PrintName();
  
  // unp->u_type->unparseType(type_ref->get_type_name(), ninfo);
     unp->u_fortran_type->unparseType(type_ref->get_type_name(), ninfo);
   }

void 
FortranCodeGeneration_locatedNode::unparseSubscriptExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgSubscriptExpression* sub_expr = isSgSubscriptExpression(expr);
     ROSE_ASSERT(sub_expr != NULL);

  // printf ("In FortranCodeGeneration_locatedNode::unparseSubscriptExpr(%p) \n",expr);

     ROSE_ASSERT(sub_expr->get_lowerBound() != NULL);
     ROSE_ASSERT(sub_expr->get_upperBound() != NULL);
     ROSE_ASSERT(sub_expr->get_stride() != NULL);

     if (isSgNullExpression(sub_expr->get_lowerBound()) == NULL)
        {
          unparseExpression(sub_expr->get_lowerBound(), info);
          curprint(":");
        }
       else
        {
          curprint(":");
        }

  // unparseExpression(sub_expr->get_upperBound(), info);
     if (isSgNullExpression(sub_expr->get_upperBound()) == NULL)
        {
       // curprint(":");
          unparseExpression(sub_expr->get_upperBound(), info);
        }

#if 0
     if (stride)
        {
          curprint(":");
          unparseExpression(stride, info);
        }
#else
     SgExpression* strideExpression = sub_expr->get_stride();
     ROSE_ASSERT(strideExpression != NULL);
     ROSE_ASSERT(isSgNullExpression(strideExpression) == NULL);

  // DQ (11/18/2007): If this is a SgNullExpression, then ignore the second colon
  // if (isSgNullExpression(strideExpression) == NULL)
  //    {
     SgIntVal* integerValue = isSgIntVal(strideExpression);

  // See if this is the default value for the stride (unit stride) and skip the output in this case.
     bool defaultValue = ( (integerValue != NULL) && (integerValue->get_value() == 1) ) ? true : false;
     if (defaultValue == false)
        {
          curprint(":");
          ROSE_ASSERT(sub_expr->get_stride() != NULL);
          unparseExpression(sub_expr->get_stride(), info);
        }
  //    }
#endif
   }


void 
FortranCodeGeneration_locatedNode::unparseColonShapeExp(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgColonShapeExp* colon = isSgColonShapeExp(expr);
     ROSE_ASSERT(colon != NULL);

     curprint(":");
   }

void 
FortranCodeGeneration_locatedNode::unparseAsteriskShapeExp(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgAsteriskShapeExp* sub_ast = isSgAsteriskShapeExp(expr);
     ROSE_ASSERT(sub_ast != NULL);

     curprint("*");
   }

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<initializers>
//----------------------------------------------------------------------------

#if 0
// DQ (4/28/2008): I don't think this is used!
void 
FortranCodeGeneration_locatedNode::unparseExprInit(SgExpression* expr, SgUnparse_Info& info) 
{
}
#endif

void
FortranCodeGeneration_locatedNode::unparseInitializerList(SgExpression* expr, SgUnparse_Info& info)
   {
     ROSE_ASSERT(expr);
     SgExprListExp* expr_list = isSgExprListExp(expr);

     info.set_nested_expression();

  // bool paren = false;
     bool paren = true;
     if (paren)
        {
       // DQ (12/9/2010): This is a bug in test2010_136.f90.
       // curprint("(");
          curprint("(/");
        }

  // DQ (12/9/2010): This is a bug in test2010_136.f90.
  // curprint("/");

     SgExpressionPtrList::iterator it = expr_list->get_expressions().begin();
     while (it != expr_list->get_expressions().end())
        {
       // printf ("In unparseInitializerList(): *it = %p = %s \n",*it,(*it)->class_name().c_str());

          unparseExpression(*it, info);
          it++;
          if (it != expr_list->get_expressions().end())
             {
               curprint(","); 
             }
        }

  // DQ (12/9/2010): This is a bug in test2010_136.f90.
  // curprint("/");

     if (paren)
        {
       // DQ (12/9/2010): This is a bug in test2010_136.f90.
       // curprint(")");
          curprint("/)");
        }

     info.unset_nested_expression();
   }

void 
FortranCodeGeneration_locatedNode::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (4/28/2008): It might be that we should use these soom, but for now I am not using them.
  // printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
  // ROSE_ASSERT(false);

     SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
     ROSE_ASSERT(aggr_init != NULL);

  // printf ("In unparseAggrInit(): aggr_init->get_type() = %p = %s \n",aggr_init->get_type(),(aggr_init->get_type() != NULL) ? aggr_init->get_type()->class_name().c_str() : "NULL");

#if 0
     SgUnparse_Info ninfo(info);
     curprint("{");
  
     SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();
     SgExpressionPtrList::iterator p = list.begin();
     while (p != list.end())
        {
          unparseExpression((*p), ninfo);
          p++;
          if (p != list.end())
             {
               curprint(", ");  
             }
        }

     curprint("}");
#else

  // DQ (12/9/2010): This is a bug in test2010_136.f90.
  // curprint("(");

  // info.set_nested_expression();
  // unparseExpression(aggr_init->get_initializers(), info);
     unparseInitializerList(aggr_init->get_initializers(), info);
  // info.unset_nested_expression();

  // DQ (12/9/2010): This is a bug in test2010_136.f90.
  // curprint(")");
#endif
   }

void
FortranCodeGeneration_locatedNode::unparseConInit(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (5/3/2008): This is now used for all initialization of user-defined types.

  // DQ (1/25/2011): This is not used within Fortran 90 code.
  // printf ("Case SgConstructorInitializer not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
  // ROSE_ASSERT(false);

     SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(expr);
     ROSE_ASSERT(constructorInitializer != NULL);

     SgType* type = constructorInitializer->get_expression_type();
     SgClassType* classType = isSgClassType(type);
     ROSE_ASSERT(classType != NULL);

     string className = classType->get_name().getString();
     curprint(className);

     curprint("(");

     ROSE_ASSERT(constructorInitializer->get_args() != NULL);
  // unparseInitializerList(constructorInitializer->get_args(), info);
     unparseExpression(constructorInitializer->get_args(), info);

     curprint(")");

   }

void
FortranCodeGeneration_locatedNode::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (4/28/2008): This is used for simple initializers and we use the SgAggregateInitializer for structures!
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ROSE_ASSERT(assn_init != NULL);

#if 0
     SgExprListExp* exprListExp = isSgExprListExp(assn_init->get_operand());
  // ROSE_ASSERT(exprListExp != NULL);

     int numberOfInitializers = exprListExp != NULL ? exprListExp->get_expressions().size() : 1;
     if (numberOfInitializers > 1)
        {
          curprint("(/");
        }

     unparseExpression(assn_init->get_operand(), info);

     if (numberOfInitializers > 1)
        {
          curprint("/)");
        }
#else
// DQ (4/28/2008): Now that we support the SgAggregateInitializer, this case is much simpler.
     unparseExpression(assn_init->get_operand(), info);
#endif
   }

#if 0
// DQ (10/4/2008): I no longer agree that these are expressions, they are just parts of the SgUseStatment.

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<rename/only lists>
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparseUseRename(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran rename expression ('use a => a_old')
  SgUseRenameExpression* rename_expr = isSgUseRenameExpression(expr);
  ROSE_ASSERT(rename_expr != NULL);

  SgExpression* oldnm = rename_expr->get_oldname();
  SgExpression* newnm = rename_expr->get_newname();
  
  unparseExpression(newnm, info);
  curprint(" => ");
  unparseExpression(oldnm, info);
}
#endif

#if 0
// DQ (10/4/2008): I no longer agree that these are expressions, they are just parts of the SgUseStatment.

void 
FortranCodeGeneration_locatedNode::unparseUseOnly(SgExpression* expr, SgUnparse_Info& info)
   {
  // Sage node corresponds to a Fortran 'use, only' expression
     SgUseOnlyExpression* only_expr = isSgUseOnlyExpression(expr);
     ROSE_ASSERT(only_expr != NULL);

     SgExprListExp* lst = only_expr->get_access_list();
     curprint(", ONLY: ");
  // unparseExprList(lst, info, false /*paren*/);
  // unparseExprList(lst, info, false /*paren*/);
     unparseExprList(lst, info);
   }
#endif

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<IO>
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparseIOItemExpr(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node represents Fortran IO
  SgIOItemExpression* ioitem_expr = isSgIOItemExpression(expr);
  ROSE_ASSERT(ioitem_expr != NULL);

  SgExpression* ioitem = ioitem_expr->get_io_item();

  if (isSgExprListExp(ioitem)) {
 // unparseExprList(isSgExprListExp(ioitem), info, false /*paren*/);
    unparseExprList(isSgExprListExp(ioitem), info);
  }
  else {
    unparseExpression(ioitem, info);
  }
}

void 
FortranCodeGeneration_locatedNode::unparseImpliedDo(SgExpression* expr, SgUnparse_Info& info) 
   {
  // Sage node corresponds to a Fortran implied do
     SgImpliedDo* ioitem_expr = isSgImpliedDo(expr);
     ROSE_ASSERT(ioitem_expr != NULL);

     SgExprListExp* object_list = ioitem_expr->get_object_list();

  // DQ (9/22/2010): test2010_49.f90 demonstrates that this can be an expression in terms of an 
  // index variable, so this is more general that first exptected and as a result has changed 
  // the IR and the name of the datamember.
  // SgVarRefExp* varRef = ioitem_expr->get_do_var();
  // SgExpression* indexExpression = ioitem_expr->get_do_var_exp();
  // SgExpression* lb              = ioitem_expr->get_first_val();
     SgExpression* lb   = ioitem_expr->get_do_var_initialization();
     SgExpression* ub   = ioitem_expr->get_last_val();
     SgExpression* step = ioitem_expr->get_increment();

#if 0
     if (indexExpression == NULL)
        {
       // OFP dos not yet provide a loop index variable for the case of an implied do loop 
       // in an initializer to a variable declaration or a data statement (only for an IO 
       // statement).  So we have to build one.  I have elected to do so by looking for it
       // in the SgPntrArrRefExp objects found in the object_list, however this is unsafe.
          printf ("Warning, implied do loop index variable not found (unavailable in OFP for initilizers and data statements) lookinf for one to use in the object_list \n");

          SgExpressionPtrList & expressionList = object_list->get_expressions();
          SgExpressionPtrList::iterator i = expressionList.begin();

          SgVarRefExp* indexVariable = NULL;
          while ( indexVariable == NULL && i != expressionList.end() )
             {
               SgPntrArrRefExp* pointerArrayRef = isSgPntrArrRefExp(*i);
               if ( pointerArrayRef != NULL)
                  {
                    SgExprListExp* indexListExp = isSgExprListExp(pointerArrayRef->get_rhs_operand());
                    ROSE_ASSERT(indexListExp != NULL);

                    SgExpressionPtrList & indexList = indexListExp->get_expressions();
                    SgExpressionPtrList::iterator j = indexList.begin();

                 // Note that if the size is greater then one then there will likely be more than on index 
                 // variable and we can decide which one to use. This is why the OFP needs to provide the 
                 // loop index variable.
                    ROSE_ASSERT(indexList.size() == 1);
                    
                    while ( indexVariable == NULL && j != indexList.end() )
                       {
                         printf ("In unparseImpliedDo(): (building a index variable reference) *j = %p = %s \n",*j,(*j)->class_name().c_str());
                         indexVariable = isSgVarRefExp(*j);
                         j++;
                       }

                    ROSE_ASSERT(indexVariable != NULL);
                  }
                 else
                  {
                    printf ("Searching for implied do loop variable, but object_list contains non SgPntrArrRefExp entry i = %s \n",(*i)->class_name().c_str());
                  }

               i++;
             }

          ROSE_ASSERT(indexVariable != NULL);
          indexExpression = indexVariable;
        }

     ROSE_ASSERT(indexExpression != NULL);
#endif

     ROSE_ASSERT(lb != NULL);
     ROSE_ASSERT(ub != NULL);
     ROSE_ASSERT(step != NULL);

     curprint("(");
     if (object_list != NULL)
        {
       // unparseExprList(object_list, info, false /*paren*/);
          unparseExprList(object_list, info);

       // DQ (9/26/2010): Handle cases where the list is empty (see test2010_49.f90)
          if (object_list->empty() == false)
               curprint(",");
        }
  // unparseExpression(indexExpression, info);
  // curprint(" = ");
#if 0
     unparseExpression(lb, info);
#else
  // DQ (10/9/2010): This is an iterative step in the correct handling of implied do expressions.
  // Unparse the lhs and rhs separately to about extra "()".  A little later this will be a 
  // variable declaration, but we will not be able to unparse it as such since the type 
  // (integer) is not explicitly represented.
     ROSE_ASSERT(lb != NULL);
     SgBinaryOp* binaryExpression = isSgBinaryOp(lb);
     if (binaryExpression == NULL)
        {
       // This is a temporary fix to support some initial testing.
          printf ("lb = %p = %s \n",lb,lb->class_name().c_str());
          unparseExpression(lb, info);
          curprint(" = 1");
        }
       else
        {
     ROSE_ASSERT(binaryExpression != NULL);
     ROSE_ASSERT(binaryExpression->get_lhs_operand() != NULL);
     unparseExpression(binaryExpression->get_lhs_operand(), info);
     curprint(" = ");
     ROSE_ASSERT(binaryExpression->get_rhs_operand() != NULL);
     unparseExpression(binaryExpression->get_rhs_operand(), info);
        }
#endif

     curprint(", ");
     unparseExpression(ub, info);

  // If there is an increment, and it is not the SgNullExpression, then unparse it.
     if (step != NULL && isSgNullExpression(step) == NULL)
        {
          curprint(", ");
          unparseExpression(step, info);
        }
     curprint(")");
   }


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<symbol references>
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
   {
  // Sage node corresponds to a Fortran variable reference
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ROSE_ASSERT(var_ref != NULL);
     ROSE_ASSERT(var_ref->get_symbol() != NULL);
  
     SgInitializedName* decl = var_ref->get_symbol()->get_declaration();
     ROSE_ASSERT (decl != NULL);
     SgVariableDeclaration* vd = isSgVariableDeclaration(decl->get_declaration());

     if (false /*vd != NULL*/)
        {
       // FIXME:eraxxon: how to handle renamed module vars?
          SgClassDefinition* cdef = isSgClassDefinition(vd->get_parent());
          if (cdef != NULL)
             {
            // TV (09/27/2018): it does not work because "cdecl" is a keyword in VC++. Replaced it by xdecl and commented out the guard
//#ifndef _MSC_VER
            // tps (02/02/2010): Does not work for some reason under Windows: SgClassDeclaration unknown.
               SgClassDeclaration* xdecl = isSgClassDeclaration(cdef->get_declaration());
               if (xdecl != NULL && vd->get_declarationModifier().get_storageModifier().isStatic()) 
                  {
                    curprint(xdecl->get_qualified_name().str());
                    curprint("::");
                  }
//#endif
             }
        }
  
     curprint(var_ref->get_symbol()->get_name().str());
   }

void 
FortranCodeGeneration_locatedNode::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran function reference
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  ROSE_ASSERT(func_ref != NULL);
  string func_name = func_ref->get_symbol()->get_name().str();
  curprint(func_name);
}

void
FortranCodeGeneration_locatedNode::unparseMFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ASSERT(false);

#if 0
  // Sage node has no Fortran correspondence (unless semantics are twisted)
  // FIXME:eraxxon

  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  ROSE_ASSERT(mfunc_ref != NULL);
  
  SgMemberFunctionDeclaration* mfd  = mfunc_ref->get_symbol_i()->get_declaration();

  // DQ (8/14/2007): Fixed reference to the SgClassDefinition by a SgScopeStatement
  // SgClassDefinition*           cdef = mfd->get_scope();
     SgClassDefinition*           cdef = isSgClassDefinition(mfd->get_scope());

  ROSE_ASSERT (cdef != NULL);
  SgClassDeclaration* cdecl = cdef->get_declaration();

  // qualified name is always outputed except when the p_need_qualifier is
  // set to 0 (when the naming class is identical to the selection class, and
  // and when we aren't suppressing the virtual function mechanism).  
  ROSE_ASSERT(cdecl != NULL);
  ROSE_ASSERT(cdecl->get_parent() != NULL);

     bool print_colons = false;
     if (mfunc_ref->get_need_qualifier())
        {
          curprint(cdecl->get_qualified_name().str()); 
          curprint("::"); 
          print_colons = true;
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
      curprint(func_name);
    }
  }
#endif
   }

void 
FortranCodeGeneration_locatedNode::unparseClassRef(SgExpression* expr, SgUnparse_Info& info) 
   {
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ASSERT(false);

#if 0
  // Sage node has no Fortran correspondence (unless semantics are twisted)
  // FIXME:eraxxon
     SgClassNameRefExp* classname_ref = isSgClassNameRefExp(expr);
     ROSE_ASSERT(classname_ref != NULL);
     curprint(classname_ref->get_symbol()->get_declaration()->get_name().str());
#endif
   }


void
FortranCodeGeneration_locatedNode::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (9/16/2007): Note that string unparsing is language dependent so this is not handled by the language independent base class.

  // Sage node corresponds to a Fortran string constant
     SgStringVal* str_val = isSgStringVal(expr);
     ROSE_ASSERT(str_val != NULL);

  // DQ (1/30/2011): It is OK to have an empty string value (see test2010_156.f90).
  // ROSE_ASSERT(str_val->get_value().empty() == false);      

  // String values in fortran can use either double or single quotes ("..." or '...') to be used.
     string str;
  // printf ("In unparseStringVal(): str_val->get_usesSingleQuotes() = %s \n",str_val->get_usesSingleQuotes() ? "true" : "false");

  // We add the quotes back in since they are not saved with the string value (so that C/C++ and Fortran can be handled similarly).
     if (str_val->get_usesSingleQuotes() == true)
        {
       // str = string("/* single quotes */ \'") + str_val->get_value() + string("\'");
          str = string("\'") + str_val->get_value() + string("\'");
        }
       else
        {
       // str = string("/* double quotes */ \"") + str_val->get_value() + string("\"");
          str = string("\"") + str_val->get_value() + string("\"");
        }
     curprint(str);
   }


//----------------------------------------------------------------------------
//  void FortranCodeGeneration_locatedNode::<constants>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseBoolVal(SgExpression* expr, SgUnparse_Info& info)
   {
  // Sage node corresponds to a Fortran logical constant
     SgBoolValExp* bool_val = isSgBoolValExp(expr);
     ROSE_ASSERT(bool_val != NULL);

     if (bool_val->get_value() == true)
        {
          curprint(".TRUE.");
        }
       else
        {
          curprint(".FALSE.");
        }
   }

#if 0
// DQ (8/14/2007): Use the base class implementation to support unparing of values, we can
// provide Fortran specific variations if required later.

void 
FortranCodeGeneration_locatedNode::unparseCharVal(SgExpression* expr, SgUnparse_Info& info) 
   {
  // Sage node corresponds to a Fortran character constant
     SgCharVal* char_val = isSgCharVal(expr);
     ROSE_ASSERT(char_val != NULL);
  // curprint(char_val->get_value());
  // const string value = char_val->get_value();
     char value[2];
     value[0] = char_val->get_value();
     value[1] = '\0';
     curprint(value);
   }

void 
FortranCodeGeneration_locatedNode::unparseUCharVal(SgExpression* expr, SgUnparse_Info& info) 
   {
  // Sage node corresponds to a Fortran integer constant
     SgUnsignedCharVal* uchar_val = isSgUnsignedCharVal(expr);
     ROSE_ASSERT(uchar_val != NULL);
  // curprint(uchar_val->get_value());
  // string value = char_val->get_value();
     char value[2];
     value[0] = uchar_val->get_value();
     value[1] = '\0';
     curprint(value);
   }

void 
FortranCodeGeneration_locatedNode::unparseWCharVal(SgExpression* expr, SgUnparse_Info& info) 
   {
     printf ("Case not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ASSERT(false);

#if 0
  // Sage node has no Fortran correspondence
     SgWcharVal* wchar_val = isSgWcharVal(expr);
     ROSE_ASSERT(wchar_val != NULL);
     ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparseWCharVal");
     curprint((int) wchar_val->get_value());
#endif
   }

void 
FortranCodeGeneration_locatedNode::unparseShortVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgShortVal* short_val = isSgShortVal(expr);
  ROSE_ASSERT(short_val != NULL);
  curprint(short_val->get_value());
}

void 
FortranCodeGeneration_locatedNode::unparseUShortVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedShortVal* ushort_val = isSgUnsignedShortVal(expr);
  ROSE_ASSERT(ushort_val != NULL);
  curprint(ushort_val->get_value());
}

void
FortranCodeGeneration_locatedNode::unparseEnumVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node has no Fortran correspondence
  SgEnumVal* enum_val = isSgEnumVal(expr);
  ROSE_ASSERT(enum_val != NULL);
  
  // FIXME:eraxxon [could be Fortran parameter]
  ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparseEnumVal");

  if (info.inEnumDecl()) {
    cur << enum_val->get_value();
  }
  else {
    SgClassDefinition* classdefn = NULL;
    if (enum_val->get_declaration() && (classdefn = isSgClassDefinition(enum_val->get_declaration()->get_parent()))) {
      cur << classdefn->get_qualified_name().str() <<  "::";
    }
    
    // ROSE_ASSERT (enum_val->get_name().str() != NULL);
    cur << enum_val->get_name().str();
  }
}

void 
FortranCodeGeneration_locatedNode::unparseIntVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran integer constant
  SgIntVal* int_val = isSgIntVal(expr);
  ROSE_ASSERT(int_val != NULL);
  cur << int_val->get_value();
}

void
FortranCodeGeneration_locatedNode::unparseUIntVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedIntVal* uint_val = isSgUnsignedIntVal(expr);
  ROSE_ASSERT(uint_val != NULL);
  cur << uint_val->get_value();
}

void 
FortranCodeGeneration_locatedNode::unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgLongIntVal* longint_val = isSgLongIntVal(expr);
  ROSE_ASSERT(longint_val != NULL);
  cur << longint_val->get_value();
}

void 
FortranCodeGeneration_locatedNode::unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedLongVal* ulongint_val = isSgUnsignedLongVal(expr);
  ROSE_ASSERT(ulongint_val != NULL);
  cur << ulongint_val->get_value();
}

void 
FortranCodeGeneration_locatedNode::unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgLongLongIntVal* longlongint_val = isSgLongLongIntVal(expr);
  ROSE_ASSERT(longlongint_val != NULL);
  cur << longlongint_val->get_value();
}

void 
FortranCodeGeneration_locatedNode::unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran integer constant
  SgUnsignedLongLongIntVal* ulonglongint_val = isSgUnsignedLongLongIntVal(expr);
  ROSE_ASSERT(ulonglongint_val != NULL);
  cur << ulonglongint_val->get_value();
}

void 
FortranCodeGeneration_locatedNode::unparseFLoatVal(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node corresponds to a Fortran real constant
  SgFloatVal* float_val = isSgFloatVal(expr);
  ROSE_ASSERT(float_val != NULL);
  cur << float_val->get_value();
}

void
FortranCodeGeneration_locatedNode::unparseDblVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran real constant
  SgDoubleVal* dbl_val = isSgDoubleVal(expr);
  ROSE_ASSERT(dbl_val != NULL);
  cur << dbl_val->get_value(); 
}

void 
FortranCodeGeneration_locatedNode::unparseLongDblVal(SgExpression* expr, SgUnparse_Info& info)
{
  // Sage node corresponds to a Fortran real constant
  SgLongDoubleVal* longdbl_val = isSgLongDoubleVal(expr);
  ROSE_ASSERT(longdbl_val != NULL);
  cur << longdbl_val->get_value();
}

// DQ (8/14/2007): Use the base class implementation to support unparing of values, we can
// provide Fortran specific variations if required later.
#endif

//----------------------------------------------------------------------------
//  helpers
//----------------------------------------------------------------------------

// bool FortranCodeGeneration_locatedNode::unparseExprList(SgExprListExp* expr, SgUnparse_Info& info, bool paren)
// void FortranCodeGeneration_locatedNode::unparseExprList(SgExpression* expr, SgUnparse_Info& info, bool paren)
void
FortranCodeGeneration_locatedNode::unparseExprList(SgExpression* expr, SgUnparse_Info& info)
{
  ROSE_ASSERT(expr);
  SgExprListExp* expr_list = isSgExprListExp(expr);

// DQ (3/28/2017): Removed this from the function parameter list so that it would match the base class virtual function.
// This is part of removing warnings from ROSE specific to Clang.
  bool paren = false;

  info.set_nested_expression();

  if (paren) {
    curprint("(");
  }
  SgExpressionPtrList::iterator it = expr_list->get_expressions().begin();
  while (it != expr_list->get_expressions().end()) {
    unparseExpression(*it, info);
    it++;
    if (it != expr_list->get_expressions().end()) {
      curprint(","); 
    }
  }
  if (paren) {
    curprint(")");
  }

  info.unset_nested_expression();
}


bool
FortranCodeGeneration_locatedNode::isSubroutineCall(SgFunctionCallExp* fcall)
   {
  // Returns true if this is a subroutine call (as opposed to a function call)

#if 0
     SgFunctionRefExp* funcref = isSgFunctionRefExp(fcall->get_function());
     ROSE_ASSERT(funcref); 

  // SgFunctionType* ftype = funcref->get_function_type();
     SgFunctionType* ftype = isSgFunctionType(funcref->get_type());
     ROSE_ASSERT(ftype != NULL);
     SgType* rtype = ftype->get_return_type();

  // Note: 'rtype' should be equivalent to 'fcall->get_type()'

     switch (rtype->variantT())
        {
          case V_SgTypeVoid:
          case V_SgTypeGlobalVoid:
               return true;

          default:
               return false;
        }
#else
  // Note that the function declaration is explicitly marked and I think this is better than
  // getting the return type.

     SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(fcall->get_function());
     ROSE_ASSERT(functionRefExp != NULL);


     SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
     ROSE_ASSERT(functionSymbol != NULL);
     //cout << "function name is : " << functionSymbol->get_name().str()<<endl;

     SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
     ROSE_ASSERT(functionDeclaration != NULL);

     //printf ("functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());

     SgProcedureHeaderStatement* procedureHeaderStatement = isSgProcedureHeaderStatement(functionDeclaration);
     ROSE_ASSERT(procedureHeaderStatement != NULL);

     return (procedureHeaderStatement->get_subprogram_kind() == SgProcedureHeaderStatement::e_subroutine_subprogram_kind);
#endif
   }

void
FortranCodeGeneration_locatedNode::unparseUnknownArrayOrFunctionReference(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnknownArrayOrFunctionReference* assumeArrayReference = isSgUnknownArrayOrFunctionReference(expr);

  // curprint("\n    ! SgUnknownArrayOrFunctionReference (post-processing required to resolve reference) \n      ");
     curprint("\n    ! SgUnknownArrayOrFunctionReference (post-processing required to resolve reference): reference name = ");
  // For debugging support output a simple variable reference.
     SgExpression* variableReference = assumeArrayReference->get_named_reference();
     unparseVarRef(variableReference,info);

  // Output 1 new line so that new statements will appear on their own line after the SgProgramHeaderStatement declaration.
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseUserDefinedUnaryOp  (SgExpression* expr, SgUnparse_Info& info)
   {
     SgUserDefinedUnaryOp* userDefinedUnaryOp = isSgUserDefinedUnaryOp(expr);

     unparseUnaryOperator(expr, userDefinedUnaryOp->get_operator_name().str(), info);
   }


void
FortranCodeGeneration_locatedNode::unparseUserDefinedBinaryOp (SgExpression* expr, SgUnparse_Info& info)
   {
     SgUserDefinedBinaryOp* userDefinedBinaryOp = isSgUserDefinedBinaryOp(expr);

     unparseBinaryOperator(expr, userDefinedBinaryOp->get_operator_name().str(), info);
   }


void FortranCodeGeneration_locatedNode::unparseCoArrayExpression (SgExpression * expr, SgUnparse_Info & info)
{
    // get subparts
    SgCAFCoExpression * coExpr     = isSgCAFCoExpression(expr);
    SgExpression *      referData  = coExpr->get_referData();
    SgExpression *      teamRank   = coExpr->get_teamRank();
    SgVarRefExp *       teamVarRef = coExpr->get_teamId();

    // print the data reference
    ROSE_ASSERT(referData);
    unparseLanguageSpecificExpression(referData, info);

    // print the image selector

    curprint("[");

    if( teamRank )
    {
        SgIntVal * val = isSgIntVal(teamRank);
        if( val )
            unparseIntVal(val, info);
        else
            unparseLanguageSpecificExpression(teamRank, info);
    }

    if( teamRank && teamVarRef ) curprint(" ");

    if( teamVarRef )
    {
        string name = teamVarRef->get_symbol()->get_declaration()->get_name();
        if( name == "team_world" && !teamRank )
            curprint("*");
        else if( name == "team_default" && !teamRank )
            curprint("@");
        else
        {
            curprint("@");
            curprint(name);
        }
    }

    else
        /* print nothing */ ;

    curprint("]");
}


bool FortranCodeGeneration_locatedNode::requiresParentheses(SgExpression* expr, SgUnparse_Info& info)
{
    // same as in base class except always respect 'need_paren' property of a node
    // seems like this would be a good idea in general, but it breaks C++ unparsing for some reason

    if( expr->get_need_paren() )
        return true;
    else
        return UnparseLanguageIndependentConstructs::requiresParentheses(expr, info);
}


PrecedenceSpecifier FortranCodeGeneration_locatedNode::getPrecedence(SgExpression* exp)
{
    // same as in base class except unary plus/minus have equal precedence with binary plus.

    SgAddOp * addOp = new SgAddOp(NULL, NULL, NULL, NULL);
    PrecedenceSpecifier addOpPrec = UnparseLanguageIndependentConstructs::getPrecedence(addOp);
    delete addOp;
    return (isSgMinusOp(exp) || isSgUnaryAddOp(exp) ? addOpPrec : UnparseLanguageIndependentConstructs::getPrecedence(exp));
}


