/**
 *  \file ThisExprs.cc
 *  \brief Preprocessor phase to convert 'this' expressions
 *  to-be-outlined into references to a local variable.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include <iostream>
#include <list>
#include <string>


#include "Preprocess.hh"
#include "ASTtools.hh"
#include "This.hh"
#include "VarSym.hh"
#include "PreprocessingInfo.hh"
#include "StmtRewrite.hh"

// =====================================================================

using namespace std;
using namespace Outliner;
using namespace SageInterface;

// =====================================================================

/*!
 *  Checks that a set of 'this' expressions has the same class symbol,
 *  and returns that symbol.
 */
static
SgClassSymbol *
getClassSymAndVerify (const ASTtools::ThisExprSet_t& E)
{
  SgClassSymbol* sym = 0;
  if (!E.empty ())
    {
      for (ASTtools::ThisExprSet_t::const_iterator i = E.begin ();
           i != E.end (); ++i)
        {
          const SgThisExp* t = *i;
          ROSE_ASSERT (t);
          if (!sym)
            sym = t->get_class_symbol ();
          else if (sym != t->get_class_symbol ())
            {
              cerr << "*** 'this' expressions use different symbols! ***" << endl;
              return 0; // Signal error
            }
        }
    }
  return sym;
}
//!  class Hello *this__ptr__ = this; 
// creation and insertion if it does not yet exist
// multiple outlining targets within the same member function can share the same declaration
static
SgVariableDeclaration *
createThisShadowDecl (const string& name,
                      SgClassSymbol* sym, /* the class symbol for this pointer*/
                      SgFunctionDefinition* func_def /*The enclosing class member function*/)
//                      SgScopeStatement* scope)
{
#ifdef __linux__
  if (enable_debug)  
    cout<<"Entering "<< __PRETTY_FUNCTION__ <<endl;
#endif
  SgVariableDeclaration* decl = NULL;
  ROSE_ASSERT (sym && func_def);

  // Analyze function definition.
  const SgMemberFunctionDeclaration* func_decl =
    isSgMemberFunctionDeclaration (func_def->get_declaration ());
  ROSE_ASSERT (func_decl);
  SgBasicBlock* func_body = func_def ->get_body();
  ROSE_ASSERT (func_body);

  // Build name for shadow variable.
  SgName var_name (name);

  SgVariableSymbol * exist_symbol = func_body->lookup_variable_symbol(var_name);
  if (exist_symbol)
  {
    //decl = isSgVariableDeclaration(exist_symbol->get_declaration()->get_definition());
    decl = isSgVariableDeclaration(exist_symbol->get_declaration()->get_declaration());
    ROSE_ASSERT (decl);
    ROSE_ASSERT (decl->get_scope() == isSgScopeStatement(func_body));
  }
  else 
  {
    // Build variable's type. class A*  or const class A *
    SgType* class_type = sym->get_type  ();
    ROSE_ASSERT (class_type);
    SgType* var_type = 0;
    if (ASTtools::isConstMemFunc (func_decl))
    {
#if 0   // Liao 8/13/2010 changes needed to use the new type table   
      SgModifierType* mod_type = SageBuilder::buildModifierType (class_type);
      ROSE_ASSERT (mod_type);
      mod_type->get_typeModifier ().get_constVolatileModifier ().setConst ();
#else
      SgModifierType* mod_type = SageBuilder::buildConstType (class_type);
#endif      
      var_type = SgPointerType::createType (mod_type);
    }
    else
      var_type = SgPointerType::createType (class_type);
    ROSE_ASSERT (var_type);

    // Build initial value: this pointer
    SgThisExp* this_expr = SageBuilder::buildThisExp (sym);
    ROSE_ASSERT (this_expr);
    SgAssignInitializer* init =
      SageBuilder::buildAssignInitializer (this_expr);

    // Build final declaration.
    decl =  SageBuilder::buildVariableDeclaration (var_name, var_type, init, func_body);
    //SageBuilder::buildVariableDeclaration (var_name, var_type, init, scope);
    ROSE_ASSERT(decl->get_variableDeclarationContainsBaseTypeDefiningDeclaration ()==false);
    SageInterface::prependStatement(decl, func_body);
  }
  ROSE_ASSERT (decl);
  // Add some comments to mark it
  SageBuilder::buildComment(decl, "//A declaration for this pointer"); 

  // We insert it to the enclosing member function definition
  if (enable_debug)
  {
    cout<<"prepending a statement declaring this__ptr into a function body:"<<func_body<<endl;
    cout<<"The function body's file info is:"<<endl;
    func_body->get_file_info()->display();
    func_body->unparseToString();
  }

  // Liao (1/i28/2020): When used in conjunction with header file unparsing we need to set the physical file id on entirety of the subtree being inserted.
  SgSourceFile* sfile = getEnclosingSourceFile (func_body);
  if (sfile->get_unparseHeaderFiles())
  {
    int physical_file_id = func_body->get_startOfConstruct()->get_physical_file_id();
    string physical_filename_from_id = Sg_File_Info::getFilenameFromID(physical_file_id);
    if (enable_debug)
    {
      printf ("scope for function call transformation: physical_filename_from_id = %s \n",physical_filename_from_id.c_str());
    }

    SageBuilder::fixupSourcePositionFileSpecification(decl,physical_filename_from_id);
  }
  //decl->set_isModified(true);
  return decl;
}

