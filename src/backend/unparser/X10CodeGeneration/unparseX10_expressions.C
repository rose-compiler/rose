/* unparseX10_expressions.C
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


void Unparse_X10::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info) {
  // This is the X10 specific expression code generation
#if 0
     printf ("In Unparse_X10::unparseLanguageSpecificExpression ( expr = %p = %s ) language = %s \n",expr,expr->class_name().c_str(),languageName().c_str());
#endif

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
}

PrecedenceSpecifier
Unparse_X10::getPrecedence(SgExpression* expr) {
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
        case V_SgCommaOpExp:                   return 1; // charles4 10/14/2011

        default:                               return ROSE_UNPARSER_NO_PRECEDENCE;
    }
}

AssociativitySpecifier
Unparse_X10::getAssociativity(SgExpression* expr) {
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
Unparse_X10::unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info& info)
   {
     ASSERT_not_null(templateInstantiationDeclaration);
   }

void
Unparse_X10::unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info)
   {
     ASSERT_not_null(templateParameter);
     printf ("unparseTemplateParameter(): Sorry, not implemented! \n");
     ROSE_ASSERT(false);
   }


void
Unparse_X10::unparseTemplateArgument(SgTemplateArgument* templateArgument, SgUnparse_Info& info)
   {
      ASSERT_not_null(templateArgument);
   }


void
Unparse_X10::unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info & info)
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
Unparse_X10::unparseAssnExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
cout << "unparseAssignExpr" << endl;
   }

void
Unparse_X10::unparseVarRef(SgExpression* expr, SgUnparse_Info& info) {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ASSERT_not_null(var_ref);

     unparseName(var_ref->get_symbol()->get_name(), info);
}

void
Unparse_X10::unparseClassRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgClassNameRefExp* classname_ref = isSgClassNameRefExp(expr);
     ASSERT_not_null(classname_ref);

     curprint (  classname_ref->get_symbol()->get_declaration()->get_name().str());
   }

void
Unparse_X10::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     ASSERT_not_null(func_ref);

     cout << "unparser: warning. SgFunctionRef should be SgMemberFunctionRef: "
          << func_ref->get_symbol()->get_name().getString() << endl;
     unparseName(func_ref->get_symbol()->get_name(), info);
   }

void
Unparse_X10::unparseMFuncRef ( SgExpression* expr, SgUnparse_Info& info )
   {
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     ASSERT_not_null(mfunc_ref);
     unparseName(mfunc_ref->get_symbol()->get_name(), info);
   }

void
Unparse_X10::unparseBoolVal(SgExpression* expr, SgUnparse_Info& info) {
    SgBoolValExp *bool_val = isSgBoolValExp(expr);
    ASSERT_not_null(bool_val);
    curprint (bool_val->get_value() ? "true" : "false");
}

void
Unparse_X10::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgStringVal *str_val = isSgStringVal(expr);
     ASSERT_not_null(str_val);

  // Handle special case of macro specification (this is a temporary hack to permit us to
  // specify macros within transformations)

     int wrap = unp -> u_sage -> cur_get_linewrap();
     unp -> u_sage->cur_get_linewrap();

#ifndef CXX_IS_ROSE_CODE_GENERATION
     // TODO: Remove this
     /*
  // DQ (3/25/2006): Finally we can use the C++ string class
     string targetString = "ROSE-MACRO-CALL:";
     int targetStringLength = targetString.size();

     string tempString = str_val->get_value();
     string stringValue = "";
     for (int i = 0; i < tempString.length(); i++) { // Replace each occurrence of '\"' in the string by "\\\""
         if (tempString[i] == '\"')
              stringValue += "\\\"";
         else stringValue += tempString[i];
     }

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
          curprint ( "\"" + stringValue + "\"");
        }
     */

     string tempString = str_val -> get_value();
     string stringValue = Utf8::getPrintableJavaUnicodeString(tempString.c_str());
     curprint ( "\"" + stringValue + "\"");
#endif

     unp -> u_sage->cur_set_linewrap(wrap);
   }


/**
 * PC: override UnparseLanguageIndependentConstructs::unparseWCharVal(),
 */
