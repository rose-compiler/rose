/**
 *  \file Transform/GenerateCall.cc
 *
 *  \brief Given the outlined-function, this routine generates the
 *  actual function call.
 */

#include <iostream>
#include <string>
#include <sstream>

#include <rose.h>
#include "Transform.hh"
#include "ASTtools.hh"
#include "VarSym.hh"
#include "StmtRewrite.hh"
#include "Outliner.hh"

// =====================================================================

using namespace std;

// =====================================================================

//! Convert the given set of variable symbols into function call arguments.
static
void
appendArgs (const ASTtools::VarSymSet_t& syms, std::string arg_name, SgExprListExp* e_list, SgScopeStatement* scope)
{
  if (!e_list)
    return;
  if (Outliner::useParameterWrapper && (syms.size()>0))  // using void * __out_argv[n] as a wrapper
  { 
    ROSE_ASSERT(scope!=NULL);
    SageInterface::appendExpression(e_list,SageBuilder::buildVarRefExp(arg_name ,scope));
   return; 
  }
  else 
  {
    for (ASTtools::VarSymSet_t::const_iterator i = syms.begin ();
        i != syms.end (); ++i)
    {
      // Create variable reference to pass to the function.
      SgVarRefExp* v_ref = new SgVarRefExp (ASTtools::newFileInfo (),
          const_cast<SgVariableSymbol *> (*i));
      ROSE_ASSERT (v_ref);
      // Liao, 12/14/2007  Pass by reference is default behavior for Fortran
      if (SageInterface::is_Fortran_language())
        e_list->append_expression(v_ref);
      else {
        // Construct actual function argument.
        SgType* i_arg_type = SgPointerType::createType (v_ref->get_type ());
        ROSE_ASSERT (i_arg_type);
        SgExpression* i_arg = new SgAddressOfOp (ASTtools::newFileInfo (),
            v_ref, i_arg_type);
        ROSE_ASSERT (i_arg);
        e_list->append_expression (i_arg);
      } //end if

    }
  }
}

// =====================================================================

SgStatement *
Outliner::Transform::generateCall (SgFunctionDeclaration* out_func,
                                      const ASTtools::VarSymSet_t& syms, std::string wrapper_name, SgScopeStatement* scope)
{
  // Create a reference to the function.
  SgFunctionSymbol* func_symbol = new SgFunctionSymbol (out_func);
  ROSE_ASSERT (func_symbol);
  SgFunctionRefExp* func_ref_exp =
    new SgFunctionRefExp (ASTtools::newFileInfo (),
                          func_symbol, out_func->get_type ());
  ROSE_ASSERT (func_ref_exp);

  // Create an argument list.
  SgExprListExp* exp_list_exp = new SgExprListExp (ASTtools::newFileInfo ());
  ROSE_ASSERT (exp_list_exp);
  appendArgs (syms, wrapper_name, exp_list_exp,scope);

  // Generate the actual call.
  SgFunctionCallExp* func_call_expr =
    new SgFunctionCallExp (ASTtools::newFileInfo (),
                           func_ref_exp,
                           exp_list_exp,
                           out_func->get_type ());
  ROSE_ASSERT (func_call_expr);

  SgExprStatement *func_call_stmt = new SgExprStatement (ASTtools::newFileInfo (), func_call_expr);

  ROSE_ASSERT (func_call_stmt);

  return func_call_stmt;
}

// eof
