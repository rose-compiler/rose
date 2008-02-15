/* modified_sage.C
 *
 * This C file includes functions that test for operator overloaded functions and 
 * helper unparse functions such as unparse_helper and printSpecifier.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"
#include "unparser.h"
// include "bool.h"

//-----------------------------------------------------------------------------------
//  void Unparser::isOperator
//  
//  General function to test if this expression is an unary or binary operator
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparser::isOperator(SgExpression* expr) {
  ROSE_ASSERT(expr != NULL);
  
  if (isBinaryOperator(expr) || isUnaryOperator(expr))
    return TRUE;
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isBinaryEqualsOperator
//  
//  Auxiliary function to test if this expression is a binary operator= overloading 
//  function
//-----------------------------------------------------------------------------------
bool Unparser::isBinaryEqualsOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  if (!func_ref && !mfunc_ref) return FALSE;

  char* func_name;
  if (func_ref != NULL)
       func_name = strdup(func_ref->get_symbol()->get_name().str());
    else
       func_name = strdup(mfunc_ref->get_symbol()->get_name().str());
  
  if (!strcmp(func_name, "operator=")) 
    return TRUE;
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isBinaryEqualityOperator
//  
//  Auxiliary function to test if this expression is a binary operator== 
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparser::isBinaryEqualityOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  if (!func_ref && !mfunc_ref) return FALSE;

  char* func_name;
  if (func_ref != NULL)
       func_name = strdup(func_ref->get_symbol()->get_name().str());
    else
       func_name = strdup(mfunc_ref->get_symbol()->get_name().str());
  
  if (!strcmp(func_name, "operator==")) 
    return TRUE;
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isBinaryInequalityOperator
//  
//  Auxiliary function to test if this expression is a binary operator== 
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparser::isBinaryInequalityOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  if (!func_ref && !mfunc_ref) return FALSE;

  char* func_name;
  if(func_ref != NULL)
    func_name = strdup(func_ref->get_symbol()->get_name().str());
  else
    func_name = strdup(mfunc_ref->get_symbol()->get_name().str());
  
  if( !strcmp(func_name, "operator<=") ||
      !strcmp(func_name, "operator>=") || 
      !strcmp(func_name, "operator<" ) || 
      !strcmp(func_name, "operator>" ) ||
      !strcmp(func_name, "operator!=" )  ) 
    return TRUE;

  return FALSE;

}

//-----------------------------------------------------------------------------------
//  void Unparser::isBinaryArithmeticOperator
//  
//  Auxiliary function to test if this expression is a binary operator== 
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparser::isBinaryArithmeticOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  if (!func_ref && !mfunc_ref) return FALSE;

  char* func_name;
  if(func_ref != NULL)
    func_name = strdup(func_ref->get_symbol()->get_name().str());
  else
    func_name = strdup(mfunc_ref->get_symbol()->get_name().str());
  
  if( !strcmp(func_name, "operator+")  ||
      !strcmp(func_name, "operator-")  || 
      !strcmp(func_name, "operator*" ) || 
      !strcmp(func_name, "operator/" ) || 
      !strcmp(func_name, "operator+=") ||
      !strcmp(func_name, "operator-=") || 
      !strcmp(func_name, "operator*=" )|| 
      !strcmp(func_name, "operator/=" )  ) 
    return TRUE;

  return FALSE;

}

//-----------------------------------------------------------------------------------
//  void Unparser::isBinaryParenOperator
//  
//  Auxiliary function to test if this expression is a binary operator() overloading 
//  function
//-----------------------------------------------------------------------------------
bool Unparser::isBinaryParenOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  if (!func_ref && !mfunc_ref) return FALSE;

  char* func_name;
  if (func_ref != NULL)
       func_name = strdup(func_ref->get_symbol()->get_name().str());
    else
       func_name = strdup(mfunc_ref->get_symbol()->get_name().str());

  if (!strcmp(func_name, "operator()")) 
    return TRUE;
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isBinaryBracketOperator
//  
//  Auxiliary function to test if this expression is a binary operator[] overloading 
//  function
//-----------------------------------------------------------------------------------
bool Unparser::isBinaryBracketOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  if (!func_ref && !mfunc_ref) return FALSE;

  char* func_name;
  if (func_ref != NULL)
       func_name = strdup(func_ref->get_symbol()->get_name().str());
    else
       func_name = strdup(mfunc_ref->get_symbol()->get_name().str());

  if (!strcmp(func_name, "operator[]")) 
    return TRUE;
  return FALSE;

}


//-----------------------------------------------------------------------------------
//  void Unparser::isBinaryOperator
//  
//  Function to test if this expression is a binary operator overloading function
//-----------------------------------------------------------------------------------
bool Unparser::isBinaryOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  if (!func_ref && !mfunc_ref) return FALSE;

  char* func_name;
  if (func_ref != NULL)
       func_name = strdup(func_ref->get_symbol()->get_name().str());
    else
       func_name = strdup(mfunc_ref->get_symbol()->get_name().str());
  
  if (!strcmp(func_name, "operator+") ||
      !strcmp(func_name, "operator-") ||
      !strcmp(func_name, "operator*") ||
      !strcmp(func_name, "operator/") ||
      !strcmp(func_name, "operator%") ||
      !strcmp(func_name, "operator^") ||
      !strcmp(func_name, "operator&") ||
      !strcmp(func_name, "operator|") ||
      !strcmp(func_name, "operator=") ||
      !strcmp(func_name, "operator<") ||
      !strcmp(func_name, "operator>") ||
      !strcmp(func_name, "operator+=") ||
      !strcmp(func_name, "operator-=") ||
      !strcmp(func_name, "operator*=") ||
      !strcmp(func_name, "operator/=") ||
      !strcmp(func_name, "operator%=") ||
      !strcmp(func_name, "operator^=") ||
      !strcmp(func_name, "operator&=") ||
      !strcmp(func_name, "operator|=") ||
      !strcmp(func_name, "operator<<") ||
      !strcmp(func_name, "operator>>") ||
      !strcmp(func_name, "operator>>=") ||
      !strcmp(func_name, "operator<<=") ||
      !strcmp(func_name, "operator==") ||
      !strcmp(func_name, "operator!=") ||
      !strcmp(func_name, "operator<=") ||
      !strcmp(func_name, "operator>=") ||
      !strcmp(func_name, "operator&&") ||
      !strcmp(func_name, "operator||") ||
      !strcmp(func_name, "operator,") ||
      !strcmp(func_name, "operator->*") ||
      !strcmp(func_name, "operator->") ||
      !strcmp(func_name, "operator()") ||
      !strcmp(func_name, "operator[]"))
    return TRUE;
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isUnaryOperatorPlus
//  
//  Auxiliary function to test if this expression is an unary operator+ overloading 
//  function
//-----------------------------------------------------------------------------------
bool Unparser::isUnaryOperatorPlus(SgExpression* expr) {
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  if (mfunc_ref != NULL) {
    SgMemberFunctionSymbol* mfunc_sym = mfunc_ref->get_symbol();
    if (mfunc_sym != NULL) {
      SgMemberFunctionDeclaration* mfunc_decl = mfunc_sym->get_declaration();
      if (mfunc_decl != NULL) {
	SgName func_name = mfunc_decl->get_name();
	if (!strcmp(func_name.str(), "operator+")) {
	  SgInitializedNamePtrList argList = mfunc_decl->get_args();
	  if (argList.size() == 0) return TRUE;
	}
      }
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isUnaryOperatorMinus
//  
//  Auxiliary function to test if this expression is an unary operator- overloading 
//  function
//-----------------------------------------------------------------------------------
bool Unparser::isUnaryOperatorMinus(SgExpression* expr) {
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  if (mfunc_ref != NULL) {
    SgMemberFunctionSymbol* mfunc_sym = mfunc_ref->get_symbol();
    if (mfunc_sym != NULL) {
      SgMemberFunctionDeclaration* mfunc_decl = mfunc_sym->get_declaration();
      if (mfunc_decl != NULL) {
	SgName func_name = mfunc_decl->get_name();
	if (!strcmp(func_name.str(), "operator-")) {
	  SgInitializedNamePtrList argList = mfunc_decl->get_args();
	  if (argList.size() == 0) return TRUE;
	}
      }
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isUnaryOperator
//  
//  Function to test if this expression is an unary operator overloading function
//-----------------------------------------------------------------------------------
bool Unparser::isUnaryOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  
  if (!func_ref && !mfunc_ref) return FALSE;

  char* func_name;
  if (func_ref != NULL)
       func_name = strdup(func_ref->get_symbol()->get_name().str());
    else 
       func_name = strdup(mfunc_ref->get_symbol()->get_name().str());

  if (isUnaryOperatorPlus(mfunc_ref) ||
      isUnaryOperatorMinus(mfunc_ref) ||
      !strcmp(func_name, "operator!") ||
      !strcmp(func_name, "operator*") ||
      !strcmp(func_name, "operator--") ||
      !strcmp(func_name, "operator++") ||
      !strcmp(func_name, "operator~"))
    return TRUE;
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isUnaryPostfixOperator
//  
//  Auxiliary function to test if this expression is an unary postfix operator 
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparser::isUnaryPostfixOperator(SgExpression* expr) {
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  if (mfunc_ref != NULL) {
    SgMemberFunctionSymbol* mfunc_sym = mfunc_ref->get_symbol();
    if (mfunc_sym != NULL) {
      SgMemberFunctionDeclaration* mfunc_decl = mfunc_sym->get_declaration();
      if (mfunc_decl != NULL) {
	SgName func_name = mfunc_decl->get_name();
	if (!strcmp(func_name.str(), "operator++") ||
	    !strcmp(func_name.str(), "operator--")) {
	  SgInitializedNamePtrList argList = mfunc_decl->get_args();
	  //postfix operators have one argument (0), prefix operators have none ()
	  if (argList.size() == 1) return TRUE;
	}
      }
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::RemoveArgs
//  
//  This function determines whether to remove the argument list of overloaded 
//  operators. It returns TRUE if the function is an unary operator. Otherwise, it
//  returns FALSE for all oterh functions. 
//-----------------------------------------------------------------------------------
bool Unparser::RemoveArgs(SgExpression* expr) {
  SgBinaryOp* binary_op = isSgBinaryOp(expr);
  if (!binary_op) return FALSE;
  
  //all overloaded unary operators must be the rhs of a binary expression. 
  if (binary_op->get_rhs_operand() != NULL &&
      (isUnaryOperator(binary_op->get_rhs_operand()) ||
       isOverloadedArrowOperator(binary_op->get_rhs_operand())))
    return TRUE;
  return FALSE;
}


//QY 6/23/04: added code for precedence of expressions
//-----------------------------------------------------------------------------------
//  void GetOperatorVariant
//  
//  Function that returns the expression variant of overloaded operators
//-----------------------------------------------------------------------------------

static int GetOperatorVariant(SgExpression* expr) {
  SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
  if (func_call == NULL) {
      return expr->variantT();
  }
 
  SgName name;
  SgExpression *func = func_call->get_function();
  switch (func->variantT()) {
  case V_SgFunctionRefExp:
        name = isSgFunctionRefExp(func)->get_symbol()->get_name();
        break;
  case V_SgDotExp:
  case V_SgArrowExp:
   {
       SgExpression *mfunc = isSgBinaryOp(func)->get_rhs_operand(); 
       SgMemberFunctionRefExp* mfunc_ref =
             isSgMemberFunctionRefExp(mfunc);
       assert (mfunc_ref != 0);
       name = mfunc_ref->get_symbol()->get_name();
       break;   
   }
   default:
       return V_SgFunctionCallExp;
   }

   char* func_name = name.str();
   ROSE_ASSERT(func_name != NULL);
   if (!strcmp(func_name, "operator,")) return V_SgCommaOpExp;
    else if (!strcmp(func_name, "operator=")) return V_SgAssignOp;
    else if (!strcmp(func_name, "operator+=")) return V_SgPlusAssignOp;
    else if (!strcmp(func_name, "operator-=")) return V_SgMinusAssignOp;
    else if (!strcmp(func_name, "operator&=")) return V_SgAndAssignOp;
    else if (!strcmp(func_name, "operator|=")) return V_SgIorAssignOp;
    else if (!strcmp(func_name, "operator*=")) return V_SgMultAssignOp;
    else if (!strcmp(func_name, "operator/=")) return V_SgDivAssignOp;
    else if (!strcmp(func_name, "operator%=")) return V_SgModAssignOp;
    else if (!strcmp(func_name, "operator^=")) return V_SgXorAssignOp;
    else if (!strcmp(func_name, "operator<<=")) return  V_SgLshiftAssignOp;
    else if (!strcmp(func_name, "operator>>=")) return V_SgRshiftAssignOp;
    else if (!strcmp(func_name, "operator||")) return  V_SgOrOp;
    else if (!strcmp(func_name, "operator&&")) return V_SgAndOp;
    else if (!strcmp(func_name, "operator|")) return V_SgBitOrOp;
    else if (!strcmp(func_name, "operator^")) return  V_SgBitXorOp;
    else if (!strcmp(func_name, "operator&")) return V_SgBitAndOp;
    else if (!strcmp(func_name, "operator==")) return V_SgEqualityOp;
    else if (!strcmp(func_name, "operator!=")) return V_SgNotEqualOp;
    else if (!strcmp(func_name, "operator<")) return V_SgLessThanOp;
    else if (!strcmp(func_name, "operator>")) return V_SgGreaterThanOp;
    else if (!strcmp(func_name, "operator<=")) return V_SgLessOrEqualOp;
    else if (!strcmp(func_name, "operator>=")) return V_SgGreaterOrEqualOp;
    else if (!strcmp(func_name, "operator<<")) return V_SgLshiftOp;
    else if (!strcmp(func_name, "operator>>")) return V_SgRshiftOp;
    else if (!strcmp(func_name, "operator+")) return V_SgAddOp;
    else if (!strcmp(func_name, "operator-")) return V_SgSubtractOp;
    else if (!strcmp(func_name, "operator*")) return V_SgMultiplyOp;
    else if (!strcmp(func_name, "operator/")) return V_SgDivideOp;
    else if (!strcmp(func_name, "operator%")) return V_SgModOp;
    else if (!strcmp(func_name, "operator.*")) return V_SgDotStarOp;
    else if (!strcmp(func_name, "operator->*")) return V_SgArrowStarOp;
    else if (!strcmp(func_name, "operator++")) return V_SgPlusPlusOp;
    else if (!strcmp(func_name, "operator--")) return V_SgMinusMinusOp;
    else if (!strcmp(func_name, "operator~")) return V_SgBitComplementOp;
    else if (!strcmp(func_name, "operator!")) return V_SgNotOp;
    else if (!strcmp(func_name, "operator[]")) return V_SgPntrArrRefExp;
    else if (!strcmp(func_name, "operator->")) return V_SgArrowExp;
    else if (!strcmp(func_name, "operator.")) return V_SgDotExp;
    else if (!strstr(func_name,"operator") || !strcmp(func_name, "operator()")) return V_SgFunctionCallExp;
    else if (strstr(func_name,"operator")) return V_SgCastExp;
    else
     {
       ROSE_ASSERT(func_name != NULL);
       printf ("Error: default case reached in GetOperatorVariant func_name = %s \n",func_name);
       assert(false);
     }
}

static SgExpression* GetFirstOperand(SgExpression* expr) {
  SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
  if (func_call != NULL) 
     return func_call->get_function();
  else {
    SgUnaryOp *op1 = isSgUnaryOp(expr);
    if (op1 != 0)
       return op1->get_operand();
    else {
       SgBinaryOp *op2 = isSgBinaryOp(expr);
       if (op2 != 0)
          return op2->get_lhs_operand();
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------------
//  int GetPrecedence
//  
//  returns the precedence (1-17) of the expression variants, 
//  such that 17 has the highest precedence and 1 has the lowest precedence. 
//-----------------------------------------------------------------------------------
static int GetPrecedence(int variant) {
  switch (variant) {
    case V_SgExprListExp: 
    case V_SgCommaOpExp: return 1;
    case V_SgAssignOp: return 2;
    case V_SgPlusAssignOp: return 2;
    case V_SgMinusAssignOp: return 2; 
    case V_SgAndAssignOp: return 2; 
    case V_SgIorAssignOp: return 2;
    case V_SgMultAssignOp: return 2;
    case V_SgDivAssignOp: return 2; 
    case V_SgModAssignOp: return 2;
    case V_SgXorAssignOp: return 2;
    case V_SgLshiftAssignOp: return 2;
    case V_SgRshiftAssignOp: return 2;
    case V_SgConditionalExp: return 3;
    case V_SgOrOp: return 4; 
    case V_SgAndOp: return 5; 
    case V_SgBitOrOp: return 6;
    case V_SgBitXorOp: return 7; 
    case V_SgBitAndOp: return 8; 
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
    case V_SgModOp: return 13; 
    case V_SgDotStarOp: return 14;
    case V_SgArrowStarOp: return 14;
    case V_SgPlusPlusOp: return 15;
    case V_SgMinusMinusOp: return 15;
    case V_SgBitComplementOp: return 15;
    case V_SgNotOp: return 15;
    case V_SgPointerDerefExp:
    case V_SgAddressOfOp:
    case V_SgSizeOfOp: return 15;
    case V_SgFunctionCallExp: return 16;
    case V_SgPntrArrRefExp: return 16;
    case V_SgArrowExp: return 16;
    case V_SgDotExp: return 16;
  }
  return 0;
}

//-----------------------------------------------------------------------------------
//  GetAssociativity
//
//  Function that returns the associativity of the expression variants,
//  -1: left associative; 1: right associative; 0 : not associative/unknown
//-----------------------------------------------------------------------------------
static int GetAssociativity(int variant) {
  switch (variant) {
    case V_SgCommaOpExp: return -1;
    case V_SgAssignOp: return 1;
    case V_SgPlusAssignOp: return 1;
    case V_SgMinusAssignOp: return 1;
    case V_SgAndAssignOp: return 1;
    case V_SgIorAssignOp: return 1;
    case V_SgMultAssignOp: return 1;
    case V_SgDivAssignOp: return 1;
    case V_SgModAssignOp: return 1;
    case V_SgXorAssignOp: return 1;
    case V_SgLshiftAssignOp: return 1;
    case V_SgRshiftAssignOp: return 1;
    case V_SgConditionalExp: return 1;
    case V_SgOrOp: return -1;
    case V_SgAndOp: return -1;
    case V_SgBitOrOp: return -1;
    case V_SgBitXorOp: return -1;
    case V_SgBitAndOp: return -1;
    case V_SgEqualityOp: return -1;
    case V_SgNotEqualOp: return -1;
    case V_SgLessThanOp: return -1;
    case V_SgGreaterThanOp: return -1;
    case V_SgLessOrEqualOp:  return -1;
    case V_SgGreaterOrEqualOp:  return -1;
    case V_SgLshiftOp:  return -1;
    case V_SgRshiftOp:  return -1;
    case V_SgAddOp: return -1;
    case V_SgSubtractOp: return -1;
    case V_SgMultiplyOp: return -1;
    case V_SgIntegerDivideOp:
    case V_SgDivideOp: return -1;
    case V_SgModOp: return -1;
    case V_SgDotStarOp: return -1;
    case V_SgArrowStarOp: return -1;
    case V_SgBitComplementOp: return 1;
    case V_SgNotOp: return 1;
    case V_SgPointerDerefExp:
    case V_SgAddressOfOp:
    case V_SgSizeOfOp: return 1;
    case V_SgFunctionCallExp: return -1;
    case V_SgPntrArrRefExp: return -1;
    case V_SgArrowExp: return -1;
    case V_SgDotExp: return -1;
  }
  return 0;
}



//-----------------------------------------------------------------------------------
//  void Unparser::PrintStartParen
//  
//  Auxiliary function that determines whether "(" should been printed for a binary
//  expression. This function is needed whenever the rhs of the binary expression
//  is an operator= overloaded function. 
//-----------------------------------------------------------------------------------
bool Unparser::PrintStartParen(SgExpression* expr, SgUnparse_Info& info) 
{
  SgExpression* parentExpr = isSgExpression(expr->get_parent());
  if (parentExpr == 0 || parentExpr->variantT() == V_SgExpressionRoot || 
      expr->variantT() == V_SgExprListExp 
      || expr->variantT() == V_SgConstructorInitializer)
       return false;
  if (parentExpr->variantT() == V_SgAssignInitializer) { 
      return true; 
  }
  switch (expr->variant()) {
      case VAR_REF: 
      case CLASSNAME_REF: 
      case FUNCTION_REF: 
      case MEMBER_FUNCTION_REF: 
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
      case AGGREGATE_INIT:
         return false;
      default:
        {
         int parentVariant = GetOperatorVariant(parentExpr);
         SgExpression* first = GetFirstOperand(parentExpr);
         if (parentVariant  == V_SgPntrArrRefExp && first != expr)
             return false;
         int parentPrecedence = GetPrecedence(parentVariant);
         if (parentPrecedence == 0)
             return true;
         int exprVariant = GetOperatorVariant(expr);
         int exprPrecedence = GetPrecedence(exprVariant);
         if (exprPrecedence > parentPrecedence)
             return false;
         else if (exprPrecedence == parentPrecedence) {
            if (first == 0)
                 return true; 
            int assoc =  GetAssociativity(parentVariant);
            if (assoc > 0 && first != expr)
               return false;
            if (assoc < 0 && first == expr)
                return false;
         }
       }
   }
   return true;
}

//-----------------------------------------------------------------------------------
//  bool Unparser::RemovePareninExprList
//
//  Auxiliary function to determine whether parenthesis is needed around
//  this expression list. If the list only contains one element and the
//  element is a binary operator whose rhs is an operator(), then parens
//  are removed (return TRUE). Otherwise, return FALSE.
//-----------------------------------------------------------------------------------
bool Unparser::RemovePareninExprList(SgExprListExp* expr_list) {
  ROSE_ASSERT(expr_list != NULL);
  SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();

  if (i != expr_list->get_expressions().end()) {
#if USE_SAGE3
    SgFunctionCallExp* func_call = isSgFunctionCallExp(*i);
#else
    SgFunctionCallExp* func_call = isSgFunctionCallExp((*i).irep());
#endif
    i++;
    if (func_call != NULL) {
      SgDotExp* dot_exp = isSgDotExp(func_call->get_function());
      if (dot_exp != NULL) {
	SgBinaryOp* binary_op = isSgBinaryOp(dot_exp);
	if (binary_op != NULL) {
	  //check if there is only one expression in the list and this expression
	  //contains the member operator() overloaded function
	  if (i == expr_list->get_expressions().end() && 
	      isBinaryParenOperator(binary_op->get_rhs_operand()))
	    return TRUE;
	}
      }
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  bool Unparser::isOneElementList
//
//  Auxiliary function to determine if the argument list of the Constructor
//  Initializer contains only one element.
//-----------------------------------------------------------------------------------
bool Unparser::isOneElementList(SgConstructorInitializer* con_init) {
  ROSE_ASSERT(con_init != NULL);
  if (con_init->get_args()) {
    SgExprListExp* expr_list = isSgExprListExp(con_init->get_args());
    if (expr_list != NULL) {
      SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();
      if (i != expr_list->get_expressions().end()) {
	i++;
	if (i == expr_list->get_expressions().end())
	  return TRUE;
      }
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::unparseOneElemConInit
//
//  This function is used to only unparse constructor initializers with one 
//  element in their argument list. Because there is only one argument in the
//  list, the parenthesis around the arguments are not unparsed (to control
//  the excessive printing of parenthesis). 
//-----------------------------------------------------------------------------------
void Unparser::unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info)
   {
     ROSE_ASSERT(con_init != NULL);
  /* code inserted from specification */
  
  // taken from unparseConInit
     SgUnparse_Info newinfo(info);
     if(con_init->get_need_name())
        {
          SgName nm;
          if(con_init->get_declaration())
             {
               if(con_init->get_need_qualifier()) 
                    nm = con_init->get_declaration()->get_qualified_name();
                 else
                    nm = con_init->get_declaration()->get_name();
             }
            else
             {
//             ROSE_ASSERT (con_init->get_class_decl() != NULL);
               if(con_init->get_class_decl())
                  {
                    if(con_init->get_need_qualifier()) 
                         nm = con_init->get_class_decl()->get_qualified_name();
                      else
                         nm = con_init->get_class_decl()->get_name();
                  }
             }

          if ( printConstructorName(con_init) && (nm.str() != NULL) )
             {
               cur << nm.str();
             }
        }

  // taken from unparseExprList
  // check whether the constructor name was printed. If so, we need to surround
  // the arguments of the constructor with parenthesis.
     if (con_init->get_need_name() && printConstructorName(con_init)) 
        {
          cur << "("; printDebugInfo("( from OneElemConInit", TRUE);
        }
     if (con_init->get_args())
        {
          SgExprListExp* expr_list = isSgExprListExp(con_init->get_args());
          ROSE_ASSERT(expr_list != NULL);
          SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();
          if (i != expr_list->get_expressions().end())
             {
               SgUnparse_Info newinfo(info);
               newinfo.set_SkipBaseType();
               unparseExpression(*i, newinfo);
             }
        }

     if (con_init->get_need_name() && printConstructorName(con_init)) 
        {
          cur << ")";
          printDebugInfo(") from OneElemConInit", TRUE);
        }
   }

