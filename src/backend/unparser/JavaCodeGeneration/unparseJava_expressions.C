/* unparseJava_expressions.C
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


void Unparse_Java::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info) {
  // This is the Java specific expression code generation

    //
    // Check if this expression requires parentheses.  If so, process the opening parentheses now.
    //
    AstIntAttribute *parenthesis_attribute = (AstIntAttribute *) expr->getAttribute("java-parentheses-count");
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
        case JAVA_INSTANCEOF_OP:      { unparseJavaInstanceOfOp(expr, info); break; }

        case V_SgJavaMarkerAnnotation:       { unparseJavaMarkerAnnotation(expr, info); break; }
        case V_SgJavaSingleMemberAnnotation: { unparseJavaSingleMemberAnnotation(expr, info); break; }
        case V_SgJavaNormalAnnotation:       { unparseJavaNormalAnnotation(expr, info); break; }

        case V_SgJavaTypeExpression:         { unparseJavaTypeExpression(expr, info); break; }


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
                     ROSE_ABORT();
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
        case V_SgCommaOpExp:                   return 1; // charles4 10/14/2011

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

void
Unparse_Java::unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info &)
   {
     ASSERT_not_null(templateInstantiationDeclaration);
   }

void
Unparse_Java::unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info &)
   {
     ASSERT_not_null(templateParameter);
     printf ("unparseTemplateParameter(): Sorry, not implemented! \n");
     ROSE_ABORT();
   }


void
Unparse_Java::unparseTemplateArgument(SgTemplateArgument* templateArgument, SgUnparse_Info &)
   {
      ASSERT_not_null(templateArgument);
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
     unparseUnaryExpr(expr, newinfo);

  //
  // Now set the "this" option back to its original state
  //
     if( !orig_this_opt )
          unp->opt.set_this_opt(false);
   }

void
Unparse_Java::unparseAssnExpr(SgExpression*, SgUnparse_Info &)
   {
   }

void
Unparse_Java::unparseVarRef(SgExpression* expr, SgUnparse_Info &info) {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ASSERT_not_null(var_ref);

     unparseName(var_ref->get_symbol()->get_name(), info);
}

void
Unparse_Java::unparseClassRef(SgExpression* expr, SgUnparse_Info &)
   {
     SgClassNameRefExp* classname_ref = isSgClassNameRefExp(expr);
     ASSERT_not_null(classname_ref);

     curprint (  classname_ref->get_symbol()->get_declaration()->get_name().str());
   }

void
Unparse_Java::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     ASSERT_not_null(func_ref);

     cout << "unparser: warning. SgFunctionRef should be SgMemberFunctionRef: "
          << func_ref->get_symbol()->get_name().getString() << endl;
     unparseName(func_ref->get_symbol()->get_name(), info);
   }

void
Unparse_Java::unparseMFuncRef ( SgExpression* expr, SgUnparse_Info& info )
   {
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     ASSERT_not_null(mfunc_ref);

     unparseName(mfunc_ref->get_symbol()->get_name(), info);
   }

void
Unparse_Java::unparseBoolVal(SgExpression* expr, SgUnparse_Info &) {
    SgBoolValExp *bool_val = isSgBoolValExp(expr);
    ASSERT_not_null(bool_val);
    curprint (bool_val->get_value() ? "true" : "false");
}

void
Unparse_Java::unparseStringVal(SgExpression* expr, SgUnparse_Info &)
   {
     SgStringVal *str_val = isSgStringVal(expr);
     ASSERT_not_null(str_val);

  // Handle special case of macro specification (this is a temporary hack to permit us to
  // specify macros within transformations)

     int wrap = unp -> u_sage -> cur_get_linewrap();
     unp -> u_sage->cur_get_linewrap();
     unp -> u_sage->cur_set_linewrap(wrap);
   }


/**
 * PC: override UnparseLanguageIndependentConstructs::unparseWCharVal(),
 */
