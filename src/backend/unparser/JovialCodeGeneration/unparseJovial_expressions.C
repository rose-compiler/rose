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
#if 0      
      printf ("In Unparse_Jovial::unparseLanguageSpecificExpression ( expr = %p = %s ) language = %s \n",expr,expr->class_name().c_str(),languageName().c_str());
#endif

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

       // operators
       // case V_SgUnaryOp:            unparseUnaryExpr (expr, info); break;
          case V_SgBinaryOp:           unparseBinaryExpr(expr, info); break;

          case V_SgAssignOp:           unparseAssignOp(expr, info);   break;
          case V_SgVarRefExp:          unparseVarRef(expr, info);     break;

#if 0
                case V_SgAddOp:
                case V_SgAndOp:
                case V_SgAssignOp:
                case V_SgBitAndOp:
                case V_SgBitOrOp:
                case V_SgBitXorOp:
                case V_SgDivideOp:
                case V_SgDotExp:
                case V_SgArrowExp:
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
                case V_SgCommaOpExp: // charles4 10/14/2011
                     unparseBinaryOp(isSgBinaryOp(expr), info ); break;

                case V_SgPlusPlusOp:
                case V_SgMinusMinusOp:
                case V_SgUnaryAddOp:
                case V_SgMinusOp:
                case V_SgNotOp:
                case V_SgBitComplementOp:
                     unparseUnaryOp(isSgUnaryOp(expr), info ); break;
                case V_SgFunctionRefExp:        { unparseFuncRef(expr, info); break; }
                case V_SgMemberFunctionRefExp:  { unparseMFuncRef(expr, info); break; }

                case V_SgNullExpression:        { curprint ("null"); break; }
#endif

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
     printf ("Unparse_Jovial::unparseStringVal(): not implemented! \n");
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
     curprint(" ;");
   }


//----------------------------------------------------------------------------
//  ::<symbol references>
//----------------------------------------------------------------------------

void 
Unparse_Jovial::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ROSE_ASSERT(var_ref != NULL);
     ROSE_ASSERT(var_ref->get_symbol() != NULL);

     curprint(var_ref->get_symbol()->get_name().str());
   }
