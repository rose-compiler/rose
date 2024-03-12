/* unparseFortran_expressions.C
 * 
 * Code to unparse Sage/Fortran expression nodes.
 * 
 */
#include "sage3basic.h"
#include "unparser.h"

using namespace std;
using namespace Rose;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

void
FortranCodeGeneration_locatedNode::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info)
   {
  // This is the Fortran specific expression code generation

     switch (expr->variantT())
        {
       // function, intrinsic calls
          case V_SgFunctionCallExp:    unparseFuncCall(expr, info); break;

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

          case V_SgNewExp:             unparseNewOp(expr, info); break;
          case V_SgDeleteExp:          unparseDeleteOp(expr, info); break;

          case V_SgArrowExp:           unparsePointStOp(expr, info); break;
          case V_SgPointerDerefExp:    unparseDerefOp(expr, info); break;
          case V_SgAddressOfOp:        unparseAddrOp(expr, info); break;
          case V_SgRefExp:             unparseTypeRef(expr, info); break;

          case V_SgSubscriptExpression: unparseSubscriptExpr(expr, info); break;
          case V_SgColonShapeExp:       unparseColonShapeExp(expr, info); break;
          case V_SgAsteriskShapeExp:    unparseAsteriskShapeExp(expr, info); break;

       // initializers
          case V_SgAggregateInitializer:   unparseAggrInit(expr, info); break;
          case V_SgConstructorInitializer: unparseConInit(expr, info); break;
          case V_SgAssignInitializer:      unparseAssnInit(expr, info); break;

       // IO
          case V_SgIOItemExpression:      unparseIOItemExpr(expr, info); break;
          case V_SgImpliedDo:             unparseImpliedDo(expr, info); break;

       // symbol references
          case V_SgVarRefExp:             unparseVarRef(expr, info); break;
          case V_SgFunctionRefExp:        unparseFuncRef(expr, info); break;
          case V_SgMemberFunctionRefExp:  unparseMFuncRef(expr, info); break;
          case V_SgClassNameRefExp:       unparseClassRef(expr, info); break;

          case V_SgNullExpression:        unparseNullExpression(expr, info); break;

       // DQ (11/24/2007): Support for unparsing the IR node which must be post-processed into either an array reference or a function call.
          case V_SgUnknownArrayOrFunctionReference: unparseUnknownArrayOrFunctionReference(expr, info); break;

          case V_SgBoolValExp:         unparseBoolVal(expr, info); break;
          case V_SgLabelRefExp:        unparseLabelRefExp(expr, info); break;
          case V_SgActualArgumentExpression:  unparseActualArgumentExpression(expr, info); break;

          case V_SgUserDefinedUnaryOp:        unparseUserDefinedUnaryOp (expr, info); break;
          case V_SgUserDefinedBinaryOp:       unparseUserDefinedBinaryOp(expr, info); break;
          case V_SgCAFCoExpression:           unparseCoArrayExpression(expr, info); break;

          default:
             {
               printf("FortranCodeGeneration_locatedNode::unparseExpression: Error: No handler for %s (variant: %d)\n",expr->sage_class_name(), expr->variantT());
               ROSE_ABORT();
             }
        }
   }

void
FortranCodeGeneration_locatedNode::unparseActualArgumentExpression(SgExpression* expr, SgUnparse_Info& info)
   {
     SgActualArgumentExpression* actualArgumentExpression = isSgActualArgumentExpression(expr);

     curprint(actualArgumentExpression->get_argument_name());

  // DQ (2/2/2011): Now we don't want to support the use of SgActualArgumentExpression 
  // to hide a alternative return argument.  So the name should never be "*". Now we
  // use a newer implementation with SgLabelRefExp instead (and a new SgTypeLabel IR node).
     ROSE_ASSERT(actualArgumentExpression->get_argument_name() != "*");
     curprint("=");

     unparseExpression(actualArgumentExpression->get_expression(),info);
   }

