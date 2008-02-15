/**
 *  \file Transform/Transform.cc
 *
 *  \brief Implements the outlining transformation.
 */

#include <iostream>
#include <list>
#include <string>
#include <rose.h>

#include "Transform/Transform.hh"
#include "ASTtools/ASTtools.hh"
#include "ASTtools/PreprocessingInfo.hh"
#include "ASTtools/StmtRewrite.hh"

// =====================================================================

using namespace std;

// =====================================================================

LiaOutliner::Result
LiaOutliner::Transform::outlineBlock (SgBasicBlock* s,
                                      const string& func_name_str)
{
  // Save some preprocessing information.
  AttachedPreprocessingInfoType ppi_before, ppi_after;
  ASTtools::cutPreprocInfo (s, PreprocessingInfo::before, ppi_before);
  ASTtools::cutPreprocInfo (s, PreprocessingInfo::after, ppi_after);

  // Determine variables to be passed to outlined routine.
  ASTtools::VarSymSet_t syms;
  collectVars (s, syms);

  // Generate outlined function.
  printf ("In LiaOutliner::Transform::outlineBlock() function name to build: func_name_str = %s \n",func_name_str.c_str());
  SgFunctionDeclaration* func = generateFunction (s, func_name_str, syms);
  ROSE_ASSERT (func != NULL);

  // Insert outlined function.
  SgGlobal* glob_scope =
    const_cast<SgGlobal *> (TransformationSupport::getGlobalScope (s));
  SgFunctionDeclaration* func_orig =
    const_cast<SgFunctionDeclaration *> (ASTtools::getEnclosingFuncDecl (s));
  insert (func, glob_scope, func_orig);
// DQ (9/7/2007): Need to add function symbol to global scope!
//   printf ("Fixing up the symbol table in scope = %p = %s for function = %p = %s \n",glob_scope,glob_scope->class_name().c_str(),func,func->get_name().str());
   SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(func);
   glob_scope->insert_symbol(func->get_name(),functionSymbol);
   ROSE_ASSERT(glob_scope->lookup_function_symbol(func->get_name()) != NULL);

   ROSE_ASSERT(func->get_definingDeclaration()         != NULL);
   ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);

  // Generate a call to the outlined function.
  SgStatement *func_call = generateCall (func, syms);
  ROSE_ASSERT (func_call != NULL);

  ASTtools::replaceStatement (s, func_call);

  // Restore preprocessing information.
  ASTtools::moveInsidePreprocInfo (s, func->get_definition ()->get_body ());
  ASTtools::pastePreprocInfoFront (ppi_before, func_call);
  ASTtools::pastePreprocInfoBack (ppi_after, func_call);
  return Result (func, func_call);
}

// eof
