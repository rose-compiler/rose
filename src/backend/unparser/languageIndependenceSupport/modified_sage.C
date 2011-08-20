/* modified_sage.C
 *
 * This C file includes functions that test for operator overloaded functions and
 * helper unparse functions such as unparse_helper and printSpecifier.
 */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "unparser.h"
#include "modified_sage.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isOperator
//
//  General function to test if this expression is an unary or binary operator
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isOperator(SgExpression* expr)
   {
     ROSE_ASSERT(expr != NULL);

     if (isBinaryOperator(expr) || isUnaryOperator(expr))
          return true;
     return false;
   }

// DQ (8/13/2007): Added by Thomas to refactor unparser.
void Unparse_MOD_SAGE::curprint(std::string str) {
  unp->cur << str ;
}

// DQ (8/13/2007): Added by Thomas to refactor unparser.
void Unparse_MOD_SAGE::curprint_newline() {
  unp->cur.insert_newline();
}

// DQ (8/13/2007): Added by Thomas to refactor unparser.
int Unparse_MOD_SAGE::cur_get_linewrap () {
  return unp->cur.get_linewrap();
}

// DQ (8/13/2007): Added by Thomas to refactor unparser.
void Unparse_MOD_SAGE::cur_set_linewrap (int nr) {
  unp->cur.set_linewrap(-1);
}

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isBinaryEqualsOperator
//
//  Auxiliary function to test if this expression is a binary operator= overloading
//  function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isBinaryEqualsOperator(SgExpression* expr)
   {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

  if (!func_ref && !mfunc_ref) return false;

  string func_name;
  if (func_ref != NULL)
       func_name = func_ref->get_symbol()->get_name().str();
    else
       func_name = mfunc_ref->get_symbol()->get_name().str();

  if (func_name == "operator=")
    return true;
  return false;
}

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isBinaryEqualityOperator
//
//  Auxiliary function to test if this expression is a binary operator==
//  overloading function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isBinaryEqualityOperator(SgExpression* expr)
   {
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

     if (!func_ref && !mfunc_ref) return false;

     string func_name;
     if (func_ref != NULL)
          func_name = func_ref->get_symbol()->get_name().str();
       else
          func_name = mfunc_ref->get_symbol()->get_name().str();

     if (func_name == "operator==")
          return true;

     return false;
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isBinaryInequalityOperator
//
//  Auxiliary function to test if this expression is a binary operator==
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isBinaryInequalityOperator(SgExpression* expr)
   {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

  if (!func_ref && !mfunc_ref) return false;

  string func_name;
  if(func_ref != NULL)
    func_name = func_ref->get_symbol()->get_name().str();
  else
    func_name = mfunc_ref->get_symbol()->get_name().str();

  if( func_name == "operator<=" ||
      func_name == "operator>=" ||
      func_name == "operator<" ||
      func_name == "operator>" ||
      func_name == "operator!="  )
    return true;

  return false;

}

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isBinaryArithmeticOperator
//
//  Auxiliary function to test if this expression is a binary operator==
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isBinaryArithmeticOperator(SgExpression* expr)
   {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

  if (!func_ref && !mfunc_ref) return false;

  string func_name;
  if(func_ref != NULL)
    func_name = func_ref->get_symbol()->get_name().str();
  else
    func_name = mfunc_ref->get_symbol()->get_name().str();

  if( func_name == "operator+"  ||
      func_name == "operator-"  ||
      func_name == "operator*" ||
      func_name == "operator/" ||
      func_name == "operator+=" ||
      func_name == "operator-=" ||
      func_name == "operator*="||
      func_name == "operator/="  )
    return true;

  return false;

}

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isBinaryParenOperator
//
//  Auxiliary function to test if this expression is a binary operator() overloading
//  function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isBinaryParenOperator(SgExpression* expr)
   {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

  if (!func_ref && !mfunc_ref) return false;

  string func_name;
  if (func_ref != NULL)
       func_name = func_ref->get_symbol()->get_name().str();
    else
       func_name = mfunc_ref->get_symbol()->get_name().str();

  if (func_name == "operator()")
    return true;
  return false;
}

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isBinaryBracketOperator
//
//  Auxiliary function to test if this expression is a binary operator[] overloading
//  function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isBinaryBracketOperator(SgExpression* expr)
   {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

  if (!func_ref && !mfunc_ref) return false;

  string func_name;
  if (func_ref != NULL)
       func_name = func_ref->get_symbol()->get_name().str();
    else
       func_name = mfunc_ref->get_symbol()->get_name().str();

  if (func_name == "operator[]")
    return true;
  return false;

}


//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isBinaryOperator
//
//  Function to test if this expression is a binary operator overloading function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isBinaryOperator(SgExpression* expr)
   {
     bool isBinaryOperatorResult = false;

     SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

     if (!func_ref && !mfunc_ref)
          return false;

     string func_name;
     if (func_ref != NULL)
        {
          ROSE_ASSERT(func_ref->get_symbol() != NULL);
          func_name = func_ref->get_symbol()->get_name().str();
        }
       else
        {
          ROSE_ASSERT(mfunc_ref != NULL);
          ROSE_ASSERT(mfunc_ref->get_symbol() != NULL);
          func_name = mfunc_ref->get_symbol()->get_name().str();
        }

     if (func_name == "operator+" ||
         func_name == "operator-" ||
         func_name == "operator*" ||
         func_name == "operator/" ||
         func_name == "operator%" ||
         func_name == "operator^" ||
         func_name == "operator&" ||
         func_name == "operator|" ||
         func_name == "operator=" ||
         func_name == "operator<" ||
         func_name == "operator>" ||
         func_name == "operator+=" ||
         func_name == "operator-=" ||
         func_name == "operator*=" ||
         func_name == "operator/=" ||
         func_name == "operator%=" ||
         func_name == "operator^=" ||
         func_name == "operator&=" ||
         func_name == "operator|=" ||
         func_name == "operator<<" ||
         func_name == "operator>>" ||
         func_name == "operator>>=" ||
         func_name == "operator<<=" ||
         func_name == "operator==" ||
         func_name == "operator!=" ||
         func_name == "operator<=" ||
         func_name == "operator>=" ||
         func_name == "operator&&" ||
         func_name == "operator||" ||
         func_name == "operator," ||
         func_name == "operator->*" ||
         func_name == "operator->" ||
         func_name == "operator()" ||
         func_name == "operator[]")
        {
       // DQ (5/6/2007): Make sure this could not be a unary operator (using new fix for unary operators).
          if (isUnaryOperatorPlus(expr) || isUnaryOperatorMinus(expr))
               isBinaryOperatorResult = false;
            else
               isBinaryOperatorResult = true;
        }

     return isBinaryOperatorResult;
   }



//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryIncrementOperator
//
//  Auxiliary function to test if this expression is an unary prefix/postfix operator++ overloading function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryIncrementOperator(SgExpression* expr)
   {
  // DQ (5/6/2007): This might be a non-member function and if so we don't handle this case correctly!
  // If it is a non-member function this it will have a single argument
     ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);

     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     if (mfunc_ref != NULL)
        {
          SgMemberFunctionSymbol* mfunc_sym = mfunc_ref->get_symbol();
          if (mfunc_sym != NULL)
             {
               SgMemberFunctionDeclaration* mfunc_decl = mfunc_sym->get_declaration();
               if (mfunc_decl != NULL)
                  {
                    SgName func_name = mfunc_decl->get_name();
                    if (func_name.getString() == "operator++")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 1)
                              return true;
                       }
                  }
             }
        }

     return false;
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryDecrementOperator
//
//  Auxiliary function to test if this expression is an unary prefix/postfix operator-- overloading function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryDecrementOperator(SgExpression* expr)
   {
  // DQ (5/6/2007): This might be a non-member function and if so we don't handle this case correctly!
  // If it is a non-member function this it will have a single argument
     ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);

     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     if (mfunc_ref != NULL)
        {
          SgMemberFunctionSymbol* mfunc_sym = mfunc_ref->get_symbol();
          if (mfunc_sym != NULL)
             {
               SgMemberFunctionDeclaration* mfunc_decl = mfunc_sym->get_declaration();
               if (mfunc_decl != NULL)
                  {
                    SgName func_name = mfunc_decl->get_name();
                    if (func_name.getString() == "operator--")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 1)
                              return true;
                       }
                  }
             }
        }

     return false;
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryOperator
//
//  Function to test if this expression is an unary operator overloading function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryOperator(SgExpression* expr)
   {
  // DQ (5/6/2007): This might be a non-member function and if so we don't handle this case correctly!
  // If it is a non-member function this it will have a single argument
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);
     if (isSgFunctionRefExp(expr) != NULL && isSgMemberFunctionRefExp(expr) == NULL)
        {
#if PRINT_DEVELOPER_WARNINGS
       // printf ("Case not implemented in Unparse_MOD_SAGE::isUnaryOperator(): expr = %p = %s = %s \n",expr,expr->class_name().c_str(),SageInterface::get_name(expr).c_str());
          printf ("Case not implemented in Unparse_MOD_SAGE::isUnaryOperator(): expr = %p = %s = %s \n",expr,expr->class_name().c_str(),isSgFunctionRefExp(expr)->get_symbol()->get_name().str());
#endif
        }
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);

     SgFunctionRefExp* func_ref        = isSgFunctionRefExp(expr);
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

  // Ignore things that are not functions
  // if (!func_ref && !mfunc_ref)
     if (func_ref == NULL && mfunc_ref == NULL)
        {
       // DQ (8/27/2007): This really is called for many non-functions reference expressions.
       // printf ("Warning, Unparse_MOD_SAGE::isUnaryOperator() test on non-function \n");
          return false;
        }

     string func_name;
     if (func_ref != NULL)
          func_name = func_ref->get_symbol()->get_name().str();
       else
          func_name = mfunc_ref->get_symbol()->get_name().str();

  // DQ (2/14/2005): Need special test for operator*(), and other unary operators,
  // similar to operator+() and operator-().
  // Error need to check number of parameters in arg list of operator* to verify it
  // is a unary operators else it could be a binary multiplication operator
  // Maybe also for the binary operator&() (what about operator~()?)
  // Added support for isUnaryDereferenceOperator(), isUnaryAddressOperator(),
  // isUnaryOrOperator(), isUnaryComplementOperator().
     if ( isUnaryOperatorPlus(mfunc_ref) ||
          isUnaryOperatorMinus(mfunc_ref) ||
          func_name == "operator!" ||
       // func_name == "operator*" ||
          isUnaryDereferenceOperator(mfunc_ref) ||
       // DQ (11/24/2004): Added support for address operator "operator&"
       // func_name == "operator&" ||
          isUnaryAddressOperator(mfunc_ref) ||
          func_name == "operator--" ||
          func_name == "operator++" ||
          isUnaryOrOperator(mfunc_ref) ||
       // func_name == "operator~")
          isUnaryComplementOperator(mfunc_ref) )
          return true;

     return false;
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryPostfixOperator
//
//  Auxiliary function to test if this expression is an unary postfix operator
//  overloading function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isUnaryPostfixOperator(SgExpression* expr)
   {
  // DQ (5/6/2007): This might be a non-member function and if so we don't handle this case correctly!
     ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);

     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     if (mfunc_ref != NULL)
        {
          SgMemberFunctionSymbol* mfunc_sym = mfunc_ref->get_symbol();
          if (mfunc_sym != NULL)
             {
               SgMemberFunctionDeclaration* mfunc_decl = mfunc_sym->get_declaration();
               if (mfunc_decl != NULL)
                  {
                    SgName func_name = mfunc_decl->get_name();
                    if (func_name.getString() == "operator++" || func_name.getString() == "operator--")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                           // postfix operators have one argument (0), prefix operators have none ()
                         if (argList.size() == 1)
                            return true;
                       }
                  }
             }
        }

     return false;
   }