#if 0
// TODO: Move to ASTtools?
//! Inserts a variable declaration into a scope, with symbol insertion.
/*!
 *  This routine inserts the given variable declaration at the
 *  beginning of the specified scope. In addition, it creates variable
 *  symbols and inserts these into the symbol table for the scope.
 */
static
void
prependVarDecl (SgVariableDeclaration* decl,
                SgBasicBlock* block)
{
  ROSE_ASSERT (decl && block);

  block->prepend_statement (decl);
  decl->set_parent (block);

  SgInitializedNamePtrList& names = decl->get_variables ();
  for (SgInitializedNamePtrList::iterator i = names.begin ();
       i != names.end (); ++i)
    {
      SgVariableSymbol* sym = new SgVariableSymbol (*i);
      ROSE_ASSERT (sym);
      (*i)->set_scope (block);
      block->insert_symbol ((*i)->get_name (), sym);
    }
}
#endif

//! Replace this->member  with this__ptr__->member
static
void
replaceThisExprs (ASTtools::ThisExprSet_t& this_exprs,
                  SgVariableDeclaration* decl)
{
  SgVariableSymbol* sym = SageInterface::getFirstVarSym (decl);
  ROSE_ASSERT (sym);

  for (ASTtools::ThisExprSet_t::iterator i = this_exprs.begin ();
       i != this_exprs.end (); ++i)
    {
      SgThisExp* e_this = const_cast<SgThisExp *> (*i);
      ROSE_ASSERT (e_this);

      SgVarRefExp* e_repl = SageBuilder::buildVarRefExp (sym);
      ROSE_ASSERT (e_repl);

      SgNode* e_par = e_this->get_parent ();
      ROSE_ASSERT (e_par);
      if (isSgBinaryOp (e_par))
        {
          SgBinaryOp* bin_op = isSgBinaryOp (e_par);
          SgExpression* lhs = isSgThisExp (bin_op->get_lhs_operand ());
          if (lhs == e_this)
            bin_op->set_lhs_operand (e_repl);
          else
            {
              SgExpression* rhs = isSgThisExp (bin_op->get_rhs_operand ());
              if (rhs == e_this)
                bin_op->set_rhs_operand (e_repl);
              else
                {
                  ROSE_ASSERT (!"*** Binary op does not use 'this' as expected. ***");
                }
            }
        }
      else if (isSgUnaryOp (e_par))
        {
          SgUnaryOp* un_op = isSgUnaryOp (e_par);
          SgThisExp* e = isSgThisExp (un_op->get_operand ());
          if (e == e_this)
            un_op->set_operand (e_repl);
          else
            {
              ROSE_ASSERT (!"*** Unary op does not use 'this' as expected. ***");
            }
        }
      else if (isSgExprListExp (e_par))
        {
          SgExprListExp* e_list = isSgExprListExp (e_par);
          SgExpressionPtrList& exprs = e_list->get_expressions ();
          SgExpressionPtrList::iterator i = find (exprs.begin (), exprs.end (),
                                                  e_this);
          if (i == exprs.end ())
            {
              ROSE_ASSERT (!"*** Expression list does not contain 'this' as expected. ***");
            }
          else
            *i = e_repl;
        }
      else if (isSgSizeOfOp (e_par))
        {
          SgSizeOfOp* e_sizeof = isSgSizeOfOp (e_par);
          ROSE_ASSERT (e_sizeof->get_operand_expr () == e_this);
          e_sizeof->set_operand_expr (e_repl);
        }
#if 1        
      else if (isSgAssignInitializer (e_par))
        {
          SgAssignInitializer* e_assign = isSgAssignInitializer (e_par);
          ROSE_ASSERT (e_assign->get_operand_i () == e_this);
          e_assign->set_operand_i (e_repl);
        }
#endif        
      else // Don't know how to handle this...
        {
          cerr << "*** '" << e_par->class_name () << "' ***" << endl;
          ROSE_ASSERT (!"*** Case not handled ***");
        }

      // Set parent pointer of replacement expression.
      e_repl->set_parent (e_par);
    }
}

