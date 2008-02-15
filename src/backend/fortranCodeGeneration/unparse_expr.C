/* unparse_expr_f90.C
 * 
 * This C file contains the general unparse function for expressions
 * and functions to unparse every kind of expression. Note that there
 * are no definitions for the following functions (Sage didn't provide
 * this): AssnExpr, ExprRoot, AbstractOp, ClassInit, DyCast, ForDecl,
 * VConst, and ExprInit.
 * 
 * NOTE: Look over WCharVal. Sage provides no public function to
 * access p_valueUL, so just use p_value for now. When Sage is
 * rebuilt, we should be able to fix this.
 *
 */

#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "rose.h"
#include "unparser.h"

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0

//-----------------------------------------------------------------------------------
//  void Unparser::unparseExpression
//  
//  General unparse function for expressions. Then it routes to the appropriate 
//  function to unparse each kind of expression. Type and symbols still use the 
//  original unparse function because they don't have file_info and therefore, 
//  will not print out file information
//-----------------------------------------------------------------------------------
void Unparser::unparseExpression(SgExpression* expr, SgUnparse_Info& info)
   {
  // directives(expr);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Unparse expression (%p): sage_class_name() = %s \n",expr,expr->sage_class_name());
     cur << "\n/* Top of unparseExpression() " << string(expr->sage_class_name()) << " */\n";
#endif

  // DQ (3/21/2004): This assertion should have been in place before now!
     ROSE_ASSERT (expr != NULL);

  // MS 2003: experimental backend source replacement
  // Either use the source string attached to the AST by a transformation
  // (and do not traverse the subtree with 'epxr' as its root node)
  // OR unparse the expression (the whole subtree)
     if (expr->attribute.exists("_UnparserSourceReplacement"))
        {
          string rep=(expr->attribute["_UnparserSourceReplacement"])->toString();
          cout << "UNPARSER: SOURCE REPLACEMENT:" << rep << endl;
          cur << rep;
        }
       else
        {
       // DQ (5/21/2004): revised need_paren handling in EDG/SAGE III and within SAGE III IR)
       // QY (7/9/2004): revised to use the new PrintStartParen test
          bool printParen = PrintStartParen(expr,info);
          if (printParen)
             {
            // Make sure this is not an expresion list
               ROSE_ASSERT (isSgExprListExp(expr) == NULL);

            // Output the left paren
               cur << "(";
            // cur << "/* " << expr->sage_class_name() << " */ (";
             }

       // DQ (10/7/2004): Definitions should never be unparsed within code generation for expressions
          if (info.SkipClassDefinition() == false)
             {
            // printf ("Skip output of class definition in unparseExpression \n");
            // DQ (10/8/2004): Skip all definitions when outputing expressions!
            // info.set_SkipClassDefinition();
//             info.set_SkipDefinition();
             }

          switch (expr->variant())
             {
               case UNARY_EXPRESSION:  { unparseUnaryExpr (expr, info); break; }
               case BINARY_EXPRESSION: { unparseBinaryExpr(expr, info); break; }
#ifndef USE_SAGE3
               case ASSIGNMENT_EXPRESSION: { unparseAssnExpr(expr, info); break; } 
#endif
               case EXPRESSION_ROOT: { unparseExprRoot(expr, info); break; }
               case EXPR_LIST: { unparseExprList(expr, info); break; }
               case VAR_REF: { unparseVarRef(expr, info); break; }
               case CLASSNAME_REF: { unparseClassRef(expr, info); break; }
               case FUNCTION_REF: { unparseFuncRef(expr, info); break; }
               case MEMBER_FUNCTION_REF: { unparseMFuncRef(expr, info); break; }
               case BOOL_VAL: { unparseBoolVal(expr, info); break; }
               case SHORT_VAL: { unparseShortVal(expr, info); break; }
               case CHAR_VAL: { unparseCharVal(expr, info); break; }
               case UNSIGNED_CHAR_VAL: { unparseUCharVal(expr, info); break; }
               case WCHAR_VAL: { unparseWCharVal(expr, info); break; }
               case STRING_VAL: { unparseStringVal(expr, info); break; }
               case UNSIGNED_SHORT_VAL: { unparseUShortVal(expr, info); break; }
               case ENUM_VAL: { unparseEnumVal(expr, info); break; }
               case INT_VAL: { unparseIntVal(expr, info); break; }
               case UNSIGNED_INT_VAL: { unparseUIntVal(expr, info); break; }
               case LONG_INT_VAL: { unparseLongIntVal(expr, info); break; }
               case LONG_LONG_INT_VAL: { unparseLongLongIntVal(expr, info); break; }
               case UNSIGNED_LONG_LONG_INT_VAL: { unparseULongLongIntVal(expr, info); break; }
               case UNSIGNED_LONG_INT_VAL: { unparseULongIntVal(expr, info); break; }
               case FLOAT_VAL: { unparseFLoatVal(expr, info); break; }
               case DOUBLE_VAL: { unparseDblVal(expr, info); break; }
               case LONG_DOUBLE_VAL: { unparseLongDblVal(expr, info); break; }
               case FUNC_CALL: { unparseFuncCall(expr, info); break; }
               case POINTST_OP: { unparsePointStOp(expr, info); break; }
               case RECORD_REF: { unparseRecRef(expr, info); break; }
               case DOTSTAR_OP: { unparseDotStarOp(expr, info); break; }
               case ARROWSTAR_OP: { unparseArrowStarOp(expr, info); break; }
               case EQ_OP: { unparseEqOp(expr, info); break; }
               case LT_OP: { unparseLtOp(expr, info); break; }
               case GT_OP: { unparseGtOp(expr, info); break; }
               case NE_OP: { unparseNeOp(expr, info); break; }
               case LE_OP: { unparseLeOp(expr, info); break; }
               case GE_OP: { unparseGeOp(expr, info); break; }
               case ADD_OP: { unparseAddOp(expr, info); break; }
               case SUBT_OP: { unparseSubtOp(expr, info); break; }
               case MULT_OP: { unparseMultOp(expr, info); break; }
               case DIV_OP: { unparseDivOp(expr, info); break; }
               case INTEGER_DIV_OP: { unparseIntDivOp(expr, info); break; }
               case MOD_OP: { unparseModOp(expr, info); break; }
               case AND_OP: { unparseAndOp(expr, info); break; }
               case OR_OP: { unparseOrOp(expr, info); break; }
               case BITXOR_OP: { unparseBitXOrOp(expr, info); break; }
               case BITAND_OP: { unparseBitAndOp(expr, info); break; }
               case BITOR_OP: { unparseBitOrOp(expr, info); break; }
               case COMMA_OP: { unparseCommaOp(expr, info); break; }
               case LSHIFT_OP: { unparseLShiftOp(expr, info); break; }
               case RSHIFT_OP: { unparseRShiftOp(expr, info); break; }
               case UNARY_MINUS_OP: { unparseUnaryMinusOp(expr, info); break; }
               case UNARY_ADD_OP: { unparseUnaryAddOp(expr, info); break; }
               case SIZEOF_OP: { unparseSizeOfOp(expr, info); break; }
               case TYPEID_OP: { unparseTypeIdOp(expr, info); break; }
               case NOT_OP: { unparseNotOp(expr, info); break; }
               case DEREF_OP: { unparseDerefOp(expr, info); break; }
               case ADDRESS_OP: { unparseAddrOp(expr, info); break; }
               case MINUSMINUS_OP: { unparseMinusMinusOp(expr, info); break; }
               case PLUSPLUS_OP: { unparsePlusPlusOp(expr, info); break; }
#ifndef USE_SAGE3
               case ABSTRACT: { unparseAbstractOp(expr, info); break; }
#endif
               case BIT_COMPLEMENT_OP: { unparseBitCompOp(expr, info); break; }
               case EXPR_CONDITIONAL: { unparseExprCond(expr, info); break; }
#ifndef USE_SAGE3
               case CLASSINIT_OP: { unparseClassInitOp(expr, info); break; }
               case DYNAMICCAST_OP: { unparseDyCastOp(expr, info); break; }
#endif
               case CAST_OP:                 { unparseCastOp(expr, info); break; }
               case ARRAY_OP:                { unparseArrayOp(expr, info); break; }
               case NEW_OP:                  { unparseNewOp(expr, info); break; }
               case DELETE_OP:               { unparseDeleteOp(expr, info); break; }
               case THIS_NODE:               { unparseThisNode(expr, info); break; }
               case SCOPE_OP:                { unparseScopeOp(expr, info); break; }
               case ASSIGN_OP:               { unparseAssnOp(expr, info); break; }
               case PLUS_ASSIGN_OP:          { unparsePlusAssnOp(expr, info); break; }
               case MINUS_ASSIGN_OP:         { unparseMinusAssnOp(expr, info); break; }
               case AND_ASSIGN_OP:           { unparseAndAssnOp(expr, info); break; }
               case IOR_ASSIGN_OP:           { unparseIOrAssnOp(expr, info); break; }
               case MULT_ASSIGN_OP:          { unparseMultAssnOp(expr, info); break; }
               case DIV_ASSIGN_OP:           { unparseDivAssnOp(expr, info); break; }
               case MOD_ASSIGN_OP:           { unparseModAssnOp(expr, info); break; }
               case XOR_ASSIGN_OP:           { unparseXorAssnOp(expr, info); break; }
               case LSHIFT_ASSIGN_OP:        { unparseLShiftAssnOp(expr, info); break; }
               case RSHIFT_ASSIGN_OP:        { unparseRShiftAssnOp(expr, info); break; }
#ifndef USE_SAGE3
               case FORDECL_OP:              { unparseForDeclOp(expr, info); break; }
#endif
               case TYPE_REF:                { unparseTypeRef(expr, info); break; }
#ifndef USE_SAGE3
               case VECTOR_CONST:            { unparseVConst(expr, info); break; }
#endif
               case EXPR_INIT:               { unparseExprInit(expr, info); break; }
               case AGGREGATE_INIT:          { unparseAggrInit(expr, info); break; }
               case CONSTRUCTOR_INIT:        { unparseConInit(expr, info); break; }
               case ASSIGN_INIT:             { unparseAssnInit(expr, info); break; }
               case THROW_OP:                { unparseThrowOp(expr, info); break; }
               case VA_START_OP:             { unparseVarArgStartOp(expr, info); break; }
               case VA_START_ONE_OPERAND_OP: { unparseVarArgStartOneOperandOp(expr, info); break; }
               case VA_OP:                   { unparseVarArgOp(expr, info); break; }
               case VA_END_OP:               { unparseVarArgEndOp(expr, info); break; }
               case VA_COPY_OP:              { unparseVarArgCopyOp(expr, info); break; }

               default:
                  {
                    ROSE_ASSERT(false);
                    break;
                  }
      
             }

          if (printParen)
             {
            // Output the right paren
               cur << ")";
             }

       // calls the logical_unparse function in the sage files
       // expr->logical_unparse(info, cur);
        } // unparse source replacement END


#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving unparse expression (%p): sage_class_name() = %s \n",expr,expr->sage_class_name());
     cur << "\n/* Bottom of unparseExpression() " << string(expr->sage_class_name()) << " */\n";
#endif
   }

