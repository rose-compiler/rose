/**
 *  \file Block.cc
 *  \brief Preprocessor phase to convert a statement into an
 *  SgBasicBlock.
 */
#include <rose.h>
#include <iostream>
#include <list>
#include <string>


#include "Preprocess.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include "Copy.hh"
#include "StmtRewrite.hh"

// =====================================================================

using namespace std;

// =====================================================================

/*!
 *  If the given initialized name (located in the given scope) has an
 *  initializer, returns a new expression statement equivalent to the
 *  initializer.
 *
 *  \pre 'name' must correspond to a variable declaration in 'scope'.
 *
 *  \returns The new statement, or 0 if none the initialized name does
 *  not have an initializer.
 *
 *  \todo Currently works only for SgAssignInitializers on
 *  non-reference primitive types. SgAggregateInitializers will return
 *  0.
 */
static
SgExprStatement *
createAssignment (const SgInitializedName* name, const SgScopeStatement* scope)
{
  if (!name)
    return 0;

  const SgAssignInitializer* rhs = isSgAssignInitializer (name->get_initializer ());
  if (!rhs)
    return 0;

  // Has an assign initializer (rhs). If 'name's' type is a reference,
  // keep the initializer.
  if (isSgReferenceType (name->get_type ()))
    return 0;

  const SgType* rhs_type = rhs->get_type ();
  if (!rhs_type
      || ASTtools::isConstObj (name->get_type ())
      || isSgClassType (rhs_type)
      || (isSgModifierType (rhs_type)
          && isSgClassType (isSgModifierType (rhs_type)->get_base_type ())))
    return 0;

  // Build RHS
  SgExpression* rhs_op =
    isSgExpression (ASTtools::deepCopy (rhs->get_operand ()));
  ROSE_ASSERT (rhs_op);

  // Build LHS (i.e., variable reference)
  ROSE_ASSERT (scope);
  SgVariableSymbol* v_sym =
    const_cast<SgScopeStatement *> (scope)->lookup_var_symbol (name->get_name ());
  ROSE_ASSERT (v_sym);
  SgVarRefExp* v = new SgVarRefExp (ASTtools::newFileInfo (), v_sym);
  ROSE_ASSERT (v);

  // Build assignment expression
  SgAssignOp* assign_op =
    new SgAssignOp (ASTtools::newFileInfo (), v, rhs_op);
  ROSE_ASSERT (assign_op);

  // Build expression statement
#if 0
  SgExpressionRoot* expr_root =
    new SgExpressionRoot (ASTtools::newFileInfo (),
                          assign_op, v_sym->get_type ());
  ROSE_ASSERT (expr_root);
#endif
  SgExprStatement* expr_stmt =
    new SgExprStatement (ASTtools::newFileInfo (), assign_op);
  ROSE_ASSERT (expr_stmt);

  // Done
  return expr_stmt;
}

/*!
 *  If the given initialized name has an initializer, create an
 *  expression statement equivalent to the initializer and append that
 *  statement to the target basic block.
 *
 *  \pre 'name' must correspond to a variable declaration in 'scope'.
 *
 *  \returns true if an initializer exists, and false otherwise.
 *
 *  \todo See TODO for \ref createAssignment().
 */
static
bool
appendAssignment (const SgInitializedName* name, const SgScopeStatement* scope,
                  SgBasicBlock* target)
{
  if (target)
    {
      SgExprStatement* assign = createAssignment (name, scope);
      if (assign)
        {
          target->append_statement (assign);
          return true;
        }
    }
  return false; // default: no assignment needed
}

SgBasicBlock *
Outliner::Preprocess::normalizeVarDecl (SgVariableDeclaration* s)
{
  if (!s) return 0;

  // Verify at least one variable exists.
  SgInitializedNamePtrList& vars_orig = s->get_variables ();
  SgInitializedNamePtrList::iterator i = vars_orig.begin ();
  ROSE_ASSERT (i != vars_orig.end ());

  // Prepare new basic block to contain initializers.
  SgBasicBlock* assigns_new = new SgBasicBlock (ASTtools::newFileInfo ());
  SgScopeStatement* s_scope = s->get_scope ();
  ROSE_ASSERT (s_scope);

  do
    {
      if (appendAssignment (*i, s_scope, assigns_new))
        (*i)->set_initializer (0); // \todo Fix this memory leak!
      ++i;
    }
  while (i != vars_orig.end ());

  // Insert block of assignments after the variable declaration.
  s_scope->insert_statement (s, assigns_new, false);
  assigns_new->set_parent (s_scope); // needed?
  return assigns_new;
}

/*!
 *  \brief Convert the "plain-old" statement into an SgBasicBlock.
 *  This normalization simplifies outlining of single statements.
 */
SgBasicBlock *
Outliner::Preprocess::createBlock (SgStatement* s)
{
  SgStatement* s_outline = s;
  if (!isSgBasicBlock (s))
    {
      SgBasicBlock* b_new = new SgBasicBlock (ASTtools::newFileInfo ());
      ROSE_ASSERT (b_new);
      SgStatement * parent = isSgStatement(s->get_parent ());
      ROSE_ASSERT(parent); 
      ASTtools::moveUpPreprocInfo (b_new, s);
#if 0      
     ASTtools::replaceStatement (s, b_new);
      b_new->set_parent (parent);
#else
      SageInterface::replaceStatement(s,b_new);
#endif 
      // insert s to b_new
      b_new->append_statement (s);
      s->set_parent (b_new);
      s_outline = b_new;
    }
  return isSgBasicBlock (s_outline);
}

// eof