//-----------------------------------------------------------------------------------
//  bool Unparser::printConstructorName
//
//  This function determines whether to print the name of the constructor or not. 
//  The constructor name is printed if the operator overloaded function in the 
//  constructor returns a reference or pointer to the object. If a value is 
//  returned, then the printing of the constructor is suppressed. 
//-----------------------------------------------------------------------------------
bool Unparser::printConstructorName(SgExpression* expr)
   {
     SgConstructorInitializer* con_init = isSgConstructorInitializer(expr);
  
  if (con_init != NULL) {
    SgExprListExp* expr_list = isSgExprListExp(con_init->get_args());
    if (expr_list != NULL) {
      SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();
      if (i != expr_list->get_expressions().end()) {
	//now iterate through all the arguments
	while (1) {
	  //check if the argument is a constructor initialiaizer. If so, we
	  //recursively call the printConstructorName function.
#if USE_SAGE3
	  if ((*i)->variant() == CONSTRUCTOR_INIT)
	    return printConstructorName(*i);
	  //check if the argument is a function call
	  if ((*i)->variant() == FUNC_CALL) {
	    SgFunctionCallExp* func_call = isSgFunctionCallExp(*i);
#else
	  if ((*i).irep()->variant() == CONSTRUCTOR_INIT)
	    return printConstructorName((*i).irep());
	  //check if the argument is a function call
	  if ((*i).irep()->variant() == FUNC_CALL) {
	    SgFunctionCallExp* func_call = isSgFunctionCallExp((*i).irep());
#endif
	    //now check if the function is an overloaded operator function
	    if (isOperator(func_call->get_function())) {
	      //if so, get the return type
	      SgFunctionRefExp* func_ref = isSgFunctionRefExp(func_call->get_function());
	      SgMemberFunctionRefExp* mfunc_ref = 
		isSgMemberFunctionRefExp(func_call->get_function());
	      SgType* return_type;
	      if (func_ref) return_type = func_ref->get_type();
	      else if (mfunc_ref) return_type = mfunc_ref->get_type();
	      //return type not defined, so default to printing the constructor name
	      else return TRUE;
	      //if the return type is a reference or pointer type, then we print the 
	      //constructor name. Otherwise, the return type is by value and we suppress
	      //the printing of the constructor name.
	      if (return_type->variant() == T_REFERENCE || 
	          return_type->variant() == T_POINTER)
	        return TRUE;
	      return FALSE;
	    }
	  }
	    
	  //continue iterating if we didnt find a constructor initializer 
          //or function call
          i++;
          if (i != expr_list->get_expressions().end());
          else break;
        }
      }
    }
  }
  return TRUE;
}

//-----------------------------------------------------------------------------------
//  void Unparser::isOverloadedArrowOperator
//  
//  Auxiliary function to test if this expression is a binary operator-> or
//  operator->* overloading function
//-----------------------------------------------------------------------------------
bool Unparser::isOverloadedArrowOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

  if (!func_ref && !mfunc_ref) return FALSE;

  char *func_name;
  if (func_ref != NULL)
  {
    func_name = strdup(func_ref->get_symbol()->get_name().str());
  }
  else
  { //
    // [DT] 4/4/2000 -- Adding checks before trying to access
    //      mfunc_ref->get_symbol()->get_name().str().
    //
    //
    //    func_name = strdup(mfunc_ref->get_symbol()->get_name().str());
    //
    // (4/4,cont'd) Substituting the following code, because the call to 
    // strdup() gives rise to a core dump sometimes.  I don't know why.
    //
    // Unfortunately, this doesn't solve the problem anyway.  
    //
    // strcmp(mfunc_ref->get_symbol()->get_name().str(), "operator->"); 
    // strcmp(mfunc_ref->get_symbol()->get_name().str(), "operator->");  
    //
    if( ( !strcmp(mfunc_ref->get_symbol()->get_name().str(), "operator->") || 
          !strcmp(mfunc_ref->get_symbol()->get_name().str(), "operator->*")   ) )
       return TRUE;
    else
       return FALSE;

  }
  
  //
  // [DT] 4/4/2000 -- Added check for func_name!=NULL.
  //
  if ( func_name!=NULL && (!strcmp(func_name, "operator->") || !strcmp(func_name, "operator->*")) )
    return TRUE;
  return FALSE;

}