void
Unparser::unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
  //
  // Flag to keep to original state of the "this" option
  //
     bool orig_this_opt = opt.get_this_opt();
     SgUnparse_Info newinfo(info);
     newinfo.set_operator_name(op);
  //
  // If the "this" option was originally false, then we shouldn't print "this."
  // however, this only applies when the "this" is part of a binary expression.
  // In the unary case, we must print "this," otherwise a syntax error will be
  // produced. (i.e. *this)
  //
     if ( !orig_this_opt )
          opt.set_this_opt(TRUE);

     unparseUnaryExpr(expr, newinfo);

  //
  // Now set the "this" option back to its original state
  //
     if( !orig_this_opt )
          opt.set_this_opt(FALSE);
   }

void
Unparser::unparseBinaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info newinfo(info);
     newinfo.set_operator_name(op);
     unparseBinaryExpr(expr, newinfo);
   }

void
Unparser::unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
     printDebugInfo("entering unparseUnaryExpr", TRUE);

     SgUnaryOp* unary_op = isSgUnaryOp(expr);
     ROSE_ASSERT(unary_op != NULL);

  // int toplevel_expression = !info.get_nested_expression();

     info.set_nested_expression();

  //
  // Flag to indicate whether the operand contains an overloaded arrow operator
  //
     bool arrow_op = FALSE;
     arrow_op = NoDereference(expr);

  // printf ("In Unparser::unparseUnaryExpr: unary_op->get_parent() = %s pointer \n",
  //      (unary_op->get_parent() != NULL) ? "VALID" : "NULL");
  // printf ("In Unparser::unparseUnaryExpr: toplevel_expression = %d arrow_op = %d \n",toplevel_expression,arrow_op);

  // We have to test to see if the operand associated with this unary expression is a function pointer 
  // then we can use either function pointer calling syntax
  // (for void (*functionPointer) (int) as a declaration):
  //      1) functionPointer (x);
  //      2) (*functionPointer) (x);
  // Either is valid syntax (see Stroustrup (2nd Edition) section 7.7 "Pointer to Function" page 156.)
  // It seems that EDG and SAGE represent the two similarly, so we have to choose which format we 
  // want to have in the unparsing. Likely this should be an option to the unparser.

  // printf ("unary_op->get_operand()->sage_class_name() = %s \n",unary_op->get_operand()->sage_class_name());
  // printf ("unary_op->get_type()->sage_class_name() = %s \n",unary_op->get_type()->sage_class_name());

     bool isFunctionType = (isSgFunctionType(unary_op->get_type()) != NULL) ? TRUE : FALSE;
  // printf ("isFunctionType = %s \n",(isFunctionType == TRUE) ? "TRUE" : "FALSE");

  // Bugfix (2/26/2001) If this is for a function pointer then skip printing out 
  // the operator name (for dereferencing operator)
#if 0
     if(unary_op->get_mode() != SgUnaryOp::postfix && !arrow_op)
#else
     if(unary_op->get_mode() != SgUnaryOp::postfix && !arrow_op && !isFunctionType)
#endif
        {
          cur <<  info.get_operator_name() ;
        }

     unparseExpression(unary_op->get_operand(), info);

     if (unary_op->get_mode() == SgUnaryOp::postfix && !arrow_op) 
        { 
          cur <<  info.get_operator_name(); 
        }

     info.unset_nested_expression();
   }

