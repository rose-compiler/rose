/* unparseJava_expressions.C
 * 
 *
 */
#include "sage3basic.h"
#include "unparser.h"
#include <limits>

using namespace std;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_HIDDEN_LIST_DATA 0
#define OUTPUT_DEBUGGING_INFORMATION 0

#ifdef _MSC_VER
#include "Cxx_Grammar.h"
#endif

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"


void
Unparse_Java::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info)
   {
  // This is the Java specific expression code generation

#if 0
     printf ("In Unparse_Java::unparseLanguageSpecificExpression ( expr = %p = %s ) language = %s \n",expr,expr->class_name().c_str(),languageName().c_str());
#endif

     switch (expr->variant())
        {
          case UNARY_EXPRESSION:  { unparseUnaryExpr (expr, info); break; }
          case BINARY_EXPRESSION: { unparseBinaryExpr(expr, info); break; }
          case CLASSNAME_REF: { unparseClassRef(expr, info); break; }

          case UNSIGNED_INT_VAL: { unparseUIntVal(expr, info); break; }
          case LONG_INT_VAL: { unparseLongIntVal(expr, info); break; }
          case LONG_LONG_INT_VAL: { unparseLongLongIntVal(expr, info); break; }
          case UNSIGNED_LONG_LONG_INT_VAL: { unparseULongLongIntVal(expr, info); break; }
          case UNSIGNED_LONG_INT_VAL: { unparseULongIntVal(expr, info); break; }
          case FLOAT_VAL: { unparseFloatVal(expr, info); break; }
          case LONG_DOUBLE_VAL: { unparseLongDoubleVal(expr, info); break; }

          case FUNC_CALL: { unparseFuncCall(expr, info); break; }
          case UNARY_MINUS_OP: { unparseUnaryMinusOp(expr, info); break; }
          case UNARY_ADD_OP: { unparseUnaryAddOp(expr, info); break; }

          case SIZEOF_OP:             { unparseSizeOfOp(expr, info); break; }

          case TYPEID_OP: { unparseTypeIdOp(expr, info); break; }
          case NOT_OP: { unparseNotOp(expr, info); break; }
          case BIT_COMPLEMENT_OP: { unparseBitCompOp(expr, info); break; }
          case EXPR_CONDITIONAL: { unparseExprCond(expr, info); break; }
          case CAST_OP:                 { unparseCastOp(expr, info); break; }
          case ARRAY_OP:                { unparseArrayOp(expr, info); break; }
          case NEW_OP:                  { unparseNewOp(expr, info); break; }
          case DELETE_OP:               { unparseDeleteOp(expr, info); break; }
          case THIS_NODE:               { unparseThisNode(expr, info); break; }

          case TYPE_REF:                { unparseTypeRef(expr, info); break; }
          case EXPR_INIT:               { unparseExprInit(expr, info); break; }
          case AGGREGATE_INIT:          { unparseAggrInit(expr, info); break; }
          case CONSTRUCTOR_INIT:        { unparseConInit(expr, info); break; }
          case ASSIGN_INIT:             { unparseAssnInit(expr, info); break; }
          case THROW_OP:                { unparseThrowOp(expr, info); break; }
          case DESIGNATED_INITIALIZER:  { unparseDesignatedInitializer(expr, info); break; }
          case PSEUDO_DESTRUCTOR_REF:   { unparsePseudoDtorRef(expr, info); break; }
          case JAVA_INSTANCEOF_OP:      { unparseJavaInstanceOfOp(expr, info); break; }

          default:

     // migrate the above switch stmt to use variantT() instead on variant(). The former has much
     // more consistent names
     switch (expr->variantT()) {
         case V_SgPlusAssignOp:
         case V_SgMinusAssignOp:
         case V_SgMultAssignOp:
         case V_SgDivAssignOp:
         case V_SgModAssignOp:
         case V_SgAndAssignOp:
         case V_SgXorAssignOp:
         case V_SgIorAssignOp:
         case V_SgRshiftAssignOp:
         case V_SgLshiftAssignOp:
         case V_SgJavaUnsignedRshiftAssignOp:
             unparseCompoundAssignOp( isSgCompoundAssignOp(expr), info ); break;

         case V_SgAddOp:
         case V_SgAndOp:
         case V_SgAssignOp:
         case V_SgBitAndOp:
         case V_SgBitOrOp:
         case V_SgBitXorOp:
         case V_SgDivideOp:
         case V_SgDotExp:
         case V_SgEqualityOp:
         case V_SgGreaterOrEqualOp:
         case V_SgGreaterThanOp:
         case V_SgJavaUnsignedRshiftOp:
         case V_SgLessOrEqualOp:
         case V_SgLessThanOp:
         case V_SgLshiftOp:
         case V_SgModOp:
         case V_SgMultiplyOp:
         case V_SgOrOp:
         case V_SgNotEqualOp:
         case V_SgRshiftOp:
         case V_SgSubtractOp:
             unparseBinaryOp( isSgBinaryOp(expr), info ); break;

         case V_SgPlusPlusOp:
         case V_SgMinusMinusOp:
         case V_SgUnaryAddOp:
         case V_SgMinusOp:
         case V_SgNotOp:
         case V_SgBitComplementOp:
             unparseUnaryOp( isSgUnaryOp(expr), info ); break;

         case V_SgVarRefExp:             { unparseVarRef(expr, info); break; }
         case V_SgFunctionRefExp:        { unparseFuncRef(expr, info); break; }
         case V_SgMemberFunctionRefExp:  { unparseMFuncRef(expr, info); break; }

         default:
               cout << "error: unparseExpression() is unimplemented for " << expr->class_name() << endl;
               ROSE_ASSERT(false);
               break;
     }
    }
   }