//-----------------------------------------------------------------------------------
//  bool Unparser::NoDereference
//
//  Auxiliary function used by unparseUnaryExpr and unparseBinaryExpr to 
//  determine whether the dereference operand contains an overloaded arrow operator.
//-----------------------------------------------------------------------------------
bool
Unparser::NoDereference(SgExpression* expr)
   {
     bool arrow = FALSE;

  // now we first check of this is an dereference operator. If so, we need to 
  // determine if the operand contains an overloaded arrow operator. If this
  // is true, set arrow = TRUE so the dereference operator will be suppressed.
     SgPointerDerefExp* deref_exp = isSgPointerDerefExp(expr);
     if (deref_exp != NULL)
        {
          SgFunctionCallExp* func_call = isSgFunctionCallExp(deref_exp->get_operand());
          if (func_call != NULL)
             {
               SgBinaryOp* binary_op = isSgBinaryOp(func_call->get_function());
               if (binary_op != NULL)
                  {
                    arrow = isOverloadedArrowOperator(binary_op->get_rhs_operand());
                  }
             }
        }

     return arrow;
   }

//-----------------------------------------------------------------------------------
//  bool Unparser::isIOStreamOperator
//
//  checks if this expression is an iostream overloaded function
//-----------------------------------------------------------------------------------
bool Unparser::isIOStreamOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  if (!func_ref && !mfunc_ref) return FALSE;