void
FortranCodeGeneration_locatedNode::unparseLabelRefExp(SgExpression* expr, SgUnparse_Info&)
   {
     SgLabelRefExp* labelRefExp = isSgLabelRefExp(expr);
     ASSERT_not_null(labelRefExp);

     SgLabelSymbol* labelSymbol = labelRefExp->get_symbol();
     ASSERT_not_null(labelSymbol);

     int numericLabel = labelSymbol->get_numeric_label_value();
     ROSE_ASSERT(numericLabel >= 0);

     string numericLabelString = StringUtility::numberToString(numericLabel);

  // DQ (2/2/2011): We can't do this since it will effect where lables are unparse in the OPEN statement (and likely other I/O statements).
  // After some email with Scott this is required to be handled via a special case so since in all other case the SgLabelRefExp shuld
  // have a IOStatement as a parent, we will look for where the parent is part of an expression list.  This could be improved later.
  // the best way to handle this would be to do the type checking, and I'm OK with that approach.  The backup plan is to embed names
  // into (what) that would trigger these to be treated as alternative return arguments.
  // We could also check if the enclosing statement is an IO statement.  So there are a number of options here.

     SgStatement* tmp_statement = SageInterface::getEnclosingStatement(labelRefExp);
     ASSERT_not_null(tmp_statement);

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
               ASSERT_not_null(functionDefinition);

               SgFunctionDeclaration* functionDeclaration = functionDefinition->get_declaration();
               ASSERT_not_null(functionDeclaration);

               SgInitializedNamePtrList & args = functionDeclaration->get_args();
               ROSE_ASSERT(alternativeReturnValue < args.size());

            // The Fortran world starts at one (not zero)!
               size_t counter = 1;

               for (size_t i = 0; i < args.size(); i++)
                  {
                     SgType* argumentType = args[i]->get_type();
                     SgTypeLabel* labelType = isSgTypeLabel(argumentType);
                     if (labelType != nullptr)
                        {
                       // Search the argument list for a matching symbol.
                          SgSymbol* tmp_symbol = args[i]->get_symbol_from_symbol_table();
                          if (tmp_symbol == labelSymbol && alternativeReturnValue < 1)
                             {
                            // We have a match.
                               alternativeReturnValue = counter;
                             }
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
   }

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::unparseExprRoot
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparseExprRoot(SgExpression*, SgUnparse_Info&)
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
  ASSERT_not_null(func_call);

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
        unparseExpression((*arg), ninfo);
        arg++;
        if (arg != list.end()) {
          curprint(",");
        }
      }
    }
    curprint(")");
}

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<operators>
//----------------------------------------------------------------------------

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
                 ASSERT_not_null(isSgTypeBool(rhs_type));
               }
               outputLogicalOperator = true;
             }
        }
       else
        {
          printf ("Error: this function only needs to handle binary operators. ");
          ROSE_ABORT();
        }

     return outputLogicalOperator;
   }

void
FortranCodeGeneration_locatedNode::unparseEqOp(SgExpression* expr, SgUnparse_Info& info)
   { 
  // Sage node corresponds to Fortran equals operator
     ASSERT_not_null(expr);
     
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
     ASSERT_not_null(expr);

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
}