PrecedenceSpecifier
Unparse_Java::getPrecedence(SgExpression* expr) {
    switch (expr->variantT()) {
        case V_SgPlusPlusOp:
        case V_SgMinusMinusOp:
            return (isSgUnaryOp(expr)->get_mode() == SgUnaryOp::postfix) ? 14 : 13;

        case V_SgUnaryAddOp:                   return 13;
        case V_SgMinusOp:                      return 13;
        case V_SgBitComplementOp:              return 13;
        case V_SgNotOp:                        return 13;

        case V_SgMultiplyOp:                   return 12;
        case V_SgDivideOp:                     return 12;
        case V_SgModOp:                        return 12;

        case V_SgAddOp:                        return 11;
        case V_SgSubtractOp:                   return 11;

        case V_SgRshiftOp:                     return 10;
        case V_SgLshiftOp:                     return 10;
        case V_SgJavaUnsignedRshiftOp:         return 10;

        case V_SgLessThanOp:                   return 9;
        case V_SgGreaterThanOp:                return 9;
        case V_SgLessOrEqualOp:                return 9;
        case V_SgGreaterOrEqualOp:             return 9;
        case V_SgJavaInstanceOfOp:             return 9;

        case V_SgEqualityOp:                   return 8;
        case V_SgNotEqualOp:                   return 8;

        case V_SgBitAndOp:                     return 7;
        case V_SgBitXorOp:                     return 6;
        case V_SgBitOrOp:                      return 5;
        case V_SgAndOp:                        return 4;
        case V_SgOrOp:                         return 3;
        case V_SgConditionalExp:               return 2;

        case V_SgAssignOp:                     return 1;
        case V_SgPlusAssignOp:                 return 1;
        case V_SgMinusAssignOp:                return 1;
        case V_SgMultAssignOp:                 return 1;
        case V_SgDivAssignOp:                  return 1;
        case V_SgModAssignOp:                  return 1;
        case V_SgAndAssignOp:                  return 1;
        case V_SgXorAssignOp:                  return 1;
        case V_SgIorAssignOp:                  return 1;
        case V_SgRshiftAssignOp:               return 1;
        case V_SgLshiftAssignOp:               return 1;
        case V_SgJavaUnsignedRshiftAssignOp:   return 1;

        default:                               return ROSE_UNPARSER_NO_PRECEDENCE;
    }
}

AssociativitySpecifier
Unparse_Java::getAssociativity(SgExpression* expr) {
    switch (expr->variantT()) {
        case V_SgAssignOp:
        case V_SgPlusAssignOp:
        case V_SgMinusAssignOp:
        case V_SgMultAssignOp:
        case V_SgDivAssignOp:
        case V_SgModAssignOp:
        case V_SgAndAssignOp:
        case V_SgXorAssignOp:
        case V_SgIorAssignOp:
        case V_SgRshiftAssignOp:
        case V_SgLshiftAssignOp:
        case V_SgJavaUnsignedRshiftAssignOp:
            return e_assoc_right;

        default:
            return e_assoc_left;
    }
}


// DQ (2/16/2005): This function has been moved to this file from unparse_type.C
void
Unparse_Java::unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info& info)
   {
     ROSE_ASSERT (templateInstantiationDeclaration != NULL);
   }