// This results in a FMR in purify since the casting operator for 
// SgName (operator char*) causes a SgName object to be built and 
// then the char* pointer is taken from that temporary char* object 
// (and then the temporary is deleted (goes out of scope) leaving 
// the char* pointing to the string in the deleted object).
  char* func_name = NULL;
  if (func_ref)
       func_name = strdup(func_ref->get_symbol()->get_name().str());
    else 
       func_name = strdup(mfunc_ref->get_symbol()->get_name().str());
  
  //check if the function name is "operator<<" or "operator>>"
  if (!strcmp(func_name, "operator<<") || !strcmp(func_name, "operator>>"))
    return TRUE;

  return FALSE;
}

//-----------------------------------------------------------------------------------
//  bool Unparser::isCast_ConstCharStar
//
//  auxiliary function to determine if this cast expression is "const char*"
//-----------------------------------------------------------------------------------
bool Unparser::isCast_ConstCharStar(SgType* type)
   {
     SgPointerType* pointer_type = isSgPointerType(type);
     if (pointer_type != NULL)
        {
          SgModifierType* modifier_type = isSgModifierType(pointer_type->get_base_type());
          if (modifier_type != NULL && modifier_type->get_typeModifier().get_constVolatileModifier().isConst())
             {
               SgTypeChar* char_type = isSgTypeChar(modifier_type->get_base_type());
               if (char_type != NULL)
                    return true;
             }
        }

     return false;
   }
 