void Unparse_X10::unparseWCharVal(SgExpression* expr, SgUnparse_Info& info) {
    SgWcharVal* wchar_val = isSgWcharVal(expr);
    ASSERT_not_null(wchar_val);

    if (wchar_val->get_valueString() == "") {
        int value = wchar_val->get_value();
        if (value < 256) {
            if (value == '\b') {
                curprint("\'\\b\'");
            }
            else if (value == '\t') {
                curprint("\'\\t\'");
            }
            else if (value == '\n') {
                curprint("\'\\n\'");
            }
            else if (value == '\f') {
                curprint("\'\\f\'");
            }
            else if (value == '\r') {
                curprint("\'\\r\'");
            }
            else if (value == '\"') {
                curprint("\'\\\"\'");
            }
            else if (value == '\'') {
                curprint("\'\\\'\'");
            }
            else if (value == '\\') {
                curprint("\'\\\\\'");
            }
            else if (isprint(value)) {
                string str = "\'";
                str += (char) value;
                str += "\'";
                curprint(str.c_str());
            }
            else {
                ostringstream octal; // stream used for the conversion
                octal.fill('0');
                octal.width(3);
                octal << oct << value; // compute the octal character representation of the value
                curprint("\'\\" + octal.str() + "\'");
            }
        }
        else {
            ostringstream unicode; // stream used for the conversion
            unicode.fill('0');
            unicode.width(4);
            unicode << hex << value; // compute the Unicode character representation of the value
            curprint("\'\\u" + unicode.str() + "\'");
        }
    }
    else {
        curprint(wchar_val->get_valueString());
    }
}

void
Unparse_X10::unparseUIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedIntVal* uint_val = isSgUnsignedIntVal(expr);
     ASSERT_not_null(uint_val);

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
Unparse_X10::unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongIntVal* longint_val = isSgLongIntVal(expr);
     ASSERT_not_null(longint_val);

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
Unparse_X10::unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongLongIntVal* longlongint_val = isSgLongLongIntVal(expr);
     ASSERT_not_null(longlongint_val);

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
Unparse_X10::unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedLongLongIntVal* ulonglongint_val = isSgUnsignedLongLongIntVal(expr);
     ASSERT_not_null(ulonglongint_val);

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
Unparse_X10::unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedLongVal* ulongint_val = isSgUnsignedLongVal(expr);
     ASSERT_not_null(ulongint_val);

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
Unparse_X10::unparseFloatVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFloatVal* float_val = isSgFloatVal(expr);
     ASSERT_not_null(float_val);

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.  Note that we can't use the C++
  // interface since the input program, and thus the generated code, might not have 
  // included the "limits" header file.
     float float_value = float_val->get_value();

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
Unparse_X10::unparseLongDoubleVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongDoubleVal* longdbl_val = isSgLongDoubleVal(expr);
     ASSERT_not_null(longdbl_val);
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
Unparse_X10::unparseComplexVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgComplexVal* complex_val = isSgComplexVal(expr);
     ASSERT_not_null(complex_val);

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


void replaceString (std::string& str, const std::string& from, const std::string& to); 

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
void Unparse_X10::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info) {
    SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
    ASSERT_not_null(func_call);
// TODO: Remove this !!!
/*
    if (func_call -> attributeExists("invocation_parameter_types")) {
        curprint("<");
        AstSgNodeListAttribute *parameter_types_attribute = (AstSgNodeListAttribute *) func_call -> getAttribute("invocation_parameter_types");
        for (int i = 0; i < parameter_types_attribute -> size(); i++) {
            SgType *type = isSgType(parameter_types_attribute -> getNode(i));
            ROSE_ASSERT(type);
            unparseType(type, info);
            if (i + 1 < parameter_types_attribute -> size()) {
                curprint(", ");
            }
        }
        curprint("> ");
    }
*/

        bool isConnectionUtil = 0;
        if (func_call -> attributeExists("prefix")) {
                AstRegExAttribute *attribute = (AstRegExAttribute *) func_call->getAttribute("prefix");
                string class_name("X10RoseUtility");
                string expr = attribute->expression;
                if (expr.compare(0, class_name.length(), class_name) == 0) {
                        isConnectionUtil = 1;
                }
        }
        if (isConnectionUtil) {
                SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(func_call->get_function());
                string funcName = mfunc_ref->get_symbol()->get_name(); 
                string parse = "Long_parse";
                if (funcName == "Rail_size") {
                        unparseExpression(func_call->get_args()->get_expressions()[0], info);
                        curprint(".size");
                }
                else if (funcName.compare(0, parse.length(), parse) == 0) {
                        curprint("Long.parse(");
  //                      curprint(funcName);
                        unparseExpression(func_call->get_args()->get_expressions()[0], info);
                        curprint(")");
                }

                return;
        }

        if (func_call -> attributeExists("prefix")) {
                AstRegExAttribute *attribute = (AstRegExAttribute *) func_call->getAttribute("prefix");
                string expr = attribute->expression;
                if (expr.compare(0, 2, "::") == 0) {
                    expr = expr.substr(2);
                }
                replaceString(expr, "::", ".");
                curprint(expr);
                curprint(".");
        }

    if (func_call -> attributeExists("function_parameter_types")) {
        AstRegExAttribute *attribute = (AstRegExAttribute *) func_call -> getAttribute("function_parameter_types");
        curprint(attribute -> expression);
        curprint(" ");
    }

    if (func_call -> attributeExists("<init>")) {
        AstRegExAttribute *constructor_attribute = (AstRegExAttribute *) func_call -> getAttribute("<init>");
        curprint(constructor_attribute -> expression);
    }
    else unparseExpression(func_call->get_function(), info);

    curprint("(");
    unparseExpression(func_call->get_args(), info);
    curprint(")");
}