void
Unparse_Java::unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info)
   {
     ROSE_ASSERT(templateParameter != NULL);
     printf ("unparseTemplateParameter(): Sorry, not implemented! \n");
     ROSE_ASSERT(false);
   }


void
Unparse_Java::unparseTemplateArgument(SgTemplateArgument* templateArgument, SgUnparse_Info& info)
   {
      ROSE_ASSERT(templateArgument != NULL);
   }


void
Unparse_Java::unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info & info)
   {
  //
  // Flag to keep to original state of the "this" option
  //
     bool orig_this_opt = unp->opt.get_this_opt();
     SgUnparse_Info newinfo(info);
     newinfo.set_operator_name(op);
  //
  // If the "this" option was originally false, then we shouldn't print "this."
  // however, this only applies when the "this" is part of a binary expression.
  // In the unary case, we must print "this," otherwise a syntax error will be
  // produced. (i.e. *this)
  //
     if ( !orig_this_opt )
          unp->opt.set_this_opt(true);
#if 0
     curprint ( "\n /* Inside of unparseUnaryOperator(" + expr->sage_class_name() + "," + op <+ ",SgUnparse_Info) */ \n");
#endif
     unparseUnaryExpr(expr, newinfo);

  //
  // Now set the "this" option back to its original state
  //
     if( !orig_this_opt )
          unp->opt.set_this_opt(false);
   }

void
Unparse_Java::unparseAssnExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
   }

void
Unparse_Java::unparseVarRef(SgExpression* expr, SgUnparse_Info& info) {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ROSE_ASSERT(var_ref != NULL);

     unparseName(var_ref->get_symbol()->get_name(), info);
}

void
Unparse_Java::unparseClassRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgClassNameRefExp* classname_ref = isSgClassNameRefExp(expr);
     ROSE_ASSERT(classname_ref != NULL);

     curprint (  classname_ref->get_symbol()->get_declaration()->get_name().str());
   }

void
Unparse_Java::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     ROSE_ASSERT(func_ref != NULL);

     cout << "unparser: warning. SgFunctionRef should be SgMemberFunctionRef: "
          << func_ref->get_symbol()->get_name().getString() << endl;
     unparseName(func_ref->get_symbol()->get_name(), info);
   }

void
Unparse_Java::unparseMFuncRef ( SgExpression* expr, SgUnparse_Info& info )
   {
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     ROSE_ASSERT(mfunc_ref != NULL);

     unparseName(mfunc_ref->get_symbol()->get_name(), info);
   }

void
Unparse_Java::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgStringVal* str_val = isSgStringVal(expr);
     ROSE_ASSERT(str_val != NULL);

  // Handle special case of macro specification (this is a temporary hack to permit us to
  // specify macros within transformations)

     int wrap = unp->u_sage->cur_get_linewrap();
     unp->u_sage->cur_get_linewrap();

#ifndef CXX_IS_ROSE_CODE_GENERATION
  // DQ (3/25/2006): Finally we can use the C++ string class
     string targetString = "ROSE-MACRO-CALL:";
     int targetStringLength = targetString.size();
     string stringValue = str_val->get_value();
     string::size_type location = stringValue.find(targetString);
     if (location != string::npos)
        {
       // unparse the string without the surrounding quotes and with a new line at the end
          string remainingString = stringValue.replace(location,targetStringLength,"");
       // printf ("Specify a MACRO: remainingString = %s \n",remainingString.c_str());
          remainingString.replace(remainingString.find("\\\""),4,"\"");
          curprint ( "\n" + remainingString + "\n");
        }
       else
        {
       // curprint ( "\"" + str_val->get_value() + "\"";
          if (str_val->get_wcharString() == true)
               curprint ( "L");
          curprint ( "\"" + str_val->get_value() + "\"");
        }
#endif

     unp->u_sage->cur_set_linewrap(wrap);
   }


void
Unparse_Java::unparseUIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedIntVal* uint_val = isSgUnsignedIntVal(expr);
     ROSE_ASSERT(uint_val != NULL);

  // curprint ( uint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "U";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (uint_val->get_valueString() == "")
        {
          curprint ( tostring(uint_val->get_value()) + "U");
        }
       else
        {
          curprint ( uint_val->get_valueString());
        }
   }

void
Unparse_Java::unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongIntVal* longint_val = isSgLongIntVal(expr);
     ROSE_ASSERT(longint_val != NULL);

  // curprint ( longint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "L";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (longint_val->get_valueString() == "")
        {
          curprint ( tostring(longint_val->get_value()) + "L");
        }
       else
        {
          curprint ( longint_val->get_valueString());
        }
   }