void
Unparser::unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info) 
{
  printDebugInfo("entering unparseBinaryExpr", TRUE);
  SgBinaryOp* binary_op = isSgBinaryOp(expr);
  ROSE_ASSERT(binary_op != NULL);

// printf ("In Unparser::unparseBinaryExpr() expr = %s \n",expr->sage_class_name());

  // int toplevel_expression = !info.get_nested_expression();
  //
  bool iostream_op = FALSE;
  //
  // Same reasoning above except with parenthesis operator function.
  //
  bool paren_op = FALSE;
  //
  // Same reasoning above except with "this" expression.
  //
  bool this_op = FALSE;
  //
  // Flag to indicate whether the rhs operand is an overloaded arrow operator
  // (to control the printing of parenthesis).
  //
  bool arrow_op = FALSE;
  //
  // Flag to indicate whether the lhs operand contains an overloaded arrow operator
  // to control printing of operator.
  //
  bool overload_arrow = FALSE;
  
  if (!opt.get_overload_opt() && 
      isIOStreamOperator(binary_op->get_rhs_operand()) ) iostream_op = TRUE;

  if (!opt.get_overload_opt() &&
      isBinaryParenOperator(binary_op->get_rhs_operand()) ) paren_op = TRUE;

  if (!opt.get_this_opt() && 
      isSgThisExp(binary_op->get_lhs_operand()) ) this_op = TRUE;

  if (!opt.get_overload_opt() && 
      isOverloadedArrowOperator(binary_op->get_rhs_operand()) ) arrow_op = TRUE;

  info.set_nested_expression();
  //
  // [DTdbug] 3/23/2000 -- Trying to figure out why overloaded square bracket
  //          operators are not being handled correctly.
  //
  //          3/30/2000 -- operator[]s have been handled.  See unparseFuncCall().
  // 
  //
  if(strcmp(info.get_operator_name(),"[]")==0 )
  { // 
    // Special case:
    //
    printDebugInfo("we have special case: []", TRUE);
    printDebugInfo("lhs: ", FALSE);
    unparseExpression(binary_op->get_lhs_operand(), info);
    cur <<  "[" ; 
    printDebugInfo("rhs: ", FALSE);
    unparseExpression(binary_op->get_rhs_operand(), info);
    cur <<  "]"; 
  } 
  else 
  { //
    // Check whether overload option is turned on or off.  If it is off, the 
    // conditional is true.
    //
    if (!opt.get_overload_opt()) 
    { //
      // First check if the right hand side is an unary operator function. 
      //
      if (isUnaryOperator(binary_op->get_rhs_operand())) 
      { //
	// Two cases must be considered here: prefix unary and postfix unary 
        // operators. Most of the unary operators are prefix. In this case, we must
        // first unparse the rhs and then the lhs.	
        //
	if (isUnaryPostfixOperator(binary_op->get_rhs_operand())); // Postfix unary operator.
	else 
        { //  
	  // Prefix unary operator.
          //
	  printDebugInfo("prefix unary operator found", TRUE);
	  printDebugInfo("rhs: ", FALSE);

	  unparseExpression(binary_op->get_rhs_operand(), info);

	  printDebugInfo("lhs: ", FALSE);

	  unparseExpression(binary_op->get_lhs_operand(), info);
	  info.unset_nested_expression();
	  
	  return; 

	}
      }
    }
    //    
    // Check if this is a dot expression and the overload option is turned off. If so,
    // we need to handle this differently. Otherwise, skip this section and unparse
    // using the default case below.
    //
    // if (!opt.get_overload_opt() && strcmp(info.get_operator_name(),".")==0) 
    //
    if (!opt.get_overload_opt() && isSgDotExp(expr)) 
    {
      overload_arrow = NoDereference(binary_op->get_lhs_operand()); 

      SgMemberFunctionRefExp* mfunc_ref = 
	isSgMemberFunctionRefExp(binary_op->get_rhs_operand());
      printDebugInfo("lhs: ", FALSE);
      unparseExpression(binary_op->get_lhs_operand(), info);
      printDebugInfo(getSgVariant(expr->variant()), TRUE);

      //   
      // Check if the rhs is not a member function. If so, then it is most likely a  
      // data member of a class. We print the dot in this case.
      //
      if (!mfunc_ref && !overload_arrow)  
      { //
        // Print out the dot:
        //
	cur << info.get_operator_name();
	printDebugInfo("printed dot because is not member function", TRUE);

      }
      //
      // Now check if this member function is an operator overloading function. If it
      // is, then we don't print the dot. If not, then print the dot.
      //
      else if (!isOperator(mfunc_ref) && !overload_arrow) 
      {
	cur << info.get_operator_name();
	printDebugInfo("printed dot because is not operator overloading function", TRUE);

      }
    }
    //
    // Check if this is an arrow expression and the "this" option is turned off. If 
    // so, we need to handle this differently. Otherwise, skip this section and 
    // unparse using the default case below.
    //
    // else if (!opt.get_this_opt() && strcmp(info.get_operator_name(),"->")==0) 
    //
    else if (!opt.get_this_opt() && isSgArrowExp(expr)) 
    {
      //
      // This is a special case to check for. We are now checking for arrow 
      // expressions with the overload operator option off. If so, that means that 
      // we cannot print "operator", which means that printing "->" is wrong. So we 
      // must dereference the variable and suppress the printing of "->". Jump to
      // "dereference" is true.
      //
      if (!opt.get_overload_opt() && isOperator(binary_op->get_rhs_operand()))
	goto dereference;
      
      printDebugInfo("lhs: ", FALSE);
      unparseExpression(binary_op->get_lhs_operand(), info);
      printDebugInfo(getSgVariant(expr->variant()), TRUE);
      //
      // Check if the lhs is a this expression. If so, then don't print the arrow. 
      //
      if (!isSgThisExp(binary_op->get_lhs_operand())) 
      {
     // Is not "this" exp, so print the arrow.
        cur << info.get_operator_name();
      }
    }
    //
    // We must also check the special case here mentioned above since the "this" 
    // option may be true, but the overload option is false.
    //
    // else if (!opt.get_overload_opt() && strcmp(info.get_operator_name(),"->")==0 &&
    //     isOperator(binary_op->get_rhs_operand())) 
    //
    else if (!opt.get_overload_opt() && isSgArrowExp(expr) &&
	     isOperator(binary_op->get_rhs_operand())) 
    {
dereference:
      //
      // Before dereferencing, first check if the function is preceded by a class
      // name. If not, then dereference.
      //
          if (noQualifiedName(binary_op->get_rhs_operand())) 
             {
               cur << "(*"; 
               printDebugInfo("lhs: ", FALSE);
               unparseExpression(binary_op->get_lhs_operand(), info);
               printDebugInfo(getSgVariant(expr->variant()), TRUE);
               cur << ")";
             }
    }
    //
    // We reach this if the options were on, or if this was not a dot, arrow, or unary
    // prefix expression. This is the default case. 
    //
    else 
    {
      printDebugInfo("lhs: ", FALSE);
      unparseExpression(binary_op->get_lhs_operand(), info);
      printDebugInfo(getSgVariant(expr->variant()), TRUE);
      //
      // Before checking to insert a newline to prevent linewrapping, check that this
      // expression is a primitive operator and not dot or arrow expressions.
      //
      cur << " " << info.get_operator_name() << " ";
      
    }

    if(strcmp(info.get_operator_name(),",")==0) 
    {
      SgUnparse_Info newinfo(info);
      newinfo.set_inRhsExpr();
      printDebugInfo("rhs: ", FALSE);
      unparseExpression(binary_op->get_rhs_operand(), newinfo);

    } 
    else 
      unparseExpression(binary_op->get_rhs_operand(), info);

  }
  info.unset_nested_expression();

}

void Unparser::unparseAssnExpr(SgExpression* expr, SgUnparse_Info& info) {}
void Unparser::unparseExprRoot(SgExpression* expr, SgUnparse_Info& info) {}

void Unparser::unparseExprList(SgExpression* expr, SgUnparse_Info& info)
   {
     SgExprListExp* expr_list = isSgExprListExp(expr);
     ROSE_ASSERT(expr_list != NULL);
  /* code inserted from specification */
  
     SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();

     if (i != expr_list->get_expressions().end())
        {
          while (1)
             {
               SgUnparse_Info newinfo(info);
               newinfo.set_SkipBaseType();
#if USE_SAGE3
               unparseExpression(*i, newinfo);
#else
               unparseExpression((*i).irep(), newinfo);
#endif
               i++;
               if (i != expr_list->get_expressions().end())
                  {
                    cur << ",";
                  }
                 else
                    break;
             }
        }
   }