#if 0
// DQ (2/20/2005): The need for this has been eliminated with the more uniform
// handling of binary operator unparsing.
//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::RemoveArgs
//
//  This function determines whether to remove the argument list of overloaded
//  operators. It returns true if the function is an unary operator. Otherwise, it
//  returns false for all other functions.
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::RemoveArgs(SgExpression* expr)
   {
     SgBinaryOp* binary_op = isSgBinaryOp(expr);
  // if (!binary_op)
     if (binary_op == NULL)
        {
          curprint( "/* In RemoveArgs: this is a binary operator  */ \n");
          return false;
        }
#if 0
  // all overloaded unary operators must be the rhs of a binary expression.
     if ( binary_op->get_rhs_operand() != NULL &&
         ( isUnaryOperator( binary_op->get_rhs_operand() ) ||
           isOverloadedArrowOperator( binary_op->get_rhs_operand() )))
        {
          curprint( "/* In RemoveArgs: rhs is a unary operator or is isOverloadedArrowOperator(rhs) == true  */ \n");
          return true;
        }
#else
     if ( (binary_op->get_rhs_operand() != NULL) && isUnaryOperator( binary_op->get_rhs_operand() ) )
        {
          curprint( "/* In RemoveArgs: rhs is a unary operator */ \n");
          return true;
        }
#endif

     curprint( "/* In RemoveArgs: rhs is not a unary operator or a OverloadedArrowOperator */ \n");
     return false;
   }
#endif

//QY 6/23/04: added code for precedence of expressions
//-----------------------------------------------------------------------------------
//  void GetOperatorVariant
//
//  Function that returns the expression variant of overloaded operators
//-----------------------------------------------------------------------------------

