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
using namespace Rose;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0

// MS: temporary flag for experiments with uparsing of template instantiations
bool Unparse_MOD_SAGE::experimentalMode=false;
int Unparse_MOD_SAGE::experimentalModeVerbose=0;

Unparse_MOD_SAGE::Unparse_MOD_SAGE(Unparser* unp):unp(unp) {
}

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
        {
          return true;
        }

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
        {
          return true;
        }

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
     SgFunctionRefExp* func_ref        = isSgFunctionRefExp(expr);
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
     ROSE_ASSERT(expr != NULL);

     bool isBinaryOperatorResult = false;

     SgFunctionRefExp* func_ref        = isSgFunctionRefExp(expr);
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
       // if (isUnaryOperatorPlus(expr) || isUnaryOperatorMinus(expr))
          if (isUnaryOperator(expr) == true)
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
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);
     ROSE_ASSERT(expr != NULL);

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

    // DQ (2/1/2018): Added to catch case of non-member function unary operator
       else
        {
          SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
          if (func_ref != NULL)
             {
               SgFunctionSymbol* func_sym = func_ref->get_symbol();
               if (func_sym != NULL)
                  {
                    SgFunctionDeclaration* func_decl = func_sym->get_declaration();
                    if (func_decl != NULL)
                       {
                         SgName func_name = func_decl->get_name();
                         if (func_name.getString() == "operator++")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 0) 
                                   return true;
                            }
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
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);
     ROSE_ASSERT(expr != NULL);

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

    // DQ (2/1/2018): Added to catch case of non-member function unary operator
       else
        {
          SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
          if (func_ref != NULL)
             {
               SgFunctionSymbol* func_sym = func_ref->get_symbol();
               if (func_sym != NULL)
                  {
                    SgFunctionDeclaration* func_decl = func_sym->get_declaration();
                    if (func_decl != NULL)
                       {
                         SgName func_name = func_decl->get_name();
                         if (func_name.getString() == "operator--")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 0) 
                                   return true;
                            }
                       }
                  }
             }
        }

     return false;
   }


bool
Unparse_MOD_SAGE::isUnaryLiteralOperator(SgExpression* expr)
   {
     ROSE_ASSERT(expr != NULL);

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
                    std::string s = func_name.getString();
                    if (s.find("operator \"\" ",0) != std::string::npos)
                       {
                         return true;
                       }
                  }
             }
        }

    // DQ (2/12/2019): Added to catch case of non-member function unary operator
       else
        {
          SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
          if (func_ref != NULL)
             {
               SgFunctionSymbol* func_sym = func_ref->get_symbol();
               if (func_sym != NULL)
                  {
                    SgFunctionDeclaration* func_decl = func_sym->get_declaration();
                    if (func_decl != NULL)
                       {
                         SgName func_name = func_decl->get_name();
                         std::string s = func_name.getString();
                         if (s.find("operator \"\" ",0) != std::string::npos)
                            {
                              return true;
                            }
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
#if PRINT_DEVELOPER_WARNINGS || 0
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
#if 0
          printf ("Warning, Unparse_MOD_SAGE::isUnaryOperator() test on non-function \n");
#endif
          return false;
        }

     string func_name;
     if (func_ref != NULL)
        {
#if 0
          printf ("In isUnaryOperator(): this is a non-member function \n");
#endif
          func_name = func_ref->get_symbol()->get_name().str();
        }
       else
        {
#if 0
          printf ("In isUnaryOperator(): this is a member function \n");
#endif
          func_name = mfunc_ref->get_symbol()->get_name().str();
        }

#if 0
     printf ("In isUnaryOperator(): func_name = %s \n",func_name.c_str());
#endif

  // DQ (2/14/2005): Need special test for operator*(), and other unary operators,
  // similar to operator+() and operator-().
  // Error need to check number of parameters in arg list of operator* to verify it
  // is a unary operators else it could be a binary multiplication operator
  // Maybe also for the binary operator&() (what about operator~()?)
  // Added support for isUnaryDereferenceOperator(), isUnaryAddressOperator(),
  // isUnaryOrOperator(), isUnaryComplementOperator().
#if 1
  // DQ (2/1/2018): The argument to these functions in the predicate should be "exp" not "mfunc_ref"
     if ( isUnaryOperatorPlus(expr) ||
          isUnaryOperatorMinus(expr) ||
       // DQ (2/1/2018): This operator now has a function to support the evaluation of it being unary.
       // func_name == "operator!" ||
          isUnaryNotOperator(expr) ||
       // func_name == "operator*" ||
          isUnaryDereferenceOperator(expr) ||
       // DQ (11/24/2004): Added support for address operator "operator&"
       // func_name == "operator&" ||
          isUnaryAddressOperator(expr) ||
          func_name == "operator--" ||
          func_name == "operator++" ||
       // DQ (2/12/2019): Adding support for C++11 literal operators.
          isUnaryLiteralOperator(expr) ||
       // DQ (2/1/2018): I don't think this operator can exist.
       // isUnaryOrOperator(mfunc_ref) ||
       // func_name == "operator~")
          isUnaryComplementOperator(expr) )
#else
  // Original code.

#error "DEAD CODE!"

     if ( isUnaryOperatorPlus(mfunc_ref) ||
          isUnaryOperatorMinus(mfunc_ref) ||
       // DQ (2/1/2018): This operator now has a function to support the evaluation of it being unary.
       // func_name == "operator!" ||
          isUnaryNotOperator(mfunc_ref) ||
       // func_name == "operator*" ||
          isUnaryDereferenceOperator(mfunc_ref) ||
       // DQ (11/24/2004): Added support for address operator "operator&"
       // func_name == "operator&" ||
          isUnaryAddressOperator(mfunc_ref) ||
          func_name == "operator--" ||
          func_name == "operator++" ||
       // DQ (2/1/2018): I don't think this operator can exist.
       // isUnaryOrOperator(mfunc_ref) ||
       // func_name == "operator~")
          isUnaryComplementOperator(mfunc_ref) )
#endif
        {
#if 0
          printf ("In isUnaryOperator(): returning true \n");
#endif
          return true;
        }

#if 0
     printf ("In isUnaryOperator(): returning false \n");
#endif

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
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);
     ROSE_ASSERT(expr != NULL);

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
                            {
#if 0
                              printf ("In isUnaryPostfixOperator(): returning true \n");
#endif
                              return true;
                            }
                           else
                            {
                           // DQ (2/12/2019): Check if this is a literal operator.
                              if (mfunc_decl->get_specialFunctionModifier().isUldOperator() == true)
                                 {
#if 0
                                   printf ("In isUnaryPostfixOperator(): literal operator: returning true \n");
#endif
                                   return true;
                                 }
                            }
                       }
                  }
             }
        }

    // DQ (2/1/2018): Added to catch case of non-member function unary operator
       else
        {
          SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
          if (func_ref != NULL)
             {
               SgFunctionSymbol* func_sym = func_ref->get_symbol();
               if (func_sym != NULL)
                  {
                    SgFunctionDeclaration* func_decl = func_sym->get_declaration();
                    if (func_decl != NULL)
                       {
                         SgName func_name = func_decl->get_name();
                      // if (func_name.getString() == "operator--")
                         if (func_name.getString() == "operator++" || func_name.getString() == "operator--")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 2) 
                                 {
#if 0
                                   printf ("In isUnaryPostfixOperator(): returning true \n");
#endif
                                   return true;
                                 }
                            }
                           else
                            {
                           // DQ (2/12/2019): Check if this is a literal operator.
                              if (func_decl->get_specialFunctionModifier().isUldOperator() == true)
                                 {
#if 0
                                   printf ("In isUnaryPostfixOperator(): literal operator: returning true \n");
#endif
                                   return true;
                                 }
                            }
                       }
                  }
             }
        }