void
FortranCodeGeneration_locatedNode::unparseCastOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (8/16/2007): Allow SgCast operators to work since we wnat to test the unparser using C code
  // and we will later want to add cast operators to the Fortran AST to explicitly mark implicit casts
  // in fortran (marked as compiler generated).

  // Rasmussen (5/17/2023): I think this means this code can be removed, added abort to see if testing fails
  // Perhaps the warning can be turned off as some of the switch options don't abort.
     mlog[FATAL] << "Case operators not defined for Fortran code generation! node = " << expr->class_name() << "\n";
     ROSE_ABORT();

     SgCastExp* cast_op = isSgCastExp(expr);
     ASSERT_not_null(cast_op);

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
               ROSE_ABORT();
             }

          case SgCastExp::e_default:
             {
               printf ("SgCastExp::e_default found \n");
               ROSE_ABORT();
             }

          case SgCastExp::e_dynamic_cast:
             {
            // dynamic_cast <P *> (expr)
               printf ("SgCastExp::e_dynamic_cast found (Now defined in Fortran) \n");
               ROSE_ABORT();
             }
          case SgCastExp::e_static_cast:
             {
            // static_cast <P *> (expr)
               printf ("SgCastExp::e_static_cast found (Now defined in Fortran) \n");
               ROSE_ABORT();
             }
          case SgCastExp::e_reinterpret_cast:
             {
            // reinterpret_cast <P *> (expr)
               printf ("SgCastExp::e_reinterpret_cast found (Now defined in Fortran) \n");
               ROSE_ABORT();
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
               ROSE_ABORT();
             }

     // DQ (3/28/2017): Adding this here to eliminate warning from Clang.
        case SgCastExp::e_last_cast:
             {
               printf ("SgCastExp::e_last_cast is an error (end of enum list) \n");
               ROSE_ABORT();
             }

     // DQ (3/28/2017): Adding this here to eliminate warning from Clang.
          default:
             {
            // DQ (3/28/2017): There should be no other cases, but make the default an error (just in case).
               printf ("SgCastExp::e_last_cast is an error (end of enum list) \n");
               ROSE_ABORT();
             }
        }

     unparseExpression(cast_op->get_operand(), info); 
   }

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<FIXME>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseNewOp(SgExpression* expr, SgUnparse_Info&)
   {
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ABORT();
   }

void
FortranCodeGeneration_locatedNode::unparseDeleteOp(SgExpression* expr, SgUnparse_Info&)
   {
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ABORT();
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
FortranCodeGeneration_locatedNode::unparseDerefOp(SgExpression*, SgUnparse_Info&)
{ 
  // Sage node has no explicit Fortran correspondence
}

void
FortranCodeGeneration_locatedNode::unparseAddrOp(SgExpression*, SgUnparse_Info&)
{ 
  // Sage node has no explicit Fortran correspondence
}

void 
FortranCodeGeneration_locatedNode::unparseTypeRef(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgRefExp* type_ref = isSgRefExp(expr);
     ASSERT_not_null(type_ref);

     SgUnparse_Info ninfo(info);
     ninfo.unset_PrintName();
  
     unp->u_fortran_type->unparseType(type_ref->get_type_name(), ninfo);
   }

void 
FortranCodeGeneration_locatedNode::unparseSubscriptExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgSubscriptExpression* sub_expr = isSgSubscriptExpression(expr);
     ASSERT_not_null(sub_expr);

     ASSERT_not_null(sub_expr->get_lowerBound());
     ASSERT_not_null(sub_expr->get_upperBound());
     ASSERT_not_null(sub_expr->get_stride());

     if (isSgNullExpression(sub_expr->get_lowerBound()) == NULL)
        {
          unparseExpression(sub_expr->get_lowerBound(), info);
          curprint(":");
        }
       else
        {
          curprint(":");
        }

     if (isSgNullExpression(sub_expr->get_upperBound()) == NULL)
        {
          unparseExpression(sub_expr->get_upperBound(), info);
        }

     SgExpression* strideExpression = sub_expr->get_stride();
     ASSERT_not_null(strideExpression);
     ASSERT_require(isSgNullExpression(strideExpression) == nullptr);

     SgIntVal* integerValue = isSgIntVal(strideExpression);

  // See if this is the default value for the stride (unit stride) and skip the output in this case.
     bool defaultValue = ( (integerValue != NULL) && (integerValue->get_value() == 1) ) ? true : false;
     if (defaultValue == false)
        {
          curprint(":");
          ASSERT_not_null(sub_expr->get_stride());
          unparseExpression(sub_expr->get_stride(), info);
        }
   }

void 
FortranCodeGeneration_locatedNode::unparseColonShapeExp(SgExpression* expr, SgUnparse_Info&)
   {
     SgColonShapeExp* colon = isSgColonShapeExp(expr);
     ASSERT_not_null(colon);

     curprint(":");
   }