void
Unparse_Java::unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongLongIntVal* longlongint_val = isSgLongLongIntVal(expr);
     ROSE_ASSERT(longlongint_val != NULL);

  // curprint ( longlongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "LL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (longlongint_val->get_valueString() == "")
        {
          curprint ( tostring(longlongint_val->get_value()) + "LL");
        }
       else
        {
          curprint ( longlongint_val->get_valueString());
        }
   }

void
Unparse_Java::unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedLongLongIntVal* ulonglongint_val = isSgUnsignedLongLongIntVal(expr);
     ROSE_ASSERT(ulonglongint_val != NULL);

  // curprint ( ulonglongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "ULL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (ulonglongint_val->get_valueString() == "")
        {
          curprint ( tostring(ulonglongint_val->get_value()) + "ULL");
        }
       else
        {
          curprint ( ulonglongint_val->get_valueString());
        }
   }

void
Unparse_Java::unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedLongVal* ulongint_val = isSgUnsignedLongVal(expr);
     ROSE_ASSERT(ulongint_val != NULL);

  // curprint ( ulongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "UL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (ulongint_val->get_valueString() == "")
        {
          curprint ( tostring(ulongint_val->get_value()) + "UL");
        }
       else
        {
          curprint ( ulongint_val->get_valueString());
        }
   }

void
Unparse_Java::unparseFloatVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFloatVal* float_val = isSgFloatVal(expr);
     ROSE_ASSERT(float_val != NULL);

#if 0
     printf ("Inside of unparseFloatVal = %p \n",float_val);
     float_val->get_file_info()->display("unparseFloatVal");
#endif

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.  Note that we can't use the C++
  // interface since the input program, and thus the generated code, might not have 
  // included the "limits" header file.
     float float_value = float_val->get_value();
#if 0
     if (SageInterface::is_C_language() == true)
        {
          if (float_val->get_valueString() == "")
             {
               curprint ( tostring(float_val->get_value()));
             }
            else
             {
               curprint ( float_val->get_valueString());
             }
        }
       else
        {
        }
#endif

     if (float_value == std::numeric_limits<float>::infinity())
        {
       // printf ("Infinite value found as value in unparseFloatVal() \n");
       // curprint ( "std::numeric_limits<float>::infinity()";
          curprint ( "__builtin_huge_valf()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ((float_value != float_value) || (float_value == std::numeric_limits<float>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<float>::quiet_NaN()";
               curprint ( "__builtin_nanf (\"\")");
             }
            else
             {
               if (float_value == std::numeric_limits<float>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<float>::signaling_NaN()";
                    curprint ( "__builtin_nansf (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( float_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (float_val->get_valueString() == "")
                       {
                         curprint ( tostring(float_val->get_value()) + "F");
                       }
                      else
                       {
                         curprint ( float_val->get_valueString());
                       }
                  }
             }
        }
   }



void
Unparse_Java::unparseLongDoubleVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongDoubleVal* longdbl_val = isSgLongDoubleVal(expr);
     ROSE_ASSERT(longdbl_val != NULL);
  /* code inserted from specification */
  
  // curprint ( longdbl_val->get_value();

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.
     long double longDouble_value = longdbl_val->get_value();
     if (longDouble_value == std::numeric_limits<long double>::infinity())
        {
       // printf ("Infinite value found as value in unparseFloatVal() \n");
       // curprint ( "std::numeric_limits<long double>::infinity()";
          curprint ( "__builtin_huge_vall()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ( (longDouble_value != longDouble_value) || (longDouble_value == std::numeric_limits<long double>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<long double>::quiet_NaN()";
               curprint ( "__builtin_nanl (\"\")");
             }
            else
             {
               if (longDouble_value == std::numeric_limits<long double>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<long double>::signaling_NaN()";
                    curprint ( "__builtin_nansl (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( longdbl_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (longdbl_val->get_valueString() == "")
                         curprint ( tostring(longDouble_value));
                      else
                         curprint ( longdbl_val->get_valueString());
                  }
             }
        }
   }