void Unparser::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ROSE_ASSERT(var_ref != NULL);

  // printf ("In Unparser::unparseVarRef \n");

  /* code inserted from specification */

  // todo: when get_parent() works for this class we can
  // get back to the lhs of the SgArrowExp or SgDotExp that
  // may be a parent of this expression.  This will let
  // us avoid outputting the class qualifier when its not needed.

  // For now we always output the class qualifier.

     if (var_ref->get_symbol() == NULL)
        {
          printf ("Error in unparseVarRef() at line %d column %d \n",
               var_ref->get_file_info()->get_line(),
               var_ref->get_file_info()->get_col());
        }
     ROSE_ASSERT(var_ref->get_symbol() != NULL);

     SgInitializedName* theName = var_ref->get_symbol()->get_declaration();
    // SgInitializedName* prev_decl = theName->get_prev_decl_item();
     SgVariableDeclaration* vd = NULL;
    // if (prev_decl != NULL)
    //      vd = isSgVariableDeclaration(prev_decl->get_declaration());
    //   else
          vd = isSgVariableDeclaration(theName->get_declaration());

  // if (vd && vd->isStatic())
  // printf ("In Unparser::unparseVarRef: vd = %p \n",vd);
  // printf ("In Unparser::unparseVarRef: vd->get_declarationModifier().get_storageModifier().isStatic() = %s \n",
  //      (vd->get_declarationModifier().get_storageModifier().isStatic() == true) ? "true" : "false");

  // DQ (10/16/2004): Not clear on why this was previously dependent upon isStatic modifier!
  // if ( (vd != NULL) && (vd->get_declarationModifier().get_storageModifier().isStatic() == true) )
     if (vd != NULL)
        {
       // printf ("vd = %p = %s  vd->get_parent() = %p = %s \n",vd,vd->sage_class_name(),vd->get_parent(),vd->get_parent()->sage_class_name());
          SgClassDefinition* cdef = isSgClassDefinition(vd->get_parent());
       // printf ("cdef = %p \n",cdef);
          if (cdef != NULL)
             {
               SgClassDeclaration* cdecl = isSgClassDeclaration(cdef->get_declaration());
               if (cdecl != NULL &&  vd->get_declarationModifier().get_storageModifier().isStatic())
                  {
                    cur <<  cdecl->get_qualified_name().str() <<  "::";
                  }
             }

          SgNamespaceDefinitionStatement* namespacedef = isSgNamespaceDefinitionStatement(vd->get_parent());
       // printf ("namespacedef = %p \n",namespacedef);
          if (namespacedef != NULL)
             {
               SgNamespaceDeclarationStatement* namespacedecl = 
                    isSgNamespaceDeclarationStatement(namespacedef->get_namespaceDeclaration());
               if (namespacedecl != NULL)
                  {
                    cur <<  namespacedecl->get_qualified_name().str() <<  "::";
                  }
             }
        }

     cur <<  var_ref->get_symbol()->get_name().str();
   }

void Unparser::unparseClassRef(SgExpression* expr, SgUnparse_Info& info) {
  
  SgClassNameRefExp* classname_ref = isSgClassNameRefExp(expr);
  ROSE_ASSERT(classname_ref != NULL);
  /* code inserted from specification */
  cur <<  classname_ref->get_symbol()->get_declaration()->get_name().str();
}

void Unparser::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info) {

  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  ROSE_ASSERT(func_ref != NULL);
  /* code inserted from specification */

// DQ: This acceses the string pointed to by the pointer in the SgName object 
// directly ans is thus UNSAFE! A copy of the string should be made.
// char* func_name = func_ref->get_symbol()->get_name();
  char* func_name = strdup (func_ref->get_symbol()->get_name().str());
  int diff = 0;    //the length difference between "operator" and function 

  //check that this an operator overloading function
  if (!opt.get_overload_opt() && !strncmp(func_name, "operator", 8)) { 
    //set the length difference between "operator" and function 
    diff = strlen(func_name) - strlen("operator");
    //now we check if the difference is larger than 0. If it is, that means that
    //there is something following "operator". Then we can get the substring after
    //"operator." If the diff is not larger than 0, then don't get the substring.
    if (diff > 0) {
      //get the substring after "operator." If you are confused with how strchr 
      //works, look up the man page for it.
      func_name = strchr(func_name, func_name[8]); 
    }
  }
  
  //if func_name is not "()", print it. Otherwise, we don't print it because we want
  //to print out, for example, A(0) = 5, not A()(0) = 5.
  if (strcmp(func_name, "()")) {
    cur <<  func_name ; 
  }
  printDebugInfo("unparseFuncRef, Function Name: ", FALSE); printDebugInfo(func_name, TRUE);
}

void
Unparser::unparseMFuncRef ( SgExpression* expr, SgUnparse_Info& info )
   {
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     ROSE_ASSERT(mfunc_ref != NULL);
  /* code inserted from specification */
  
     SgMemberFunctionDeclaration* mfd  = mfunc_ref->get_symbol_i()->get_declaration();
     SgClassDefinition*           cdef = mfd->get_scope();

  // DQ (2/16/2004): error in templates (test2004_18.C)
     ROSE_ASSERT (cdef != NULL);
     SgClassDeclaration* cdecl = cdef->get_declaration();

  // cur << "\n/* Inside of unparseMFuncRef */\n";

  // qualified name is always outputed except when the p_need_qualifier is
  // set to 0 (when the naming class is identical to the selection class, and
  // and when we aren't suppressing the virtual function mechanism).
  
  // if (!get_is_virtual_call()) -- take off because this is not properly set

  // DQ (9/17/2004): Added assertion
     ROSE_ASSERT(cdecl != NULL);
     ROSE_ASSERT(cdecl->get_parent() != NULL);

     bool print_colons = FALSE;
     if (mfunc_ref->get_need_qualifier())
        {
       // check if this is a iostream operator function and the value of the overload opt is false
       // if (!opt.get_overload_opt() && isIOStreamOperator(mfunc_ref));
          if (!opt.get_overload_opt() && isIOStreamOperator(mfunc_ref))
             {
            // ... nothing to do here
             }
            else
             {
               cur <<  cdecl->get_qualified_name().str() << "::"; 
               print_colons = TRUE;
             }
        }
  
  // comments about the logic below can be found above in the unparseFuncRef function.

  // char* func_name = mfunc_ref->get_symbol()->get_name();
     char* func_name = strdup( mfunc_ref->get_symbol()->get_name().str() );
     int diff = 0;

  // check that this an operator overloading function and that colons were not printed
     if (!opt.get_overload_opt() && !strncmp(func_name, "operator", 8) && !print_colons)
        {
       // the length difference between "operator" and function
          diff = strlen(func_name) - strlen("operator"); 
          if (diff > 0)
             {
            // get the substring after "operator"
               func_name = strchr(func_name, func_name[8]);
             }
        }

     if( !strcmp(func_name,"[]") ) 
        {
       //
       // [DT] 3/30/2000 -- Don't unparse anything here.  The square brackets will
       //      be handled from unparseFuncCall().
       //
       //      May want to handle overloaded operator() the same way.
       //
        }
       else
        {
          if (strcmp(func_name, "()"))
             {
               cur <<  func_name;
             }
       // cur << mfunc_ref->get_symbol()->get_name();
          printDebugInfo("unparseMFuncRef, Function Name: ", FALSE); printDebugInfo(func_name, TRUE);
        }

  // cur << "\n/* leaving unparseMFuncRef */\n";
   }

void
Unparser::unparseBoolVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgBoolValExp* bool_val = isSgBoolValExp(expr);
     ROSE_ASSERT(bool_val != NULL);  

  /* code inserted from specification */
     if (opt.get_num_opt())
        {
       // we want to print the boolean values as numerical values
          if (bool_val->get_value() == true)
             {
               cur << "1";
             }
            else
             {
               cur <<  "0";
             }
        }
       else
        {
       // print them as "true" or "false"
          if (bool_val->get_value() == true)
             {
               cur <<  "true";
             }
            else
             {
               cur <<  "false";
             }
        }
   }

void Unparser::unparseShortVal(SgExpression* expr, SgUnparse_Info& info) {
  SgShortVal* short_val = isSgShortVal(expr);
  ROSE_ASSERT(short_val != NULL);
  /* code inserted from specification */

  cur << short_val->get_value();
}

void Unparser::unparseCharVal(SgExpression* expr, SgUnparse_Info& info) {
  SgCharVal* char_val = isSgCharVal(expr);
  ROSE_ASSERT(char_val != NULL);
  /* code inserted from specification */

   cur << (int) char_val->get_value();
}

void Unparser::unparseUCharVal(SgExpression* expr, SgUnparse_Info& info) {
  SgUnsignedCharVal* uchar_val = isSgUnsignedCharVal(expr);
  ROSE_ASSERT(uchar_val != NULL);
  /* code inserted from specification */

   cur << (int) uchar_val->get_value();
}

void Unparser::unparseWCharVal(SgExpression* expr, SgUnparse_Info& info) {
  SgWcharVal* wchar_val = isSgWcharVal(expr);
  ROSE_ASSERT(wchar_val != NULL);
  /* code inserted from specification */

  //DONT KNOW HOW TO GET ACCESS TO p_valueUL, so just use p_value for now
  //if(wchar_val->p_valueUL) {
  //  cur << (wchar_t ) wchar_val->p_valueUL;
  //} else cur << (int) wchar_val->get_value();

   cur << (int) wchar_val->get_value();
}