void Unparse_X10::unparseUnaryMinusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "-", info); }
void Unparse_X10::unparseUnaryAddOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "+", info); }

void
Unparse_X10::unparseSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgSizeOfOp* sizeof_op = isSgSizeOfOp(expr);
     ASSERT_not_null(sizeof_op);

     curprint ( "sizeof(");
     if (sizeof_op->get_operand_expr() != NULL)
        {
          ASSERT_not_null(sizeof_op->get_operand_expr());
          unparseExpression(sizeof_op->get_operand_expr(), info);
        }
       else
        {
          ASSERT_not_null(sizeof_op->get_operand_type());
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
Unparse_X10::unparseTypeIdOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgTypeIdOp* typeid_op = isSgTypeIdOp(expr);
     ASSERT_not_null(typeid_op);
  /* code inserted from specification */

     curprint ( "typeid(");
     if (typeid_op->get_operand_expr() != NULL)
         {
           ASSERT_not_null(typeid_op->get_operand_expr());
           unparseExpression(typeid_op->get_operand_expr(), info);
         }
        else
         {
           ASSERT_not_null(typeid_op->get_operand_type());
           SgUnparse_Info info2(info);
           info2.unset_SkipBaseType();
           info2.set_SkipClassDefinition();
           unp->u_type->unparseType(typeid_op->get_operand_type(), info2);
         }
      curprint ( ")");
   }

void Unparse_X10::unparseNotOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "!", info); }
void Unparse_X10::unparseAbstractOp(SgExpression* expr, SgUnparse_Info& info) {}
void Unparse_X10::unparseBitCompOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "~", info); }

void
Unparse_X10::unparseExprCond(SgExpression* expr, SgUnparse_Info& info)
   {
     SgConditionalExp* expr_cond = isSgConditionalExp(expr);
     ASSERT_not_null(expr_cond);

     unparseExpression(expr_cond->get_conditional_exp(), info);
     curprint(" ? ");
     unparseExpression(expr_cond->get_true_exp(), info);
     curprint(" : ");
     unparseExpression(expr_cond->get_false_exp(), info);
   }

void
Unparse_X10::unparseClassInitOp(SgExpression* expr, SgUnparse_Info& info)
   {
   } 

void
Unparse_X10::unparseDyCastOp(SgExpression* expr, SgUnparse_Info& info)
   {
   }

void
Unparse_X10::unparseCastOp(SgExpression* expr, SgUnparse_Info& info) {
    SgCastExp* cast = isSgCastExp(expr);
    ASSERT_not_null(cast);

    curprint("(");
    unparseExpression(cast->get_operand(), info);
    curprint(") ");

        curprint("as ");
    AstRegExAttribute *attribute = (AstRegExAttribute *) cast -> getAttribute("type");
    if (attribute) {
        curprint(attribute -> expression);
    }
    else {
        unparseType(cast -> get_type(), info);
    }
}

void
Unparse_X10::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
   { 
    SgPntrArrRefExp *array_ref = isSgPntrArrRefExp(expr);
    ASSERT_not_null(array_ref);

    unparseExpression(array_ref -> get_lhs_operand(), info);
    curprint("(");
    unparseExpression(array_ref -> get_rhs_operand(), info);
    curprint(")");
   }

void
Unparse_X10::unparseNewOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("In Unparse_ExprStmt::unparseNewOp \n");
  // curprint ( "\n /* In Unparse_ExprStmt::unparseNewOp */ \n";