void
Unparse_Java::unparseComplexVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgComplexVal* complex_val = isSgComplexVal(expr);
     ROSE_ASSERT(complex_val != NULL);

     if (complex_val->get_valueString() != "") { // Has string
       curprint (complex_val->get_valueString());
     } else if (complex_val->get_real_value() == NULL) { // Pure imaginary
       curprint ("(");
       unparseValue(complex_val->get_imaginary_value(), info);
       curprint (" * _Complex_I)");
     } else { // Complex number
       curprint ("(");
       unparseValue(complex_val->get_real_value(), info);
       curprint (" + ");
       unparseValue(complex_val->get_imaginary_value(), info);
       curprint (" * _Complex_I)");
     }
   }

//-----------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparseFuncCall 
//  
//  This function is called whenever we unparse a function call. It is divided up 
//  into two parts. The first part unparses the function call and its arguments 
//  using an "in-order" tree traversal method. This is done when we have a binary 
//  operator overloading function and the operator overloading option is turned off.
//  The second part unparses the function call directly in a list-like manner. This 
//  is done for non-operator function calls, or when the operator overloading option
//  is turned on. 
//-----------------------------------------------------------------------------------
void
Unparse_Java::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
     ROSE_ASSERT(func_call != NULL);

     unparseExpression(func_call->get_function(), info);
     curprint("(");
     unparseExpression(func_call->get_args(), info);
     curprint(")");
   }

void Unparse_Java::unparseUnaryMinusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "-", info); }
void Unparse_Java::unparseUnaryAddOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "+", info); }

void
Unparse_Java::unparseSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgSizeOfOp* sizeof_op = isSgSizeOfOp(expr);
     ROSE_ASSERT(sizeof_op != NULL);

     curprint ( "sizeof(");
     if (sizeof_op->get_operand_expr() != NULL)
        {
          ROSE_ASSERT(sizeof_op->get_operand_expr() != NULL);
          unparseExpression(sizeof_op->get_operand_expr(), info);
        }
       else
        {
          ROSE_ASSERT(sizeof_op->get_operand_type() != NULL);
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();
          info2.set_SkipClassDefinition();
          info2.unset_isTypeFirstPart();
          info2.unset_isTypeSecondPart();
          unp->u_type->unparseType(sizeof_op->get_operand_type(), info2);
        }
     curprint ( ")");
   }

void
Unparse_Java::unparseTypeIdOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgTypeIdOp* typeid_op = isSgTypeIdOp(expr);
     ROSE_ASSERT(typeid_op != NULL);
  /* code inserted from specification */

     curprint ( "typeid(");
     if (typeid_op->get_operand_expr() != NULL)
         {
           ROSE_ASSERT(typeid_op->get_operand_expr() != NULL);
           unparseExpression(typeid_op->get_operand_expr(), info);
         }
        else
         {
           ROSE_ASSERT(typeid_op->get_operand_type() != NULL);
           SgUnparse_Info info2(info);
           info2.unset_SkipBaseType();
           info2.set_SkipClassDefinition();
           unp->u_type->unparseType(typeid_op->get_operand_type(), info2);
         }
      curprint ( ")");
   }

void Unparse_Java::unparseNotOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "!", info); }
void Unparse_Java::unparseAbstractOp(SgExpression* expr, SgUnparse_Info& info) {}
void Unparse_Java::unparseBitCompOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "~", info); }

void
Unparse_Java::unparseExprCond(SgExpression* expr, SgUnparse_Info& info)
   {
     SgConditionalExp* expr_cond = isSgConditionalExp(expr);
     ROSE_ASSERT(expr_cond != NULL);

     unparseExpression(expr_cond->get_conditional_exp(), info);
     curprint(" ? ");
     unparseExpression(expr_cond->get_true_exp(), info);
     curprint(" : ");
     unparseExpression(expr_cond->get_false_exp(), info);
   }

void
Unparse_Java::unparseClassInitOp(SgExpression* expr, SgUnparse_Info& info)
   {
   } 

void
Unparse_Java::unparseDyCastOp(SgExpression* expr, SgUnparse_Info& info)
   {
   }

void
Unparse_Java::unparseCastOp(SgExpression* expr, SgUnparse_Info& info) {
    SgCastExp* cast = isSgCastExp(expr);
    ROSE_ASSERT(cast != NULL);

    curprint("(");
    unparseType(cast->get_type(), info);
    curprint(") ");
    unparseExpression(cast->get_operand(), info);
}

void
Unparse_Java::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
   { 
     //unparseBinaryOperator(expr, "[]", info); 
     ROSE_ASSERT(!"unimplemented");
   }

void
Unparse_Java::unparseNewOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("In Unparse_ExprStmt::unparseNewOp \n");
  // curprint ( "\n /* In Unparse_ExprStmt::unparseNewOp */ \n";