#if 0
     printf ("In isUnaryPostfixOperator(): returning false \n");
#endif

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

int
GetOperatorVariant(SgExpression* expr) 
   {
#if 0
     printf ("In GetOperatorVariant(): expr = %p = %s \n",expr,expr->class_name().c_str());
  // curprint(string("/* In GetOperatorVariant(): expr = ") + expr->class_name() + " */ \n");
#endif

     SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
     if (func_call == NULL)
        {
          return expr->variantT();
        }

     SgName name;
     SgExpression *func = func_call->get_function();
     switch (func->variantT())
        {
          case V_SgFunctionRefExp:
               name = isSgFunctionRefExp(func)->get_symbol()->get_name();
               break;
          case V_SgDotExp:
          case V_SgArrowExp:
             {
               SgExpression *mfunc = isSgBinaryOp(func)->get_rhs_operand();

            // DQ (9/28/2012): Added assertion.
               ROSE_ASSERT(mfunc != NULL);

               if (mfunc->variantT() == V_SgPseudoDestructorRefExp)
                    return V_SgFunctionCallExp;

            // DQ (9/28/2012): This could be a variable (pointer to function).
               SgVarRefExp* var_ref = isSgVarRefExp(mfunc);
               if (var_ref != NULL)
                    return (int)V_SgVarRefExp;

#if 0
               SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(mfunc);
            // DQ (9/28/2012): Added debug support.
               if (mfunc_ref == NULL)
                  {
                    printf ("ERROR: mfunc = %p = %s mfunc->get_startOfConstruct() = %p mfunc->get_operatorPosition() = %p \n",mfunc,mfunc->class_name().c_str(),mfunc->get_startOfConstruct(),mfunc->get_operatorPosition());
                    mfunc->get_startOfConstruct()->display("Error in GetOperatorVariant() in modified_sage.C (unparser): debug");
                  }
               ROSE_ASSERT(mfunc_ref != NULL);
               name = mfunc_ref->get_symbol()->get_name();
#else
            // DQ (11/27/2012): Added more general support for templates to include new IR nodes.
               SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(mfunc);
               SgTemplateMemberFunctionRefExp* tplmfunc_ref = isSgTemplateMemberFunctionRefExp(mfunc);
               SgNonrealRefExp * nrref = isSgNonrealRefExp(mfunc);
               if (mfunc_ref != NULL) {
                 name = mfunc_ref->get_symbol()->get_name();
               } else if (tplmfunc_ref != NULL) {
                 name = tplmfunc_ref->get_symbol()->get_name();
               } else if (nrref != NULL) {
                 name = nrref->get_symbol()->get_name();
               } else {
                 printf("ERROR: unexpected reference expression for a member-function: %p (%s)\n", mfunc, mfunc ? mfunc->class_name().c_str() : "");
                 ROSE_ASSERT(false);
               }
#endif
               break;
             }

          default:
               return V_SgFunctionCallExp;
        }

     string func_name = name.str();

#if 0
     printf ("In GetOperatorVariant(): func_name = %s \n",func_name.c_str());
#endif

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
    // Detect function call or function call operator.
       else if (func_name.find("operator") == string::npos || func_name == "operator()") return V_SgFunctionCallExp;
    // Detect case operator from one class to another.
       else if (func_name.find("operator") != string::npos) return V_SgCastExp;
       else
        {
          printf ("Error: default case reached in GetOperatorVariant func_name = %s \n",func_name.c_str());
          assert(false);
       /* avoid MSCV warning by adding return stmt */
          return -1;
        }
   }


