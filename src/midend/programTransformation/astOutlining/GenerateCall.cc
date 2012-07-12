/**
 *  \file Transform/GenerateCall.cc
 *
 *  \brief Given the outlined-function, this routine generates the
 *  actual function call.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include <iostream>
#include <string>
#include <sstream>


#include "Outliner.hh"
#include "ASTtools.hh"
#include "VarSym.hh"
#include "StmtRewrite.hh"
#include "Outliner.hh"

// =====================================================================

using namespace std;

// =====================================================================

//! Convert the given set of variable symbols into function call arguments.
// This is used for generating the function call to the outlined function.
static
void
appendArgs (const ASTtools::VarSymSet_t& syms,  std::set<SgInitializedName*> readOnlyVars, std::string arg_name, SgExprListExp* e_list, SgScopeStatement* scope)
{
  if (!e_list)
    return;
  if ((Outliner::useParameterWrapper|| Outliner::useStructureWrapper) && (syms.size()>0))  
  { 
    ROSE_ASSERT(scope!=NULL);
    if (Outliner::useStructureWrapper)
    {
      //using &_out_argv as a wrapper  
      SageInterface::appendExpression(e_list,SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(arg_name ,scope)));
    }
    else  
    {
     // using void * __out_argv[n] as a wrapper
      SageInterface::appendExpression(e_list,SageBuilder::buildVarRefExp(arg_name ,scope));
    }

    return; 
  }
  else if (Outliner::useStructureWrapper && (syms.size() ==0))
  {
    //For OpenMP lowering, we have to have a void * parameter even if there is no need to pass any parameters 
    //in order to match the gomp runtime lib 's function prototype for function pointers
    SgFile* cur_file = SageInterface::getEnclosingFileNode(scope);
    ROSE_ASSERT (cur_file != NULL);
    if (cur_file->get_openmp_lowering ())
    {
      SageInterface::appendExpression(e_list, SageBuilder::buildIntVal(0));
    }
  }
  else // no parameter wrapping, a parameter for each variable
  {
    for (ASTtools::VarSymSet_t::const_iterator i = syms.begin ();
        i != syms.end (); ++i)
    {
      bool readOnly =false;
      SgInitializedName* iname = (*i)->get_declaration();
      if (iname)
        if (readOnlyVars.find(iname)!=readOnlyVars.end())
          readOnly = true;

      // Create variable reference to pass to the function.
      SgVarRefExp* v_ref = SageBuilder::buildVarRefExp (const_cast<SgVariableSymbol *> (*i));
      ROSE_ASSERT (v_ref);
      // Liao, 12/14/2007  Pass by reference is default behavior for Fortran
      if (SageInterface::is_Fortran_language())
        e_list->append_expression(v_ref);
      else 
      {
        // Construct actual function argument.
        SgExpression* i_arg=NULL;
        if (Outliner::enable_classic && readOnly)
        { // classic translation, read only variable, pass by value directly
          i_arg = v_ref;
        }
        else
        {
          SgType* i_arg_type = SgPointerType::createType (v_ref->get_type ());
          ROSE_ASSERT (i_arg_type);
          i_arg =  SageBuilder::buildAddressOfOp (v_ref);// % i_arg_type);
        }
        ROSE_ASSERT (i_arg);

        e_list->append_expression (i_arg);
      } //end if

    }
  }
}

// =====================================================================

SgStatement *
Outliner::generateCall (SgFunctionDeclaration* out_func,
                                      const ASTtools::VarSymSet_t& syms, 
                                       std::set<SgInitializedName*>  readOnlyVars, 
                                      std::string wrapper_name, 
                                      SgScopeStatement* scope)
{
  // Create a reference to the function.
  SgGlobal* glob_scope = TransformationSupport::getGlobalScope(scope);
  ROSE_ASSERT(glob_scope != NULL);
  SgFunctionSymbol* func_symbol = glob_scope->lookup_function_symbol(out_func->get_name());
  if (func_symbol == NULL)
  {
    printf("Failed to find a function symbol in %p for function %s\n", glob_scope, out_func->get_name().getString().c_str());
    ROSE_ASSERT(func_symbol != NULL);
  }
  ROSE_ASSERT (func_symbol);
//  SgFunctionRefExp* func_ref_exp = SageBuilder::buildFunctionRefExp(func_symbol);
//    new SgFunctionRefExp (ASTtools::newFileInfo (),
//        func_symbol, out_func->get_type ());
//  ROSE_ASSERT (func_ref_exp);

  // Create an argument list.
  SgExprListExp* exp_list_exp = SageBuilder::buildExprListExp();
  ROSE_ASSERT (exp_list_exp);
  appendArgs (syms, readOnlyVars, wrapper_name, exp_list_exp,scope);

  // Generate the actual call.
  SgFunctionCallExp* func_call_expr = SageBuilder::buildFunctionCallExp(func_symbol,exp_list_exp);
//    new SgFunctionCallExp (ASTtools::newFileInfo (),
//        func_ref_exp,
//        exp_list_exp,
//        out_func->get_type ());
  ROSE_ASSERT (func_call_expr);

  SgExprStatement *func_call_stmt = SageBuilder::buildExprStatement (func_call_expr);

  ROSE_ASSERT (func_call_stmt);

  return func_call_stmt;
}

// eof