void Unparse_Java::unparseWCharVal(SgExpression* expr, SgUnparse_Info &) {
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
Unparse_Java::unparseUIntVal(SgExpression* expr, SgUnparse_Info &)
   {
     SgUnsignedIntVal* uint_val = isSgUnsignedIntVal(expr);
     ASSERT_not_null(uint_val);

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
Unparse_Java::unparseLongIntVal(SgExpression* expr, SgUnparse_Info &)
   {
     SgLongIntVal* longint_val = isSgLongIntVal(expr);
     ASSERT_not_null(longint_val);

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
Unparse_Java::unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info &)
   {
     SgLongLongIntVal* longlongint_val = isSgLongLongIntVal(expr);
     ASSERT_not_null(longlongint_val);

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
Unparse_Java::unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info &)
   {
     SgUnsignedLongLongIntVal* ulonglongint_val = isSgUnsignedLongLongIntVal(expr);
     ASSERT_not_null(ulonglongint_val);

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
Unparse_Java::unparseULongIntVal(SgExpression* expr, SgUnparse_Info &)
   {
     SgUnsignedLongVal* ulongint_val = isSgUnsignedLongVal(expr);
     ASSERT_not_null(ulongint_val);

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
Unparse_Java::unparseFloatVal(SgExpression* expr, SgUnparse_Info &)
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
          curprint ( "__builtin_huge_valf()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ((float_value != float_value) || (float_value == std::numeric_limits<float>::quiet_NaN()) )
             {
               curprint ( "__builtin_nanf (\"\")");
             }
            else
             {
               if (float_value == std::numeric_limits<float>::signaling_NaN())
                  {
                    curprint ( "__builtin_nansf (\"\")");
                  }
                 else
                  {
                 // typical case!
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
Unparse_Java::unparseLongDoubleVal(SgExpression* expr, SgUnparse_Info &)
   {
     SgLongDoubleVal* longdbl_val = isSgLongDoubleVal(expr);
     ASSERT_not_null(longdbl_val);
  /* code inserted from specification */
  
     long double longDouble_value = longdbl_val->get_value();
     if (longDouble_value == std::numeric_limits<long double>::infinity())
        {
          curprint ( "__builtin_huge_vall()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ( (longDouble_value != longDouble_value) || (longDouble_value == std::numeric_limits<long double>::quiet_NaN()) )
             {
               curprint ( "__builtin_nanl (\"\")");
             }
            else
             {
               if (longDouble_value == std::numeric_limits<long double>::signaling_NaN())
                  {
                    curprint ( "__builtin_nansl (\"\")");
                  }
                 else
                  {
                 // typical case!
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
void Unparse_Java::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info) {
    SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
    ASSERT_not_null(func_call);

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

void Unparse_Java::unparseUnaryMinusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "-", info); }
void Unparse_Java::unparseUnaryAddOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "+", info); }

void
Unparse_Java::unparseSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
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
Unparse_Java::unparseTypeIdOp(SgExpression* expr, SgUnparse_Info& info)
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

void Unparse_Java::unparseNotOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "!", info); }
void Unparse_Java::unparseAbstractOp(SgExpression*, SgUnparse_Info &) {}
void Unparse_Java::unparseBitCompOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "~", info); }

void
Unparse_Java::unparseExprCond(SgExpression* expr, SgUnparse_Info& info)
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
Unparse_Java::unparseClassInitOp(SgExpression*, SgUnparse_Info &)
   {
   } 

void
Unparse_Java::unparseDyCastOp(SgExpression*, SgUnparse_Info &)
   {
   }

void
Unparse_Java::unparseCastOp(SgExpression* expr, SgUnparse_Info& info) {
    SgCastExp* cast = isSgCastExp(expr);
    ASSERT_not_null(cast);

    curprint("(");
    AstRegExAttribute *attribute = (AstRegExAttribute *) cast -> getAttribute("type");
    if (attribute) {
        curprint(attribute -> expression);
    }
    else {
        unparseType(cast -> get_type(), info);
    }
    curprint(") ");

    curprint("(");
    unparseExpression(cast->get_operand(), info);
    curprint(") ");
}

void
Unparse_Java::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
   { 
    SgPntrArrRefExp *array_ref = isSgPntrArrRefExp(expr);
    ASSERT_not_null(array_ref);

    unparseExpression(array_ref -> get_lhs_operand(), info);
    curprint("[");
    unparseExpression(array_ref -> get_rhs_operand(), info);
    curprint("]");
   }

void
Unparse_Java::unparseNewOp(SgExpression* expr, SgUnparse_Info& info)
   {
#ifndef CXX_IS_ROSE_CODE_GENERATION
     SgNewExp* new_op = isSgNewExp(expr);
     ASSERT_not_null(new_op);
  /* code inserted from specification */

     //
     // If this is an allocation expression for an anonymous class, output the body of the class.
     //
     curprint("new ");

     if (isSgArrayType(new_op -> get_specified_type())) {
             AstRegExAttribute *attribute = (AstRegExAttribute *) new_op -> getAttribute("type");
             if (attribute) {
                 curprint(attribute -> expression);
             }
             else {
                 SgType *type = new_op -> get_specified_type();
                 do {
                     ASSERT_not_null(isSgArrayType(type));
                     type = isSgArrayType(type) -> get_base_type();
                    } 
                 while (isSgArrayType(type));
                 unparseType(type, info);
             }

         bool has_aggregate_initializer = new_op -> attributeExists("initializer");
         SgConstructorInitializer *init = new_op -> get_constructor_args();
         ROSE_ASSERT(init);
         vector<SgExpression *> args = init -> get_args() -> get_expressions();
         for (size_t i = 0; i < args.size(); i++) {
             curprint ("[");
             if (! has_aggregate_initializer) {
                 unparseExpression(args[i], info);
             }
             curprint("]");
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
             AstRegExAttribute *attribute = (AstRegExAttribute *) new_op -> getAttribute("type");
             if (attribute) {
                 curprint(attribute -> expression);
             }
             else {
                 unparseType(new_op -> get_specified_type(), info);
             }

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
#endif
   }

void
Unparse_Java::unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info)
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
Unparse_Java::unparseThisNode(SgExpression* expr, SgUnparse_Info &)
   {
     SgThisExp* this_node = isSgThisExp(expr);

     ASSERT_not_null(this_node);

     if (unp->opt.get_this_opt()) // Checks options to determine whether to print "this"  
        {
          curprint ("this"); 
        }
   }

void
Unparse_Java::unparseSuperNode(SgExpression* expr, SgUnparse_Info &) {
    SgSuperExp* super_node = isSgSuperExp(expr);

    ASSERT_not_null(super_node);
    curprint ("super"); 
}

void
Unparse_Java::unparseClassNode(SgExpression* expr, SgUnparse_Info &) {
    SgClassExp* class_node = isSgClassExp(expr);

    ASSERT_not_null(class_node);
    curprint ("class"); 
}

void
Unparse_Java::unparseScopeOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgScopeOp* scope_op = isSgScopeOp(expr);
     ASSERT_not_null(scope_op);
  /* code inserted from specification */

     if (scope_op->get_lhs_operand())
          unparseExpression(scope_op->get_lhs_operand(), info);
     curprint ( "::");
     unparseExpression(scope_op->get_rhs_operand(), info);
   }

void
Unparse_Java::unparseForDeclOp(SgExpression*, SgUnparse_Info &) {}

void
Unparse_Java::unparseTypeRef(SgExpression* expr, SgUnparse_Info &info)
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

void Unparse_Java::unparseVConst(SgExpression*, SgUnparse_Info &) {}
void Unparse_Java::unparseExprInit(SgExpression*, SgUnparse_Info &) {}

void
Unparse_Java::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info) {
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
Unparse_Java::unparseConInit(SgExpression *expr, SgUnparse_Info& info)
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
Unparse_Java::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ASSERT_not_null(assn_init);
     unparseExpression(assn_init->get_operand(), info);
   }

void
Unparse_Java::unparseThrowOp(SgExpression* expr, SgUnparse_Info &)
   {
     SgThrowOp* throw_op = isSgThrowOp(expr);
     ASSERT_not_null(throw_op);
   }


void
Unparse_Java::unparseDesignatedInitializer(SgExpression*, SgUnparse_Info &)
   {
   }

void
Unparse_Java::unparseJavaInstanceOfOp(SgExpression* expr, SgUnparse_Info & info) {
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
Unparse_Java::unparsePseudoDtorRef(SgExpression*, SgUnparse_Info &)
   {
   }

void
Unparse_Java::unparseCompoundAssignOp(SgCompoundAssignOp* op, SgUnparse_Info &info) {
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
             ROSE_ABORT();
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
        case V_SgDotExp:               curprint(".");     break; // charles4: There is no Dot operation per se in Java
        case V_SgArrowExp:             curprint(".");     break; // charles4: The Java dot operator is equivalent to the C Arrow operator
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
             ROSE_ABORT();
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

/**
 * When parsing Java source code, parentheses are stored in an AST attributes.
 * Although we know the exact number of parentheses specified by the user we only
 * output one pair, mostly to conform with rose's API that returns a bool.
 */
bool
Unparse_Java::requiresParentheses(SgExpression* expr, SgUnparse_Info &) {
    const string attributeName = "java-parenthesis-info";
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
//  void Unparse_Java::unparseExpression
//  
//  General unparse function for expressions. Then it routes to the appropriate 
//  function to unparse each kind of expression. Type and symbols still use the 
//  original unparse function because they don't have file_info and therefore, 
//  will not print out file information
//-----------------------------------------------------------------------------------
void Unparse_Java::unparseExpression(SgExpression *expr, SgUnparse_Info &info) {
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

void Unparse_Java::unparseUnaryExpr(SgExpression *expr, SgUnparse_Info &info) {
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


void Unparse_Java::unparseBinaryExpr(SgExpression *expr, SgUnparse_Info &info) {
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
Unparse_Java::unparseJavaMarkerAnnotation(SgExpression *expr, SgUnparse_Info& info) {
    SgJavaMarkerAnnotation *marker_annotation = isSgJavaMarkerAnnotation(expr);
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
Unparse_Java::unparseJavaSingleMemberAnnotation(SgExpression *expr, SgUnparse_Info& info) {
    SgJavaSingleMemberAnnotation *single_member_annotation = isSgJavaSingleMemberAnnotation(expr);
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
Unparse_Java::unparseJavaNormalAnnotation(SgExpression *expr, SgUnparse_Info& info) {
    SgJavaNormalAnnotation *normal_annotation = isSgJavaNormalAnnotation(expr);
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
Unparse_Java::unparseJavaTypeExpression(SgExpression *expr, SgUnparse_Info& info) {
    SgJavaTypeExpression *type_expression = isSgJavaTypeExpression(expr);
    AstRegExAttribute *attribute = (AstRegExAttribute *) type_expression -> getAttribute("type");
    if (attribute) {
        curprint(attribute -> expression);
    }
    else {
        unparseType(type_expression -> get_type(), info);
    }
}