void
Unparser::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgStringVal* str_val = isSgStringVal(expr);
     ROSE_ASSERT(str_val != NULL);
  /* code inserted from specification */

     
        {
       // Handle special case of macro specification (this is a temporary hack to permit us to
       // specify macros within transformations)

          int wrap = cur.get_linewrap();
          cur.set_linewrap(-1);
       // const char* targetString = "ROSE-TRANSFORMATION-MACRO:";
          const char* targetString = "ROSE-MACRO-EXPRESSION:";
          int targetStringLength = strlen(targetString);
          if (strncmp(str_val->get_value(),targetString,targetStringLength) == 0)
             {
            // unparse the string without the surrounding quotes and with a new line at the end
               char* remainingString = str_val->get_value()+targetStringLength;
               printf ("Specify a MACRO: remainingString = %s \n",remainingString);
            // Put in a leading CR so that the macro will always be unparsed onto its own line
            // Put in a trailing CR so that the trailing ";" will be unparsed onto its own line too!
              cur << "\n" << remainingString << "\n";
             }
            else
             {
               cur << "\"" << str_val->get_value() << "\"";
             }
          cur.set_linewrap(wrap); 
        }

     ROSE_ASSERT(str_val->get_value() != NULL);
   }

void Unparser::unparseUShortVal(SgExpression* expr, SgUnparse_Info& info) {
  SgUnsignedShortVal* ushort_val = isSgUnsignedShortVal(expr);
  ROSE_ASSERT(ushort_val != NULL);
  /* code inserted from specification */

   cur << ushort_val->get_value();
}

void
Unparser::unparseEnumVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgEnumVal* enum_val = isSgEnumVal(expr);
     ROSE_ASSERT(enum_val != NULL);
  /* code inserted from specification */

  // printf ("In Unparser::unparseEnumVal: info.inEnumDecl() = %s \n",info.inEnumDecl() ? "TRUE" : "FALSE");

  // todo: optimize this so that the qualified name is only printed when necessary.
     if (info.inEnumDecl())
        {
          cur << enum_val->get_value();
        }
       else
        {
          SgClassDefinition* classdefn = NULL;
#if PRINT_DEVELOPER_WARNINGS
          printf ("IMPLEMENTATION NOTE: Handle more complex cases of enums hidden in typedefs and variable declarations and within namespaces! \n");
#endif
          if (enum_val->get_declaration() && (classdefn = isSgClassDefinition(enum_val->get_declaration()->get_parent())))
             {
               cur <<  classdefn->get_qualified_name().str() <<  "::";
             }

       // printf ("In Unparser::unparseEnumVal: classdefn = %s pointer \n",classdefn ? "VALID" : "NULL");

       // Sometimes this name string is NULL (is this a bug: YES, it is fixed now in sage_il_to_str.C)
          ROSE_ASSERT (enum_val->get_name().str() != NULL);
          cur <<  enum_val->get_name().str();
        }
   }

void 
Unparser::unparseIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgIntVal* int_val = isSgIntVal(expr);
     ROSE_ASSERT(int_val != NULL);
  // printf ("In Unparser::unparseIntVal(): int_val->get_value() = %d \n",int_val->get_value());

  /* code inserted from specification */     
     cur << int_val->get_value();
   }

void
Unparser::unparseUIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedIntVal* uint_val = isSgUnsignedIntVal(expr);
     ROSE_ASSERT(uint_val != NULL);

  /* code inserted from specification */     
     cur << uint_val->get_value();
   }

void Unparser::unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info) {
  SgLongIntVal* longint_val = isSgLongIntVal(expr);
  ROSE_ASSERT(longint_val != NULL);
  /* code inserted from specification */

   cur << longint_val->get_value();
}

void Unparser::unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info) {
  SgLongLongIntVal* longlongint_val = isSgLongLongIntVal(expr);
  ROSE_ASSERT(longlongint_val != NULL);
  /* code inserted from specification */

  /* note these are not correct */
   cur << longlongint_val->get_value();
}
void Unparser::unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info) {
  SgUnsignedLongLongIntVal* ulonglongint_val = isSgUnsignedLongLongIntVal(expr);
  ROSE_ASSERT(ulonglongint_val != NULL);
  /* code inserted from specification */

  /* note these are not correct */
   cur << ulonglongint_val->get_value();
}

void Unparser::unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info) {
  SgUnsignedLongVal* ulongint_val = isSgUnsignedLongVal(expr);
  ROSE_ASSERT(ulongint_val != NULL);
  /* code inserted from specification */
   cur << ulongint_val->get_value();
}

void Unparser::unparseFLoatVal(SgExpression* expr, SgUnparse_Info& info) {
  SgFloatVal* float_val = isSgFloatVal(expr);
  ROSE_ASSERT(float_val != NULL);
  /* code inserted from specification */
   cur << float_val->get_value();
}

#if 0
// DQ: This is no longer used (we use properties of the iostream to formate floating point now).
bool
Unparser::zeroRemainder( long double doubleValue )
   {
     int integerValue = (int)doubleValue;
     double remainder = doubleValue - (double)integerValue;

     return (remainder == 0) ? true : false;
   }
#endif

void
Unparser::unparseDblVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgDoubleVal* dbl_val = isSgDoubleVal(expr);
     ROSE_ASSERT(dbl_val != NULL);
  /* code inserted from specification */
  // os->setf(ios::showpoint);

  // cur << dbl_val->get_value();
  // cur << setiosflags(ios::showpoint) << setprecision(4) << dbl_val->get_value();

     setiosflags(ios::showpoint);

  // DQ (10/16/2004): Not sure what 4 implies, but we get 16 digits after the decimal 
  // point so it should be fine (see test2004_114.C)!
     setprecision(4);

     cur << dbl_val->get_value(); 

  // os->unsetf(ios::showpoint);
   }

void Unparser::unparseLongDblVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongDoubleVal* longdbl_val = isSgLongDoubleVal(expr);
     ROSE_ASSERT(longdbl_val != NULL);
  /* code inserted from specification */
  
     cur << longdbl_val->get_value();
   }

