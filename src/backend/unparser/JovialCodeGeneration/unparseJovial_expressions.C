/* unparseJovial_expressions.C
 * 
 *
 */
#include "sage3basic.h"
#include "unparser.h"
#include "Utf8.h"
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


void Unparse_Jovial::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info) 
   {
      ROSE_ASSERT(expr != NULL);

    // Check if this expression requires parentheses.  If so, process the opening parentheses now.
    //
    AstIntAttribute *parenthesis_attribute = (AstIntAttribute *) expr->getAttribute("x10-parentheses-count");
    if (parenthesis_attribute) { // Output the left paren
        for (int i = 0; i < parenthesis_attribute -> getValue(); i++) {
            curprint("(");
        }
    }

    switch (expr->variantT())
       {
       // function, intrinsic calls
          case V_SgFunctionCallExp:     unparseFuncCall(expr, info);             break;

       // expressions
          case V_SgSubscriptExpression: unparseSubscriptExpr(expr, info);        break;
          case V_SgAsteriskShapeExp:    unparseAsteriskShapeExpr(expr, info);    break;

       // symbol references
          case V_SgFunctionRefExp:      unparseFuncRef    (expr, info);          break;
          case V_SgVarRefExp:           unparseVarRef     (expr, info);          break;

       // operators
          case V_SgUnaryOp:             unparseUnaryExpr  (expr, info);          break;
          case V_SgBinaryOp:            unparseBinaryExpr (expr, info);          break;
          case V_SgAssignOp:            unparseAssignOp   (expr, info);          break;

          case V_SgAddOp:               unparseBinaryOperator(expr, "+", info);  break;
          case V_SgSubtractOp:          unparseBinaryOperator(expr, "-", info);  break;
          case V_SgMultiplyOp:          unparseBinaryOperator(expr, "*", info);  break;
          case V_SgDivideOp:            unparseBinaryOperator(expr, "/", info);  break;
          case V_SgModOp:               unparseBinaryOperator(expr,"MOD",info);  break;
          case V_SgExponentiationOp:    unparseBinaryOperator(expr,"**", info);  break;
          case V_SgLessThanOp:          unparseBinaryOperator(expr, "<", info);  break;
          case V_SgLessOrEqualOp:       unparseBinaryOperator(expr,"<=", info);  break;
          case V_SgGreaterThanOp:       unparseBinaryOperator(expr, ">", info);  break;
          case V_SgGreaterOrEqualOp:    unparseBinaryOperator(expr,">=", info);  break;
          case V_SgEqualityOp:          unparseBinaryOperator(expr, "=", info);  break;
          case V_SgNotEqualOp:          unparseBinaryOperator(expr,"<>", info);  break;

          case V_SgUnaryAddOp:          unparseUnaryOperator(expr, "+", info);   break;
          case V_SgMinusOp:             unparseUnaryOperator(expr, "-", info);   break;

          case V_SgPntrArrRefExp:       unparseArrayOp(expr, info);              break;

       // initializers
          case V_SgAssignInitializer:   unparseAssnInit  (expr, info);           break;

#if 0
                case V_SgAndOp:
                case V_SgAssignOp:
                case V_SgBitAndOp:
                case V_SgBitOrOp:
                case V_SgBitXorOp:
                case V_SgDotExp:
                case V_SgArrowExp:
                case V_SgJavaUnsignedRshiftOp:
                case V_SgLshiftOp:
                case V_SgOrOp:
                case V_SgRshiftOp:
                case V_SgCommaOpExp: // charles4 10/14/2011
                     unparseBinaryOp(isSgBinaryOp(expr), info ); break;

                case V_SgPlusPlusOp:
                case V_SgMinusMinusOp:
                case V_SgNotOp:
                case V_SgBitComplementOp:
                     unparseUnaryOp(isSgUnaryOp(expr), info ); break;
                case V_SgMemberFunctionRefExp:  { unparseMFuncRef(expr, info); break; }
#endif

                case V_SgNullExpression:         break;

                default:
                     cout << "error: unparseExpression() is unimplemented for " << expr->class_name() << endl;
                     ROSE_ASSERT(false);
                     break;
       }

    // If this expression requires closing parentheses, emit them now.
    //
    if (parenthesis_attribute)
       {
       // Output the right parentheses
          for (int i = 0; i < parenthesis_attribute -> getValue(); i++) {
             curprint(")");
          }
       }
   }