//-----------------------------------------------------------------------------------
//  bool Unparser::noQualifiedName
//
//  auxiliary function to determine if "::" is printed out
//-----------------------------------------------------------------------------------
bool Unparser::noQualifiedName(SgExpression* expr) {
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  if (mfunc_ref != NULL) {
    if (mfunc_ref->get_need_qualifier()) {
      //check if this is a iostream operator function and the value of the overload opt is false
      if (!opt.get_overload_opt() && isIOStreamOperator(mfunc_ref));
      else return FALSE;
    }
  }
  return TRUE;
}
  
//-----------------------------------------------------------------------------------
//  void Unparser::output
//  
//  prints out the line number and file information of the node  
//-----------------------------------------------------------------------------------
/*
void Unparser::output(SgLocatedNode* node) {
  Sg_File_Info* node_file = node->get_file_info();
  int cur_line = node_file->getCurrentLine();
  char* cur_file = node_file->getCurrentFilename();
  char* p_filename = node_file->get_filename();
  int   p_line = node_file->get_line();
  int   p_col = node_file->get_col();
  
  int newFile = !cur_file || !p_filename || (::strcmp(p_filename, cur_file) != 0);
  int newLine = p_line != cur_line;

  if (!newFile && newLine && (p_line > cur_line) && ((p_line - cur_line) < 4)) {
    cur_line = p_line;
  } else if (p_line && (newFile || newLine)) {
    cur << "#" << p_line;
    cur_line = p_line;
    if (newFile && p_filename) {
       cur << " \"" << p_filename << "\"";
      cur_file = p_filename;
    }
  }
  
}
*/
//-----------------------------------------------------------------------------------
//  void Unparser::directives
//  
//  checks the linefile option to determine whether to print line and file info
//  prints out pragmas  
//-----------------------------------------------------------------------------------
void
Unparser::directives(SgLocatedNode* lnode)
   {
  // checks option status before printing line and file info
     if (opt.get_linefile_opt())
        {
/*
          if (lnode->get_file_info()) 
             {
               output(lnode);
             }
*/
        }

     printf ("Unparser::directives(): commented out call to output_pragma() \n");
  // DQ (8/20/2004): Removed this function (old implementation of pragmas)
  // stringstream out;
  // lnode->output_pragma(out);
  // cur << out.str();
   }