// =====================================================================

SgBasicBlock *
Outliner::Preprocess::transformThisExprs (SgBasicBlock* b)
{
#ifdef __linux__
  if (enable_debug)  
    cout<<"Entering "<< __PRETTY_FUNCTION__ <<endl;
#endif
  // Find all 'this' expressions.
  ASTtools::ThisExprSet_t this_exprs;
  ASTtools::collectThisExpressions (b, this_exprs);
  if (this_exprs.empty ()) // No transformation required.
  {
#ifdef __linux__
    if (enable_debug)  
      cout<<"empty this expression set, exiting "<< __PRETTY_FUNCTION__ <<" without create this shadow declaration. " <<endl;
#endif
    return b;
  }

  if (enable_debug)  
  {
    cout<<"The input BB is:"<<b<<endl;
    b->get_file_info()->display();
  }
  // Get the class symbol for the set of 'this' expressions.
  SgClassSymbol* sym = getClassSymAndVerify (this_exprs);
  ROSE_ASSERT (sym);
#if 0
  // Create a shell to hold local declarations:
  // Add an additional inner level of BB  and move b's content to it,
  // return the inner level BB as the new outlining target
  // In the meantime, prepend a this-pointer declaration to the original BB and keep it
  SgBasicBlock* b_this = ASTtools::transformToBlockShell (b);
  ROSE_ASSERT (b_this);
  ASTtools::moveUpPreprocInfo (b_this, b);

  // Create a shadow-declaration for 'this'.
  SgVariableDeclaration* decl =
    createThisShadowDecl (string("this__ptr__"), sym,
                          ASTtools::findFirstFuncDef (b),b);
  ROSE_ASSERT (decl);
//  prependVarDecl (decl, b);
   SageInterface::prependStatement(decl,b);  
#else
   // Liao 10/27/2009
   // we have to consider the AST changes for both #pragma rose_outline and #pragma omp parallel/task
   // They have different layout:
   // the first one has an outlining target following the pragma
   //    generating an inner level BB and return it as the new outlining target can keep the this_ptr declaration
   // but the 2nd case has a child block as the outlining target
   //    the whole child block will be replaced and the this_ptr declaration will get lost
   // So the solution is to create the this__ptr__ declaration within the enclosing member function definition
   // No new inner level BB is created at all.
  SgVariableDeclaration* decl =  createThisShadowDecl (string("this__ptr__"), sym, const_cast<SgFunctionDefinition *>(ASTtools::findFirstFuncDef (b)));
  ROSE_ASSERT (decl);
#endif

  // Replace instances of SgThisExp with the shadow variable.
  replaceThisExprs (this_exprs, decl);
  if (enable_debug) 
  {
    cout<<"Debug Outliner::Preprocess::transformThisExprs() output BB is:"<<b<<endl;
    b->unparseToString();
  }

  //return b_this;
  return b;
}

// eof