//-----------------------------------------------------------------------------------
//  void Unparser::unparseFuncCall 
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
Unparser::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("In Unparser::unparseFuncCall \n");

     SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
     ROSE_ASSERT(func_call != NULL);
     SgUnparse_Info newinfo(info);
     bool needSquareBrackets = false;

  // FIRST PART
  // check if this is an binary operator overloading function and if the overloading 
  // option is off. If so, we traverse using "in-order" tree traversal. However, do 
  // not enter this segment if we have a dot expression. Dot expressions are handled 
  // by the second part. 
     if (!opt.get_overload_opt() && isBinaryOperator(func_call->get_function()) && 
         !(isSgDotExp(func_call->get_function())) && !(isSgArrowExp(func_call->get_function())))
        {
          printDebugInfo("in FIRST PART of unparseFuncCall", TRUE);

          SgExpressionPtrList& list = func_call->get_args()->get_expressions();
          SgExpressionPtrList::iterator arg = list.begin();
          if (arg != list.end())
             {
               newinfo.set_nested_expression();

             // unparse the lhs operand
                printDebugInfo("left arg: ", FALSE);
                unparseExpression((*arg), newinfo);
             // unparse the operator
                unparseExpression(func_call->get_function(), info);
                arg++;
             // unparse the rhs operand
                printDebugInfo("right arg: ", FALSE);
                unparseExpression((*arg), newinfo);
                newinfo.unset_nested_expression();
              }
        }
       else
        {
       // SECOND PART
       // this means that we have an unary operator overloading function, a non-operator
       // overloading function, or that the overloading option was turned on. 
          printDebugInfo("in SECOND PART of unparseFuncCall", TRUE);
          bool print_paren = TRUE;

          if (opt.get_overload_opt())
               info.set_nested_expression();

       // cur << " /* output 2nd part */ ";

       //
       // Unparse the function first.
       //
          unparseExpression(func_call->get_function(), info);

       // cur << " /* after output func_call->get_function() */ ";

          if (opt.get_overload_opt())
               info.unset_nested_expression();

       // check if we have an unary operator or an overloaded arrow operator and the 
       // operator overloading option is turned off. If so, then we can return right 
       // away because there are no arguments to unparse. Sage provides default arguments
       // for unary operators, such as (), or (0). We want to avoid printing this if
       // the conditional is true.
          if (!opt.get_overload_opt())
             {
               if (RemoveArgs(func_call->get_function())) return;
             }

          SgUnparse_Info newinfo(info);

       // now check if the overload option is off and that the function is dot binary
       // expression. If so, check if the rhs is an operator= overloading function (and 
       // that the function isn't preceded by a class name). If the operator= is preceded
       // by a class name ("<class>::operator=") then do not set print_paren to FALSE.
       // If so, set print_paren to FALSE, otherwise, set print_paren to TRUE for
       // all other functions.
       //
       // [DT] 4/6/2000 -- Need to check for operator==, also, as well
       //      any other potential overloaded operator that having
       //      this paren would cause a problem.  e.g. in the case
       //      of operator==, we would get something like (x==)(y)
       //      where the paren at ==) comes from unparseBinaryExpr()
       //      and the paren at (y comes from here.
       //
       //      NOTE:  I went ahead and created isBinaryEqualityOperator()
       //      and put the check here.  But there needs to be a more
       //      thorough fix that handles operator<, operator>=, etc...
       //
       //      4/10/2000 -- Created isBinaryInequalityOperator() and
       //      isBinaryArithmeticOperator().  Thinking about simply
       //      creating an isBinaryOverloadedOperator().
       //
          SgBinaryOp* binary_op = isSgBinaryOp(func_call->get_function());
          if (!opt.get_overload_opt())
             {
               if (binary_op != NULL && 
                  ( isBinaryEqualsOperator(binary_op->get_rhs_operand())     || 
                    isBinaryEqualityOperator(binary_op->get_rhs_operand())   ||  
                    isBinaryInequalityOperator(binary_op->get_rhs_operand()) ||
                    isBinaryArithmeticOperator(binary_op->get_rhs_operand())  ) &&
                    noQualifiedName(binary_op->get_rhs_operand()) )
            	      print_paren = FALSE; 
             }
       //
       // [DT] 3/30/2000 -- In the case of overloaded [] operators, 
       //      set a flag indicating that square brackets should be
       //      wrapped around the argument below.  This will
       //      result in the desired syntax in the unparsed code 
       //      as long as the unparseMFuncExpr() function knows better 
       //      than to output any of ".operator[]".
       //
       //      Q: Need to check opt.get_overload_opt()?
       //
       // MK: Yes! We only need square brackets if
       //     1. opt.get_overload_opt() is FALSE (= keyword "operator" not required in the output), and
       //     2. we do not have to specify a qualifier; i.e., <classname>::<funcname>
       //     Otherwise, we print "operator[]" and need parenthesis "()" around the
       //     function argument.
          if ( !opt.get_overload_opt() && binary_op != NULL &&
               isBinaryBracketOperator(binary_op->get_rhs_operand()) && 
               noQualifiedName(binary_op->get_rhs_operand()) )
             {
	            needSquareBrackets=TRUE;
            // Turn off parens in order to output [i] instead of [(i)].
               print_paren=FALSE; 
             }

          if( needSquareBrackets) { cur << "[";  }

       // now unparse the function's arguments
          if (func_call->get_args()) printDebugInfo("unparsing arguments of function call", TRUE);

          if (print_paren)
             { 
               cur << "(";
            // cur << "/* Unparse args in unparseFuncCall */ ("; 
               printDebugInfo("( from FuncCall", TRUE);
             } 

          if (func_call->get_args())
             {
               SgExpressionPtrList& list = func_call->get_args()->get_expressions();
               SgExpressionPtrList::iterator arg = list.begin();
               while (arg != list.end())
                  {
                    SgConstructorInitializer* con_init = isSgConstructorInitializer(*arg);
                    if (con_init != NULL && isOneElementList(con_init))
                       {
                         printDebugInfo("entering unparseOneElemConInit", TRUE);
                         unparseOneElemConInit(con_init, newinfo);
                       }
                      else 
                         unparseExpression((*arg), newinfo);
                    arg++;
                    if (arg != list.end())
                       {
                          cur << ","; 
                       }
                  }
             }

          if (print_paren)
             {
               cur << ")";
               printDebugInfo(") from FuncCall", TRUE);
             }

          if ( needSquareBrackets) { cur << "]";  }
        }

  // printf ("Leaving Unparser::unparseFuncCall \n");
   }

void Unparser::unparsePointStOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "->", info); }
void Unparser::unparseRecRef(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ".",info); }
void Unparser::unparseDotStarOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ".*", info); }
void Unparser::unparseArrowStarOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "->*", info); }
void Unparser::unparseEqOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "==", info); }
void Unparser::unparseLtOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<", info); }
void Unparser::unparseGtOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ">", info); }
void Unparser::unparseNeOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "!=", info); }
void Unparser::unparseLeOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<=", info); }
void Unparser::unparseGeOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ">=", info); }
void Unparser::unparseAddOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "+", info); }
void Unparser::unparseSubtOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "-", info); }
void Unparser::unparseMultOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "*", info); }
void Unparser::unparseDivOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "/", info); }
void Unparser::unparseIntDivOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "/", info); }
void Unparser::unparseModOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "%", info); }
void Unparser::unparseAndOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "&&", info); }
void Unparser::unparseOrOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "||", info); }
void Unparser::unparseBitXOrOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "^", info); }
void Unparser::unparseBitAndOp(SgExpression* expr, SgUnparse_Info& info) {unparseBinaryOperator(expr, "&", info); }
void Unparser::unparseBitOrOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "|", info); }
void Unparser::unparseCommaOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ",", info); }
void Unparser::unparseLShiftOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<<", info); }
void Unparser::unparseRShiftOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ">>", info); }
void Unparser::unparseUnaryMinusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "-", info); }
void Unparser::unparseUnaryAddOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "+", info); }

void Unparser::unparseSizeOfOp(SgExpression* expr, SgUnparse_Info& info) {
  SgSizeOfOp* sizeof_op = isSgSizeOfOp(expr);
  ROSE_ASSERT(sizeof_op != NULL);
  /* code inserted from specification */

   cur << "sizeof(";
  if(sizeof_op->get_operand_expr()) unparseExpression(sizeof_op->get_operand_expr(), info);
  else unparseType(sizeof_op->get_operand_type(), info);
   cur << ")";
}

void Unparser::unparseTypeIdOp(SgExpression* expr, SgUnparse_Info& info) {
  SgTypeIdOp* typeid_op = isSgTypeIdOp(expr);
  ROSE_ASSERT(typeid_op != NULL);
  /* code inserted from specification */

   cur << "typeid(";
  if(typeid_op->get_operand_expr()) unparseExpression(typeid_op->get_operand_expr(), info);
  else unparseType(typeid_op->get_operand_type(), info);
   cur << ")";
}

void Unparser::unparseNotOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "!", info); }
void Unparser::unparseDerefOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "*", info); }
void Unparser::unparseAddrOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "&", info); }
void Unparser::unparseMinusMinusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "--", info); }
void Unparser::unparsePlusPlusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "++", info); }
void Unparser::unparseAbstractOp(SgExpression* expr, SgUnparse_Info& info) {}
void Unparser::unparseBitCompOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "~", info); }