void
Unparser::unparseFunctionParameterDeclaration ( 
   SgFunctionDeclaration* funcdecl_stmt, 
   SgInitializedName* initializedName,
   bool outputParameterDeclaration,
   SgUnparse_Info& info )
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

     SgName        tmp_name  = initializedName->get_name();
     SgInitializer *tmp_init = initializedName->get_initializer();
     SgType        *tmp_type = initializedName->get_type();

  // printf ("In unparseFunctionParameterDeclaration(): Argument name = %s \n",
  //      (tmp_name.str() != NULL) ? tmp_name.str() : "NULL NAME");

  // initializedName.get_storageModifier().display("New storage modifiers in unparseFunctionParameterDeclaration()");

     SgStorageModifier & storage = initializedName->get_storageModifier();
     if (storage.isExtern())
        {
          cur << "extern ";
        }

     if (storage.isStatic())
        {
          cur << "static ";
        }

     if (storage.isAuto())
        {
       // DQ (4/30/2004): Auto is a default which is to be supressed 
       // in C old-style parameters and not really ever needed anyway?
       // cur << "auto ";
        }

     if (storage.isRegister())
        {
          cur << "register ";
        }

     if (storage.isMutable())
        {
          cur << "mutable ";
        }

     if (storage.isTypedef())
        {
          cur << "typedef ";
        }

     if (storage.isAsm())
        {
          cur << "asm ";
        }

     if ( (funcdecl_stmt->get_oldStyleDefinition() == false) || (outputParameterDeclaration == true) )
        {
       // output the type name for each argument
          if (tmp_type != NULL)
             {
#if 0
            // DQ (10/17/2004): This is now made more uniform and output in the unparseType() function
               if (isSgNamedType(tmp_type))
                  {
                    SgName theName;
                    theName = isSgNamedType(tmp_type)->get_qualified_name().str();
                    if (!theName.is_null())
                       {
                         cur << theName.str() << "::";
                       }
                  }
#endif
               info.set_isTypeFirstPart();
            // cur << "\n/* unparse_helper(): output the 1st part of the type */\n";
               unparseType(tmp_type, info);

            // forward declarations don't necessarily need the name of the argument
            // so we must check if not NULL before adding to chars_on_line
            // This is a more consistant way to handle the NULL string case
            // cur << "\n/* unparse_helper(): output the name of the type */\n";
               if (tmp_name.str() != NULL)
                  {
                    cur << tmp_name.str();
                  }
                 else
                  {
                 // printf ("In unparse_helper(): Argument name is NULL \n");
                  }

            // output the rest of the type
               info.set_isTypeSecondPart();

            // info.display("unparse_helper(): output the 2nd part of the type");

            // printf ("unparse_helper(): output the 2nd part of the type \n");
            // cur << "\n/* unparse_helper(): output the 2nd part of the type */\n";
               unparseType(tmp_type, info);
            // printf ("DONE: unparse_helper(): output the 2nd part of the type \n");
            // cur << "\n/* DONE: unparse_helper(): output the 2nd part of the type */\n";
             }
            else
             {
               cur << tmp_name.str(); // for ... case
             }
        }
       else
        {
          cur << tmp_name.str(); // for ... case
        }
 
     SgUnparse_Info ninfo3(info);
     ninfo3.unset_inArgList();

  // Add an initializer if it exists
     if(tmp_init)
        {
          if(!(funcdecl_stmt->get_from_template() && !funcdecl_stmt->isForward()))
             {
               cur << "=";
               unparseExpression(tmp_init, ninfo3);
             }
        }
   }

void
Unparser::unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

     SgInitializedNamePtrList::iterator p = funcdecl_stmt->get_args().begin();
     while ( p != funcdecl_stmt->get_args().end() )
        {
          unparseFunctionParameterDeclaration (funcdecl_stmt,*p,false,info);

       // Move to the next argument
          p++;

       // Check if this is the last argument (output a "," separator if not)
          if (p != funcdecl_stmt->get_args().end())
             {
               cur << ",";
             }
        }
   }