int GetOperatorVariant(SgExpression* expr) {
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
       if (mfunc->variantT() == V_SgPseudoDestructorRefExp)
            return V_SgFunctionCallExp;
       SgMemberFunctionRefExp* mfunc_ref =
             isSgMemberFunctionRefExp(mfunc);
       assert (mfunc_ref != 0);
       name = mfunc_ref->get_symbol()->get_name();
       break;
   }
   default:
       return V_SgFunctionCallExp;
   }

   string func_name = name.str();
   if (func_name == "operator,") return V_SgCommaOpExp;
    else if (func_name == "operator=") return V_SgAssignOp;
    else if (func_name == "operator+=") return V_SgPlusAssignOp;
    else if (func_name == "operator-=") return V_SgMinusAssignOp;
    else if (func_name == "operator&=") return V_SgAndAssignOp;
    else if (func_name == "operator|=") return V_SgIorAssignOp;
    else if (func_name == "operator*=") return V_SgMultAssignOp;
    else if (func_name == "operator/=") return V_SgDivAssignOp;
    else if (func_name == "operator%=") return V_SgModAssignOp;
    else if (func_name == "operator^=") return V_SgXorAssignOp;
    else if (func_name == "operator<<=") return  V_SgLshiftAssignOp;
    else if (func_name == "operator>>=") return V_SgRshiftAssignOp;
    else if (func_name == "operator||") return  V_SgOrOp;
    else if (func_name == "operator&&") return V_SgAndOp;
    else if (func_name == "operator|") return V_SgBitOrOp;
    else if (func_name == "operator^") return  V_SgBitXorOp;
    else if (func_name == "operator&") return V_SgBitAndOp;
    else if (func_name == "operator==") return V_SgEqualityOp;
    else if (func_name == "operator!=") return V_SgNotEqualOp;
    else if (func_name == "operator<") return V_SgLessThanOp;
    else if (func_name == "operator>") return V_SgGreaterThanOp;
    else if (func_name == "operator<=") return V_SgLessOrEqualOp;
    else if (func_name == "operator>=") return V_SgGreaterOrEqualOp;
    else if (func_name == "operator<<") return V_SgLshiftOp;
    else if (func_name == "operator>>") return V_SgRshiftOp;
    else if (func_name == "operator+") return V_SgAddOp;
    else if (func_name == "operator-") return V_SgSubtractOp;
    else if (func_name == "operator*") return V_SgMultiplyOp;
    else if (func_name == "operator/") return V_SgDivideOp;
    else if (func_name == "operator%") return V_SgModOp;
    else if (func_name == "operator.*") return V_SgDotStarOp;
    else if (func_name == "operator->*") return V_SgArrowStarOp;
    else if (func_name == "operator++") return V_SgPlusPlusOp;
    else if (func_name == "operator--") return V_SgMinusMinusOp;
    else if (func_name == "operator~") return V_SgBitComplementOp;
    else if (func_name == "operator!") return V_SgNotOp;
    else if (func_name == "operator[]") return V_SgPntrArrRefExp;
    else if (func_name == "operator->") return V_SgArrowExp;
    else if (func_name == "operator.") return V_SgDotExp;
    else if (func_name.find("operator") == string::npos ||
             func_name == "operator()") return V_SgFunctionCallExp;
    else if (func_name.find("operator") != string::npos) return V_SgCastExp;
    else
     {
       printf ("Error: default case reached in GetOperatorVariant func_name = %s \n",func_name.c_str());
       assert(false);
    /* avoid MSCV warning by adding return stmt */
       return -1;
     }
}

SgExpression* GetFirstOperand(SgExpression* expr) {
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

  return NULL;
}

//-----------------------------------------------------------------------------------
//  int GetPrecedence
//
//  returns the precedence (1-17) of the expression variants,
//  such that 17 has the highest precedence and 1 has the lowest precedence.
//-----------------------------------------------------------------------------------
int GetPrecedence(int variant)
   {
       ROSE_ASSERT(!"Deprecated. Use UnparseLanguageIndependentConstructs::getPrecedence instead");
   }

//-----------------------------------------------------------------------------------
//  GetAssociativity
//
//  Function that returns the associativity of the expression variants,
//  -1: left associative; 1: right associative; 0 : not associative/unknown
//-----------------------------------------------------------------------------------
int GetAssociativity(int variant)
   {
       ROSE_ASSERT(!"Deprecated. Use UnparseLanguageIndependentConstructs::getAssociativity instead");
   }



//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::PrintStartParen
//
//  Auxiliary function that determines whether "(" should been printed for a binary
//  expression. This function is needed whenever the rhs of the binary expression
//  is an operator= overloaded function.
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::PrintStartParen(SgExpression* expr, SgUnparse_Info& info)
   {
       ROSE_ASSERT(!"deprecated. use UnparseLanguageIndependentConstructs::requiresParentheses instead");
   }

//-----------------------------------------------------------------------------------
//  bool Unparse_MOD_SAGE::RemovePareninExprList
//
//  Auxiliary function to determine whether parenthesis is needed around
//  this expression list. If the list only contains one element and the
//  element is a binary operator whose rhs is an operator(), then parens
//  are removed (return true). Otherwise, return false.
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::RemovePareninExprList(SgExprListExp* expr_list) {
  ROSE_ASSERT(expr_list != NULL);
  SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();

  if (i != expr_list->get_expressions().end()) {
    SgFunctionCallExp* func_call = isSgFunctionCallExp(*i);
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
      return true;
  }
      }
    }
  }
  return false;
}

//-----------------------------------------------------------------------------------
//  bool Unparse_MOD_SAGE::isOneElementList
//
//  Auxiliary function to determine if the argument list of the Constructor
//  Initializer contains only one element.
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isOneElementList(SgConstructorInitializer* con_init)
   {
     ROSE_ASSERT(con_init != NULL);
     if (con_init->get_args())
        {
          SgExprListExp* expr_list = isSgExprListExp(con_init->get_args());
          if (expr_list != NULL)
             {
#if 0
               SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();
               if (i != expr_list->get_expressions().end())
                  {
                    i++;
                    if (i == expr_list->get_expressions().end())
                         return true;
                  }
#else
            // DQ (4/1/2005): This is a simpler implementation
            // printf ("In Unparse_MOD_SAGE::isOneElementList: expr_list->get_expressions().size() = %zu \n",expr_list->get_expressions().size());
               if (expr_list->get_expressions().size() == 1)
                    return true;
#endif
             }
            else
             {
                printf ("In Unparse_MOD_SAGE::isOneElementList: expr_list == NULL \n");
             }
        }
       else
        {
          printf ("In Unparse_MOD_SAGE::isOneElementList: con_init->get_args() == NULL \n");
        }

     return false;
   }


