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


//Generate a parameter list for a function call: 
// The caller of this function should analyze (figure out) exactly which form to use .
// This function only takes care of the actual transformation.
//
//const ASTtools::VarSymSet_t& syms: a set of variables to be used in the parameter list
//   each input variable in the syms will be converted to a function parameter either using its original form (a) or addressOf form (&a)
//std::set<SgInitializedName*> varUsingOriginalType: 
//            indicate if some of the syms should using original types:  passed by value (original type) for scalar, in C/C++;  arrays for 
//            syms - varUsingOriginalType==> the rest will be variables using address of (&A): passed-by-reference: original type for arrays  or address of for others
void Outliner::appendIndividualFunctionCallArgs (const ASTtools::VarSymSet_t& syms,  const std::set<SgInitializedName*> varUsingOriginalType, SgExprListExp* e_list)
{
  for (ASTtools::VarSymSet_t::const_iterator i = syms.begin ();
      i != syms.end (); ++i)
  {
    bool using_orig_type=false;
    SgInitializedName* iname = (*i)->get_declaration();
    if (iname)
      if (varUsingOriginalType.find(iname)!=varUsingOriginalType.end())
        using_orig_type = true;

    // Create variable reference to pass to the function.
    SgVarRefExp* v_ref = SageBuilder::buildVarRefExp (const_cast<SgVariableSymbol *> (*i));
    ROSE_ASSERT (v_ref);
    // Liao, 12/14/2007  Pass by reference is default behavior for Fortran
    if (SageInterface::is_Fortran_language())
      e_list->append_expression(v_ref);
    else  // C/C++ call convention
    {
      // Construct actual function argument. //TODO: consider array types, they can only be passed by reference, no further addressing/de-referencing is needed
      SgExpression* i_arg=NULL;
      if (Outliner::enable_classic && using_orig_type ) // TODO expand to the default case also: using local declaration for transfer parameters
//      if (using_orig_type ) // using a 
      { // classic translation, read only variable, pass by value directly
        i_arg = v_ref;
      }
      else // conservatively always use &a for the default case (no wrapper, none classic)
      {
        i_arg =  SageBuilder::buildAddressOfOp (v_ref);
      }
      ROSE_ASSERT (i_arg);
      e_list->append_expression (i_arg);
    } //end if
  } // end for
}

// Append a single wrapper argument for a call to the outlined function
//
// ASTtools::VarSymSet_t& syms: original list of variables to be passed to the outlined function
//  std::string arg_name: name for the wrapper argument enclosing all syms
//  SgExprListExp* e_list: parameter list to be expanded
// SgScopeStatement* scope : scope of the function call to be inserted into
static
  void
appendSingleWrapperArgument(const ASTtools::VarSymSet_t& syms,  std::string arg_name, SgExprListExp* e_list, SgScopeStatement* scope)
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
    else // using array of pointers wrapper 
    {
      // using void * __out_argv[n] as a wrapper
      SageInterface::appendExpression(e_list,SageBuilder::buildVarRefExp(arg_name ,scope));
    }
  }
  else if ((Outliner::useStructureWrapper|| Outliner::useParameterWrapper) && (syms.size() ==0))
  {
    // TODO: move this outside of outliner since it is OpenMP-specific
    //For OpenMP lowering, we have to have a void * parameter even if there is no need to pass any parameters 
    //in order to match the gomp runtime lib 's function prototype for function pointers
    SgFile* cur_file = SageInterface::getEnclosingFileNode(scope);
    ROSE_ASSERT (cur_file != NULL);
    //if (cur_file->get_openmp_lowering ())
    {
      SageInterface::appendExpression(e_list, SageBuilder::buildIntVal(0));
    }
  }
#if 0  
  else
  {
    cerr<<"Error. unreachable branch reached in GenerateCall.cc: appendSingleWrapperArgument()"<<endl;
    ROSE_ASSERT (false);  
  }
#endif  
}



#if 0
  //! Convert the given set of variable symbols into function call arguments.
  // This is used for generating the function call to the outlined function.
  // syms: all variables to be passed into/out the outlined function
  // varsUsingOriginalType: only used in C/C++ outlining without any wrapper parameter: decide if a parameter should use original form (a) or addressOf form (&a)
  //
  // We have two major cases
  // Using parameter wrapping: useParameterWrapper (array) vs. useStructureWrapper
  // Without using parameter wrapping: each variable is an individual function argument
static
  void
appendArgs (const ASTtools::VarSymSet_t& syms,  std::set<SgInitializedName*> varsUsingOriginalType, std::string arg_name, SgExprListExp* e_list, SgScopeStatement* scope)
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
    // TODO: move this outside of outliner since it is OpenMP-specific
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
    appendIndividualFunctionCallArgs (syms,varsUsingOriginalType,e_list);
  }
}

#endif
// =====================================================================

//! Generate a call to the outlined function
//  We have two ways to generate the corresponding parameter lists
//  Choice 1: each variable is converted to a function parameter. 
//      varsUsingOriginalForm: is used to decide if  original form (a) should be used, the rest should use addressOf form (&a)
//      wrapper_name: is irrelevant in this case
//  Choice 2: all variables are wrapped into a single parameter
//     wrapper_name: is the name of the wrapper parameter
//     varsUsingOriginalForm: is irrelevant in this choice
SgStatement *
Outliner::generateCall (SgFunctionDeclaration* out_func, // the outlined function we want to call
                        const ASTtools::VarSymSet_t& syms, // variables for generating function arguments
                        const std::set<SgInitializedName*>  varsUsingOriginalForm,  // used to the classic outlining without wrapper: using a (originalForm) vs. &a
                        std::string wrapper_name,  // when parameter wrapping is used, provide wrapper argument's name
                        SgScopeStatement* scope) // the scope in which we insert the function call
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

  // Create an argument list.
  SgExprListExp* exp_list_exp = SageBuilder::buildExprListExp();
  ROSE_ASSERT (exp_list_exp);
//appendArgs (syms, readOnlyVars, wrapper_name, exp_list_exp,scope);
  if (Outliner::useParameterWrapper|| Outliner::useStructureWrapper)
    appendSingleWrapperArgument (syms, wrapper_name,exp_list_exp,scope);
  else
    appendIndividualFunctionCallArgs (syms, varsUsingOriginalForm, exp_list_exp);

  // Generate the actual call.
  SgFunctionCallExp* func_call_expr = SageBuilder::buildFunctionCallExp(func_symbol,exp_list_exp);
  ROSE_ASSERT (func_call_expr);

  SgExprStatement *func_call_stmt = SageBuilder::buildExprStatement (func_call_expr);
  ROSE_ASSERT (func_call_stmt);

  return func_call_stmt;
}

// eof