#ifndef CXX_IS_ROSE_CODE_GENERATION
     SgNewExp* new_op = isSgNewExp(expr);
     ASSERT_not_null(new_op);
  /* code inserted from specification */

     // charles4: 02/26/2012  Seems to be left-over code from C++
     /*
     if (new_op->get_need_global_specifier())
        {
       // DQ (1/5/2006): I don't think that we want the extra space after the "::".
       // curprint ( ":: ";
          curprint ( "::");
        }
     */

     //
     // If this is an allocation expression for an anonymous class, output the body of the class.
     //
// TODO: Remove this!
/*
     if (new_op -> attributeExists("new_prefix")) {
         AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) new_op -> getAttribute("new_prefix");
         SgExpression *prefix = isSgExpression(attribute -> getNode());
         ROSE_ASSERT(prefix);
         unparseExpression(prefix, info);
         curprint(".");
     }
*/
     curprint("new ");

  // curprint ( "\n /* Output any placement arguments */ \n";
     // charles4: 02/26/2012  I don't understand the importance of this code.
     /*
     SgUnparse_Info newinfo(info);
     newinfo.unset_inVarDecl();
     if (new_op->get_placement_args() != NULL)
        {
       // printf ("Output placement arguments for new operator \n");
       curprint ( "\n*//* Output placement arguments for new operator *//*\n");

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
     */
  // This fixes a bug having to do with the unparsing of the type name of constructors in return statements.

  // curprint ( "\n /* Output type name for new operator */ \n";

  // printf ("In Unparse_ExprStmt::unparseNewOp: new_op->get_type()->sage_class_name() = %s \n",new_op->get_type()->sage_class_name());

  // DQ (1/17/2006): The the type specified explicitly in the new expressions syntax, 
  // get_type() has been modified to return a pointer to new_op->get_specified_type().
  // unp->u_type->unparseType(new_op->get_type(), newinfo);

  //
  // charles4 - 02/26/2012 The specialized call below to unparse the type was replaced by the simpler call because it invokes the wrong function!
  //
     /*
     unp->u_type->unparseType(new_op->get_specified_type(), newinfo);
     */
     if (isSgArrayType(new_op -> get_specified_type())) {
// TODO: Remove this!
/*
         SgArrayType *array_type = isSgArrayType(new_op -> get_specified_type());
         while(isSgArrayType(array_type -> get_base_type())) { // find the base type...
             array_type = isSgArrayType(array_type -> get_base_type());
         }

         if (isSgDotExp(new_op -> get_parent()) && isSgClassType(new_op -> get_specified_type())) {
             SgClassType *class_type = isSgClassType(array_type -> get_base_type());
             ROSE_ASSERT(class_type);
             curprint(class_type -> get_name().getString());
         }
         else {
*/
// TODO: Remove this!
//             unparseType(pointer_type -> get_base_type(), info);

                        curprint("Rail[");
             AstRegExAttribute *attribute = (AstRegExAttribute *) new_op -> getAttribute("type");
             if (attribute) {
                 curprint(attribute -> expression);
             }
             else {
#if 0
              // DQ (3/28/2017): Original code (warning generated by Clang).
                 SgType *type = new_op -> get_specified_type();
                 do {
                     SgType *type = isSgArrayType(type) -> get_base_type();
                 } while (isSgArrayType(type));
#else
              // DQ (3/28/2017): Rewritten code (warning free from Clang).
                 SgType *type = new_op -> get_specified_type();
                 do {
                     SgType *inner_type = isSgArrayType(type) -> get_base_type();
                     type = inner_type;
                 } while (isSgArrayType(type));
#endif
                 unparseType(type, info);
             }
                                curprint("]");
//         }

         bool has_aggregate_initializer = new_op -> attributeExists("initializer");
         SgConstructorInitializer *init = new_op -> get_constructor_args();
         ROSE_ASSERT(init);
         vector<SgExpression *> args = init -> get_args() -> get_expressions();
         for (size_t i = 0; i < args.size(); i++) {
//             curprint ("[");
             curprint ("(");
             if (! has_aggregate_initializer) {
                 unparseExpression(args[i], info);
             }
//             curprint("]");
             curprint(")");
         }

         //
         // If this array allocation expression contains an aggregate initializer process it now.
         //
         if (has_aggregate_initializer) {
             AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) new_op -> getAttribute("initializer");
             SgAggregateInitializer *initializer = isSgAggregateInitializer(attribute -> getNode());
             ROSE_ASSERT(initializer);
             unparseAggrInit(initializer, info);
         }
     }
     else {
// TODO: Remove this!
/*
         if (isSgDotExp(new_op -> get_parent()) && isSgClassType(new_op->get_specified_type())) {
             SgClassType *class_type = isSgClassType(new_op->get_specified_type());
             ROSE_ASSERT(class_type);
             curprint(class_type -> get_name().getString());
         }
         else {
*/
// TODO: Remove this!
//             unparseType(new_op->get_specified_type(), info);


             AstRegExAttribute *attribute = (AstRegExAttribute *) new_op -> getAttribute("type");
             if (attribute) {
                 curprint(attribute -> expression);
             }
             else {
                 unparseType(new_op -> get_specified_type(), info);
             }
//         }

         curprint ("(");
         ROSE_ASSERT(new_op -> get_constructor_args());
         SgConstructorInitializer *init = new_op -> get_constructor_args();
         ROSE_ASSERT(init);
         vector<SgExpression *> args = init -> get_args() -> get_expressions();
         for (size_t i = 0; i < args.size(); i++) {
             unparseExpression(args[i], info);
             if (i + 1 < args.size())
                 curprint(", ");
         }
         curprint (")");

         //
         // Check if this is an allocation expression for an anonymous class. In such a case, output the body of the class.
         //
         if (new_op -> attributeExists("body")) {
             AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) new_op -> getAttribute("body");
             SgClassDeclaration *class_declaration = isSgClassDeclaration(attribute -> getNode());
             ROSE_ASSERT(class_declaration);
             unparseClassDefnStmt(class_declaration -> get_definition(), info);
         }
     }

  // printf ("DONE: new_op->get_type()->sage_class_name() = %s \n",new_op->get_type()->sage_class_name());

  // curprint ( "\n /* Output constructor args */ \n";
     // charles4: 02/26/2012  I don't understand the importance of this code.
     /*
     if (new_op->get_constructor_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_constructor_args() \n");
          unparseExpression(new_op->get_constructor_args(), newinfo);
        }
     else curprint ( "()"); // charles4: Still need parentheses for empty argument list.
     */
