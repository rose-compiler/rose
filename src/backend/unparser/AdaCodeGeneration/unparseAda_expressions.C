/* unparseAda_expressions.C
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


void Unparse_Ada::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info) 
   {
  // This is the X10 specific expression code generation
#if 1
     printf ("In Unparse_X10::unparseLanguageSpecificExpression ( expr = %p = %s ) language = %s \n",expr,expr->class_name().c_str(),languageName().c_str());
#endif

#if 0
    //
    // Check if this expression requires parentheses.  If so, process the opening parentheses now.
    //
    AstIntAttribute *parenthesis_attribute = (AstIntAttribute *) expr->getAttribute("x10-parentheses-count");
    if (parenthesis_attribute) { // Output the left paren
        for (int i = 0; i < parenthesis_attribute -> getValue(); i++) {
            curprint("(");
        }
    }

    switch (expr->variant()) {
        case UNARY_EXPRESSION:  { unparseUnaryExpr (expr, info); break; }
        case BINARY_EXPRESSION: { unparseBinaryExpr(expr, info); break; }
        case CLASSNAME_REF: { unparseClassRef(expr, info); break; }

        case CHAR_VAL: { unparseCharVal(expr,info); break; }
        case UNSIGNED_CHAR_VAL: { unparseUCharVal(expr, info); break; }
        case WCHAR_VAL: { unparseWCharVal(expr, info); break; }

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

        case TYPEID_OP: { unparseTypeIdOp(expr, info); break; }
        case NOT_OP: { unparseNotOp(expr, info); break; }
        case BIT_COMPLEMENT_OP: { unparseBitCompOp(expr, info); break; }
        case EXPR_CONDITIONAL: { unparseExprCond(expr, info); break; }
        case CAST_OP:                 { unparseCastOp(expr, info); break; }
        case ARRAY_OP:                { unparseArrayOp(expr, info); break; }
        case NEW_OP:                  { unparseNewOp(expr, info); break; }
        case DELETE_OP:               { unparseDeleteOp(expr, info); break; }
        case THIS_NODE:               { unparseThisNode(expr, info); break; }
        case SUPER_NODE:              { unparseSuperNode(expr, info); break; }
        case CLASS_NODE:              { unparseClassNode(expr, info); break; }

        case TYPE_REF:                { unparseTypeRef(expr, info); break; }
        case EXPR_INIT:               { unparseExprInit(expr, info); break; }
        case AGGREGATE_INIT:          { unparseAggrInit(expr, info); break; }
        case CONSTRUCTOR_INIT:        { unparseConInit(expr, info); break; }
        case ASSIGN_INIT:             { unparseAssnInit(expr, info); break; }
        case THROW_OP:                { unparseThrowOp(expr, info); break; }
        case DESIGNATED_INITIALIZER:  { unparseDesignatedInitializer(expr, info); break; }
        case PSEUDO_DESTRUCTOR_REF:   { unparsePseudoDtorRef(expr, info); break; }
        case JAVA_INSTANCEOF_OP:      { unparseX10InstanceOfOp(expr, info); break; }

        case V_SgJavaMarkerAnnotation:       { unparseX10MarkerAnnotation(expr, info); break; }
        case V_SgJavaSingleMemberAnnotation: { unparseX10SingleMemberAnnotation(expr, info); break; }
        case V_SgJavaNormalAnnotation:       { unparseX10NormalAnnotation(expr, info); break; }

        case V_SgJavaTypeExpression:         { unparseX10TypeExpression(expr, info); break; }
                // MH-20140730
        case V_SgHereExp:       { unparseHereExpression(expr, info); break; }


        default: {

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

                case V_SgVarRefExp:             { unparseVarRef(expr, info); break; }
                case V_SgFunctionRefExp:        { unparseFuncRef(expr, info); break; }
                case V_SgMemberFunctionRefExp:  { unparseMFuncRef(expr, info); break; }

                case V_SgNullExpression:        { curprint ("null"); break; }

                default:
                     cout << "error: unparseExpression() is unimplemented for " << expr->class_name() << endl;
                     ROSE_ASSERT(false);
                     break;
            }
        }
    }

    //
    // If this expression requires closing parentheses, emit them now.
    //
    if (parenthesis_attribute) { // Output the right parentheses
        for (int i = 0; i < parenthesis_attribute -> getValue(); i++) {
            curprint(")");
        }
    }
#endif
}




void Unparse_Ada::unparseStringVal               (SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Unparse_Ada::unparseStringVal(): not implemented! \n");
     ROSE_ASSERT(false);
   }