//-----------------------------------------------------------------------------------
//  bool Unparse_MOD_SAGE::printConstructorName
//
//  This function determines whether to print the name of the constructor or not.
//  The constructor name is printed if the operator overloaded function in the
//  constructor returns a reference or pointer to the object. If a value is
//  returned, then the printing of the constructor is suppressed.
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::printConstructorName(SgExpression* expr)
   {
     SgConstructorInitializer* con_init = isSgConstructorInitializer(expr);

     if (con_init != NULL)
        {
       // DQ (11/13/2004): Added support to ignore non-explicit constructor initializers
       // curprint( "\n /* In printConstructorName: con_init != NULL */ \n");
#if 0
       // DQ (12/8/2004): This is overly restrictive for the return statement
       // within compilation of polygonaldiffusionsolverswig within Kull.
       // I suspect that there are many places where the EDG's meaning of explicit cast
       // is more conservative than the one which I want.  However, I can imagine that
       // this is a relatively complex issue.
          if (con_init->get_is_explicit_cast() == true)
             {
               curprint( "\n /* In printConstructorName: con_init->get_is_explicit_cast() == true */ \n");
#endif
               SgExprListExp* expr_list = isSgExprListExp(con_init->get_args());
            // if (expr_list != NULL)
               if (con_init->get_need_parenthesis_after_name() == false)
                  {
                 // DQ (12/8/2004): If there is more than one argument then we have to return "true" since
                 // "return A(1,2);" can be be replace with "return (1,2);" where as "return A(1);" can be
                 // replaced by "return 1;" if all other rules are satisfied. Wrong!  If the argument list is
                 // from a function call then there could be an arbitrary number of arguments which could
                 // have default values.  E.g. "return A(foo(1,2));" could be replaces with "return foo(1);"
                 // if the function foo was defined "A foo(int i, int j = 2);"
                    SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();
                    if (i != expr_list->get_expressions().end())
                       {
                      // now iterate through all the arguments
                         while (true)
                            {
                           // check if the argument is a constructor initialiaizer. If so, we
                           // recurprintsively call the printConstructorName function.
                              if ((*i)->variant() == CONSTRUCTOR_INIT)
                                   return printConstructorName(*i);

                           // check if the argument is a function call
                              if ((*i)->variant() == FUNC_CALL)
                                 {
                                   SgFunctionCallExp* func_call = isSgFunctionCallExp(*i);
                                // now check if the function is an overloaded operator function
                                   if (isOperator(func_call->get_function()))
                                      {
                                     // if so, get the return type
                                        SgFunctionRefExp* func_ref = isSgFunctionRefExp(func_call->get_function());
                                        SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(func_call->get_function());
                                        SgType* return_type;
                                        if (func_ref)
                                             return_type = func_ref->get_type();
                                          else
                                             if (mfunc_ref)
                                                  return_type = mfunc_ref->get_type();
                                          // return type not defined, so default to printing the constructor name
                                               else
                                                  return true;
                                     // if the return type is a reference or pointer type, then we print the
                                     // constructor name. Otherwise, the return type is by value and we suppress
                                     // the printing of the constructor name.
                                        if (return_type->variant() == T_REFERENCE || return_type->variant() == T_POINTER)
                                             return true;
                                        return false;
                                      }
                                 }

                           // continue iterating if we didn't find a constructor initializer
                           // or function call
                              i++;
                              if (i != expr_list->get_expressions().end());
                                else
                                 break;
                            }
                       }
                  }
#if 0
             }
            else
             {
            // con_init->get_is_explicit_cast() == false, so skip the output of the constructor initializer name
               curprint( "\n /* In printConstructorName: con_init->get_is_explicit_cast() == false */ \n");
               return false;
             }
#endif
        }

     return true;
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isOverloadedArrowOperator
//
//  Auxiliary function to test if this expression is a binary operator-> or
//  operator->* overloading function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isOverloadedArrowOperator(SgExpression* expr)
   {
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);

     if (!func_ref && !mfunc_ref) return false;

     string func_name;
     if (func_ref != NULL)
        {
          func_name = func_ref->get_symbol()->get_name().str();
        }
       else
        {
       // [DT] 4/4/2000 -- Adding checks before trying to access
       //      mfunc_ref->get_symbol()->get_name().str().
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
          if ( ( mfunc_ref->get_symbol()->get_name().getString() == "operator->" ||
                mfunc_ref->get_symbol()->get_name().getString() == "operator->*"   ) )
               return true;
            else
               return false;
        }

  // [DT] 4/4/2000 -- Added check for func_name!=NULL.
     if ( func_name == "operator->" || func_name == "operator->*" )
          return true;
     return false;
   }

bool
Unparse_MOD_SAGE::isUnaryOperatorArrowSubtree(SgExpression* expr)
   {
  // DQ (12/11/2004): This function recognizes a subtree which represents the "S* T::operator->()" for some class type "T" and type "S"
     bool returnValue = false;
     SgPointerDerefExp *returnType = isSgPointerDerefExp(expr);
     if (returnType != NULL)
        {
          SgFunctionCallExp* functionCall = isSgFunctionCallExp(returnType->get_operand());

       // Note recurprintsive function call!
          returnValue = isUnaryOperatorArrowSubtree(functionCall);
        }
       else
        {
          SgFunctionCallExp* functionCall = isSgFunctionCallExp(expr);
          if (functionCall != NULL)
             {
               SgBinaryOp *binaryOperator = isSgBinaryOp(functionCall->get_function());
               if (binaryOperator != NULL)
                  {
                 // Verify that this is either a SgDotExp or a SgArrowExp
                    SgDotExp   *dotExpression   = isSgDotExp  (binaryOperator);
                    SgArrowExp *arrowExpression = isSgArrowExp(binaryOperator);
                    ROSE_ASSERT(dotExpression != NULL || arrowExpression != NULL);

                    if (isOverloadedArrowOperator(binaryOperator->get_rhs_operand()) == true)
                       {
                         returnValue = true;
                       }
                  }
             }
        }

     return returnValue;
   }