void 
FortranCodeGeneration_locatedNode::unparseAsteriskShapeExp(SgExpression* expr, SgUnparse_Info&)
   {
     SgAsteriskShapeExp* sub_ast = isSgAsteriskShapeExp(expr);
     ASSERT_not_null(sub_ast);

     curprint("*");
   }

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<initializers>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseInitializerList(SgExpression* expr, SgUnparse_Info& info)
   {
     ROSE_ASSERT(expr);
     SgExprListExp* expr_list = isSgExprListExp(expr);

     info.set_nested_expression();

     bool paren = true;
     if (paren)
        {
          curprint("(/");
        }

     SgExpressionPtrList::iterator it = expr_list->get_expressions().begin();
     while (it != expr_list->get_expressions().end())
        {
          unparseExpression(*it, info);
          it++;
          if (it != expr_list->get_expressions().end())
             {
               curprint(","); 
             }
        }

     if (paren)
        {
          curprint("/)");
        }

     info.unset_nested_expression();
   }

void 
FortranCodeGeneration_locatedNode::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
     ASSERT_not_null(aggr_init);

     unparseInitializerList(aggr_init->get_initializers(), info);
   }

void
FortranCodeGeneration_locatedNode::unparseConInit(SgExpression* expr, SgUnparse_Info& info)
   {
  // initialization of user-defined types
     SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(expr);
     ASSERT_not_null(constructorInitializer);

     SgType* type = constructorInitializer->get_expression_type();
     SgClassType* classType = isSgClassType(type);
     ASSERT_not_null(classType);

     string className = classType->get_name().getString();
     curprint(className);

     curprint("(");

     ASSERT_not_null(constructorInitializer->get_args());
     unparseExpression(constructorInitializer->get_args(), info);

     curprint(")");

   }