//
// chares4: I don't understand what the purpose of the unreachable "else" block below is.
//          I added the "else " alternative above because it is required for the Unparser
//          to generate correct code.  If it turns out that the else block below is important
//          then its creator should merge it with the else statement that I added above like this:
//
// -----------------------------------------------------------------------------------------------
//
//               else
//                {
//                  curprint ( "()"); // charles4: Still need parentheses for empty argument list.
//        #if 0
//               // printf ("In Unparse_ExprStmt::unparseNewOp: Call unparse type \n");
//                  unp->u_type->unparseType(new_op->get_type(), newinfo);
//        #endif
//                }
//
// -----------------------------------------------------------------------------------------------
//
#if 0
       else
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Call unparse type \n");
          unp->u_type->unparseType(new_op->get_type(), newinfo);
        }
#endif

  // curprint ( "\n /* Output builtin args */ \n";
     /*
     if (new_op->get_builtin_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_builtin_args() \n");
          unparseExpression(new_op->get_builtin_args(), newinfo);
        }
     */
  // curprint ( "\n /* Leaving Unparse_ExprStmt::unparseNewOp */ \n";
  // printf ("Leaving Unparse_ExprStmt::unparseNewOp \n");
#endif
   }

void
Unparse_X10::unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgDeleteExp* delete_op = isSgDeleteExp(expr);
     ASSERT_not_null(delete_op);
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
Unparse_X10::unparseThisNode(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgThisExp* this_node = isSgThisExp(expr);

     ASSERT_not_null(this_node);

  // printf ("In Unparse_ExprStmt::unparseThisNode: unp->opt.get_this_opt() = %s \n", (unp->opt.get_this_opt()) ? "true" : "false");

     if (unp->opt.get_this_opt()) // Checks options to determine whether to print "this"  
        {
          curprint ("this"); 
        }
   }

void
Unparse_X10::unparseSuperNode(SgExpression* expr, SgUnparse_Info& info) {
    SgSuperExp* super_node = isSgSuperExp(expr);

    ASSERT_not_null(super_node);
    curprint ("super"); 
}

void
Unparse_X10::unparseClassNode(SgExpression* expr, SgUnparse_Info& info) {
    SgClassExp* class_node = isSgClassExp(expr);

    ASSERT_not_null(class_node);
    curprint ("class"); 
}