//-----------------------------------------------------------------------------------
//  bool Unparse_MOD_SAGE::NoDereference
//
//  Auxiliary function used by unparseUnaryExpr and unparseBinaryExpr to
//  determine whether the dereference operand contains an overloaded arrow operator.
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::NoDereference(SgExpression* expr)
   {
     bool arrow = false;

  // now we first check of this is an dereference operator. If so, we need to
  // determine if the operand contains an overloaded arrow operator. If this
  // is true, set arrow = true so the dereference operator will be suppressed.
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
//  bool Unparse_MOD_SAGE::isIOStreamOperator
//
//  checks if this expression is an iostream overloaded function
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isIOStreamOperator(SgExpression* expr) {
  SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
  SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
  if (!func_ref && !mfunc_ref) return false;

// This results in a FMR in purify since the casting operator for
// SgName (operator char*) causes a SgName object to be built and
// then the char* pointer is taken from that temporary char* object
// (and then the temporary is deleted (goes out of scope) leaving
// the char* pointing to the string in the deleted object).
  string func_name;
  if (func_ref)
       func_name = func_ref->get_symbol()->get_name().str();
    else
       func_name = mfunc_ref->get_symbol()->get_name().str();

  //check if the function name is "operator<<" or "operator>>"
  if (func_name == "operator<<" || func_name == "operator>>")
    return true;

  return false;
}

//-----------------------------------------------------------------------------------
//  bool Unparse_MOD_SAGE::isCast_ConstCharStar
//
//  auxiliary function to determine if this cast expression is "const char*"
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::isCast_ConstCharStar(SgType* type)
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
//  bool Unparse_MOD_SAGE::noQualifiedName
//
//  auxiliary function to determine if "::" is printed out
//-----------------------------------------------------------------------------------
bool Unparse_MOD_SAGE::noQualifiedName(SgExpression* expr)
   {
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     if (mfunc_ref != NULL)
        {
       // DQ (12/11/2004): I'm not so sure that the need_qualifier data member is always set properly in Sage III from EDG, or even in EDG!
          if (mfunc_ref->get_need_qualifier())
             {
            // check if this is a iostream operator function and the value of the overload opt is false
               if (!unp->opt.get_overload_opt() && isIOStreamOperator(mfunc_ref));
                 else
                    return false;
             }
        }
     return true;
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::output
//
//  prints out the line number and file information of the node
//-----------------------------------------------------------------------------------
/*
void Unparse_MOD_SAGE::output(SgLocatedNode* node) {
  Sg_File_Info* node_file = node->get_file_info();
  int curprint_line = node_file->getCurprintrentLine();
  char* curprint_file = node_file->getCurprintrentFilename();
  char* p_filename = node_file->get_filename();
  int   p_line = node_file->get_line();
  int   p_col = node_file->get_col();

  int newFile = !curprint_file || !p_filename || (::strcmp(p_filename, curprint_file) != 0);
  int newLine = p_line != curprint_line;

  if (!newFile && newLine && (p_line > curprint_line) && ((p_line - curprint_line) < 4)) {
    curprint_line = p_line;
  } else if (p_line && (newFile || newLine)) {
    curprint( "#" << p_line;
    curprint_line = p_line;
    if (newFile && p_filename) {
       curprint( " \"" << p_filename << "\"");
      curprint_file = p_filename;
    }
  }

}
*/
//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::directives
//
//  checks the linefile option to determine whether to print line and file info
//  prints out pragmas
//-----------------------------------------------------------------------------------
void
Unparse_MOD_SAGE::directives(SgLocatedNode* lnode)
   {
  // checks option status before printing line and file info
     if (unp->opt.get_linefile_opt())
        {
/*
          if (lnode->get_file_info())
             {
               output(lnode);
             }
*/
        }

     printf ("Unparse_MOD_SAGE::directives(): commented out call to output_pragma() \n");
  // DQ (8/20/2004): Removed this function (old implementation of pragmas)
  // stringstream out;
  // lnode->output_pragma(out);
  // curprint( out.str();
   }


//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::printSpecifier1
//  void Unparse_MOD_SAGE::printSpecifier2
//  void Unparse_MOD_SAGE::printSpecifier
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
Unparse_MOD_SAGE::printSpecifier1 ( SgDeclarationStatement * decl_stmt, SgUnparse_Info & info )
   {
  // only valid in class

#if 0
     printf ("In Unparse_MOD_SAGE::printSpecifier1: decl_stmt = %p = %s info.CheckAccess() = %s \n",decl_stmt,decl_stmt->class_name().c_str(),info.CheckAccess() ? "true" : "false");
     printf ("info.isPrivateAccess()   = %s \n",info.isPrivateAccess()   ? "true" : "false");
     printf ("info.isProtectedAccess() = %s \n",info.isProtectedAccess() ? "true" : "false");
     printf ("info.isPublicAccess()    = %s \n",info.isPublicAccess()    ? "true" : "false");
  // printf ("info.isDefaultAccess()   = %s \n",info.isDefaultAccess()   ? "true" : "false");
#endif

     if (info.CheckAccess())
        {
          ROSE_ASSERT (decl_stmt != NULL);
          bool flag = false;

          if (info.isPrivateAccess())
             {
            // If the current declaration access setting if different from the one stored in
            // info then set flag to true, so that the access specified will be output.
               if (!decl_stmt->get_declarationModifier().get_accessModifier().isPrivate())
                    flag = true;
             }
            else
             {
               if (info.isProtectedAccess())
                  {
                 // If the current declaration access setting if different from the one stored in
                 // info then set flag to true, so that the access specified will be output.
                    if (!decl_stmt->get_declarationModifier().get_accessModifier().isProtected())
                         flag = true;
                  }
                 else
                  {
                    if (info.isPublicAccess())
                       {
                      // If the current declaration access setting if different from the one stored in
                      // info then set flag to true, so that the access specified will be output.
                         if (!decl_stmt->get_declarationModifier().get_accessModifier().isPublic())
                              flag = true;
                       }
                      else
                       {
                      // Initially for the first data member of a class, the info values for isPrivateAccess,
                      // isProtectedAccess, and isPublicAccess, are not set.  In this case the flag is set
                      // to true (here).  This forces the first access keyword to be output.
                         flag = true;
                       }
                  }
             }

       // Unset the access so it can be reset from an error value below
          info.set_isUnsetAccess();
#if 0
       // Note that a better implementation would take the "flag" out of the 3 nested conditionals below.
          printf ("flag = %s \n",flag ? "true" : "false");
          printf ("decl_stmt->get_declarationModifier().get_accessModifier().isPrivate()   = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isPrivate()   ? "true" : "false");
          printf ("decl_stmt->get_declarationModifier().get_accessModifier().isProtected() = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isProtected() ? "true" : "false");
          printf ("decl_stmt->get_declarationModifier().get_accessModifier().isPublic()    = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isPublic()    ? "true" : "false");
#endif
          if (decl_stmt->get_declarationModifier().get_accessModifier().isPrivate())
             {
               info.set_isPrivateAccess();
               if (flag)
                  {
                    curprint( "private: ");
                 // printf ("Output PRIVATE keyword! \n");
                  }
             }
            else
             {
               if (decl_stmt->get_declarationModifier().get_accessModifier().isProtected())
                  {
                    info.set_isProtectedAccess();
                    if (flag)
                       {
                         curprint( "protected: ");
                      // printf ("Output PROTECTED keyword! \n");
                       }
                  }
                 else
                  {
                 /* default, always print Public */
                    ROSE_ASSERT (decl_stmt->get_declarationModifier().get_accessModifier().isPublic() == true);
                    info.set_isPublicAccess();
                    if (flag)
                       {
                         curprint( "public: ");
                      // printf ("Output PUBLIC keyword! \n");
                       }
                  }
             }
#if 0
          printf ("Was this reset: info.isPrivateAccess()   = %s \n",info.isPrivateAccess()   ? "true" : "false");
          printf ("Was this reset: info.isProtectedAccess() = %s \n",info.isProtectedAccess() ? "true" : "false");
          printf ("Was this reset: info.isPublicAccess()    = %s \n",info.isPublicAccess()    ? "true" : "false");
#endif
       // If must have been set to one of the three values (but not left unset
          ROSE_ASSERT(info.isUnsetAccess() == false);
        }
   }


void
Unparse_MOD_SAGE::outputExternLinkageSpecifier ( SgDeclarationStatement* decl_stmt )
   {
  // DQ (5/10/2007): Fixed linkage to be a std::string instead of char*
  // if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && decl_stmt->get_linkage())
     if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && decl_stmt->get_linkage().empty() == false)
        {
          curprint( "extern \"" + decl_stmt->get_linkage() + "\" ");
          if (decl_stmt->isExternBrace())
             {
               curprint( "{ ");
             }
        }
   }

void
Unparse_MOD_SAGE::outputTemplateSpecializationSpecifier ( SgDeclarationStatement* decl_stmt )
   {
#if 0
     if ( (isSgTemplateInstantiationDecl(decl_stmt) != NULL) ||
          (isSgTemplateInstantiationFunctionDecl(decl_stmt) != NULL) ||
          (isSgTemplateInstantiationMemberFunctionDecl(decl_stmt) != NULL) )
        {
          curprint( "template<> ");
        }
#else
     if ( (isSgTemplateInstantiationDecl(decl_stmt) != NULL) ||
          (isSgTemplateInstantiationFunctionDecl(decl_stmt) != NULL) ||
          (isSgTemplateInstantiationMemberFunctionDecl(decl_stmt) != NULL) )
        {
          if ( isSgTemplateInstantiationDirectiveStatement(decl_stmt->get_parent()) != NULL)
             {
            // Template intanatiation directives use "template" instaed of "template<>"
               curprint( "template ");
             }
            else
             {
            // Normal case for output of template instantiations (which ROSE puts out as specializations)
               curprint( "template<> ");
             }
        }
#endif
   }


void
Unparse_MOD_SAGE::printSpecifier2(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info)
   {

  // DQ (8/29/2005): These specifiers have to be output in a different order for g++ 3.3.x and 3.4.x
#ifdef __GNUC__
   #if (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 3) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER < 4)
     outputTemplateSpecializationSpecifier(decl_stmt);
     outputExternLinkageSpecifier(decl_stmt);
   #endif
// #if (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 3) || ( (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 3) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 4) )
   #if (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 3) || ( (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 3) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 4) )

// DQ (9/12/2009): If this is a g++ version 4.3 or later compiler then skip the extern linkage specifier,
// but only if this is for a template declaration (does it only apply to a function). See tests:
// test2004_30.C, test2004_121.C, test2004_142.C, and test2004_143.C.
   #if ( (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER >= 4) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 3) )
     if (isSgTemplateInstantiationFunctionDecl(decl_stmt) == NULL && isSgTemplateInstantiationMemberFunctionDecl(decl_stmt) == NULL)
          outputExternLinkageSpecifier(decl_stmt);
   #else
     outputExternLinkageSpecifier(decl_stmt);
   #endif

     outputTemplateSpecializationSpecifier(decl_stmt);
   #endif
#else
#ifndef _MSC_VER
#warning "Non-GNU backends not specifically addressed, assuming g++ version 3.4.x compliant ..."
#else
#pragma message ("Non-GNU backends not specifically addressed, assuming g++ version 3.4.x compliant ...")
#endif
#endif

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(decl_stmt);

  // DQ (2/4/2006): Moved output of "friend" keywork inside of test for SgFunctionDeclaration

  // DQ (2/4/2006): Need this case for friend class declarations
     if (decl_stmt->get_declarationModifier().isFriend())
        {
       // This assertion fails in test2004_116.C
       // ROSE_ASSERT(functionDeclaration != NULL);
          if (functionDeclaration == NULL)
           {
             curprint( "friend ");
           }
        }

     if (functionDeclaration != NULL)
        {
       // DQ (2/4/2006): Template specialization declarations (forward declaration) can't have some modified output
          bool isDeclarationOfTemplateSpecialization = false;
          SgDeclarationStatement::template_specialization_enum specializationEnumValue = functionDeclaration->get_specialization();
          isDeclarationOfTemplateSpecialization =
               (specializationEnumValue == SgDeclarationStatement::e_specialization) ||
               (specializationEnumValue == SgDeclarationStatement::e_partial_specialization);

       // DQ (2/2/2006): friend can't be output for a Template specialization declaration
       // curprint( "/* isDeclarationOfTemplateSpecialization = " << (isDeclarationOfTemplateSpecialization ? "true" : "false") << " */ \n ");
          if ( (decl_stmt->get_declarationModifier().isFriend() == true) &&
               (isDeclarationOfTemplateSpecialization == false) )
             {
               curprint( "friend ");
             }

       // DQ (2/2/2006): Not sure if virtual can be output when isForwardDeclarationOfTemplateSpecialization == true
          if (functionDeclaration->get_functionModifier().isVirtual())
             {
               curprint( "virtual ");
             }

       // if (unp->opt.get_inline_opt())

       // DQ (2/2/2006): Not sure if virtual can be output when isForwardDeclarationOfTemplateSpecialization == true
       // DQ (4/28/2004): output "inline" even for function definitions
       // if (!info.SkipFunctionDefinition())
          if (functionDeclaration->get_functionModifier().isInline())
             {
               curprint( "inline ");
             }

       // DQ (2/2/2006): friend can't be output for a Template specialization declaration
       // if ((!info.SkipFunctionDefinition()) && functionDeclaration->get_functionModifier().isExplicit())
          if ( (info.SkipFunctionDefinition() == false) &&
               (functionDeclaration->get_functionModifier().isExplicit() == true) &&
               (isDeclarationOfTemplateSpecialization == false) )
             {
               curprint( "explicit ");
             }

       // TV (04/26/2010): CUDA function modifiers
          if (functionDeclaration->get_functionModifier().isCudaKernel())
             {
               curprint( "__global__ ");
             }
          if (functionDeclaration->get_functionModifier().isCudaDevice())
             {
               curprint( "__device__ ");
             }
          if (functionDeclaration->get_functionModifier().isCudaHost())
             {
               curprint( "__host__ ");
             }

       // TV (05/06/2010): OpenCL function modifiers
          if (functionDeclaration->get_functionModifier().isOpenclKernel())
             {
               curprint( "__kernel ");
             }
          if (functionDeclaration->get_functionModifier().hasOpenclVecTypeHint())
             {
            // SgType * opencl_vec_type = functionDeclaration->get_functionModifier().get_opencl_vec_type();
            // curprint( "__attribute__((vec_type_hint(" << get_type_name(opencl_vec_type) << "))) ");
            // curprint( "__attribute__((vec_type_hint(" << unp->u_type->unparseType(opencl_vec_type) << "))) ");
               curprint( "__attribute__((vec_type_hint(type))) ");
             }
          if (functionDeclaration->get_functionModifier().hasOpenclWorkGroupSizeHint())
             {
               SgFunctionModifier::opencl_work_group_size_t opencl_work_group_size = functionDeclaration->get_functionModifier().get_opencl_work_group_size();
            // curprint( "__attribute__((work_group_size_hint(" << opencl_work_group_size.x << ", " << opencl_work_group_size.y << ", " << opencl_work_group_size.z << "))) ");
               curprint( "__attribute__((work_group_size_hint(X, Y, Z))) ");
             }
          if (functionDeclaration->get_functionModifier().hasOpenclWorkGroupSizeReq())
             {
               SgFunctionModifier::opencl_work_group_size_t opencl_work_group_size = functionDeclaration->get_functionModifier().get_opencl_work_group_size();
            // curprint( "__attribute__((work_group_size_hint(" << opencl_work_group_size.x << ", " << opencl_work_group_size.y << ", " << opencl_work_group_size.z << "))) ");
               curprint( "__attribute__((work_group_size_hint(X, Y, Z))) ");
             }
        }

  // DQ (4/25/2004): Removed CC++ specific modifiers
  // if (decl_stmt->isAtomic() && !info.SkipAtomic() ) { curprint( "atomic "); }
  // if (decl_stmt->isGlobalClass() && !info.SkipGlobal() ) { curprint( "global "); }

  // DQ (7/202/2006): The isStatic() function in the SgStorageModifier held by the SgInitializedName object
  // should always be false, and this one held by the SgDeclaration is what is used.
  // printf ("In decl_stmt = %p = %s test the return value of storage.isStatic() = %d = %d (should be boolean value) \n",
  //      decl_stmt,decl_stmt->class_name().c_str(),decl_stmt->get_declarationModifier().get_storageModifier().isStatic(),
  //      decl_stmt->get_declarationModifier().get_storageModifier().get_modifier());

  // This was a bug mistakenly reported by Isaac
     ROSE_ASSERT(decl_stmt->get_declarationModifier().get_storageModifier().get_modifier() >= 0);

     if (decl_stmt->get_declarationModifier().get_storageModifier().isStatic())
        {
          curprint( "static ");
        }

  // if (unp->opt.get_extern_opt())
  // DQ (5/10/2007): Fixed linkage to be a std::string instead of char*
  // if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && !decl_stmt->get_linkage())
     if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && decl_stmt->get_linkage().empty() == true)
        {
       // printf ("In Unparse_MOD_SAGE::printSpecifier2(): Output the extern keyword \n");
          curprint( "extern ");
        }

  // DQ (12/1/2007): Added support for gnu extension "__thread" (will be available in EDG version > 3.3)
  // But added to support use by Gouchun Shi (UIUC).  Code generation support also added in unparser.
  // This only works on gnu backends and those compatable with gnu (which is a lot of compilers so skip
  // special code to be conditional on the backend).  According to documentation, "__thread" should
  // appear immediately after "extern" or "static" and can not be combined with other storage modifiers.
     if (decl_stmt->get_declarationModifier().get_storageModifier().get_thread_local_storage() == true)
        {
          curprint( "__thread ");
        }

     if (unp->opt.get_auto_opt())    // checks option status before printing auto (to prevent redundant use)
        {
          if (decl_stmt->get_declarationModifier().get_storageModifier().isAuto())
             {
               curprint( "auto ");
             }
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isRegister())
        {
          curprint( "register ");
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isMutable())
        {
          curprint( "mutable ");
        }

  // TV (05/06/2010): CUDA storage modifiers

     if (decl_stmt->get_declarationModifier().get_storageModifier().isCudaGlobal())
        {
          curprint( "__device__ ");
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isCudaConstant())
        {
          curprint( "__device__ __constant__ ");
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isCudaShared())
        {
          curprint( "__device__ __shared__ ");
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isCudaDynamicShared())
        {
          curprint( "extern __device__ __shared__ ");
        }

  // TV (05/06/2010): OpenCL storage modifiers

     if (decl_stmt->get_declarationModifier().get_storageModifier().isOpenclGlobal())
        {
          curprint( "__global ");
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isOpenclLocal())
        {
          curprint( "__local ");
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isOpenclConstant())
        {
          curprint( "__constant ");
        }

     if (decl_stmt->get_declarationModifier().get_storageModifier().isOpenclPrivate())
        {
          curprint( "__private ");
        }
   }


void
Unparse_MOD_SAGE::printSpecifier ( SgDeclarationStatement* decl_stmt, SgUnparse_Info & info )
   {
   // DQ (4/25/2004):  Old function was redundently represented by two separate functions
   // (call them both from this older function to remove the redundency)
      printSpecifier1 (decl_stmt,info);
      printSpecifier2 (decl_stmt,info);
   }


#if 0
// DQ (2/4/2006): Removed (not used)
void
Unparse_MOD_SAGE::printFunctionFormalArgumentSpecifier ( SgType* type, SgUnparse_Info& info )
   {
  // if ( decl_stmt->isSpecialization() && do_unparse )
  //    {
  //      curprint( "template<> ");
  //    }

  // DQ (2/4/2006): Removed (not used)
  // SgModifierNodes* modifiers = type->get_modifiers();
  // ROSE_ASSERT (modifiers != NULL);
   }
#endif




void
Unparse_MOD_SAGE::setupColorCodes ( vector< pair<bool,std::string> > & stateVector )
   {
  // std::pair<bool,std::string> p1(false,string("red"));
  // stateVector.push_back(p1);
      stateVector.push_back(pair<bool,std::string>(false,string("red")));
     stateVector.push_back(pair<bool,std::string>(false,string("orange")));
     stateVector.push_back(pair<bool,std::string>(false,string("blue")));
     stateVector.push_back(pair<bool,std::string>(false,string("green")));
     stateVector.push_back(pair<bool,std::string>(false,string("yellow")));
   }

#if 0
void
Unparse_MOD_SAGE::printColorCodes ( SgNode* node, bool openState, vector< pair<bool,std::string> > & stateVector )
   {
  // This function is part of test to embed color codes (or other information) into the unparsed output.

     SgLocatedNode* locatedNode = isSgLocatedNode(node);

  // printf ("stateVector.size() = %ld \n",stateVector.size());

  // Test the size of the stateVector agains how it is used in tests below.
     ROSE_ASSERT(stateVector.size() >= 3);

     static string staticCurprintrentColor;
     static vector<bool> staticStateVector(stateVector.size());
     ROSE_ASSERT(staticStateVector.size() >= 3);
#if 0
  // Support for debugging the embedded color codes (colorization of the generated code).
     curprint( "\n/* openState = " << ((openState == true) ? "true" : "false") << " stateVector = " << ((int)(&stateVector)) << " */ ");
     for (unsigned int i = 0; i < stateVector.size(); i++)
        {
       // If the local stat has be set then we can output the embedded color code.
          curprint( "\n/* At top: stateVector[" << i << "].first = " << ((stateVector[i].first == true) ? "true" : "false") << " */ ");
          curprint( "\n/* At top: staticStateVector[" << i << "] = " << ((staticStateVector[i] == true) ? "true" : "false") << " */ ");
        }
#endif
  // There are a few IR nodes for which we want to skip any colorization because
  // it would color the whole file or parts too large to be meaningful.
     if (isSgGlobal(node) != NULL)
        {
          return;
        }

     if (openState == true && locatedNode != NULL)
        {
          Sg_File_Info* startOfConstruct = locatedNode->get_startOfConstruct();
          Sg_File_Info* endOfConstruct   = locatedNode->get_endOfConstruct();

       // turn the color codes on
          if (startOfConstruct == NULL)
             {
            // Test the staticly held state to see if it is set, if not then allow the locally stored state to be set.
               if (staticStateVector[0] == false)
                  {
                    stateVector[0].first = true;

                 // Set the staticlly held state so that we will not trigger this color to be output until after it
                 // is turned off.  Note that it should only be turned off by the nesting level that turned it on!
                    staticStateVector[0] = true;
                  }
             }

          if (startOfConstruct != NULL)
             {
               if ( startOfConstruct->ok() == false )
                  {
                    if (staticStateVector[1] == false)
                       {
                         stateVector[1].first = true;
                         staticStateVector[1] = true;
                       }
                  }
             }

          if (endOfConstruct == NULL)
             {
               if (staticStateVector[0] == false)
                  {
                    stateVector[0].first = true;
                    staticStateVector[0] = true;
                  }
             }

          if (endOfConstruct != NULL)
             {
               if (staticStateVector[1] == false)
                  {
                    if ( endOfConstruct->ok() == false )
                       {
                         stateVector[1].first = true;
                         staticStateVector[1] = true;
                       }
                  }
             }

          if (startOfConstruct != NULL && endOfConstruct != NULL)
             {
            // Tests for consistancy of compiler generated IR nodes
               if ( startOfConstruct->isCompilerGenerated() == true )
                  {
                    if (staticStateVector[4] == false)
                       {
                         stateVector[4].first = true;
                         staticStateVector[4] = true;
                       }
                  }

               if ( endOfConstruct->isCompilerGenerated() == true )
                  {
                    if (staticStateVector[4] == false)
                       {
                         stateVector[4].first = true;
                         staticStateVector[4] = true;
                       }
                  }

               if ( startOfConstruct->isCompilerGenerated() != endOfConstruct->isCompilerGenerated() )
                  {
                    if (staticStateVector[0] == false)
                       {
                         stateVector[0].first = true;
                         staticStateVector[0] = true;
                       }
                  }
             }

       // turn the color code on (only one color!)
       // for (unsigned int i = 0; i < stateVector.size(); i++)
          bool colorOutput = false;
          for (unsigned int i = stateVector.size() - 1; i >= 0; i--)
             {
               if (staticStateVector[i] == true && colorOutput == false)
                  {
                 // If there is already a high priority color output previously then don't output anything.
                    colorOutput = true;
                  }

            // If the local state has be set then we can output the embedded color code.
               if (stateVector[i].first == true && colorOutput == false)
                  {
                 // Turn off any existing color
                    bool colorOffOutput = false;
                    unsigned int j = stateVector.size() - 1;
                    while (j >= i)
                       {
                      // Turn off the previous highest priority color
                         if (j >= i && colorOffOutput == false)
                            {
                              curprint( " /* colorCode:" << stateVector[i].second << ":off */ ");
                              colorOffOutput = true;
                            }
                         j--;
                       }

                 // Output the new curprintrent color
                    curprint( " /* colorCode:" << stateVector[i].second << ":on */ ");

                 // record the last color set
                 // staticCurprintrentColor = stateVector[i].second;

                 // record that we have output a single color!
                    colorOutput = true;
                  }

             }
        }


     if (openState == false && locatedNode != NULL)
        {
       // turn the color code off
       // for (unsigned int i = 0; i < stateVector.size(); i++)
          int size = stateVector.size();
          bool colorOutput = false;
          for (int i = size-1; i >= 0; i--)
             {
            // Since we only turn on those states that were not previously set, we can turn off all local states.
            // if (stateVector[i].first == true)
            // If the local state has be set then we can output the embedded color code.
               if (stateVector[i].first == true && colorOutput == false)
                  {
                    curprint( " /* colorCode:" << stateVector[i].second << ":off */ ");

                 // Reset the staticly held state
                    staticStateVector[i] = false;

                 // record that we have output a single color!
                    colorOutput = true;
                  }
             }
        }

#if 0
  // Support for debugging the embedded color codes (colorization of the generated code).
     for (unsigned int i = 0; i < stateVector.size(); i++)
        {
       // If the local stat has be set then we can output the embedded color code.
          curprint( "\n/* At base: stateVector[" << i << "].first = " << ((stateVector[i].first == true) ? "true" : "false") << " */ ");
          curprint( "\n/* At base: staticStateVector[" << i << "] = " << ((staticStateVector[i] == true) ? "true" : "false") << " */ ");
        }
#endif
   }
#endif


void
Unparse_MOD_SAGE::printColorCodes ( SgNode* node, bool openState, vector< pair<bool,std::string> > & stateVector )
   {
  // This function is part of test to embed color codes (or other information) into the unparsed output.

     SgLocatedNode* locatedNode = isSgLocatedNode(node);

  // printf ("stateVector.size() = %ld \n",stateVector.size());

  // Test the size of the stateVector agains how it is used in tests below.
     ROSE_ASSERT(stateVector.size() >= 3);

     static string staticCurprintrentColor;
     static vector<bool> staticStateVector(stateVector.size());
     ROSE_ASSERT(staticStateVector.size() >= 3);
#if 0
  // Support for debugging the embedded color codes (colorization of the generated code).
     curprint( "\n/* openState = " << ((openState == true) ? "true" : "false") << " stateVector = " << ((int)(&stateVector)) << " */ ");
     for (unsigned int i = 0; i < stateVector.size(); i++)
        {
       // If the local stat has be set then we can output the embedded color code.
          curprint( "\n/* At top: stateVector[" << i << "].first = " << ((stateVector[i].first == true) ? "true" : "false") << " */ ");
          curprint( "\n/* At top: staticStateVector[" << i << "] = " << ((staticStateVector[i] == true) ? "true" : "false") << " */ ");
        }
#endif
  // There are a few IR nodes for which we want to skip any colorization because
  // it would color the whole file or parts too large to be meaningful.
     if (isSgGlobal(node) != NULL)
        {
          return;
        }

     if (openState == true && locatedNode != NULL)
        {
          Sg_File_Info* startOfConstruct = locatedNode->get_startOfConstruct();
          Sg_File_Info* endOfConstruct   = locatedNode->get_endOfConstruct();

       // turn the color codes on
          if (startOfConstruct == NULL)
             {
            // Test the staticly held state to see if it is set, if not then allow the locally stored state to be set.
               if (staticStateVector[0] == false)
                  {
                    stateVector[0].first = true;

                 // Set the staticlly held state so that we will not trigger this color to be output until after it
                 // is turned off.  Note that it should only be turned off by the nesting level that turned it on!
                    staticStateVector[0] = true;
                  }
             }

          if (startOfConstruct != NULL)
             {
               if ( startOfConstruct->ok() == false )
                  {
                    if (staticStateVector[1] == false)
                       {
                         stateVector[1].first = true;
                         staticStateVector[1] = true;
                       }
                  }
             }

          if (endOfConstruct == NULL)
             {
               if (staticStateVector[0] == false)
                  {
                    stateVector[0].first = true;
                    staticStateVector[0] = true;
                  }
             }

          if (endOfConstruct != NULL)
             {
               if (staticStateVector[1] == false)
                  {
                    if ( endOfConstruct->ok() == false )
                       {
                         stateVector[1].first = true;
                         staticStateVector[1] = true;
                       }
                  }
             }

          if (startOfConstruct != NULL && endOfConstruct != NULL)
             {
            // Tests for consistancy of compiler generated IR nodes
               if ( startOfConstruct->isCompilerGenerated() == true )
                  {
                    if (staticStateVector[4] == false)
                       {
                         stateVector[4].first = true;
                         staticStateVector[4] = true;
                       }
                  }

               if ( endOfConstruct->isCompilerGenerated() == true )
                  {
                    if (staticStateVector[4] == false)
                       {
                         stateVector[4].first = true;
                         staticStateVector[4] = true;
                       }
                  }

               if ( startOfConstruct->isCompilerGenerated() != endOfConstruct->isCompilerGenerated() )
                  {
                    if (staticStateVector[0] == false)
                       {
                         stateVector[0].first = true;
                         staticStateVector[0] = true;
                       }
                  }
             }

       // turn the color code on
          for (unsigned int i = 0; i < stateVector.size(); i++)
             {
            // If the local stat has be set then we can output the embedded color code.
               if (stateVector[i].first == true)
                  {
                    curprint( " /* colorCode:" + stateVector[i].second + ":on */ ");

                 // record the last color set
                    staticCurprintrentColor = stateVector[i].second;
                  }
             }
        }


     if (openState == false && locatedNode != NULL)
        {
       // turn the color code off
       // for (unsigned int i = 0; i < stateVector.size(); i++)
          int size = stateVector.size();
          for (int i = size-1; i >= 0; i--)
             {
            // Since we only turn on those states that were not previously set, we can turn off all local states.
               if (stateVector[i].first == true)
                  {
                    curprint( " /* colorCode:" + stateVector[i].second + ":off */ ");

                 // Reset the staticly held state
                    staticStateVector[i] = false;
                  }
             }
        }

#if 0
  // Support for debugging the embedded color codes (colorization of the generated code).
     for (unsigned int i = 0; i < stateVector.size(); i++)
        {
       // If the local stat has be set then we can output the embedded color code.
          curprint( "\n/* At base: stateVector[" << i << "].first = " << ((stateVector[i].first == true) ? "true" : "false") << " */ ");
          curprint( "\n/* At base: staticStateVector[" << i << "] = " << ((staticStateVector[i] == true) ? "true" : "false") << " */ ");
        }
#endif
   }
