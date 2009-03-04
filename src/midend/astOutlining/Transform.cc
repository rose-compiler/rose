/**
 *  \file Transform/Transform.cc
 *
 *  \brief Implements the outlining transformation.
 */
#include <rose.h>
#include <iostream>
#include <list>
#include <string>


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
  // Generate a new source file for the outlined function, if requested
  SgSourceFile* new_file = NULL;
  if (Outliner::useNewFile)
    new_file = generateNewSourceFile(s,func_name_str);

  // Save some preprocessing information for later restoration. 
  AttachedPreprocessingInfoType ppi_before, ppi_after;
  ASTtools::cutPreprocInfo (s, PreprocessingInfo::before, ppi_before);
  ASTtools::cutPreprocInfo (s, PreprocessingInfo::after, ppi_after);

  // Determine variables to be passed to outlined routine.
  // Also collect symbols which must use pointer dereferencing if replaced during outlining
  ASTtools::VarSymSet_t syms, pdSyms;
  collectVars (s, syms);

  //Determine variables to be replaced by temp copy or pointer dereferencing.
  if (Outliner::temp_variable)
  {
#if 0    
    std::set<SgVarRefExp* > varRefSetB;
    ASTtools::collectVarRefsUsingAddress(s,varRefSetB);
    ASTtools::collectVarRefsOfTypeWithoutAssignmentSupport(s,varRefSetB);
#endif
    ASTtools::collectPointerDereferencingVarSyms(s,pdSyms);
  }
    
  // Insert outlined function.
  //  grab target scope first
  SgGlobal* glob_scope =
    const_cast<SgGlobal *> (TransformationSupport::getGlobalScope (s));

  if (Outliner::useNewFile)  // change scope to the one within the new source file
  {
    glob_scope = new_file->get_globalScope();
  }

  // Generate outlined function.
//  printf ("In Outliner::Transform::outlineBlock() function name to build: func_name_str = %s \n",func_name_str.c_str());
  SgFunctionDeclaration* func = generateFunction (s, func_name_str, syms, pdSyms, glob_scope);
  ROSE_ASSERT (func != NULL);

  SgFunctionDeclaration* func_orig =
    const_cast<SgFunctionDeclaration *> (SageInterface::getEnclosingFunctionDeclaration (s));
    
  // Insert the function and its prototype as necessary  
  insert (func, glob_scope, func_orig); //Outliner::Transform::insert() 
  // DQ (9/7/2007): Need to add function symbol to global scope!
  //   printf ("Fixing up the symbol table in scope = %p = %s for function = %p = %s \n",glob_scope,glob_scope->class_name().c_str(),func,func->get_name().str());
//  if (glob_scope->lookup_function_symbol(func->get_name()) == NULL)
  {
    SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(func);
    glob_scope->insert_symbol(func->get_name(),functionSymbol);
    ROSE_ASSERT(glob_scope->lookup_function_symbol(func->get_name()) != NULL);
  }

  //Generate packing statements
  std::string wrapper_name;
  if (useParameterWrapper)
    wrapper_name= generatePackingStatements(s,syms);

  // Generate a call to the outlined function.
  SgScopeStatement * p_scope = s->get_scope();
  ROSE_ASSERT(p_scope);

  SgStatement *func_call = generateCall (func, syms, wrapper_name,p_scope);
  ROSE_ASSERT (func_call != NULL);
  
  ASTtools::replaceStatement (s, func_call);

  // Restore preprocessing information.
  ASTtools::moveInsidePreprocInfo (s, func->get_definition ()->get_body ());
  ASTtools::pastePreprocInfoFront (ppi_before, func_call);
  ASTtools::pastePreprocInfoBack (ppi_after, func_call);

  SageInterface::fixVariableReferences(p_scope);
  if (new_file)
  {
    SageInterface::fixVariableReferences(new_file);
    // SgProject * project2= new_file->get_project();
    // AstTests::runAllTests(project2);// turn it off for now
    // project2->unparse();
  }
  return Result (func, func_call, new_file);
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

SgSourceFile* 
Outliner::Transform::generateNewSourceFile(SgBasicBlock* s, const string& file_name)
{
  SgSourceFile* new_file = NULL;
  SgProject * project = getEnclosingNode<SgProject> (s);
  ROSE_ASSERT(project != NULL);
  // s could be transformation generated, so use the root SgFile for file name
  SgFile* cur_file = getEnclosingNode<SgFile> (s);
  ROSE_ASSERT (cur_file != NULL);
  //grab the file suffix, 
  // TODO another way is to generate suffix according to source language type
  std::string orig_file_name = cur_file->get_file_info()->get_filenameString();
  //cout<<"debug:orig_file_name="<<orig_file_name<<endl;
  std::string file_suffix = StringUtility::fileNameSuffix(orig_file_name);
  ROSE_ASSERT(file_suffix !="");
  std::string new_file_name = file_name+"."+file_suffix;
  // remove pre-existing file with the same name
  remove (new_file_name.c_str());
  new_file = isSgSourceFile(buildFile(new_file_name, new_file_name,project));
  //new_file = isSgSourceFile(buildFile(new_file_name, new_file_name));
  ROSE_ASSERT(new_file != NULL);
  return new_file;
}

// eof