void
Unparse_X10::unparseScopeOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgScopeOp* scope_op = isSgScopeOp(expr);
     ASSERT_not_null(scope_op);
  /* code inserted from specification */

     if (scope_op->get_lhs_operand())
          unparseExpression(scope_op->get_lhs_operand(), info);
     curprint ( "::");
     unparseExpression(scope_op->get_rhs_operand(), info);
   }

void Unparse_X10::unparseForDeclOp(SgExpression* expr, SgUnparse_Info& info) {}

void
Unparse_X10::unparseTypeRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgRefExp* type_ref = isSgRefExp(expr);
     ASSERT_not_null(type_ref);
  /* code inserted from specification */

     SgUnparse_Info newinfo(info);
     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();
  
     unp->u_type->unparseType(type_ref->get_type_name(), newinfo);
   }

void Unparse_X10::unparseVConst(SgExpression* expr, SgUnparse_Info& info) {}
void Unparse_X10::unparseExprInit(SgExpression* expr, SgUnparse_Info& info) {}

// TODO: Remove this ... PC -03/16/2014
/*
// Liao 11/3/2010
// Sometimes initializers can from an included file
//  SgAssignInitializer -> SgCastExp ->SgCastExp ->SgIntVal
// We should not unparse them
// This function will check if the nth initializer is from a different file from the aggregate initializer
static bool isFromAnotherFile (SgLocatedNode* lnode)
{
  bool result = false;
  ASSERT_not_null(lnode);
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
Unparse_X10::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
    // Skip the entire thing if the initializer is from an included file
     if (isFromAnotherFile (expr))
       return;
     SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
     ASSERT_not_null(aggr_init);
  // code inserted from specification

     SgUnparse_Info newinfo(info);
     if (aggr_init->get_need_explicit_braces())
      curprint ( "{");
     SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();
     size_t last_index = list.size() -1;

     for (size_t index = 0; index < list.size(); index ++)
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
*/

void
Unparse_X10::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info) {
    SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
    ASSERT_not_null(aggr_init);

    SgUnparse_Info newinfo(info);
    curprint ("{");
    SgExpressionPtrList& list = aggr_init -> get_initializers() -> get_expressions();
    for (size_t index = 0; index < list.size(); index ++) {
        if (index > 0)
             curprint ( ", ");
        unparseExpression(list[index], newinfo);
    }
    curprint ("}");
}

void
Unparse_X10::unparseConInit(SgExpression *expr, SgUnparse_Info& info)
{
    SgConstructorInitializer *init = isSgConstructorInitializer(expr);
    ROSE_ASSERT(init);
    vector<SgExpression *> args = init -> get_args() -> get_expressions();
    for (size_t i = 0; i < args.size(); i++) {
        unparseExpression(args[i], info);
        if (i + 1 < args.size())
            curprint(", ");
    }
}

void
Unparse_X10::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ASSERT_not_null(assn_init);
     unparseExpression(assn_init->get_operand(), info);
   }

void
Unparse_X10::unparseThrowOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgThrowOp* throw_op = isSgThrowOp(expr);
     ASSERT_not_null(throw_op);

  // printf ("In unparseThrowOp(%s) \n",expr->sage_class_name());
  // curprint ( "\n/* In unparseThrowOp(" + expr->sage_class_name() + ") */ \n";
   }


void
Unparse_X10::unparseDesignatedInitializer(SgExpression* expr, SgUnparse_Info & info)
   {
   }

void
Unparse_X10::unparseX10InstanceOfOp(SgExpression* expr, SgUnparse_Info & info) {
    SgJavaInstanceOfOp* inst_op = isSgJavaInstanceOfOp(expr);
    ASSERT_not_null(inst_op);

    unparseExpression(inst_op->get_operand_expr(), info);
    curprint(" instanceof ");

    AstRegExAttribute *attribute = (AstRegExAttribute *) inst_op -> getAttribute("type");
    if (attribute) {
        curprint(attribute -> expression);
    }
    else {
        unparseType(inst_op -> get_operand_type(), info);
    }
}

void
Unparse_X10::unparsePseudoDtorRef(SgExpression* expr, SgUnparse_Info & info)
   {
   }