SgExpression*
GetFirstOperand(SgExpression* expr) 
   {
     SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
     if (func_call != NULL)
          return func_call->get_function();
       else
        {
          SgUnaryOp *op1 = isSgUnaryOp(expr);
          if (op1 != 0)
               return op1->get_operand();
            else
             {
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
int
GetPrecedence(int variant)
   {
     ROSE_ASSERT(!"Deprecated. Use UnparseLanguageIndependentConstructs::getPrecedence instead");
     return -1;
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
       return 0;
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
       return false;
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
            // printf ("In Unparse_MOD_SAGE::isOneElementList: expr_list->get_expressions().size() = %" PRIuPTR " \n",expr_list->get_expressions().size());
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
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(decl_stmt);
     if (variableDeclaration != NULL)
        {
       // DQ (6/15/2019): This is horrible API for this function.
          SgInitializedName & initalizedName = SageInterface::getFirstVariable(*variableDeclaration);
       // ROSE_ASSERT(initalizedName != NULL);
          printf (" --- initalizedName name = %s \n",initalizedName.get_name().str());
        }

     printf ("info.isPrivateAccess()   = %s \n",info.isPrivateAccess()   ? "true" : "false");
     printf ("info.isProtectedAccess() = %s \n",info.isProtectedAccess() ? "true" : "false");
     printf ("info.isPublicAccess()    = %s \n",info.isPublicAccess()    ? "true" : "false");
  // printf ("info.isDefaultAccess()   = %s \n",info.isDefaultAccess()   ? "true" : "false");

     printf ("decl_stmt->get_declarationModifier().get_accessModifier().isPrivate()   = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isPrivate()   ? "true" : "false");
     printf ("decl_stmt->get_declarationModifier().get_accessModifier().isProtected() = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isProtected() ? "true" : "false");
     printf ("decl_stmt->get_declarationModifier().get_accessModifier().isPublic()    = %s \n",decl_stmt->get_declarationModifier().get_accessModifier().isPublic()    ? "true" : "false");
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
     ROSE_ASSERT(decl_stmt != NULL);

#if 0
     printf ("Inside of outputExternLinkageSpecifier() decl_stmt = %p = %s decl_stmt->isExternBrace() = %s \n",decl_stmt,decl_stmt->class_name().c_str(),decl_stmt->isExternBrace() ? "true" : "false");
     printf ("   --- decl_stmt->isExternBrace()                                            = %s \n",decl_stmt->isExternBrace() ? "true" : "false");
     printf ("   --- decl_stmt->get_declarationModifier().get_storageModifier().isExtern() = %s \n",decl_stmt->get_declarationModifier().get_storageModifier().isExtern() ? "true" : "false");
     printf ("   --- decl_stmt->get_linkage().empty()                                      = %s \n",decl_stmt->get_linkage().empty() ? "true" : "false");
     curprint ("\n/* Inside of outputExternLinkageSpecifier() */ \n ");
#endif

  // DQ (5/10/2007): Fixed linkage to be a std::string instead of char*
  // if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && decl_stmt->get_linkage())
     if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && decl_stmt->get_linkage().empty() == false)
        {
#if 1
           printf ("/* output extern keyword */ \n");
           
#endif
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

#define DEBUG_TEMPLATE_SPECIALIZATION 0

#if DEBUG_TEMPLATE_SPECIALIZATION
     printf ("In outputTemplateSpecializationSpecifier(): experimentalMode = %s \n",experimentalMode ? "true" : "false");
     curprint( "\n/* In outputTemplateSpecializationSpecifier(): TOP of function */ ");
#endif

     if (experimentalMode)
       {
         outputTemplateSpecializationSpecifier2 ( decl_stmt );
         return;
       }

#if 0
     if ( (isSgTemplateInstantiationDecl(decl_stmt) != NULL) ||
          (isSgTemplateInstantiationFunctionDecl(decl_stmt) != NULL) ||
          (isSgTemplateInstantiationMemberFunctionDecl(decl_stmt) != NULL) )
        {

#error "DEAD CODE!"

          curprint( "template<> ");
        }
#else

     if ( (isSgTemplateInstantiationDecl(decl_stmt)               != NULL) ||
       // DQ (1/3/2016): Adding support for template variable declarations.
          (isSgTemplateVariableDeclaration(decl_stmt)             != NULL) ||
          (isSgTemplateInstantiationFunctionDecl(decl_stmt)       != NULL) ||
          (isSgTemplateInstantiationMemberFunctionDecl(decl_stmt) != NULL) )
        {
#if DEBUG_TEMPLATE_SPECIALIZATION
          curprint( "\n/* In outputTemplateSpecializationSpecifier(): This is a template instantiation */ ");
#endif
          if ( isSgTemplateInstantiationDirectiveStatement(decl_stmt->get_parent()) != NULL)
             {
            // Template instantiation directives use "template" instead of "template<>"
#if DEBUG_TEMPLATE_SPECIALIZATION
               curprint( "\n/* In outputTemplateSpecializationSpecifier(): This is a SgTemplateInstantiationDirectiveStatement */ ");
#endif
               curprint( "template ");
             }
            else
             {
            // Normal case for output of template instantiations (which ROSE puts out as specializations)
            // curprint( "template<> ");
#if DEBUG_TEMPLATE_SPECIALIZATION
               curprint( "\n/* In outputTemplateSpecializationSpecifier(): Normal case for output of template instantiations: " +  decl_stmt->class_name() + " */ ");
#endif
            // DQ (5/2/2012): If this is a function template instantiation in a class template instantiation then 
            // we don't want the "template<>" (error in g++, at least).  See test2012_59.C.
               SgTemplateInstantiationDefn* templateClassInstatiationDefn = isSgTemplateInstantiationDefn(decl_stmt->get_parent());
               if (templateClassInstatiationDefn != NULL)
                  {
                 // DQ (4/6/2014): This happens when a member function template in embedded in a class
                 // template and thus there is not an associated template for the member function separate
                 // from the class declaration.  It is not rare for many system template libraries (e.g. iostream).
#if DEBUG_TEMPLATE_SPECIALIZATION
                    printf ("This is a declaration defined in a templated class (suppress the output of template specialization syntax) \n");

                 // DQ (8/8/2012): This is a valid branch, commented out assert(false).
                 // DQ (8/2/2012): This branch should not be possible so assert false as a test (note that test2005_139.C will demonstrate this branch).
                    printf ("Warning: Rare case: It should be impossible to reach this code since SgTemplateInstantiationDefn is not a class, function or member function type \n");
                 // ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
                 // DQ (7/6/2015): template member function instantiations defined outside of the template class shoudl not be output with the "template<>" syntax.
                 // curprint("template<> ");
                    if (isSgTemplateInstantiationMemberFunctionDecl(decl_stmt) != NULL)
                       {
                      // Check for additional rule in the output of "template<>" for member function instantiations.
                         SgTemplateInstantiationDefn* templateClassInstatiationDefn = isSgTemplateInstantiationDefn(decl_stmt->get_scope());
                         if (templateClassInstatiationDefn != NULL)
                            {
                           // Supress output of "template<>" syntax for template member function instantiations.
#if DEBUG_TEMPLATE_SPECIALIZATION
                              SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(decl_stmt);
                              printf ("templateInstantiationMemberFunctionDecl = %p \n",templateInstantiationMemberFunctionDecl);
                              printf ("templateInstantiationMemberFunctionDecl->get_templateName() = %s \n",templateInstantiationMemberFunctionDecl->get_templateName().str());
                              printf ("templateInstantiationMemberFunctionDecl->get_templateDeclaration() = %p \n",templateInstantiationMemberFunctionDecl->get_templateDeclaration());
                              printf ("templateInstantiationMemberFunctionDecl->get_templateArguments().size() = %zu \n",templateInstantiationMemberFunctionDecl->get_templateArguments().size());
                              printf ("templateInstantiationMemberFunctionDecl->get_nameResetFromMangledForm() = %s \n",templateInstantiationMemberFunctionDecl->get_nameResetFromMangledForm() ? "true" : "false");
#endif
                           // DQ (7/6/2015): Check if these is a prototype that was output via the associated class being output as a template instantiation.
                           // If the class containing the member function was output (e.g. when the testTemplates translator is run on test2015_35.C) 
                           // then we don't want the "template<>" syntax on the member function instantiation, else if it was not output (e.g. when 
                           // testTranslator is run on test2015_35.C) then we require the "template<>" syntax.
                              SgTemplateInstantiationMemberFunctionDecl* nondefiningTemplateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(decl_stmt->get_firstNondefiningDeclaration());
                              ROSE_ASSERT(nondefiningTemplateInstantiationMemberFunctionDecl != NULL);
#if DEBUG_TEMPLATE_SPECIALIZATION
                              printf("  nondefiningTemplateInstantiationMemberFunctionDecl->get_parent() = %p (%s)\n", nondefiningTemplateInstantiationMemberFunctionDecl->get_parent(), nondefiningTemplateInstantiationMemberFunctionDecl->get_parent() ? nondefiningTemplateInstantiationMemberFunctionDecl->get_parent()->class_name().c_str() : "");
#endif
                              bool isOutput = false;

                              SgTemplateInstantiationDefn* nondefiningTemplateClassInstatiationDefn = isSgTemplateInstantiationDefn(nondefiningTemplateInstantiationMemberFunctionDecl->get_parent());
                              if (nondefiningTemplateClassInstatiationDefn != NULL) {
                                SgTemplateInstantiationDecl* templateClassInstantiation = isSgTemplateInstantiationDecl(nondefiningTemplateClassInstatiationDefn->get_parent());
                                ROSE_ASSERT(templateClassInstantiation != NULL);
#if DEBUG_TEMPLATE_SPECIALIZATION
                                printf ("templateClassInstantiation->get_file_info()->isCompilerGenerated()      = %s \n",templateClassInstantiation->get_file_info()->isCompilerGenerated() ? "true" : "false");
                                printf ("templateClassInstantiation->get_file_info()->isOutputInCodeGeneration() = %s \n",templateClassInstantiation->get_file_info()->isOutputInCodeGeneration() ? "true" : "false");
#endif
                             // isOutput = (templateClassInstantiation->get_file_info()->isCompilerGenerated() && templateClassInstantiation->get_file_info()->isOutputInCodeGeneration());
                             // TV (3/14/18): This need to be true whether or not it is compiler generated (template<> not used when defining a member of a class specialization)
                                isOutput = templateClassInstantiation->get_file_info()->isOutputInCodeGeneration();
                              }
                              if (isOutput == true)
                                 {
#if DEBUG_TEMPLATE_SPECIALIZATION
                                   curprint("/* Member function's class instantation WAS output, so we need to supress the output of template<> syntax */ ");
#endif
                                 }
                                else
                                 {
#if DEBUG_TEMPLATE_SPECIALIZATION
                                   curprint("/* Member function's class instantation was NOT output, so we need to output of template<> syntax */ ");
#endif
                                   curprint("template<> ");
                                 }
#if 0
                           // DQ (7/7/2015): I now don't think this is an effective test.
                              if (templateInstantiationMemberFunctionDecl->get_templateArguments().size() == 0)
                                 {
                                // This case is to support test2004_67.C
#if 0
                                   curprint("/* Member function without template arguments from template class instantiations requires the output of template<> syntax */ ");
#endif
                                // curprint("template<> ");
                                 }
                                else
                                 {
#if 0
                                   curprint("/* Supress output of template<> syntax for template member function instantiations */ ");
#endif
                                 }
#endif
                            }
                           else
                            {
#if DEBUG_TEMPLATE_SPECIALIZATION
                              curprint("/* Member function instantiations in non-template classes still output template<> syntax */ ");
#endif
                              curprint("template<> ");
                            }
                       }
                      else
                       {
#if DEBUG_TEMPLATE_SPECIALIZATION
                         curprint("/* This still might require the output of the template<> syntax */ ");
#endif
                      // DQ (11/27/2015): If this is a friend function then supress the "template<>" syntax (see test2015_123.C).
                      // But we have to check the non-defining declaration for the friend function marking.
                      // curprint("template<> ");
                         SgTemplateInstantiationFunctionDecl* nondefiningTemplateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(decl_stmt->get_firstNondefiningDeclaration());
                         if (nondefiningTemplateInstantiationFunctionDecl != NULL)
                            {
                           // DQ (1/13/2020): The firstNondefiningDeclaration might not be the friend declaration 
                           // (it might be another nondefining declaration (see Cxx11_tests/test2020_47.C)).
                           // if (nondefiningTemplateInstantiationFunctionDecl->get_declarationModifier().isFriend() == true)
                              if (decl_stmt->get_declarationModifier().isFriend() == true)
                                 {
#if DEBUG_TEMPLATE_SPECIALIZATION
                                   printf ("Supress the output of the template<> syntax \n");
                                   curprint("/* Non-Member friend function instantiations cause us to supress the output of template<> syntax */ ");
#endif
                                 }
                                else
                                 {
#if DEBUG_TEMPLATE_SPECIALIZATION
                                   curprint("/* Non-Member (non-friend) function instantiations still output template<> syntax */ ");
#endif
                                   curprint("template<> ");
                                 }
                            }
                           else
                            {
                           // DQ (4/11/2019): Check if this is a friend declaration.
                              if (decl_stmt->get_declarationModifier().isFriend() == true)
                                 {
#if DEBUG_TEMPLATE_SPECIALIZATION
                                   printf ("Supress the output of the template<> syntax for friend non-function specializations \n");
                                   curprint("/* Non-Member friend non-function instantiations cause us to supress the output of template<> syntax */ ");
#endif
                                 }
                                else
                                 {
#if DEBUG_TEMPLATE_SPECIALIZATION
                                   curprint("/* Non function instantiations still output template<> syntax */ ");
#endif
                                   curprint("template<> ");
                                 }
                            }
                       }
                  }
             }
        }

#if DEBUG_TEMPLATE_SPECIALIZATION
     curprint( "\n/* Leaving outputTemplateSpecializationSpecifier() */ ");
#endif
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
     // DQ (1/3/2016): We need to include SgTemplateVariableDeclarations as well.
  // if (isSgTemplateInstantiationFunctionDecl(decl_stmt) == NULL && isSgTemplateInstantiationMemberFunctionDecl(decl_stmt) == NULL)
     if (isSgTemplateInstantiationFunctionDecl(decl_stmt) == NULL && isSgTemplateInstantiationMemberFunctionDecl(decl_stmt) == NULL && isSgTemplateVariableDeclaration(decl_stmt) == NULL)
        {
          outputExternLinkageSpecifier(decl_stmt);
        }
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

#if 0
     printf ("In printSpecifier2(SgDeclarationStatement* decl_stmt): TOP \n");
#endif

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(decl_stmt);

  // DQ (2/4/2006): Moved output of "friend" keyword inside of test for SgFunctionDeclaration

  // DQ (2/4/2006): Need this case for friend class declarations
     if (decl_stmt->get_declarationModifier().isFriend())
        {
#if 1
       // This assertion fails in test2004_116.C
       // ROSE_ASSERT(functionDeclaration != NULL);
          if (functionDeclaration == NULL)
             {
               curprint( "friend ");
             }
#else
       // DQ (3/8/2012): We now handle a new design of templates as class derived from SgFunctionDeclaration, so this is a better implementation now.
          ROSE_ASSERT(functionDeclaration != NULL);
          ROSE_ASSERT(functionDeclaration->get_parent() != NULL);
          ROSE_ASSERT(functionDeclaration->get_scope() != NULL);
          if (functionDeclaration->get_parent() == functionDeclaration->get_scope())
             {
               curprint("friend ");
             }
            else
             {
               curprint("/* skip output of friend keyword */ ");
             }
#endif
        }

     if (functionDeclaration != NULL)
        {
       // curprint("/* printSpecifier2 */ ");

       // DQ (7/26/2014): Added support to output the C11 _Noreturn keyword.
          if (functionDeclaration->get_using_C11_Noreturn_keyword() == true)
             {
               curprint("_Noreturn ");
             }

       // DQ (8/1/2014): Added support to output the constexpr keyword.
          if (functionDeclaration->get_is_constexpr() == true)
             {
               curprint("constexpr ");
             }

       // DQ (2/4/2006): Template specialization declarations (forward declaration) can't have some modified output
          bool isDeclarationOfTemplateSpecialization = false;
          SgDeclarationStatement::template_specialization_enum specializationEnumValue = functionDeclaration->get_specialization();
          isDeclarationOfTemplateSpecialization =
               (specializationEnumValue == SgDeclarationStatement::e_specialization) ||
               (specializationEnumValue == SgDeclarationStatement::e_partial_specialization);

       // DQ (2/2/2006): friend can't be output for a Template specialization declaration
       // curprint((string("/* isDeclarationOfTemplateSpecialization = ") << ((isDeclarationOfTemplateSpecialization == true) ? string("true") : string("false")) << string(" */ \n "));
       // printf ("isDeclarationOfTemplateSpecialization = %s \n",isDeclarationOfTemplateSpecialization == true ? "true" : "false");
          if ( (decl_stmt->get_declarationModifier().isFriend() == true) && (isDeclarationOfTemplateSpecialization == false) )
             {
               ROSE_ASSERT(decl_stmt->get_parent() != NULL);
#if 0
               printf ("In printSpecifier2(SgDeclarationStatement* decl_stmt): decl_stmt->get_parent() = %p = %s \n",decl_stmt->get_parent(),decl_stmt->get_parent()->class_name().c_str());
#endif
            // DQ (11/28/2015): We need to filter the cases where the function is not output in a class definition.
            // curprint( "friend ");
               if (isSgClassDefinition(decl_stmt->get_parent()) != NULL)
                  {
                    curprint( "friend ");
                  }
             }

       // DQ (2/2/2006): Not sure if virtual can be output when isForwardDeclarationOfTemplateSpecialization == true
          if (functionDeclaration->get_functionModifier().isVirtual())
             {
#if 0
               if (functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration())
                  {
                    curprint("/* output virtual for the non-defining function declarations */ ");
                  }
                 else
                  {
                    if (functionDeclaration == functionDeclaration->get_definingDeclaration())
                       {
                         curprint("/* output virtual for the defining function declarations */ ");
                       }
                  }
#endif

            // DQ (4/11/2019): Only output the "virtual" keyword for functions defined in a class definition.
            // curprint("virtual ");
               SgClassDefinition* classDefinition = isSgClassDefinition(functionDeclaration->get_parent());
               if (classDefinition != NULL)
                  {
                 // DQ (2/15/2020): It is an error to output "friend virtual" as code. (error: "virtual functions cannot be friends").
                 // See Cxx11_tests/test2020_66.C for an example of where this is currently generated (incorrectly).
                 // curprint("virtual ");
                    bool isFriend = ( (decl_stmt->get_declarationModifier().isFriend() == true) && (isDeclarationOfTemplateSpecialization == false) );
                    if (isFriend == false)
                       {
                         curprint("virtual ");
                       }
                  }
             }

       // if (unp->opt.get_inline_opt())

       // DQ (2/2/2006): Not sure if virtual can be output when isForwardDeclarationOfTemplateSpecialization == true
       // DQ (4/28/2004): output "inline" even for function definitions
       // if (!info.SkipFunctionDefinition())
          if (functionDeclaration->get_functionModifier().isInline())
             {
            // DQ (9/25/2013): Check if this is a C file using -std=c89, and if so then unparse "__inline__" instead of "inline".
            // curprint( "inline ");
               SgFile* file = TransformationSupport::getFile(functionDeclaration);
               ROSE_ASSERT(file != NULL);
               if (file->get_C89_only() == true && file->get_C89_gnu_only() == false)
                  {
                 // DQ (9/25/2013): This is what is required when using -std=c89 (the default for GNU gcc is -std=gnu89).
                    curprint( "__inline__ ");
                  }
                 else
                  {
#if 0
                    printf ("In printSpecifier2(): Output function's inline keyword \n");
#endif
#if 1
                 // DQ (6/27/2015): We need this to be output because the isGnuAttributeAlwaysInline() maybe true, but we need to output
                 // the "inline" keyword for GNU 4.2.4 compiler (only able to demonstrate the problem on Google protobuffer on RHEL5).
                    curprint( "inline ");
#else
                    if (functionDeclaration->get_functionModifier().isGnuAttributeAlwaysInline() == true)
                       {
                      // Supress use of redundant inline keyword if the GNU attribute is being used.
#if 0
                         printf ("In printSpecifier2(): Supress use of redundant inline keyword if the GNU attribute is being used \n");
#endif
                       }
                      else
                       {
                         curprint( "inline ");
                       }
#endif
                  }
             }

#if 0
          printf ("info.SkipFunctionDefinition() = %s \n",info.SkipFunctionDefinition() ? "true" : "false");
          printf ("functionDeclaration->get_functionModifier().isExplicit() = %s \n",functionDeclaration->get_functionModifier().isExplicit() ? "true" : "false");
          printf ("isDeclarationOfTemplateSpecialization = %s \n",isDeclarationOfTemplateSpecialization ? "true" : "false");
#endif

       // DQ (4/13/2019): We want to output the explicit keyword even when info.SkipFunctionDefinition() == true.
       // DQ (2/2/2006): friend can't be output for a Template specialization declaration
       // if ((!info.SkipFunctionDefinition()) && functionDeclaration->get_functionModifier().isExplicit())
       // if ( (info.SkipFunctionDefinition() == false) &&
       //      (functionDeclaration->get_functionModifier().isExplicit() == true) &&
       //      (isDeclarationOfTemplateSpecialization == false) )
          if ( (functionDeclaration->get_functionModifier().isExplicit() == true) &&
               (isDeclarationOfTemplateSpecialization == false) )
             {
#if 0
               printf ("@@@@@@@@ Detected use of keyword and explicit keyword is being output \n");
#endif
            // DQ (4/13/2019): We can't output the "explicit" keyword for a function outside of it's class.
            // curprint( "explicit ");
            // check that this is a declaration appearing in a class.
               SgClassDefinition* classDefinition = isSgClassDefinition(functionDeclaration->get_parent());
               if (classDefinition != NULL)
                  {
                    curprint( "explicit ");
                  }
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
#if 0
               SgFunctionModifier::opencl_work_group_size_t opencl_work_group_size = functionDeclaration->get_functionModifier().get_opencl_work_group_size();
            // curprint( "__attribute__((work_group_size_hint(" << opencl_work_group_size.x << ", " << opencl_work_group_size.y << ", " << opencl_work_group_size.z << "))) ");
#endif
               curprint( "__attribute__((work_group_size_hint(X, Y, Z))) ");
             }
          if (functionDeclaration->get_functionModifier().hasOpenclWorkGroupSizeReq())
             {
#if 0
               SgFunctionModifier::opencl_work_group_size_t opencl_work_group_size = functionDeclaration->get_functionModifier().get_opencl_work_group_size();
            // curprint( "__attribute__((work_group_size_hint(" << opencl_work_group_size.x << ", " << opencl_work_group_size.y << ", " << opencl_work_group_size.z << "))) ");
#endif
               curprint( "__attribute__((work_group_size_hint(X, Y, Z))) ");
             }

       // DQ (4/20/2015): Added support for GNU cdecl attribute.
#if 0
          printf ("functionDeclaration->get_declarationModifier().get_typeModifier().isGnuAttributeCdecl() = %s \n",
               functionDeclaration->get_declarationModifier().get_typeModifier().isGnuAttributeCdecl() ? "true" : "false");
#endif
          if (functionDeclaration->get_declarationModifier().get_typeModifier().isGnuAttributeCdecl() == true)
             {
#if 0
               printf ("Output GNU cdecl attribute: functionDeclaration = %p \n",functionDeclaration);
#endif
               curprint( "__attribute__((cdecl)) ");
             }
        }


     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(decl_stmt);
     if (variableDeclaration != NULL)
        {
       // DQ (8/1/2014): Added support to output the constexpr keyword.
          if (variableDeclaration->get_is_constexpr() == true)
             {
               curprint("constexpr ");
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

#if 0
     printf ("In printSpecifier2(): decl_stmt = %p decl_stmt->get_declarationModifier().get_storageModifier().isStatic()  = %s \n",decl_stmt,decl_stmt->get_declarationModifier().get_storageModifier().isStatic() ? "true" : "false");
     printf ("In printSpecifier2(): decl_stmt = %p decl_stmt->get_declarationModifier().get_storageModifier().isExtern()  = %s \n",decl_stmt,decl_stmt->get_declarationModifier().get_storageModifier().isExtern() ? "true" : "false");
     printf ("In printSpecifier2(): decl_stmt = %p decl_stmt->get_declarationModifier().get_storageModifier().isDefault() = %s \n",decl_stmt,decl_stmt->get_declarationModifier().get_storageModifier().isDefault() ? "true" : "false");
#endif

     if (decl_stmt->get_declarationModifier().get_storageModifier().isStatic())
        {
#if 1
          printf ("In Unparse_MOD_SAGE::printSpecifier2(): Output the static keyword \n");
#endif
          curprint("static ");
        }

  // if (unp->opt.get_extern_opt())
  // DQ (5/10/2007): Fixed linkage to be a std::string instead of char*
  // if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && !decl_stmt->get_linkage())
     if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && decl_stmt->get_linkage().empty() == true)
        {
#if 1
          printf ("In Unparse_MOD_SAGE::printSpecifier2(): Output the extern keyword \n");
#endif
       // DQ (7/23/2014): Looking for greater precision in the control of the output of the "extern" keyword.
          ROSE_ASSERT(decl_stmt->get_declarationModifier().get_storageModifier().isDefault() == false);

       // DQ (4/11/2019): Don't allow friend and extern together (see Cxx11_tests/test2019_338.C).
       // DQ (1/3/2016): We may have to suppress this for SgTemplateVariableDeclaration IR nodes.
       // curprint("extern ");
       // curprint("/* extern from storageModifier */ extern ");
       // if (isSgTemplateVariableDeclaration(decl_stmt) == NULL)
          if ( (decl_stmt->get_declarationModifier().isFriend() == false) && (isSgTemplateVariableDeclaration(decl_stmt) == NULL) )
             {
               curprint("extern ");
             }
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


#if 0
    align( # )
    allocate(" segname ")
    appdomain
    code_seg(" segname ")
    deprecated
    dllimport
    dllexport
    jitintrinsic
    naked
    noalias
    noinline
#endif

     if (decl_stmt->get_declarationModifier().is_ms_declspec_align())
        {
          curprint("__declspec(align(4)) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_allocate())
        {
          curprint("__declspec(allocate(\"unknown_allocate_name\")) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_appdomain())
        {
          curprint("__declspec(appdomain) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_code_seg())
        {
          curprint("__declspec(code_seg(\"unknown_code_seg_name\")) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_deprecated())
        {
          curprint("__declspec(deprecated) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_dllimport())
        {
          curprint("__declspec(dllimport) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_dllexport())
        {
          curprint("__declspec(dllexport) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_jitintrinsic())
        {
          curprint("__declspec(jitintrinsic) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_naked())
        {
          curprint("__declspec(naked) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_noalias())
        {
          curprint("__declspec(noalias) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_noinline())
        {
          curprint("__declspec(noinline) ");
        }

#if 0
    noreturn
    nothrow
    novtable
    process
    property( {get=get_func_name|,put=put_func_name})
    restrict
    safebuffers
    selectany
    thread
    uuid(" ComObjectGUID ")
#endif

     if (decl_stmt->get_declarationModifier().is_ms_declspec_noreturn())
        {
          curprint("__declspec(noreturn) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_nothrow())
        {
          curprint("__declspec(nothrow) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_novtable())
        {
          curprint("__declspec(novtable) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_process())
        {
          curprint("__declspec(process) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_property())
        {
       // curprint("__declspec(property( {get=get_func_name|,put=put_func_name})) ");
          curprint("__declspec(property(\"");

          string get_function_name         = decl_stmt->get_declarationModifier().get_microsoft_property_get_function_name();
          bool get_function_name_non_empty = get_function_name.empty();
          string put_function_name         = decl_stmt->get_declarationModifier().get_microsoft_property_put_function_name();
          bool put_function_name_non_empty = put_function_name.empty();

          if (get_function_name_non_empty == false)
             {
               curprint("get=");
               curprint(get_function_name);
             }

          if (get_function_name_non_empty == false && put_function_name_non_empty == false)
             {
               curprint(", ");
             }

          if (put_function_name_non_empty == false)
             {
               curprint("put=");
               curprint(put_function_name);
             }

          curprint("\")) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_restrict())
        {
          curprint("__declspec(restrict) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_safebuffers())
        {
          curprint("__declspec(safebuffers) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_selectany())
        {
          curprint("__declspec(selectany) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_thread())
        {
          curprint("__declspec(thread) ");
        }

     if (decl_stmt->get_declarationModifier().is_ms_declspec_uuid())
        {
       // curprint("__declspec(uuid(\" ComObjectGUID \")) ");
          curprint("__declspec(uuid(\"");
          curprint(decl_stmt->get_declarationModifier().get_microsoft_uuid_string());
          curprint("\")) ");
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


void
Unparse_MOD_SAGE::printAttributes(SgInitializedName* initializedName, SgUnparse_Info& info)
   {
  // DQ (2/26/2013): Added support for missing attributes in unparsed code.
  // These are output after the function declaration (and before the body of the function or the closing ";").

  // DQ (9/16/2013): FIXME: __section__, __cleanup__, __init_priority__ are not yet implemented.

#if 0
     printf ("In printAttributes(SgInitializedName*): Output the flags in the declarationModifier for decl_stmt = %p = %s = %s \n",initializedName,initializedName->class_name().c_str(),SageInterface::get_name(initializedName).c_str());
#endif

  // DQ (9/16/2013): Added support for more GNU attributes.
     if (initializedName->isGnuAttributeUsed() == true)
        {
          curprint( " __attribute__((used)) ");
        }
     
  // DQ (9/16/2013): Added support for more GNU attributes.
     if (initializedName->isGnuAttributeUnused() == true)
        {
          curprint( " __attribute__((unused)) ");
        }
     
  // DQ (9/16/2013): Added support for more GNU attributes.
     if (initializedName->isGnuAttributeWeak() == true)
        {
          curprint( " __attribute__((weak)) ");
        }
     
  // DQ (9/16/2013): Added support for more GNU attributes.
     if (initializedName->isGnuAttributeDeprecated() == true)
        {
          curprint( " __attribute__((deprecated)) ");
        }
     
  // DQ (9/16/2013): Added support for more GNU attributes.
     if (initializedName->isGnuAttributeNoCommon() == true)
        {
          curprint( " __attribute__((noCommon)) ");
        }
     
  // DQ (9/16/2013): Added support for more GNU attributes.
     if (initializedName->isGnuAttributeTransparentUnion() == true)
        {
          curprint( " __attribute__((transparent_union)) ");
        }
     
  // DQ (9/16/2013): Added support for more GNU attributes.
     if (initializedName->isGnuAttributeWeakReference() == true)
        {
          curprint( " __attribute__((weak_reference)) ");
        }

  // DQ (1/18/2014): Adding support for GNU specific noreturn attribute for variable 
  // (only applies to variable that are of function pointer type).
     if (initializedName->isGnuAttributeNoReturn() == true)
        {
          curprint(" __attribute__((noreturn)) ");
#if 0
          printf ("Detected initializedName->isGnuAttributeNoReturn() == true: (not implemented) \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
  // DQ (12/31/2013): This is now handled by the new printAttributesForType() function.
  // DQ (12/30/2013): Added support for more GNU attributes.
     if (initializedName->isGnuAttributePacked() == true)
        {
          curprint(" /* from printAttributes(SgInitializedName*) */ __attribute__((packed)) ");
        }
#endif

  // DQ (3/1/2013): The default value is changed from zero to -1 (and the type was make to be a short (signed) value).
     short alignmentValue = initializedName->get_gnu_attribute_alignment();

  // DQ (7/26/2014): Adding support for _Alignas keyword.
     bool using_Alignas_keyword = (initializedName->get_using_C11_Alignas_keyword() == true);

  // if (alignmentValue >= 0)
     if (alignmentValue >= 0 && using_Alignas_keyword == false)
        {
#if 0
          curprint(" /* alignment attribute on SgInitializedName */ ");
#endif
       // DQ (7/26/2014): Fixed error in using "align" (mistake), changed to "aligned".
       // curprint( " __attribute__((align(N)))");
       // curprint( " __attribute__((align(");
          curprint( " __attribute__((aligned(");
          curprint(StringUtility::numberToString((int)alignmentValue));
          curprint("))) ");
        }
   }


void
Unparse_MOD_SAGE::printAttributesForType(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info)
   {
  // DQ (12/31/2013): Added support for missing attributes on types within declarations (in unparsed code).

     ROSE_ASSERT(decl_stmt != NULL);

#if 0
     printf ("In printAttributesForType(SgDeclarationStatement*): Output the flags in the declarationModifier for decl_stmt = %p = %s = %s \n",decl_stmt,decl_stmt->class_name().c_str(),SageInterface::get_name(decl_stmt).c_str());
#endif

#if 0
     printf ("Exiting as a test of attribute(__noreturn__) \n");
     ROSE_ASSERT(false);
#endif

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(decl_stmt);
     if (variableDeclaration != NULL)
        {
       // DQ (12/18/2013): Added support for output of packed attribute (see test2013_104.c 
       // (required after variable) and test2013_113.c (required after type and before variable)).
          if (decl_stmt->get_declarationModifier().get_typeModifier().isGnuAttributePacked() == true)
             {
            // curprint(" /* from printAttributesForType(SgDeclarationStatement*) */ __attribute__((packed))");
               curprint(" __attribute__((packed))");
             }
        }

  // DQ (1/6/2014): Added support for specification of noreturn (function type) attribute.
  // This is one of two place where the attribute may be used (after the function declaration)
  // and after the function pointer function parameter in a function's parameter list.
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(decl_stmt);
     if (functionDeclaration != NULL)
        {
       // DQ (7/26/2014): Fixed for better handling of C11 _Noreturn keyword.
       // if (functionDeclaration->get_declarationModifier().get_typeModifier().isGnuAttributeNoReturn() == true)
          if (functionDeclaration->get_declarationModifier().get_typeModifier().isGnuAttributeNoReturn() == true && functionDeclaration->get_using_C11_Noreturn_keyword() == false)
             {
               curprint(" __attribute__((noreturn))");
             }

       // DQ (2/7/2014): attribute set using: decl->get_functionModifier().set_gnu_attribute_named_alias(alias_name);
          if (functionDeclaration->get_functionModifier().get_gnu_attribute_named_alias().empty() == false)
             {
               string alias = functionDeclaration->get_functionModifier().get_gnu_attribute_named_alias();
#if 0
               printf ("Detected alias attribute: alias = %s \n",alias.c_str());
#endif
            // curprint(" __attribute__((noreturn))");
               curprint(" __attribute__((alias(\"");
               curprint(alias);
               curprint("\")))");
             }
        }
   }


void
Unparse_MOD_SAGE::printAttributes(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info)
   {
  // DQ (2/26/2013): Added support for missing attributes in unparsed code.
  // These are output after the function declaration (and before the body of the function or the closing ";").

     ROSE_ASSERT(decl_stmt != NULL);

#if 0
     printf ("In printAttributes(SgDeclarationStatement*): Output the flags in the declarationModifier for decl_stmt = %p = %s = %s \n",decl_stmt,decl_stmt->class_name().c_str(),SageInterface::get_name(decl_stmt).c_str());
     curprint("\n/* START printAttributes(SgDeclarationStatement*) */\n ");
#endif

#if 0
     if (isSgVariableDeclaration(decl_stmt) != NULL)
        {
          printf ("In printAttributes(SgDeclarationStatement*): Output the flags in the declarationModifier for decl_stmt = %p = %s = %s \n",decl_stmt,decl_stmt->class_name().c_str(),SageInterface::get_name(decl_stmt).c_str());
          decl_stmt->get_declarationModifier().display("Unparse_MOD_SAGE::printAttributes(): declarationModifier");
        }
#endif

     if (decl_stmt->get_declarationModifier().isThrow() == true)
        {
       // DQ (2/26/2013): This is output as part of the function type unparsing (since it is a part of the type system).
       // curprint( " throw()");
        }

     short alignmentValue = decl_stmt->get_declarationModifier().get_typeModifier().get_gnu_attribute_alignment();

#if 0
     printf ("In printAttributes(SgDeclarationStatement*): alignmentValue = %d \n",(int)alignmentValue);
#endif

  // DQ (3/1/2013): The default value is changed from zero to -1 (and the type was make to be a short (signed) value).
     if (alignmentValue >= 0)
        {
#if 0
          curprint(" /* alignment attribute on decl_stmt->get_declarationModifier().get_typeModifier() */ ");
#endif
       // DQ (7/26/2014): Fixed error in using "align" (mistake), changed to "aligned".
       // curprint(" __attribute__((align(N)))");
       // curprint(" __attribute__((align(");
          curprint(" __attribute__((aligned(");
          curprint(StringUtility::numberToString((int)alignmentValue));
          curprint(")))");
        }

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(decl_stmt);
     if (variableDeclaration != NULL)
        {
       // DQ (12/18/2013): Added support for output of packed attribute (see test2013_104.c).
#if 0
          if (decl_stmt->get_declarationModifier().get_typeModifier().isGnuAttributePacked() == true)
             {
               curprint(" /* from printAttributes(SgDeclarationStatement*) */ __attribute__((packed))");
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#else
       // DQ (12/31/2013): Note that we need to look at the SgInitializedName in the variable declaration, since
       // we use the type modifier on the declaration to set the attributes for the type (not the variable).
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
          ROSE_ASSERT(initializedName != NULL);
          initializedName->isGnuAttributePacked();
          if (initializedName->isGnuAttributePacked() == true)
             {
            // curprint(" /* from printAttributes(SgDeclarationStatement*) triggered from SgInitializedName */ __attribute__((packed))");
               curprint(" __attribute__((packed)) ");
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#endif
        }

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(decl_stmt);
     if (functionDeclaration != NULL)
        {
#if 0
          printf ("Output the flags in the different modifiers for function = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
          functionDeclaration->get_functionModifier().display("Unparse_MOD_SAGE::printAttributes(): functionModifier");
#endif
#if 0
       // DQ (1/3/2009): Added GNU specific attributes
          bool isGnuAttributeConstructor() const;
          bool isGnuAttributeDestructor() const;
          bool isGnuAttributePure() const;
          bool isGnuAttributeWeak() const;
          bool isGnuAttributeUnused() const;
          bool isGnuAttributeUsed() const;
          bool isGnuAttributeDeprecated() const;
          bool isGnuAttributeMalloc() const;
          bool isGnuAttributeNaked() const;
          bool isGnuAttributeNoInstrumentFunction() const;
          bool isGnuAttributeNoCheckMemoryUsage() const;
          bool isGnuAttributeNoInline() const;
          bool isGnuAttributeAlwaysInline() const;
          bool isGnuAttributeNoThrow() const;
          bool isGnuAttributeWeakReference() const;
#endif

       // DQ (2/26/2013): Added noinline attribute code generation.
          if (functionDeclaration->get_functionModifier().isGnuAttributeConstructor() == true)
             {
               curprint( " __attribute__((constructor)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeDestructor() == true)
             {
               curprint( " __attribute__((destructor)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributePure() == true)
             {
               curprint( " __attribute__((pure)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeWeak() == true)
             {
               curprint( " __attribute__((weak)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeUnused() == true)
             {
               curprint( " __attribute__((unused)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeUsed() == true)
             {
               curprint( " __attribute__((used)) ");
             }
#if 0
          printf ("In printAttributes(SgDeclarationStatement*): functionDeclaration->get_functionModifier().isGnuAttributeDeprecated() = %s \n",functionDeclaration->get_functionModifier().isGnuAttributeDeprecated() ? "true" : "false");
#endif
          if (functionDeclaration->get_functionModifier().isGnuAttributeDeprecated() == true)
             {
               curprint( " __attribute__((deprecated)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeMalloc() == true)
             {
               curprint( " __attribute__((malloc)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeNaked() == true)
             {
               curprint( " __attribute__((naked)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeNoInstrumentFunction() == true)
             {
               curprint( " __attribute__((no_instrument_function)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeNoCheckMemoryUsage() == true)
             {
               curprint( " __attribute__((no_check_memory_usage)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeNoInline() == true)
             {
               curprint( " __attribute__((noinline)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeAlwaysInline() == true)
             {
               curprint( " __attribute__((always_inline)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeNoThrow() == true)
             {
               curprint( " __attribute__((no_throw)) ");
             }

          if (functionDeclaration->get_functionModifier().isGnuAttributeWeakReference() == true)
             {
               curprint( " __attribute__((weakref)) ");
             }

       // DQ (1/5/2014): Adding support for gnu visibility attributes.
       // Set using: decl->get_declarationModifier().set_gnu_attribute_visability(get_ELF_visibility_attribute(rout->ELF_visibility, &rout->source_corresp));
          SgDeclarationModifier::gnu_declaration_visability_enum visibility = functionDeclaration->get_declarationModifier().get_gnu_attribute_visability();
          if (visibility != SgDeclarationModifier::e_unspecified_visibility)
             {
#if 0
               printf ("In printAttributes(SgDeclarationStatement*): gnu visibility attribute was specified \n");
#endif
               string s;
               switch (visibility)
                  {
                 // DQ (1/10/2014): This appears to be a common setting, but it is an error in later versions of gcc to output this specification.
                    case SgDeclarationModifier::e_unknown_visibility: s = "(\"unknown\")";
                       {
#if 0
                         printf ("In printAttributes(SgDeclarationStatement*): gnu visibility attribute was specified: unknown visibility setting (supressed) \n");
#endif
                      // ROSE_ASSERT(false);
                         break;
                       }

                    case SgDeclarationModifier::e_error_visibility:   s = "(\"error\")";
                       {
                         printf ("In printAttributes(SgDeclarationStatement*): gnu visibility attribute was specified: error visibility setting (supressed) \n");
                      // ROSE_ASSERT(false);
                         break;
                       }

                    case SgDeclarationModifier::e_unspecified_visibility: s = "(\"xxx\")"; break;
                       {
                         printf ("unspecified visibility (trapped) (supressed) \n");
                         ROSE_ASSERT(false);
                         break;
                       }

                    case SgDeclarationModifier::e_hidden_visibility:      s = "(\"hidden\")";    break;
                    case SgDeclarationModifier::e_protected_visibility:   s = "(\"protected\")"; break;
                    case SgDeclarationModifier::e_internal_visibility:    s = "(\"internal\")";  break;

                    case SgDeclarationModifier::e_default_visibility:     s = "(\"default\")";   break;

                    default:
                         printf ("ERROR: In printAttributes(SgDeclarationStatement*): Bad visibility specification: visibility = %d \n", visibility);
                         ROSE_ASSERT(false);
                  }

            // DQ (1/10/2014): Note that later versions of gcc will report use of "unknown" and "error" as an error.
               if (visibility != SgDeclarationModifier::e_unknown_visibility && 
                   visibility != SgDeclarationModifier::e_error_visibility   && 
                   visibility != SgDeclarationModifier::e_unspecified_visibility)
                  {
                 // curprint(" __attribute__((visibility%s))",s.c_str());
                    curprint(" __attribute__((visibility");
                    curprint(s);
                    curprint(")) ");
                  }
             }
#if 0
#if 0
          printf ("In printAttributes(SgDeclarationStatement*): Look for the gnu_regnum_attribute and process it if it is non-zero: gnu_regnum_attribute = %d \n",functionDeclaration->get_gnu_regnum_attribute());
#endif
       // DQ (1/19/2014): Adding support for gnu attribute regnum to support use in Valgrind application.
          int gnu_regnum_value = functionDeclaration->get_gnu_regnum_attribute();
          if (gnu_regnum_value > 0)
             {
               string s = StringUtility::numberToString(gnu_regnum_value);
               curprint(" __attribute__((regnum(");
               curprint(s);
               curprint(")))");
             }
#endif
        }

#if 0
     printf ("Leaving printAttributes(SgDeclarationStatement*): Output the flags in the declarationModifier for decl_stmt = %p = %s = %s \n",decl_stmt,decl_stmt->class_name().c_str(),SageInterface::get_name(decl_stmt).c_str());
     curprint("\n/* END printAttributes(SgDeclarationStatement*) */\n ");
#endif
   }


void
Unparse_MOD_SAGE::printPrefixAttributes(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info)
   {
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(decl_stmt);
     if (functionDeclaration != NULL)
        {
       // DQ (1/19/2014): Add support for prefix attributes.
#if 0
          printf ("In printPrefixAttributes(SgDeclarationStatement*): function = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
          printf ("In printPrefixAttributes(SgDeclarationStatement*): Look for the gnu_regparm_attribute and process it if it is non-zero: gnu_regparm_attribute = %d \n",functionDeclaration->get_gnu_regparm_attribute());
#endif
       // DQ (1/19/2014): Adding support for gnu attribute regnum to support use in Valgrind application.
          int gnu_regparm_value = functionDeclaration->get_gnu_regparm_attribute();

       // DQ (5/27/2015): Note that zero is a ligitimate value to use, so the default should be -1.
       // if (gnu_regparm_value > 0)
          if (gnu_regparm_value >= 0)
             {
               string s = StringUtility::numberToString(gnu_regparm_value);
#if 0
               printf ("Output __attribute__((regparm(%s))) for function = %p = %s = %s \n",s.c_str(),functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
#endif
               curprint(" __attribute__((regparm(");
               curprint(s);

            // Add trailing space since this is for a prefixed attribute.
               curprint("))) ");
             }
        }
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

// MS: to activate this function set Unparse_MOD_SAGE::experimentalMode=true
void Unparse_MOD_SAGE::outputTemplateSpecializationSpecifier2 ( SgDeclarationStatement* decl_stmt ) 
   {
     if (isSgTemplateInstantiationDecl(decl_stmt)
      // DQ (1/3/2015): Added support for template variables (instantiations are represented similarly to non-instantiations (but we might have to fix this).
         || isSgTemplateVariableDeclaration(decl_stmt)

         || isSgTemplateInstantiationFunctionDecl(decl_stmt)
         || isSgTemplateInstantiationMemberFunctionDecl(decl_stmt)) 
        {
          if (isSgTemplateInstantiationDirectiveStatement(decl_stmt->get_parent())) 
             {
               if(experimentalModeVerbose==1) curprint("/*0*/");
               curprint("template ");
             } 
            else 
               if (isSgTemplateInstantiationDecl(decl_stmt)) 
                  {
                    if(experimentalModeVerbose==1) curprint("/*1*/");
                    curprint("template<> ");
                  } 
                 else 
                    if (isSgTemplateInstantiationDefn(decl_stmt->get_parent())) 
                       {
                         if(experimentalModeVerbose==1) curprint("/*2*/");
                       } 
                      else 
                         if (isSgTemplateInstantiationMemberFunctionDecl(decl_stmt)) 
                            {
                              if(experimentalModeVerbose==1) curprint("/*3*/");
                            } 
                           else 
                            {
                           // DQ (1/3/2015): Added support for template variables (instantiations are represented similarly to non-instantiations (but we might have to fix this).
                              if (isSgTemplateVariableDeclaration(decl_stmt)) 
                                 {
#if 0
                                   if(experimentalModeVerbose==1) curprint("/*4*/");
                                   curprint("template<> ");
#endif
                                 }
                                else
                                 { 
                                   cerr<<"WARNING: Unknown template construct: "<<decl_stmt->class_name()<<endl;
                                // ROSE_ASSERT(0);
                                 }
                            } 
        }
   }
