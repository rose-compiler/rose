/**
 *  \file Transform/Transform.cc
 *
 *  \brief Implements the outlining transformation.
 */

#include <iostream>
#include <list>
#include <string>
#include <rose.h>

#include "Transform.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include "StmtRewrite.hh"

// =====================================================================

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;
// =====================================================================

Outliner::Result
Outliner::Transform::outlineBlock (SgBasicBlock* s,
                                      const string& func_name_str)
{
  // Save some preprocessing information.
  AttachedPreprocessingInfoType ppi_before, ppi_after;
  ASTtools::cutPreprocInfo (s, PreprocessingInfo::before, ppi_before);
  ASTtools::cutPreprocInfo (s, PreprocessingInfo::after, ppi_after);

  // Determine variables to be passed to outlined routine.
  ASTtools::VarSymSet_t syms;
  collectVars (s, syms);

  SgScopeStatement * p_scope = s->get_scope();
  ROSE_ASSERT(p_scope);
  // Generate outlined function.
//  printf ("In Outliner::Transform::outlineBlock() function name to build: func_name_str = %s \n",func_name_str.c_str());
  SgFunctionDeclaration* func = generateFunction (s, func_name_str, syms);
  ROSE_ASSERT (func != NULL);

  // Insert outlined function.
  SgGlobal* glob_scope =
    const_cast<SgGlobal *> (TransformationSupport::getGlobalScope (s));
  SgFunctionDeclaration* func_orig =
    const_cast<SgFunctionDeclaration *> (SageInterface::getEnclosingFunctionDeclaration (s));
  insert (func, glob_scope, func_orig);
// DQ (9/7/2007): Need to add function symbol to global scope!
//   printf ("Fixing up the symbol table in scope = %p = %s for function = %p = %s \n",glob_scope,glob_scope->class_name().c_str(),func,func->get_name().str());
   SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(func);
   glob_scope->insert_symbol(func->get_name(),functionSymbol);
   ROSE_ASSERT(glob_scope->lookup_function_symbol(func->get_name()) != NULL);

   ROSE_ASSERT(func->get_definingDeclaration()         != NULL);
   ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);

  //Generate packing statements
  std::string wrapper_name;
  if (useParameterWrapper)
    wrapper_name= generatePackingStatements(s,syms);

  // Generate a call to the outlined function.
  SgStatement *func_call = generateCall (func, syms, wrapper_name,p_scope);
  ROSE_ASSERT (func_call != NULL);
  
  ASTtools::replaceStatement (s, func_call);

  // Restore preprocessing information.
  ASTtools::moveInsidePreprocInfo (s, func->get_definition ()->get_body ());
  ASTtools::pastePreprocInfoFront (ppi_before, func_call);
  ASTtools::pastePreprocInfoBack (ppi_after, func_call);

  SageInterface::fixVariableReferences(p_scope);
  return Result (func, func_call);
}

/* For a set of variables to be passed into the outlined function, 
 * generate the following statements before the call of the outlined function
 * used when useParameterWrapper is set to true
   	 void * __out_argv[2];
 	*(__out_argv +0)=(void*)(&var1);
  	*(__out_argv +1)=(void*)(&var2);
 */
std::string Outliner::Transform::generatePackingStatements(SgStatement* target, ASTtools::VarSymSet_t & syms)
{
  int var_count = syms.size();
  int counter=0;
  string wrapper_name= generateFuncArgName(target); //"__out_argv";

  if (var_count==0) return wrapper_name;
  SgScopeStatement* cur_scope = target->get_scope();
  ROSE_ASSERT( cur_scope != NULL);

  // void * __out_argv[count];
  SgType* pointer_type = buildPointerType(buildVoidType()); 
  SgType* my_type = buildArrayType(pointer_type, buildIntVal(var_count));
  SgVariableDeclaration* out_argv = buildVariableDeclaration(wrapper_name, my_type, NULL,cur_scope);

  SageInterface::insertStatementBefore(target, out_argv);

  SgVariableSymbol * wrapper_symbol = getFirstVarSym(out_argv);
  //cout<<"Inserting wrapper declaration ...."<<endl;
  // 	*(__out_argv +0)=(void*)(&var1);
  for (ASTtools::VarSymSet_t::reverse_iterator i = syms.rbegin ();
      i != syms.rend (); ++i)
  {
    SgAddOp * addop = buildAddOp(buildVarRefExp(wrapper_symbol),buildIntVal(counter));
    SgPointerDerefExp *lhs = buildPointerDerefExp(addop);

    SgVarRefExp* rhsvar = buildVarRefExp((*i)->get_declaration(),cur_scope);
    SgCastExp * rhs = buildCastExp( \
        buildAddressOfOp(rhsvar), \
        pointer_type,SgCastExp::e_C_style_cast);
    SgExprStatement * expstmti= buildAssignStatement(lhs,rhs);
    SageInterface::insertStatementBefore(target, expstmti);
    counter ++;
  }
 return wrapper_name; 
}
// eof