void
Unparse_X10::unparseCompoundAssignOp(SgCompoundAssignOp* op,
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
Unparse_X10::unparseBinaryOp(SgBinaryOp* op,
                              SgUnparse_Info & info) {
        // MH-20140807
        if (requiresParentheses(op, info)) 
                curprint("(");

//cout << "BINARY LEFT=" << op->get_lhs_operand()->get_type()->get_mangled().str() << endl;
        unparseExpression(op->get_lhs_operand(), info);

    switch (op->variantT()) {
        case V_SgAddOp:                curprint(" + ");   break;
        case V_SgAndOp:                curprint(" && ");  break;
        case V_SgAssignOp:             curprint(" = ");   break;
        case V_SgBitAndOp:             curprint(" & ");   break;
        case V_SgBitOrOp:              curprint(" | ");   break;
        case V_SgBitXorOp:             curprint(" ^ ");   break;
        case V_SgDivideOp:             curprint(" / ");   break;
        case V_SgDotExp:               curprint(".");     break; // charles4: There is no Dot operation per se in X10
        case V_SgArrowExp:             curprint(".");     break; // charles4: The X10 dot operator is equivalent to the C Arrow operator
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
        case V_SgCommaOpExp:           curprint(", ");    break; // charles4 10/14/2011
        default: {
             cout << "error: cannot unparse binary op: " << op->class_name() << endl;
             ROSE_ASSERT(false);
        }
    }
    unparseExpression(op->get_rhs_operand(), info);

        if (requiresParentheses(op, info)) 
                curprint(")");
}

void
Unparse_X10::unparseUnaryOp(SgUnaryOp* op,
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

/**
 * When parsing X10 source code, parentheses are stored in an AST attributes.
 * Although we know the exact number of parentheses specified by the user we only
 * output one pair, mostly to conform with rose's API that returns a bool.
 */
bool
Unparse_X10::requiresParentheses(SgExpression* expr, SgUnparse_Info& info) {
    const string attributeName = "x10-parenthesis-info";
    if (expr->attributeExists(attributeName)) {
        AstRegExAttribute * attribute = (AstRegExAttribute *) expr->getAttribute(attributeName);
        string parentheses = attribute->expression.c_str();
        ROSE_ASSERT(parentheses.size() > 0);
        return true;
    }

    return false;
}

//
// PC: This function was copyed from Unparselanguageindependentconstructs::unparseExpression(SgExpression* expr, SgUnparse_Info & info)
//
//-----------------------------------------------------------------------------------
//  void Unparse_X10::unparseExpression
//  
//  General unparse function for expressions. Then it routes to the appropriate 
//  function to unparse each kind of expression. Type and symbols still use the 
//  original unparse function because they don't have file_info and therefore, 
//  will not print out file information
//-----------------------------------------------------------------------------------
void Unparse_X10::unparseExpression(SgExpression *expr, SgUnparse_Info &info) {
    ASSERT_not_null(expr);

    switch (expr->variant()) {
        // DQ (8/15/2007): This has been moved to the base class
        case EXPR_LIST: { unparseExprList(expr, info); break; }

        // DQ: These cases are separated out so that we can handle the 
        // original expression tree from any possible constant folding by EDG.
        case BOOL_VAL:
        case SHORT_VAL:
        case CHAR_VAL:
        case UNSIGNED_CHAR_VAL:
        case WCHAR_VAL:
        case STRING_VAL:
        case UNSIGNED_SHORT_VAL:
        case ENUM_VAL:
        case INT_VAL:
        case UNSIGNED_INT_VAL:
        case LONG_INT_VAL:
        case LONG_LONG_INT_VAL:
        case UNSIGNED_LONG_LONG_INT_VAL:
        case UNSIGNED_LONG_INT_VAL:
        case FLOAT_VAL:
        case DOUBLE_VAL:
        case LONG_DOUBLE_VAL:
        case COMPLEX_VAL: {
            unparseValue(expr, info);
            break;
        }
        default: {
             unparseLanguageSpecificExpression(expr, info);
             break;
        }
    }
}

void Unparse_X10::unparseUnaryExpr(SgExpression *expr, SgUnparse_Info &info) {
    SgUnaryOp *unary_op = isSgUnaryOp(expr);
    ASSERT_not_null(unary_op);

    if (unary_op->get_mode() != SgUnaryOp::postfix) { // a prefix unary operator?
        //
        // We have to be careful so that two consecutive unary '+' or '-' are not output
        // as "++" or "--". Hence the extra space before a prefix operator
        //
        curprint(" "); 
        curprint(info.get_operator_name());
    }

    unparseExpression(unary_op->get_operand(), info);

    if (unary_op->get_mode() == SgUnaryOp::postfix) { // a postfix unary operator?
        curprint (info.get_operator_name()); 
    }
}


void Unparse_X10::unparseBinaryExpr(SgExpression *expr, SgUnparse_Info &info) {
    SgBinaryOp *binary_op = isSgBinaryOp(expr);
    ASSERT_not_null(binary_op);


    // DQ (2/7/2011): Unparser support for more general originalExpressionTree handling.
    SgExpression *expressionTree = binary_op->get_originalExpressionTree();
    if (expressionTree != NULL) {
        unparseExpression(expressionTree, info);
        return;
    }
    SgExpression *lhs = binary_op->get_lhs_operand();
    ASSERT_not_null(lhs);
    unparseExpression(lhs, info);

    curprint(info.get_operator_name()); 

    SgExpression *rhs = binary_op->get_rhs_operand();
    ASSERT_not_null(rhs);
    unparseExpression(rhs, info);
}


void 
Unparse_X10::unparseX10MarkerAnnotation(SgExpression *expr, SgUnparse_Info& info) {
    SgJavaMarkerAnnotation *marker_annotation = isSgJavaMarkerAnnotation(expr);
    // SgClassType *type = isSgClassType(marker_annotation -> get_type());
    // ROSE_ASSERT(type);
    // curprint("@");
    // curprint(type -> get_name().getString());
    curprint("@");
    AstRegExAttribute *attribute = (AstRegExAttribute *) marker_annotation -> getAttribute("type");
    if (attribute) {
        curprint(attribute -> expression);
    }
    else {
        unparseType(marker_annotation -> get_type(), info);
    }
}


void
Unparse_X10::unparseX10SingleMemberAnnotation(SgExpression *expr, SgUnparse_Info& info) {
    SgJavaSingleMemberAnnotation *single_member_annotation = isSgJavaSingleMemberAnnotation(expr);
    // SgClassType *type = isSgClassType(single_member_annotation -> get_type());
    // ROSE_ASSERT(type);
    // curprint("@");
    // curprint(type -> get_name().getString());
    curprint("@");
    AstRegExAttribute *attribute = (AstRegExAttribute *) single_member_annotation -> getAttribute("type");
    if (attribute) {
        curprint(attribute -> expression);
    }
    else {
        unparseType(single_member_annotation -> get_type(), info);
    }

    curprint("(");
    unparseExpression(single_member_annotation -> get_value(), info);
    curprint(")");
}

void
Unparse_X10::unparseX10NormalAnnotation(SgExpression *expr, SgUnparse_Info& info) {
    SgJavaNormalAnnotation *normal_annotation = isSgJavaNormalAnnotation(expr);
    // SgClassType *type = isSgClassType(normal_annotation -> get_type());
    // ROSE_ASSERT(type);
    // curprint("@");
    // curprint(type -> get_name().getString());
    curprint("@");
    AstRegExAttribute *attribute = (AstRegExAttribute *) normal_annotation -> getAttribute("type");
    if (attribute) {
        curprint(attribute -> expression);
    }
    else {
        unparseType(normal_annotation -> get_type(), info);
    }

    SgJavaMemberValuePairPtrList &pair_list = normal_annotation -> get_value_pair_list();
    curprint("(");
    for (size_t k = 0; k < pair_list.size(); k++) {
        SgJavaMemberValuePair *pair = pair_list[k];
        if (k > 0) {
            curprint(", ");
        }
        curprint(pair -> get_name().getString());
        curprint(" = ");
        unparseExpression(pair -> get_value(), info);
    }
    curprint(")");
}


void
Unparse_X10::unparseX10TypeExpression(SgExpression *expr, SgUnparse_Info& info) {
    SgJavaTypeExpression *type_expression = isSgJavaTypeExpression(expr);
    AstRegExAttribute *attribute = (AstRegExAttribute *) type_expression -> getAttribute("type");
    if (attribute) {
        string expr = attribute->expression;
        if (expr.compare(0, 2, "::") == 0) {
           expr = expr.substr(2);
        }
        replaceString(expr, "::", ".");
        curprint(expr);
    }
    else {
        unparseType(type_expression -> get_type(), info);
    }
}

void
Unparse_X10::unparseHereExpression(SgExpression *expr, SgUnparse_Info& info) {
 // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
 // SgHereExp *here_expression = isSgHereExp(expr);
    curprint("here");
}