void
Unparse_Jovial::unparseStringVal (SgExpression* expr, SgUnparse_Info& info)
  {
     cerr << "Unparse_Jovial::unparseStringVal(): not implemented! \n";
     ROSE_ASSERT(false);
  }

void
Unparse_Jovial::unparseAssignOp(SgExpression* expr, SgUnparse_Info& info) 
  {
     SgBinaryOp* op = isSgBinaryOp(expr);
     ROSE_ASSERT(op != NULL);

     unparseExpression(op->get_lhs_operand(), info);
     curprint(" = ");
     unparseExpression(op->get_rhs_operand(), info);
     curprint(";");
  }

void
Unparse_Jovial::unparseBinaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);
     ninfo.set_operator_name(op);
     unparseBinaryExpr(expr, ninfo);
   }

void
Unparse_Jovial::unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info ninfo(info);
     ninfo.set_operator_name(op);
     unparseUnaryExpr(expr, ninfo);
   }

//----------------------------------------------------------------------------
//  Table/array subscripts
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseSubscriptExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgSubscriptExpression* sub_expr = isSgSubscriptExpression(expr);
     ROSE_ASSERT(sub_expr != NULL);

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

     if (isSgNullExpression(sub_expr->get_upperBound()) == NULL)
        {
          unparseExpression(sub_expr->get_upperBound(), info);
        }

     SgExpression* strideExpression = sub_expr->get_stride();
     ROSE_ASSERT(strideExpression != NULL);

     SgIntVal* integerValue = isSgIntVal(strideExpression);
     ROSE_ASSERT(integerValue != NULL);

  // See if this is the default value for the stride (unit stride) and skip the output in this case.
     bool defaultValue = (integerValue->get_value() == 1) ? true : false;
     if (defaultValue == false)
        {
          curprint(":");
          unparseExpression(sub_expr->get_stride(), info);
        }
   }

void
Unparse_Jovial::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // Sage node corresponds to array indicing
     SgPntrArrRefExp* arrayRefExp = isSgPntrArrRefExp(expr);

     unparseExpression(arrayRefExp->get_lhs_operand(),info);

     SgUnparse_Info ninfo(info);
     ninfo.set_SkipParen();

     curprint("(");
     unparseExpression(arrayRefExp->get_rhs_operand(),ninfo);
     curprint(")");
   }

void
Unparse_Jovial::unparseAsteriskShapeExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
     ROSE_ASSERT( isSgAsteriskShapeExp(expr) != NULL );

     curprint("*");
   }

void
Unparse_Jovial::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info)
   {
      SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
      ROSE_ASSERT(func_call != NULL);

   // function name
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
//  ::<symbol references>
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
      SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
      ROSE_ASSERT(func_ref != NULL);

      string func_name = func_ref->get_symbol()->get_name().str();
      curprint(func_name);
   }

void
Unparse_Jovial::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);

     ROSE_ASSERT(var_ref != NULL);
     ROSE_ASSERT(var_ref->get_symbol() != NULL);

     curprint(var_ref->get_symbol()->get_name().str());
   }

//----------------------------------------------------------------------------
//  initializers
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ROSE_ASSERT(assn_init != NULL);

     unparseExpression(assn_init->get_operand(), info);
   }

//----------------------------------------------------------------------------
//  Table dimension list
//----------------------------------------------------------------------------

void
Unparse_Jovial::unparseDimInfo(SgExprListExp* dim_info, SgUnparse_Info& info)
   {
      ROSE_ASSERT(dim_info != NULL);

   // If this is a scalar don't print the parens
      if (dim_info->get_expressions().size() < 1) return;

      bool first = true;
      curprint("(");

      BOOST_FOREACH(SgExpression* expr, dim_info->get_expressions())
         {
            if (first) first = false;
            else       curprint(",");

            if (isSgSubscriptExpression(expr))
               {
                  unparseSubscriptExpr(expr, info);
               }
            else if (isSgAsteriskShapeExp(expr))
               {
                  unparseAsteriskShapeExpr(expr, info);
               }
            else
               {
                  unparseExpression(expr, info);
               }
         }

      curprint(") ");
   }
