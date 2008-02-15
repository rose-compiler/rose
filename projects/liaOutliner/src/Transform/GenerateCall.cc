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
#include "Transform/Transform.hh"
#include "ASTtools/ASTtools.hh"
#include "ASTtools/VarSym.hh"
#include "ASTtools/StmtRewrite.hh"

// =====================================================================

using namespace std;

// =====================================================================

//! Convert the given set of variable symbols into function call arguments.
static
void
appendArgs (const ASTtools::VarSymSet_t& syms, SgExprListExp* e_list)
{
  if (!e_list)
    return;
  for (ASTtools::VarSymSet_t::const_iterator i = syms.begin ();
       i != syms.end (); ++i)
    {
      // Create variable reference to pass to the function.
      SgVarRefExp* v_ref = new SgVarRefExp (ASTtools::newFileInfo (),
                                            const_cast<SgVariableSymbol *> (*i));
      ROSE_ASSERT (v_ref);

      // Construct actual function argument.
      SgType* i_arg_type = SgPointerType::createType (v_ref->get_type ());
      ROSE_ASSERT (i_arg_type);
      SgExpression* i_arg = new SgAddressOfOp (ASTtools::newFileInfo (),
                                               v_ref, i_arg_type);
      ROSE_ASSERT (i_arg);

      e_list->append_expression (i_arg);
    }
}

// =====================================================================

SgStatement *
LiaOutliner::Transform::generateCall (SgFunctionDeclaration* out_func,
                                      const ASTtools::VarSymSet_t& syms)
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
  appendArgs (syms, exp_list_exp);

  // Generate the actual call.
  SgFunctionCallExp* func_call_expr =
    new SgFunctionCallExp (ASTtools::newFileInfo (),
                           func_ref_exp,
                           exp_list_exp,
                           out_func->get_type ());
  ROSE_ASSERT (func_call_expr);

#if 0
// DQ (9/7/2007): SgExpressionRoot is no longer used in the AST
  SgExpressionRoot* func_call_root =
    new SgExpressionRoot (ASTtools::newFileInfo (),
                          func_call_expr,
                          out_func->get_type ());
  ROSE_ASSERT (func_call_root);
#endif

// SgExprStatement *func_call_stmt = new SgExprStatement (ASTtools::newFileInfo (), func_call_root);
   SgExprStatement *func_call_stmt = new SgExprStatement (ASTtools::newFileInfo (), func_call_expr);

  ROSE_ASSERT (func_call_stmt);

  return func_call_stmt;
}

// eof