void
FortranCodeGeneration_locatedNode::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (4/28/2008): This is used for simple initializers and we use the SgAggregateInitializer for structures!
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ASSERT_not_null(assn_init);

     unparseExpression(assn_init->get_operand(), info);
   }

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<IO>
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparseIOItemExpr(SgExpression* expr, SgUnparse_Info& info) 
{
  // Sage node represents Fortran IO
  SgIOItemExpression* ioitem_expr = isSgIOItemExpression(expr);
  ASSERT_not_null(ioitem_expr);

  SgExpression* ioitem = ioitem_expr->get_io_item();

  if (isSgExprListExp(ioitem)) {
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
     ASSERT_not_null(ioitem_expr);

     SgExprListExp* object_list = ioitem_expr->get_object_list();
     SgExpression* lb   = ioitem_expr->get_do_var_initialization();
     SgExpression* ub   = ioitem_expr->get_last_val();
     SgExpression* step = ioitem_expr->get_increment();

     ASSERT_not_null(lb);
     ASSERT_not_null(ub);
     ASSERT_not_null(step);

     curprint("(");
     if (object_list != nullptr)
        {
          unparseExprList(object_list, info);

          if (object_list->empty() == false)
               curprint(",");
        }

  // DQ (10/9/2010): This is an iterative step in the correct handling of implied do expressions.
  // Unparse the lhs and rhs separately to about extra "()".  A little later this will be a 
  // variable declaration, but we will not be able to unparse it as such since the type 
  // (integer) is not explicitly represented.
     ASSERT_not_null(lb);
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
     ASSERT_not_null(binaryExpression);
     ASSERT_not_null(binaryExpression->get_lhs_operand());
     unparseExpression(binaryExpression->get_lhs_operand(), info);
     curprint(" = ");
     ASSERT_not_null(binaryExpression->get_rhs_operand());
     unparseExpression(binaryExpression->get_rhs_operand(), info);
        }

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
FortranCodeGeneration_locatedNode::unparseVarRef(SgExpression* expr, SgUnparse_Info&)
   {
  // Sage node corresponds to a Fortran variable reference
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ASSERT_not_null(var_ref);
     ASSERT_not_null(var_ref->get_symbol());
  
     SgInitializedName* decl = var_ref->get_symbol()->get_declaration();
     ASSERT_not_null(decl);
     SgVariableDeclaration* vd = isSgVariableDeclaration(decl->get_declaration());

     if (false /*vd != NULL*/)
        {
       // FIXME:eraxxon: how to handle renamed module vars?
          SgClassDefinition* cdef = isSgClassDefinition(vd->get_parent());
          if (cdef != NULL)
             {
               SgClassDeclaration* xdecl = isSgClassDeclaration(cdef->get_declaration());
               if (xdecl != NULL && vd->get_declarationModifier().get_storageModifier().isStatic()) 
                  {
                    curprint(xdecl->get_qualified_name().str());
                    curprint("::");
                  }
             }
        }
  
     curprint(var_ref->get_symbol()->get_name().str());
   }

void 
FortranCodeGeneration_locatedNode::unparseFuncRef(SgExpression* expr, SgUnparse_Info&)
{
  // Sage node corresponds to a Fortran function reference
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  ASSERT_not_null(func_ref);
  string func_name = func_ref->get_symbol()->get_name().str();
  curprint(func_name);
}

void
FortranCodeGeneration_locatedNode::unparseMFuncRef(SgExpression* expr, SgUnparse_Info&)
   {
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ABORT();
   }

void 
FortranCodeGeneration_locatedNode::unparseClassRef(SgExpression* expr, SgUnparse_Info&)
   {
     printf ("Case operators not defined for Fortran code generation! node = %s \n",expr->class_name().c_str());
     ROSE_ABORT();
   }


void
FortranCodeGeneration_locatedNode::unparseStringVal(SgExpression* expr, SgUnparse_Info&)
   {
  // Note that string unparsing is language dependent so this is not handled by the language independent base class.

  // Sage node corresponds to a Fortran string constant
     SgStringVal* str_val = isSgStringVal(expr);
     ASSERT_not_null(str_val);

  // String values in fortran can use either double or single quotes ("..." or '...') to be used.
     string str;

  // We add the quotes back in since they are not saved with the string value (so that C/C++ and Fortran can be handled similarly).
     if (str_val->get_usesSingleQuotes() == true)
        {
       // str = string("/* single quotes */ \'") + str_val->get_value() + string("\'");
          str = string("\'") + str_val->get_value() + string("\'");
        }
       else
        {
          str = string("\"") + str_val->get_value() + string("\"");
        }
     curprint(str);
   }


//----------------------------------------------------------------------------
//  void FortranCodeGeneration_locatedNode::<constants>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseBoolVal(SgExpression* expr, SgUnparse_Info&)
   {
  // Sage node corresponds to a Fortran logical constant
     SgBoolValExp* bool_val = isSgBoolValExp(expr);
     ASSERT_not_null(bool_val);

     if (bool_val->get_value() == true)
        {
          curprint(".TRUE.");
        }
       else
        {
          curprint(".FALSE.");
        }
   }

//----------------------------------------------------------------------------
//  helpers
//----------------------------------------------------------------------------

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

  // Note that the function declaration is explicitly marked and I think this is better than
  // getting the return type.

     SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(fcall->get_function());
     ASSERT_not_null(functionRefExp);

     SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
     ASSERT_not_null(functionSymbol);

     SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
     ASSERT_not_null(functionDeclaration);

     SgProcedureHeaderStatement* procedureHeaderStatement = isSgProcedureHeaderStatement(functionDeclaration);
     ASSERT_not_null(procedureHeaderStatement);

     return (procedureHeaderStatement->get_subprogram_kind() == SgProcedureHeaderStatement::e_subroutine_subprogram_kind);
   }

void
FortranCodeGeneration_locatedNode::unparseUnknownArrayOrFunctionReference(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnknownArrayOrFunctionReference* assumeArrayReference = isSgUnknownArrayOrFunctionReference(expr);

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

    if ( teamRank )
    {
        SgIntVal * val = isSgIntVal(teamRank);
        if( val )
            unparseIntVal(val, info);
        else
            unparseLanguageSpecificExpression(teamRank, info);
    }

    if ( teamRank && teamVarRef ) curprint(" ");

    if ( teamVarRef )
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

    curprint("]");
}

bool FortranCodeGeneration_locatedNode::requiresParentheses(SgExpression* expr, SgUnparse_Info& info)
{
    // same as in base class except always respect 'need_paren' property of a node
    // seems like this would be a good idea in general, but it breaks C++ unparsing for some reason

    if ( expr->get_need_paren() )
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