#ifndef CXX_IS_ROSE_CODE_GENERATION
     SgNewExp* new_op = isSgNewExp(expr);
     ROSE_ASSERT(new_op != NULL);
  /* code inserted from specification */

     if (new_op->get_need_global_specifier())
        {
       // DQ (1/5/2006): I don't think that we want the extra space after the "::".
       // curprint ( ":: ";
          curprint ( "::");
        }

     curprint ( "new ");

  // curprint ( "\n /* Output any placement arguments */ \n";
     SgUnparse_Info newinfo(info);
     newinfo.unset_inVarDecl();
     if (new_op->get_placement_args() != NULL)
        {
       // printf ("Output placement arguments for new operator \n");
          curprint ( "\n/* Output placement arguments for new operator */\n");

       // DQ (1/5/2006): The placement arguments require "() " (add a space to make it look nice)
          curprint ( "(");
          unparseExpression(new_op->get_placement_args(), newinfo);
          curprint ( ") ");
        }

     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();
     newinfo.set_SkipClassSpecifier();

  // DQ (11/26/2004): In KULL this is set to true when I think it should not be (could not reproduce error in smaller code!)
  // the problem is that the type name is not being output after the new keyword.  It should unparse to "new typename (args)" and 
  // instead just unparses to "new (args)".  Error occurprints in generated code (rose_polygonalaleremapswig.C).
     newinfo.unset_SkipBaseType();

  // This fixes a bug having to do with the unparsing of the type name of constructors in return statements.

  // curprint ( "\n /* Output type name for new operator */ \n";

  // printf ("In Unparse_ExprStmt::unparseNewOp: new_op->get_type()->sage_class_name() = %s \n",new_op->get_type()->sage_class_name());

  // DQ (1/17/2006): The the type specified explicitly in the new expressions syntax, 
  // get_type() has been modified to return a pointer to new_op->get_specified_type().
  // unp->u_type->unparseType(new_op->get_type(), newinfo);
     unp->u_type->unparseType(new_op->get_specified_type(), newinfo);

  // printf ("DONE: new_op->get_type()->sage_class_name() = %s \n",new_op->get_type()->sage_class_name());

  // curprint ( "\n /* Output constructor args */ \n";

     if (new_op->get_constructor_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_constructor_args() \n");
          unparseExpression(new_op->get_constructor_args(), newinfo);
        }

#if 0
       else
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Call unparse type \n");
          unp->u_type->unparseType(new_op->get_type(), newinfo);
        }
#endif

  // curprint ( "\n /* Output builtin args */ \n";

     if (new_op->get_builtin_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_builtin_args() \n");
          unparseExpression(new_op->get_builtin_args(), newinfo);
        }

  // curprint ( "\n /* Leaving Unparse_ExprStmt::unparseNewOp */ \n";
  // printf ("Leaving Unparse_ExprStmt::unparseNewOp \n");
#endif
   }

void
Unparse_Java::unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgDeleteExp* delete_op = isSgDeleteExp(expr);
     ROSE_ASSERT(delete_op != NULL);
  /* code inserted from specification */

     if (delete_op->get_need_global_specifier())
        {
          curprint ( ":: ");
        }
     curprint ( "delete ");
     SgUnparse_Info newinfo(info);
     if (delete_op->get_is_array())
        {
          curprint ( "[]");
        }
     unparseExpression(delete_op->get_variable(), newinfo);
   }

void
Unparse_Java::unparseThisNode(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgThisExp* this_node = isSgThisExp(expr);

     ROSE_ASSERT(this_node != NULL);

  // printf ("In Unparse_ExprStmt::unparseThisNode: unp->opt.get_this_opt() = %s \n", (unp->opt.get_this_opt()) ? "true" : "false");

     if (unp->opt.get_this_opt()) // Checks options to determine whether to print "this"  
        {
          curprint ( "this"); 
        }
   }

void
Unparse_Java::unparseScopeOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgScopeOp* scope_op = isSgScopeOp(expr);
     ROSE_ASSERT(scope_op != NULL);
  /* code inserted from specification */

     if (scope_op->get_lhs_operand())
          unparseExpression(scope_op->get_lhs_operand(), info);
     curprint ( "::");
     unparseExpression(scope_op->get_rhs_operand(), info);
   }

void Unparse_Java::unparseForDeclOp(SgExpression* expr, SgUnparse_Info& info) {}