//-----------------------------------------------------------------------------------
//  void Unparser::unparse_helper
//
//  prints out the function parameters in a function declaration or function
//  call. For now, all parameters are printed on one line since there is no
//  file information for each parameter.
//-----------------------------------------------------------------------------------
void
Unparser::unparse_helper(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

  // output the function name
     cur << funcdecl_stmt->get_name().str();
  
     SgUnparse_Info ninfo2(info);
     ninfo2.set_inArgList();

  // DQ (5/14/2003): Never output the class definition in the argument list.
  // Using this C++ constraint avoids building a more complex mechanism to turn it off.
     ninfo2.set_SkipClassDefinition();

     cur << "(";

#if 1
     unparseFunctionArgs(funcdecl_stmt,ninfo2);
#else
     SgName tmp_name;
     SgType *tmp_type        = NULL;
     SgInitializer *tmp_init = NULL;

     SgInitializedNamePtrList::iterator p = funcdecl_stmt->get_args().begin();
     if ( p != funcdecl_stmt->get_args().end() )
        {
       // Loop until we explicitly call "break"
          while ( 1 )
             {
               tmp_name = (*p).get_name();
            // printf ("In unparse_helper(): Argument name = %s \n",
            //      (tmp_name.str() != NULL) ? tmp_name.str() : "NULL NAME");
               tmp_init = (*p).get_initializer();
               tmp_type = (*p).get_type();
               p++;

               if ( funcdecl_stmt->get_oldStyleDefinition() )
                  {
                 // Output old-style C (K&R) function definition
                    cur << " /* Skip output of type name */ \n";
                  }
                 else
                  {
                 // output the type name for each argument
                    if (tmp_type != NULL)
                       {
                         if (isSgNamedType(tmp_type))
                            {
                              SgName theName;
                              theName = isSgNamedType(tmp_type)->get_qualified_name().str();
                              if (!theName.is_null())
                                 {
                                   cur << theName.str() << "::";
                                 }
                            }
                         ninfo2.set_isTypeFirstPart();
                      // cur << "\n/* unparse_helper(): output the 1st part of the type */\n";
                         unparseType(tmp_type, ninfo2);

                      // forward declarations don't necessarily need the name of the argument
                      // so we must check if not NULL before adding to chars_on_line
                      // This is a more consistant way to handle the NULL string case
                      // cur << "\n/* unparse_helper(): output the name of the type */\n";
                         if (tmp_name.str() != NULL)
                            {
                              cur << tmp_name.str();
                            }
                           else
                            {
                           // printf ("In unparse_helper(): Argument name is NULL \n");
                            }

                      // output the rest of the type
                         ninfo2.set_isTypeSecondPart();

                      // ninfo2.display("unparse_helper(): output the 2nd part of the type");

                      // printf ("unparse_helper(): output the 2nd part of the type \n");
                      // cur << "\n/* unparse_helper(): output the 2nd part of the type */\n";
                         unparseType(tmp_type, ninfo2);
                      // printf ("DONE: unparse_helper(): output the 2nd part of the type \n");
                       }
                      else
                       {
                         cur << tmp_name.str(); // for ... case
                       }
                  }
               SgUnparse_Info ninfo3(ninfo2);
               ninfo3.unset_inArgList();

            // Add an initializer if it exists
               if(tmp_init)
                  {
                    if(!(funcdecl_stmt->get_from_template() && !funcdecl_stmt->isForward()))
                       {
                         cur << "=";
                         unparseExpression(tmp_init, ninfo3);
                       }
                  }

            // Check if this is the last argument (output a "," separator if not)
               if (p != funcdecl_stmt->get_args().end())
                  {
                    cur << ",";
                  }
                 else
                  {
                 // Exit from while loop explicitly (this is the only way out of this while loop)
                 // printf ("End of function argument list! \n");
                    break;
                  }
             }
        }
#endif
     
  // printf ("Adding a closing \")\" to the end of the argument list \n");
     cur << ")";

     if ( funcdecl_stmt->get_oldStyleDefinition() )
        {
       // Output old-style C (K&R) function definition
       // printf ("Output old-style C (K&R) function definition \n");
       // cur << "/* Output old-style C (K&R) function definition */ \n";

          SgInitializedNamePtrList::iterator p = funcdecl_stmt->get_args().begin();
          if (p != funcdecl_stmt->get_args().end())
               cur.insert_newline();
          while ( p != funcdecl_stmt->get_args().end() )
             {
            // Output declarations for function parameters (using old-style K&R syntax)
            // printf ("Output declarations for function parameters (using old-style K&R syntax) \n");
               unparseFunctionParameterDeclaration(funcdecl_stmt,*p,true,ninfo2);
               cur << ";";
               cur.insert_newline();
               p++;
             }
        }

  // cur << endl;
  // cur << "Added closing \")\" to the end of the argument list \n";
  // cur.flush();

  // printf ("End of function Unparser::unparse_helper() \n");
   }

//-----------------------------------------------------------------------------------
//  void Unparser::printSpecifier1   
//  void Unparser::printSpecifier2
//  void Unparser::printSpecifier
//  
//  The following 2 functions: printSpecifier1 and printSpecifier2, are just the 
//  two halves from printSpecifier. These two functions are used in the unparse 
//  functions for SgMemberFunctionDeclarations and SgVariableDeclaration. 
//  printSpecifier1 is first called before the format function. If "private", 
//  "protected", or "public" is to be printed out, it does so here. Then I format
//  which will put me in position to unparse the declaration. Then I call 
//  printSpecifer2, which will print out any keywords if the option is turned on. 
//  Then the declaration is printed in the same line. If I didnt do this, the 
//  printing of keywords would be done before formatting, and would put the 
//  declaration on another line (and would look terribly formatted).
//-----------------------------------------------------------------------------------
void
Unparser::printSpecifier1 ( SgDeclarationStatement * decl_stmt, SgUnparse_Info & info )
   {
  // only valid in class
  // printf ("In Unparser::printSpecifier1: info.CheckAccess() = %s \n",info.CheckAccess() ? "true" : "false");
     if (info.CheckAccess())
        {
          ROSE_ASSERT (decl_stmt != NULL);
          bool flag = false;
          if (info.isPrivateAccess())
             {
               if (!decl_stmt->get_declarationModifier().get_accessModifier().isPrivate())
                    flag = true;
             }
            else
             {
               if (info.isProtectedAccess())
                  {
                    if (!decl_stmt->get_declarationModifier().get_accessModifier().isProtected())
                         flag = true;
                  }
                 else
                  {
                    if (info.isPublicAccess())
                       {
                         if (!decl_stmt->get_declarationModifier().get_accessModifier().isPublic())
                              flag = true;
                       }
                      else
                         flag = true;
                  }
             }

          info.set_isUnsetAccess();

          if (decl_stmt->get_declarationModifier().get_accessModifier().isPrivate())
             {
               info.set_isPrivateAccess();
               if (flag)
                  {
                    cur << "private: ";
                  }
             }
            else
             {
               if (decl_stmt->get_declarationModifier().get_accessModifier().isProtected())
                  {
                    info.set_isProtectedAccess();
                    if (flag)
                       {
                         cur << "protected: ";
                       }
                  }
                 else
                  {
                 /* default, always print Public */
                    ROSE_ASSERT (decl_stmt->get_declarationModifier().get_accessModifier().isPublic() == true);
                    info.set_isPublicAccess();
                    if (flag)
                       {
                         cur << "public: ";
                       }
                  }
             }
        }
   }