void Unparser::unparseExprCond(SgExpression* expr, SgUnparse_Info& info)
   {
     SgConditionalExp* expr_cond = isSgConditionalExp(expr);
     ROSE_ASSERT(expr_cond != NULL);
  /* code inserted from specification */

     int toplevel_expression = !info.get_nested_expression();
     info.set_nested_expression();

  // For now make sure this generates output so that we can debug this
     if ( SgProject::get_verbose() >= 1 )
          printf ("In unparseExprCond(): Fixed lvalue handling - expr_cond->get_is_lvalue() \n");

  // if (! toplevel_expression || expr_cond->get_is_lvalue())
  // if (!toplevel_expression)
     if (! toplevel_expression || expr_cond->get_lvalue())
        {
          cur << "(";
       // cur << "/* unparseExprCond */ (";
        }
     unparseExpression(expr_cond->get_conditional_exp(), info);
     cur << "?"; 
     unparseExpression(expr_cond->get_true_exp(), info);
     cur << ":"; 
     unparseExpression(expr_cond->get_false_exp(), info);
  // if (! toplevel_expression || expr_cond->get_is_lvalue())
  // if (!toplevel_expression)
     if (! toplevel_expression || expr_cond->get_lvalue())
        {
          cur << ")";
        }
     info.unset_nested_expression();
   }

void
Unparser::unparseClassInitOp(SgExpression* expr, SgUnparse_Info& info)
   {
   } 

void
Unparser::unparseDyCastOp(SgExpression* expr, SgUnparse_Info& info)
   {
   }

void
Unparser::unparseCastOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgCastExp* cast_op = isSgCastExp(expr);
     ROSE_ASSERT(cast_op != NULL);
  /* code inserted from specification */

     SgUnparse_Info newinfo(info);
     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();

  // DQ (10/8/2004): Never unparse the declaration from within a cast expression (see testcode2001_28.C)!
     newinfo.set_SkipDefinition();

     newinfo.unset_SkipBaseType();

     switch(cast_op->cast_type())
        {
          case SgCastExp::dynamic_cast_e:
             {
            // dynamic_cast <P *> (expr)
               
                    cur << "dynamic_cast <";
               unparseType(cast_op->get_expression_type(), newinfo); // first/second part
               
                    cur << ">"; // paren are in operand_i
               break; 
             }
          case SgCastExp::reinterpret_cast_e:
             {
            // reinterpret_cast <P *> (expr)
               
                    cur << "reinterpret_cast <";
               unparseType(cast_op->get_expression_type(), newinfo);
               
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
                 // cur << "/* unparseCastOp SgCastExp::c_cast_e nonstring */ ("; 
                    unparseType(cast_op->get_expression_type(), newinfo);
                    cur << ")";
                  }
            // cast_op->get_operand_i()->variant() == STRING_VAL
            // it is a string, so now check if the cast is not a "const char* "
            // or if the caststring option is on. If any of these are true,
            // then unparse the cast. Both must be false to not unparse the cast.
                 else
                  {
                    if (!isCast_ConstCharStar(cast_op->get_expression_type()) || opt.get_caststring_opt())
                       {
                         cur << "(";
                         cur << "/* unparseCastOp SgCastExp::c_cast_e case string */ ("; 
                         unparseType(cast_op->get_expression_type(), newinfo);
                         cur << ")";
                       }
                  }
               break; 
             }
        }

     unparseExpression(cast_op->get_operand_i(), info); 
   }

void
Unparser::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
   { 
     unparseBinaryOperator(expr, "[]", info); 
   }

void
Unparser::unparseNewOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("In Unparser::unparseNewOp \n");

     SgNewExp* new_op = isSgNewExp(expr);
     ROSE_ASSERT(new_op != NULL);
  /* code inserted from specification */

     if(new_op->get_need_global_specifier())
        {
          
               cur << ":: ";
        }
     
          cur << "new ";

     SgUnparse_Info newinfo(info);
     newinfo.unset_inVarDecl();
     if (new_op->get_placement_args())
        {
          unparseExpression(new_op->get_placement_args(), newinfo);
        }

     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart(); 
     newinfo.unset_isTypeSecondPart(); 
     newinfo.set_SkipClassSpecifier();

  // This fixes a bug having to do with the unparsing of the type name of constructors in return statements.

  // printf ("In Unparser::unparseNewOp: new_op->get_type()->sage_class_name() = %s \n",new_op->get_type()->sage_class_name());
     unparseType(new_op->get_type(), newinfo);

     if (new_op->get_constructor_args())
        {
       // printf ("In Unparser::unparseNewOp: Now unparse new_op->get_constructor_args() \n");
          unparseExpression(new_op->get_constructor_args(), newinfo);
        }

#if 0
       else
        {
       // printf ("In Unparser::unparseNewOp: Call unparse type \n");
          unparseType(new_op->get_type(), newinfo);
        }
#endif

     if (new_op->get_builtin_args()) 
        {
       // printf ("In Unparser::unparseNewOp: Now unparse new_op->get_builtin_args() \n");
          unparseExpression(new_op->get_builtin_args(), newinfo);
        }

  // printf ("Leaving Unparser::unparseNewOp \n");
   }

void
Unparser::unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info) {
  SgDeleteExp* delete_op = isSgDeleteExp(expr);
  ROSE_ASSERT(delete_op != NULL);
  /* code inserted from specification */

  if(delete_op->get_need_global_specifier()) {  cur << ":: ";  }
   cur << "delete ";
  SgUnparse_Info newinfo(info);
  if (delete_op->get_is_array()){
     cur << "[]";
  }
  unparseExpression(delete_op->get_variable(), newinfo);
}

void
Unparser::unparseThisNode(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgThisExp* this_node = isSgThisExp(expr);

     ROSE_ASSERT(this_node != NULL);

  // printf ("In Unparser::unparseThisNode: opt.get_this_opt() = %s \n", (opt.get_this_opt()) ? "TRUE" : "FALSE");

     if (opt.get_this_opt()) // Checks options to determine whether to print "this"  
        {
               cur << "this"; 
        }
   }

void Unparser::unparseScopeOp(SgExpression* expr, SgUnparse_Info& info) {
  SgScopeOp* scope_op = isSgScopeOp(expr);
  ROSE_ASSERT(scope_op != NULL);
  /* code inserted from specification */

  if (scope_op->get_lhs_operand()) unparseExpression(scope_op->get_lhs_operand(), info);
   cur << "::";
  unparseExpression(scope_op->get_rhs_operand(), info);
}

void Unparser::unparseAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "=", info); }
void Unparser::unparsePlusAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "+=", info); } 
void Unparser::unparseMinusAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "-=", info); }
void Unparser::unparseAndAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "&=", info); }
void Unparser::unparseIOrAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "|=", info); }
void Unparser::unparseMultAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "*=", info); }
void Unparser::unparseDivAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "/=", info); }
void Unparser::unparseModAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "%=", info); }
void Unparser::unparseXorAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "^=", info); }
void Unparser::unparseLShiftAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<<=", info); }
void Unparser::unparseRShiftAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ">>=", info); }
void Unparser::unparseForDeclOp(SgExpression* expr, SgUnparse_Info& info) {}

void Unparser::unparseTypeRef(SgExpression* expr, SgUnparse_Info& info) {
  SgRefExp* type_ref = isSgRefExp(expr);
  ROSE_ASSERT(type_ref != NULL);
  /* code inserted from specification */

  SgUnparse_Info newinfo(info);
  newinfo.unset_PrintName();
  newinfo.unset_isTypeFirstPart();
  newinfo.unset_isTypeSecondPart();
  
  unparseType(type_ref->get_type_name(), newinfo);
}

void Unparser::unparseVConst(SgExpression* expr, SgUnparse_Info& info) {}
void Unparser::unparseExprInit(SgExpression* expr, SgUnparse_Info& info) {}
void Unparser::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
     ROSE_ASSERT(aggr_init != NULL);
  /* code inserted from specification */

     SgUnparse_Info newinfo(info);
      cur << "{";

     SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();
     SgExpressionPtrList::iterator p = list.begin();
     if (p != list.end())
        {
          while (1)
             {
               unparseExpression((*p), newinfo);
               p++;
               if (p != list.end())
                  {  cur << ", ";  }
               else
                  break;
             }
        }

      cur << "}";
   }

