/**
 *  \file ThisExprs.cc
 *  \brief Preprocessor phase to convert 'this' expressions
 *  to-be-outlined into references to a local variable.
 */
#include <rose.h>
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

static
SgVariableDeclaration *
createThisShadowDecl (const string& name,
                      SgClassSymbol* sym,
                      const SgFunctionDefinition* func_def,
                      SgScopeStatement* scope)
{
  ROSE_ASSERT (sym && func_def);

  // Analyze function definition.
  const SgMemberFunctionDeclaration* func_decl =
    isSgMemberFunctionDeclaration (func_def->get_declaration ());
  ROSE_ASSERT (func_decl);

  // Build name for shadow variable.
  SgName var_name (name);

  // Build variable's type.
  SgType* class_type = sym->get_type  ();
  ROSE_ASSERT (class_type);
  SgType* var_type = 0;
  if (ASTtools::isConstMemFunc (func_decl))
    {
      SgModifierType* mod_type = SageBuilder::buildModifierType (class_type);
      ROSE_ASSERT (mod_type);
      mod_type->get_typeModifier ().get_constVolatileModifier ().setConst ();
      var_type = SgPointerType::createType (mod_type);
    }
  else
    var_type = SgPointerType::createType (class_type);
  ROSE_ASSERT (var_type);

  // Build initial value
  SgThisExp* this_expr = SageBuilder::buildThisExp (sym);
  ROSE_ASSERT (this_expr);
  SgAssignInitializer* init =
    SageBuilder::buildAssignInitializer (this_expr);

  // Build final declaration.
  SgVariableDeclaration* decl =
    SageBuilder::buildVariableDeclaration (var_name, var_type, init, scope);
  ROSE_ASSERT(decl->get_variableDeclarationContainsBaseTypeDefiningDeclaration ()==false);
  ROSE_ASSERT (decl);
  //! \todo Should constructor do this?
  //decl->set_firstNondefiningDeclaration (decl);
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
      else if (isSgAssignInitializer (e_par))
        {
          SgAssignInitializer* e_assign = isSgAssignInitializer (e_par);
          ROSE_ASSERT (e_assign->get_operand_i () == e_this);
          e_assign->set_operand_i (e_repl);
        }
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
  // Find all 'this' expressions.
  ASTtools::ThisExprSet_t this_exprs;
  ASTtools::collectThisExpressions (b, this_exprs);
  if (this_exprs.empty ()) // No transformation required.
    return b;

  // Create a shell to hold local declarations.
  SgBasicBlock* b_this = ASTtools::transformToBlockShell (b);
  ROSE_ASSERT (b_this);
  ASTtools::moveUpPreprocInfo (b_this, b);

  // Get the class symbol for the set of 'this' expressions.
  SgClassSymbol* sym = getClassSymAndVerify (this_exprs);
  ROSE_ASSERT (sym);

  // Create a shadow-declaration for 'this'.
//Liao, type mismatch!  
  //SgName shadow_name ("this__ptr__");
  SgVariableDeclaration* decl =
    createThisShadowDecl (string("this__ptr__"), sym,
                          ASTtools::findFirstFuncDef (b),b);
  ROSE_ASSERT (decl);
//  prependVarDecl (decl, b);
   SageInterface::prependStatement(decl,b);  

  // Replace instances of SgThisExp with the shadow variable.
  replaceThisExprs (this_exprs, decl);

  return b_this;
}

// eof