void
Unparse_Java::unparseTypeRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgRefExp* type_ref = isSgRefExp(expr);
     ROSE_ASSERT(type_ref != NULL);
  /* code inserted from specification */

     SgUnparse_Info newinfo(info);
     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();
  
     unp->u_type->unparseType(type_ref->get_type_name(), newinfo);
   }

void Unparse_Java::unparseVConst(SgExpression* expr, SgUnparse_Info& info) {}
void Unparse_Java::unparseExprInit(SgExpression* expr, SgUnparse_Info& info) {}

// Liao 11/3/2010
// Sometimes initializers can from an included file
//  SgAssignInitializer -> SgCastExp ->SgCastExp ->SgIntVal
// We should not unparse them
// This function will check if the nth initializer is from a different file from the aggregate initializer
static bool isFromAnotherFile (SgLocatedNode* lnode)
{
  bool result = false;
  ROSE_ASSERT (lnode != NULL);
  // Liao 11/22/2010, a workaround for enum value constant assign initializer
  // EDG passes the source location information of the original declaration of the enum value, not the location for the value's reference
  // So SgAssignInitializer has wrong file info.
  // In this case, we look down to the actual SgEnumVal for the file info instead of looking at its ancestor SgAssignInitializer  
  SgAssignInitializer *a_initor = isSgAssignInitializer (lnode);
  if (a_initor)
  {
    result = false;
    SgExpression * leaf_child = a_initor->get_operand_i();
    while (SgCastExp * cast_op = isSgCastExp(leaf_child))
    { 
      // redirect to original expression tree if possible
      if (cast_op->get_originalExpressionTree() != NULL)
        leaf_child = cast_op->get_originalExpressionTree();
      else
        leaf_child = cast_op->get_operand_i();
    }
    //if (isSgEnumVal(leaf_child))
    lnode = leaf_child;
  }

  SgFile* cur_file = SageInterface::getEnclosingFileNode(lnode);
  if (cur_file != NULL)
  {
    // normal file info 
    if (lnode->get_file_info()->isTransformation() == false &&  lnode->get_file_info()->isCompilerGenerated() ==false)
    {
      if (cur_file->get_file_info()->get_filename() != lnode->get_file_info()->get_filename())
        result = true;
    }
  } //


  return result;
}

void
Unparse_Java::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
    // Skip the entire thing if the initializer is from an included file
     if (isFromAnotherFile (expr))
       return;
     SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
     ROSE_ASSERT(aggr_init != NULL);
  /* code inserted from specification */

     SgUnparse_Info newinfo(info);
     if (aggr_init->get_need_explicit_braces())
      curprint ( "{");

     SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();
     size_t last_index = list.size() -1;

     for (size_t index =0; index < list.size(); index ++)
     {
       //bool skipUnparsing = isFromAnotherFile(aggr_init,index);
       bool skipUnparsing = isFromAnotherFile(list[index]);
       if (!skipUnparsing)
       {
         unparseExpression(list[index], newinfo);
         if (index!= last_index)
           curprint ( ", ");
       }
     }
     unparseAttachedPreprocessingInfo(aggr_init, info, PreprocessingInfo::inside);
     if (aggr_init->get_need_explicit_braces())
      curprint ( "}");
   }

void
Unparse_Java::unparseConInit(SgExpression* expr, SgUnparse_Info& info)
{
#if 0
  printf ("In Unparse_Java::unparseConInit expr = %p \n",expr);
  printf ("WARNING: This is redundent with the Unparse_Java::unp->u_sage->unparseOneElemConInit (This function does not handle qualidied names!) \n");
#endif
}

void
Unparse_Java::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ROSE_ASSERT(assn_init != NULL);

     curprint("= ");
     unparseExpression(assn_init->get_operand_i(), info);
   }

void
Unparse_Java::unparseThrowOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgThrowOp* throw_op = isSgThrowOp(expr);
     ROSE_ASSERT(throw_op != NULL);

  // printf ("In unparseThrowOp(%s) \n",expr->sage_class_name());
  // curprint ( "\n/* In unparseThrowOp(" + expr->sage_class_name() + ") */ \n";
   }


void
Unparse_Java::unparseDesignatedInitializer(SgExpression* expr, SgUnparse_Info & info)
   {
   }