void
Unparser::printSpecifier2(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info) 
   {
     if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && decl_stmt->get_linkage()) 
        {
          cur << "extern \"" << decl_stmt->get_linkage() << "\" ";
          if (decl_stmt->isExternBrace())
             {
               cur << "{ ";
             }
        }

  //
  // [DT] 3/20/2000 -- NOTE: The following commented piece
  //      of code is a relic of the past.  We are no longer
  //      trying to get the unparser to unparse the
  //      template syntax.
  //
  // [DT] ----------------------------------------------
  // if(decl_stmt->isSpecialization() ) if( do_unparse )
  //    {
  //      cur << "template<> ";
  //    }
  // ------------------------------------------------

     if (decl_stmt->get_declarationModifier().isFriend())
        {
          cur << "friend ";
        }

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(decl_stmt);
     if (functionDeclaration != NULL)
        {
          if (functionDeclaration->get_functionModifier().isVirtual())
             {
               cur << "virtual ";
             }

       // if (opt.get_inline_opt())

       // DQ (4/28/2004): output "inline" even for function definitions
       // if (!info.SkipFunctionDefinition())
          if (functionDeclaration->get_functionModifier().isInline())
             {
               cur << "inline ";
             }

       // Only declarations of constructors can be "explicit"
          if ((!info.SkipFunctionDefinition()) && functionDeclaration->get_functionModifier().isExplicit())
             {
               cur << "explicit ";
             }
        }

  // DQ (4/25/2004): Removed CC++ specific modifiers
  // if (decl_stmt->isAtomic() && !info.SkipAtomic() ) { cur << "atomic "; }
  // if (decl_stmt->isGlobalClass() && !info.SkipGlobal() ) { cur << "global "; }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isStatic())
        {
          cur << "static ";
        }

  // if (opt.get_extern_opt())
     if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && !decl_stmt->get_linkage())
        {
          cur << "extern ";
        }

     if (opt.get_auto_opt())    // checks option status before printing auto
          if (decl_stmt->get_declarationModifier().get_storageModifier().isAuto())
             {
               cur << "auto ";
             }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isRegister())
        {
          cur << "register ";
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isMutable())
        {
          cur << "mutable ";
        }
   }


void
Unparser::printSpecifier ( SgDeclarationStatement* decl_stmt, SgUnparse_Info & info )
   {
#if 1
   // DQ (4/25/2004):  Old function was redundently represented by two separate functions
   // (call them both from this older function to remove the redundency)
      printSpecifier1 (decl_stmt,info);
      printSpecifier2 (decl_stmt,info);
#else
  // only valid in class
     if (info.CheckAccess())
        {
          int i = 0;
          if (info.isPrivateAccess())
             {
               if (!decl_stmt->isPrivate())
                    i = 1;
             }
            else
             {
               if (info.isProtectedAccess())
                  {
                    if (!decl_stmt->isProtected())
                         i = 1;
                  }
                 else
                  {
                    if (info.isPublicAccess())
                       {
                         if (!decl_stmt->isPublic())
                              i = 1;
                       }
                      else
                         i = 1; 
                  }
             }
    
          info.set_isUnsetAccess();

          if (decl_stmt->isPrivate())
             {
               info.set_isPrivateAccess();
               if (i)
                  {
                    cur << "private: ";
                  }
             }
            else
               if (decl_stmt->isProtected())
                  {
                    info.set_isProtectedAccess();
                    if(i) {   cur << "protected: ";  }
                  }
                 else
                  {
                 /* default, always print Public */
                    info.set_isPublicAccess();
                    if (i) {  cur << "public: "; }
                  }
        }
    
     if (decl_stmt->isExtern() && decl_stmt->get_linkage())
        {
          cur << "extern \"" << decl_stmt->get_linkage() << "\" ";
          if (decl_stmt->isExternBrace()) {  cur << "{ ";  }
        }

  // [DT] ----------------------------------------------
  // if(decl_stmt->isSpecialization() ) if( do_unparse )
  //    {
  //      cur << "template<> ";
  //    }
  // ------------------------------------------------

     if (decl_stmt->isVirtual()) {  cur << "virtual ";  }
     if (decl_stmt->isFriend())  {  cur << "friend ";  }
  
  // if (opt.get_inline_opt()) 
     if (!info.SkipFunctionDefinition())
          if (decl_stmt->isInline())
             {
               cur << "inline ";
             }

  // DQ (4/25/2004): Removed CC++ supported modifiers
  // if (decl_stmt->isAtomic() && !info.SkipAtomic() ) {  cur << "atomic ";  }
  // if (decl_stmt->isGlobalClass() && !info.SkipGlobal() ) {  cur << "global ";  }

     if (decl_stmt->isStatic()) {  cur << "static ";  }

  // if (opt.get_extern_opt()) 
     if (decl_stmt->isExtern() && !decl_stmt->get_linkage()) {  cur << "extern ";  }

     if (opt.get_auto_opt())    //checks option status before printing auto
          if (decl_stmt->isAuto())
             {
               cur << "auto ";
             }

     if (decl_stmt->isRegister())
        {
          cur << "register ";
        }
#endif
   }


void
Unparser::printFunctionFormalArgumentSpecifier ( SgType* type, SgUnparse_Info& info ) 
   {
  // if ( decl_stmt->isSpecialization() && do_unparse )
  //    {
  //      cur << "template<> ";
  //    }

     SgModifierNodes* modifiers = type->get_modifiers();
     ROSE_ASSERT (modifiers != NULL);

#if 0
     if (decl_stmt->isVirtual()) {  cur << "virtual ";  }
     if (decl_stmt->isFriend()) {  cur << "friend ";  }
  
     if (!info.SkipFunctionDefinition())
          if (decl_stmt->isInline())
             {
               cur << "inline ";
             }
  
     if (decl_stmt->isAtomic() && !info.SkipAtomic() ) {  cur << "atomic ";  }
     if (decl_stmt->isStatic()) {  cur << "static ";  }
  
  // if (opt.get_extern_opt()) 
     if (decl_stmt->isExtern() && !decl_stmt->get_linkage()) {  cur << "extern ";  }
  
     if (decl_stmt->isGlobalClass() && !info.SkipGlobal() ) {  cur << "global ";  }
  
     if (opt.get_auto_opt())    //checks option status before printing auto
          if (decl_stmt->isAuto())
             {
               cur << "auto ";
             }

     if (decl_stmt->isRegister()) {  cur << "register ";  }
#endif
   }