void
Unparser::unparseConInit(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("In Unparser::unparseConInit expr = %p \n",expr);

     SgConstructorInitializer* con_init = isSgConstructorInitializer(expr);
     ROSE_ASSERT(con_init != NULL);
  /* code inserted from specification */

     SgUnparse_Info newinfo(info);

  // printf ("con_init->get_need_name() = %s \n",(con_init->get_need_name() == true) ? "true" : "false");

     if (con_init->get_need_name() == true)
        {
       // for foo(B())
          SgName nm;

       // DQ (12/4/2003): Added assertion (one of these is required!)
          ROSE_ASSERT (con_init->get_declaration() != NULL || con_init->get_class_decl() != NULL);

       // printf ("con_init->get_declaration() = %s \n",con_init->get_declaration() ? "true" : "false");
          if (con_init->get_declaration() != NULL)
             {
            // printf ("con_init->get_need_qualifier() = %s \n",con_init->get_need_qualifier() ? "true" : "false");
               if (con_init->get_need_qualifier())
                    nm = con_init->get_declaration()->get_qualified_name();
                 else
                    nm = con_init->get_declaration()->get_name();
             }
            else
             {
            // printf ("con_init->get_class_decl() = %s \n",con_init->get_class_decl() ? "true" : "false");
               if (con_init->get_class_decl() != NULL)
                  {
                 // printf ("con_init->get_need_qualifier() = %s \n",con_init->get_need_qualifier() ? "true" : "false");
                    if (con_init->get_need_qualifier()) 
                         nm = con_init->get_class_decl()->get_qualified_name();
                      else
                         nm = con_init->get_class_decl()->get_name();
                  }
             }

          ROSE_ASSERT ( nm.is_null() == false );
       // printf ("In Unparser::unparseConInit: info.PrintName() = %s nm = %s \n",info.PrintName() ? "true" : "false",nm.str());

       // purify error: nm.str() could be a NULL string
       // if (printConstructorName(con_init) && info.PrintName())
       // if ( printConstructorName(con_init) )
          if ( printConstructorName(con_init) && !nm.is_null() )
             {
            // printf ("printConstructorName(con_init) == TRUE \n");
               cur << nm.str();
             }
        }

  // printf ("Now unparse the constructor arguments \n");
  // newinfo.display("Unparser::unparseConInit");

     if (con_init->get_args())
        {
          cur << "(";
          unparseExpression(con_init->get_args(), newinfo);
          cur << ")";
        }
       else
        {
       // DQ (5/29/2004) Skip this so that we can avoid unparsing "B b;" as "B b();" since
       // this is a problem for g++ if a reference is taken to "b" (see test2004_44.C).
       // Verify that P::P():B() {} will still unparse correctly, ... it does!
#if 0
       // for P::P():B() {}
       // if (con_init->get_need_name() || con_init->get_need_paren())
          if (con_init->get_need_paren() == true)
             {
               printf ("Handle case of: P::P():B() {} (where B is a data member in the preinitialization list) \n");
               cur << "()";
             }
#endif
        }

  // printf ("Leaving Unparser::unparseConInit \n");
   }

void Unparser::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ROSE_ASSERT(assn_init != NULL);
  /* code inserted from specification */

     unparseExpression(assn_init->get_operand(), info);
   }

void Unparser::unparseThrowOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgThrowOp* throw_op = isSgThrowOp(expr);
     ROSE_ASSERT(throw_op != NULL);

  // printf ("In unparseThrowOp(%s) \n",expr->sage_class_name());
  // cur << "\n/* In unparseThrowOp(" << expr->sage_class_name() << ") */\n";

#if 0
     cur << "throw ";
     if ( throw_op->get_operand() != NULL )
          unparseExpression(throw_op->get_operand(), info);
#else
  // DQ (9/19/2004): Added support for different types of throw expressions!
     switch ( throw_op->get_throwKind() )
        {
          case SgThrowOp::unknown_throw:
             {
               printf ("Error: case of SgThrowOp::unknown_throw in unparseThrowOp() \n");
               break;
             }

          case SgThrowOp::throw_expression:
             {
               printf ("Case of SgThrowOp::throw_expression in unparseThrowOp() \n");
               cur << "throw ";

            // DQ not sure if we want to have the extra parentheses, EDG accepts them, but g++ does not (so skip them)
            // Nice example of where parenthesis are not meaningless.
            // cur << "(";

               ROSE_ASSERT(throw_op->get_operand() != NULL);
               unparseExpression(throw_op->get_operand(), info);

            // DQ skip  closing ")" see comment above 
            // cur << ")";
               break;
             }

          case SgThrowOp::throw_exception_specification:
             {
               printf ("Case of SgThrowOp::throw_exception_specification in unparseThrowOp() \n");
               cur << "throw";
               ROSE_ASSERT(throw_op->get_typeList() != NULL);

            // Loop over list of types and output each one as a name only
               SgTypePtrList & typeList = *(throw_op->get_typeList());

               if (typeList.size() > 0)
                  {
                    cur << ")";
                    SgUnparse_Info info1(info);

                    for (SgTypePtrList::iterator i = typeList.begin(); i != typeList.end(); ++i)
                       {
                      // output the type name
                         unparseType(*i,info1);
                       }

                    printf ("Sorry, not implemented \n");
                    ROSE_ASSERT(false);

                    cur << ")";
                  }
               
               break;
             }

          case SgThrowOp::rethrow:
             {
               printf ("Case of SgThrowOp::rethrow in unparseThrowOp() \n");
               cur << "throw";
               break;
             }

          default:
               printf ("Error: default reached in unparseThrowOp() \n");
               ROSE_ASSERT(false);
               break;
        }

#endif
   }


void
Unparser::unparseVarArgStartOp(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Inside of Unparser::unparseVarArgStartOp \n");

     SgVarArgStartOp* varArgStart = isSgVarArgStartOp(expr);
     ROSE_ASSERT (varArgStart != NULL);
     SgExpression* lhsOperand = varArgStart->get_lhs_operand();
     SgExpression* rhsOperand = varArgStart->get_rhs_operand();

     ROSE_ASSERT (lhsOperand != NULL);
     ROSE_ASSERT (rhsOperand != NULL);

     cur << "va_start(";
     unparseExpression(lhsOperand,info);
     cur << ",";
     unparseExpression(rhsOperand,info);
     cur << ")";
   }

void
Unparser::unparseVarArgStartOneOperandOp(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Inside of Unparser::unparseVarArgStartOneOperandOp \n");

     SgVarArgStartOneOperandOp* varArgStart = isSgVarArgStartOneOperandOp(expr);
     ROSE_ASSERT (varArgStart != NULL);
     SgExpression* operand = varArgStart->get_operand_expr();
     ROSE_ASSERT (operand != NULL);

     cur << "va_start(";
     unparseExpression(operand,info);
     cur << ")";
   }

void
Unparser::unparseVarArgOp(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Inside of Unparser::unparseVarArgOp \n");

     SgVarArgOp* varArg = isSgVarArgOp(expr);
     ROSE_ASSERT (varArg != NULL);
     SgExpression* operand = varArg->get_operand_expr();
     SgType*       type    = varArg->get_expression_type();

     ROSE_ASSERT (operand != NULL);
     ROSE_ASSERT (type != NULL);

     cur << "va_arg(";
     unparseExpression(operand,info);
     cur << ",";
     unparseType(type,info);
     cur << ")";
   }

void
Unparser::unparseVarArgEndOp(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Inside of Unparser::unparseVarArgEndOp \n");

     SgVarArgEndOp* varArgEnd = isSgVarArgEndOp(expr);
     ROSE_ASSERT (varArgEnd != NULL);
     SgExpression* operand = varArgEnd->get_operand_expr();
     ROSE_ASSERT (operand != NULL);

     cur << "va_end(";
     unparseExpression(operand,info);
     cur << ")";
   }

void
Unparser::unparseVarArgCopyOp(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Inside of Unparser::unparseVarArgCopyOp \n");

     SgVarArgCopyOp* varArgCopy = isSgVarArgCopyOp(expr);

     SgExpression* lhsOperand = varArgCopy->get_lhs_operand();
     SgExpression* rhsOperand = varArgCopy->get_rhs_operand();

     ROSE_ASSERT (lhsOperand != NULL);
     ROSE_ASSERT (rhsOperand != NULL);

     cur << "va_copy(";
     unparseExpression(lhsOperand,info);
     cur << ",";
     unparseExpression(rhsOperand,info);
     cur << ")";
   }