void
Unparse_Java::unparseJavaInstanceOfOp(SgExpression* expr, SgUnparse_Info & info) {
    SgJavaInstanceOfOp* inst_op = isSgJavaInstanceOfOp(expr);
    ROSE_ASSERT(inst_op != NULL);

    unparseExpression(inst_op->get_operand_expr(), info);
    curprint(" instanceof ");

    //TODO p_operand_type should be defined. Until it always is, complain.
    if (inst_op->get_operand_type() != NULL) {
        unparseType(inst_op->get_operand_type(), info);
    } else {
        cout << "unparser: error. SgJavaInstanceOfOp::p_operand_type is NULL" << endl;
        curprint("NULL_TYPE_IN_AST");
    }
}

void
Unparse_Java::unparsePseudoDtorRef(SgExpression* expr, SgUnparse_Info & info)
   {
   }

void
Unparse_Java::unparseCompoundAssignOp(SgCompoundAssignOp* op,
                                                        SgUnparse_Info & info) {
    unparseExpression(op->get_lhs_operand(), info);
    switch (op->variantT()) {
        case V_SgPlusAssignOp:               curprint(" += ");   break;
        case V_SgMinusAssignOp:              curprint(" -= ");   break;
        case V_SgMultAssignOp:               curprint(" *= ");   break;
        case V_SgDivAssignOp:                curprint(" /= ");   break;
        case V_SgModAssignOp:                curprint(" %= ");   break;
        case V_SgAndAssignOp:                curprint(" &= ");   break;
        case V_SgXorAssignOp:                curprint(" ^= ");   break;
        case V_SgIorAssignOp:                curprint(" |= ");   break;
        case V_SgRshiftAssignOp:             curprint(" >>= ");  break;
        case V_SgLshiftAssignOp:             curprint(" <<= ");  break;
        case V_SgJavaUnsignedRshiftAssignOp: curprint(" >>>= "); break;
        default: {
             cout << "error: unhandled compound assign op: " << op->class_name() << endl;
             ROSE_ASSERT(false);
        }
    }
    unparseExpression(op->get_rhs_operand(), info);
}

void
Unparse_Java::unparseBinaryOp(SgBinaryOp* op,
                              SgUnparse_Info & info) {
    unparseExpression(op->get_lhs_operand(), info);
    switch (op->variantT()) {
        case V_SgAddOp:                curprint(" + ");   break;
        case V_SgAndOp:                curprint(" && ");  break;
        case V_SgAssignOp:             curprint(" = ");   break;
        case V_SgBitAndOp:             curprint(" & ");   break;
        case V_SgBitOrOp:              curprint(" | ");   break;
        case V_SgBitXorOp:             curprint(" ^ ");   break;
        case V_SgDivideOp:             curprint(" / ");   break;
        case V_SgDotExp:               curprint(".");     break;
        case V_SgEqualityOp:           curprint(" == ");  break;
        case V_SgGreaterOrEqualOp:     curprint(" >= ");  break;
        case V_SgGreaterThanOp:        curprint(" > ");   break;
        case V_SgJavaUnsignedRshiftOp: curprint(" >>> "); break;
        case V_SgLessOrEqualOp:        curprint(" <= ");  break;
        case V_SgLessThanOp:           curprint(" < ");   break;
        case V_SgLshiftOp:             curprint(" << ");  break;
        case V_SgModOp:                curprint(" % ");   break;
        case V_SgMultiplyOp:           curprint(" * ");   break;
        case V_SgOrOp:                 curprint(" || ");  break;
        case V_SgNotEqualOp:           curprint(" != ");  break;
        case V_SgRshiftOp:             curprint(" >> ");  break;
        case V_SgSubtractOp:           curprint(" - ");   break;
        default: {
             cout << "error: cannot unparse binary op: " << op->class_name() << endl;
             ROSE_ASSERT(false);
        }
    }
    unparseExpression(op->get_rhs_operand(), info);
}

void
Unparse_Java::unparseUnaryOp(SgUnaryOp* op,
                             SgUnparse_Info & info) {
    if (op->get_mode() == SgUnaryOp::postfix)
        unparseExpression(op->get_operand(), info);

    switch (op->variantT()) {
        case V_SgPlusPlusOp:     curprint("++");   break;
        case V_SgMinusMinusOp:   curprint("--");   break;
        case V_SgUnaryAddOp:     curprint("+");    break;
        case V_SgMinusOp:        curprint("-");    break;
        case V_SgNotOp:          curprint("!");    break;
        case V_SgBitComplementOp:curprint("~");    break;
        default:
            cout << "error: unparseUnaryOp(" << op->class_name() << "*,info) is unimplemented." << endl;
            break;
    }

    if (op->get_mode() == SgUnaryOp::prefix)
        unparseExpression(op->get_operand(), info);
}
