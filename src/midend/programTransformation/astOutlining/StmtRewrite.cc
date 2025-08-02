/*!
 *  \file ASTtools/StmtRewrite.hh
 *
 *  \brief Implements routines to support basic statement-level
 *  rewriting.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 *
 *  This module differs from the low-level rewrite mechanism in that
 *  it provides specific operations on particular kinds of statement
 *  nodes. However, it could be merged with the low-level rewrite at
 *  some point in the future.
 */

#include "sage3basic.h"
#include "sageBuilder.h"
#include "ASTtools.hh"
#include "Copy.hh"
#include "StmtRewrite.hh"

// ========================================================================

using namespace std;

// ========================================================================

void
ASTtools::appendCopy (const SgStatement* s, SgBasicBlock* b)
{
  if (b && s)
    {
      SgStatement* s_copy = isSgStatement (deepCopy (s));
      ASSERT_not_null(s_copy);

      b->append_statement (s_copy); // TODO: a smarter append_statement should be
				// be able to set the symbol tables
      s_copy->set_parent(b);// needed ?

      // liao, 11/5/1007
      // fixup symbol copy for variable declaration statement
      // reason: local symbols reside in higher scope of the copied stmt,
      // current copy cannot take care of them, neither can append_statement()
      if (isSgVariableDeclaration(s)) 
      {
        SgInitializedName *initName = isSgInitializedName\
		(*(isSgVariableDeclaration(s)->get_variables()).begin());
        ROSE_ASSERT(initName);
        SgVariableSymbol * symbol_1 = new SgVariableSymbol(initName);
        b->insert_symbol(initName->get_name(), symbol_1);
      }
      // similar work for funtion declaration
      // liao, 11/5/2007, not sure why the scope becomes global here in the AST graph
      // possible reason: c functions cannot be nested, so must be global scope here
      if (isSgFunctionDeclaration(s)) 
      {
        SgFunctionDeclaration *decl=const_cast<SgFunctionDeclaration*> (isSgFunctionDeclaration(s));
        SgFunctionSymbol * symbol_1 = new SgFunctionSymbol(decl);
        SgGlobal* glob_scope =
	    const_cast<SgGlobal *> (TransformationSupport::getGlobalScope (s));
        ROSE_ASSERT(glob_scope);
        glob_scope->insert_symbol(decl->get_name(), symbol_1);
      }
    }
}

void
ASTtools::appendStmtsCopy (const SgBasicBlock* a, SgBasicBlock* b)
{
  // Replaced deprecated functions std::bind2nd and std::ptr_fun [Rasmussen, 2023.08.07]
  std::function<void(const SgStatement*,SgBasicBlock*)> ptrFun = appendCopy;

  if (a != nullptr)
    {
      SgStatementPtrList src_stmts = a->get_statements ();
        for_each (src_stmts.begin (),
                  src_stmts.end (),
                  std::bind(ptrFun, std::placeholders::_1, b));
    }
}

/*!
 *  \brief
 *
 *  This routine sets the scope of the new statement to be the same as
 *  the old.
 */
void
ASTtools::replaceStatement (SgStatement* /*s_cur*/, SgStatement* /*s_new*/)
{
  ASSERT_require2(false, "please use SageInterface::replaceStatement() instead");
}

void  // move statements from src block to dest block // same semantics to SageInterface::moveStatementsBetweenBlocks(), which is a better implementation
ASTtools::moveStatements (SgBasicBlock* src, SgBasicBlock* dest)
{
  cerr<<"ASTtools::moveStatements(SgBasicBlock*src, SgBasicBlock* dest) is used. Please use SageInterface::moveStatementsBetweenBlocks(SgBasicBlock* src, SgBasicBlock* dest) instead."<<endl;
  ASSERT_require(false);
  if (!src) return; // no work to do
  ASSERT_not_null(src);
  ASSERT_not_null(dest);

  // Move the statements.
  SgStatementPtrList& src_stmts = src->get_statements ();
  SgStatementPtrList& dest_stmts = dest->get_statements ();
  copy (src_stmts.begin (), src_stmts.end (),
        inserter (dest_stmts, dest_stmts.begin ()));
  src_stmts.clear ();

  // Copy the symbol table entries.
  SgSymbolTable* src_syms = src->get_symbol_table ();
  ASSERT_not_null(src_syms);

  for (SgSymbol* i = src_syms->find_any(); i; i = src_syms->next_any()) {
     dest->insert_symbol (i->get_name (), i);
  }

  // Clear the source symbol table.
  delete src_syms;
  src_syms = new SgSymbolTable;
  ASSERT_not_null(src_syms);
  src->set_symbol_table (src_syms);

  // Fix-up parent and scope pointers.
  for (SgStatementPtrList::iterator i = dest_stmts.begin ();
       i != dest_stmts.end (); ++i)
    {
      SgNode* par = (*i)->get_parent ();
      if (par == src)
        (*i)->set_parent (dest);

      SgDeclarationStatement* decl = isSgDeclarationStatement (*i);
      if (decl)
        {
          SgScopeStatement* scope = decl->get_scope ();
          if (scope == src)
            decl->set_scope (dest);
        }
    }
}

// =====================================================================
// add an additional inner level of block: 
// by moving the b_orig's statements to the inner level
SgBasicBlock*
ASTtools::transformToBlockShell (SgBasicBlock* b_orig)
{
  // Create new block to store 'T', and move statements to it.
  SgBasicBlock* b_shell = SageBuilder::buildBasicBlock ();
  ASSERT_not_null(b_shell);
  SageInterface::moveStatementsBetweenBlocks ( b_orig, b_shell) ; // better implementation by this interface function
  b_orig->append_statement (b_shell);
  b_shell->set_parent (b_orig);
  return b_shell;
}
