/* modified_sage.C
 *
 * This C file includes functions that test for operator overloaded functions and 
 * helper unparse functions such as unparse_helper and printSpecifier.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryOperatorPlus
//  
//  Auxiliary function to test if this expression is an unary operator+ overloading 
//  function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryOperatorPlus(SgExpression* expr)
   {
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
                    if (func_name.getString() == "operator+")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 0) 
                              return true;
                       }
                  }
             }
        }

    // DQ (5/6/2007): Added to catch case of non-member function unary operator
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
                         if (func_name.getString() == "operator+")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 1) 
                                   return true;
                            }
                       }
                  }
             }
        }

     return false;
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryOperatorMinus
//  
//  Auxiliary function to test if this expression is an unary operator- overloading 
//  function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryOperatorMinus(SgExpression* expr)
   {
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
                    if (func_name.getString() == "operator-")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 0)
                              return true;
                       }
                  }
             }
        }
    // DQ (5/6/2007): Added to catch case of non-member function unary operator
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
                         if (func_name.getString() == "operator-")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 1) 
                                   return true;
                            }
                       }
                  }
             }
        }

     return false;
   }



//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryAddressOperator
//  
//  Auxiliary function to test if this expression is an unary operator& overloading 
//  function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryAddressOperator(SgExpression* expr)
   {
  // DQ (5/6/2007): This might be a non-member function and if so we don't handle this case correctly!
  // If it is a non-member function this it will have a single argument
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);
     ASSERT_not_null(expr);

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
#if 0
                    printf ("In isUnaryAddressOperator(): member function: func_name = %s \n",func_name.str());
#endif
                    if (func_name.getString() == "operator&")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 0)
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
#if 0
                         printf ("In isUnaryAddressOperator(): non-member function: func_name = %s \n",func_name.str());
#endif
                         if (func_name.getString() == "operator&")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 1) 
                                   return true;
                            }
                       }
                  }
             }
        }

     return false;
   }


//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryDereferenceOperator
//  
//  Auxiliary function to test if this expression is an unary operator* overloading 
//  function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryDereferenceOperator(SgExpression* expr)
   {
  // DQ (5/6/2007): This might be a non-member function and if so we don't handle this case correctly!
  // If it is a non-member function this it will have a single argument
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);
     ASSERT_not_null(expr);

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
                    if (func_name.getString() == "operator*")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 0)
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
                         if (func_name.getString() == "operator*")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 1) 
                                   return true;
                            }
                       }
                  }
             }
        }

     return false;
   }


//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryComplementOperator
//  
//  Auxiliary function to test if this expression is an unary operator* overloading 
//  function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryComplementOperator(SgExpression* expr)
   {
  // DQ (5/6/2007): This might be a non-member function and if so we don't handle this case correctly!
  // If it is a non-member function this it will have a single argument
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);
     ASSERT_not_null(expr);

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
                    if (func_name.getString() == "operator~")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 0)
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
                         if (func_name.getString() == "operator~")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 1) 
                                   return true;
                            }
                       }
                  }
             }
        }

     return false;
   }


//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryNotOperator
//  
//  Auxiliary function to test if this expression is an unary operator! overloading 
//  function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryNotOperator(SgExpression* expr)
   {
  // DQ (5/6/2007): This might be a non-member function and if so we don't handle this case correctly!
  // If it is a non-member function this it will have a single argument
  // ROSE_ASSERT(isSgFunctionRefExp(expr) == NULL);
     ASSERT_not_null(expr);

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
                    if (func_name.getString() == "operator!")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 0)
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
                         if (func_name.getString() == "operator!")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 1) 
                                   return true;
                            }
                       }
                  }
             }
        }

     return false;
   }


#if 0
// DQ (2/1/2018): I don't think this operator can exist.

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::isUnaryOrOperator
//  
//  Auxiliary function to test if this expression is an unary operator| overloading 
//  function
//-----------------------------------------------------------------------------------
bool
Unparse_MOD_SAGE::isUnaryOrOperator(SgExpression* expr)
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
                    if (func_name.getString() == "operator|")
                       {
                         SgInitializedNamePtrList argList = mfunc_decl->get_args();
                         if (argList.size() == 0)
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
                         if (func_name.getString() == "operator|")
                            {
                              SgInitializedNamePtrList argList = func_decl->get_args();
                              if (argList.size() == 1) 
                                   return true;
                            }
                       }
                  }
             }
        }

     return false;
   }
#endif

