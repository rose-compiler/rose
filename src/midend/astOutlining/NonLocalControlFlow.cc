/**
 *  \file NonLocalControlFlow.cc
 *
 *  \brief Preprocessor phase to convert non-local jumps into code
 *  that performs local jumps combined with setting a state variable
 *  to indicate what non-local jump is taken. This transformation
 *  simplifies outlining of code containing non-local jumps.
 */
#include <rose.h>
#include <iostream>
#include <list>
#include <string>


#include "Preprocess.hh"
#include "ASTtools.hh"
#include "Jumps.hh"
#include "PreprocessingInfo.hh"
#include "Copy.hh"
#include "StmtRewrite.hh"

// =====================================================================

using namespace std;

// =====================================================================

//! Make sure jumps have a unique id.
static
void
renumberJumps (ASTtools::JumpMap_t& J)
{
  size_t next_id = 0;
  for (ASTtools::JumpMap_t::iterator j = J.begin (); j != J.end (); ++j)
    j->second = ++next_id;
}

//! Declares an 'int' variable, initialized to the specified value.
/*!
 *  This routine creates an integer ('int') variable declaration set
 *  to the specified value. If 'scope' is specified, then this routine
 *  also inserts a variable symbol into the symbol table of 'scope'.
 */
static
SgVariableDeclaration *
createIntDecl (const string& name, int val = 0, SgScopeStatement* scope = 0)
{
  SgType* var_type = SgTypeInt::createType ();
  ROSE_ASSERT (var_type);

  SgIntVal* init_val = new SgIntVal (ASTtools::newFileInfo (), val);
  ROSE_ASSERT (init_val);

  SgAssignInitializer* init =
    new SgAssignInitializer (ASTtools::newFileInfo (), init_val, var_type);

  ROSE_ASSERT (init);

  SgName var_name (name);
  SgVariableDeclaration* var_decl =
    new SgVariableDeclaration (ASTtools::newFileInfo (),
                               var_name, var_type, init);
  ROSE_ASSERT (var_decl);
  //! \todo Should constructor do this?
  var_decl->set_firstNondefiningDeclaration (var_decl);

  if (scope)
    {
      SgInitializedName* init_name = var_decl->get_decl_item (var_name);
      ROSE_ASSERT (init_name);
      SgVariableSymbol* var_sym = new SgVariableSymbol (init_name);
      ROSE_ASSERT (var_sym);
      scope->insert_symbol (var_name, var_sym);
      init_name->set_scope (scope);
    }

  return var_decl;
}

//! Creates a sequence of conditional statements to select a branch.
/*!
 *  Given a sequence of jump statements, JUMP[i], JUMP[i+1], ...,
 *  JUMP[end-1], this routine creates a sequence of 'if' statements of
 *  the form,
 *
 *    if (JUMP_VAR == i)
 *      JUMP[i];
 *    else if (JUMP_VAR == i+1)
 *      JUMP[i+1];
 *    else ... if (JUMP_VAR == end-1)
 *      JUMP[end-1];
 */
static
SgIfStmt *
createJumpTable (SgVariableSymbol* jump_var,
                 ASTtools::JumpMap_t::const_iterator i,
                 ASTtools::JumpMap_t::const_iterator end)
{
  if (i == end)
    return 0;

  // Extract information for current iteration (jump statement) i.
  const SgStatement* jump_stmt = i->first;
  int jump_val = (int)i->second;

  // Build 'if' condition
  SgVarRefExp* jump_var_ref = new SgVarRefExp (ASTtools::newFileInfo (),
                                               jump_var);
  ROSE_ASSERT (jump_var_ref);
  SgIntVal* val = new SgIntVal (ASTtools::newFileInfo (), jump_val);
  ROSE_ASSERT (val);
  SgEqualityOp* cond = new SgEqualityOp (ASTtools::newFileInfo (),
                                         jump_var_ref, val);
  ROSE_ASSERT (cond);
  SgExprStatement* cond_stmt =
    new SgExprStatement (ASTtools::newFileInfo (), cond);
  ROSE_ASSERT (cond_stmt);

  // The 'true' branch executes the original jump statement.
  SgBasicBlock* true_body = new SgBasicBlock (ASTtools::newFileInfo ());
  ROSE_ASSERT (true_body);
  SgStatement* true_stmt =
    const_cast<SgStatement *> (isSgStatement (ASTtools::deepCopy (jump_stmt)));
  ROSE_ASSERT (true_stmt);
  true_body->append_statement (true_stmt);

  // The 'false' branch executes any other jump statements.
  SgBasicBlock* false_body = new SgBasicBlock (ASTtools::newFileInfo ());
  SgStatement* false_stmt = createJumpTable (jump_var, ++i, end);
  if (false_stmt)
    false_body->append_statement (false_stmt);

  // Assemble final 'if'
  SgIfStmt* new_jump = new SgIfStmt (ASTtools::newFileInfo (),
                                     cond_stmt,
                                     true_body,
                                     false_body);
  ROSE_ASSERT (new_jump);
  return new_jump;
}

//! Creates a "replacement goto."
/*!
 *  Given a jump variable, JUMP_VAR, a jump ID number, i, and a goto
 *  label, LOCAL_TARGET, this routine returns an SgBasicBlock of the
 *  form,
 *
 *    {
 *      JUMP_VAR = i;
 *      goto LOCAL_TARGET;
 *    }
 */
static
SgBasicBlock *
createJumpReplacementGoto (SgVariableSymbol* jump_var,
                           size_t jump_id,
                           SgLabelStatement* target)
{
  ROSE_ASSERT (jump_var && target);

  // Block to return.
  SgBasicBlock* b = new SgBasicBlock (ASTtools::newFileInfo ());
  ROSE_ASSERT (b);

  // Create assignment, 'v = id'
  SgVarRefExp* v = new SgVarRefExp (ASTtools::newFileInfo (), jump_var);
  ROSE_ASSERT (v);

  SgIntVal* id = new SgIntVal (ASTtools::newFileInfo (), jump_id);
  ROSE_ASSERT (id);

  SgAssignOp* assign = new SgAssignOp (ASTtools::newFileInfo (), v, id);
  ROSE_ASSERT (assign);
  v->set_parent (assign);
  id->set_parent (assign);

  SgExprStatement* assign_stmt =
    new SgExprStatement (ASTtools::newFileInfo (), assign);
  ROSE_ASSERT (assign_stmt);

  // Create 'goto target' statement.
  SgGotoStatement* goto_stmt = new SgGotoStatement (ASTtools::newFileInfo (),
                                                    target);
  ROSE_ASSERT (goto_stmt);

  // Construct and return block.
  b->append_statement (assign_stmt);
  assign_stmt->set_parent (b);
  b->append_statement (goto_stmt);
  goto_stmt->set_parent (b);

  return b;
}

//! Convert a list of non-local jumps into local gotos.
/*!
 *  Given a jump variable, JUMP_VAR, a list of jump statements,
 *  J[0..n-1], and a local goto target, LOCAL_TARGET, this routine
 *  replaces each jump statement, J[i], with
 *
 *    {
 *      JUMP_VAR = i;
 *      goto LOCAL_TARGET;
 *    }
 *
 *  \pre 'local_target' and all the jumps must reside in the same
 *  function definition scope.
 */
static
void
convertJumpsToGotos (SgVariableSymbol* jump_var,
                     ASTtools::JumpMap_t& jumps,
                     SgLabelStatement* local_target)
{
  for (ASTtools::JumpMap_t::iterator i = jumps.begin ();
       i != jumps.end (); ++i)
    {
      SgStatement* jump_stmt = const_cast<SgStatement *> (i->first);
      ROSE_ASSERT (jump_stmt);
      size_t jump_id = i->second;

      SgBasicBlock* new_block = createJumpReplacementGoto (jump_var,
                                                           jump_id,
                                                           local_target);
      ROSE_ASSERT (new_block);

      SgStatement* jump_par = isSgStatement (jump_stmt->get_parent ());
      ROSE_ASSERT (jump_par);

      // \todo Is the original 'jump_stmt' leaked at this point?
      jump_par->replace_statement (jump_stmt, new_block);
    }
}

// =====================================================================

SgBasicBlock *
Outliner::Preprocess::transformNonLocalControlFlow (SgBasicBlock* b_orig)
{
  // Determine if any non-local jumps exist.
  ASTtools::JumpMap_t jumps; // non-goto jumps
  ASTtools::collectNonLocalJumps (b_orig, jumps);
  ASTtools::collectNonLocalGotos (b_orig, jumps);

  // Return if nothing to transform
  if (jumps.empty ())
    return b_orig;

  renumberJumps (jumps); // A little post-processing

  // Create new block to store 'T', and move statements to it.
  SgBasicBlock* b_gotos = ASTtools::transformToBlockShell (b_orig);
  ROSE_ASSERT (b_gotos);
  ASTtools::moveUpPreprocInfo (b_gotos, b_orig);

  // Create a declaration for 'EXIT_TAKEN__'
  SgName var_exit_name ("EXIT_TAKEN__");
  SgVariableDeclaration* var_exit = createIntDecl (var_exit_name,
                                                   0,
                                                   b_orig);
  ROSE_ASSERT (var_exit);
  b_orig->prepend_statement (var_exit);
  var_exit->set_parent (b_orig);

  // Retrieve a symbol for 'EXIT_TAKEN__' for future use.
  SgVariableSymbol* jump_var = b_orig->lookup_var_symbol (var_exit_name);
  ROSE_ASSERT (jump_var);

  // Create a non-local exit label.
  SgName label_exit_name ("NON_LOCAL_EXIT__");
  SgLabelStatement* label_exit =
    new SgLabelStatement (ASTtools::newFileInfo (), label_exit_name);
  ROSE_ASSERT (label_exit);
  b_gotos->append_statement (label_exit);
  label_exit->set_parent (b_gotos);
  label_exit->set_scope (b_gotos); // seg fault if missing, Liao
  // add SgLabelSymbol,liao, 10/30,2007
  SgLabelSymbol *lsymbol= new SgLabelSymbol(label_exit);
  b_gotos->insert_symbol(label_exit_name,lsymbol);

  // Convert all non-local jumps to local gotos to 'label_exit'
  convertJumpsToGotos (jump_var, jumps, label_exit);

  // Assemble the non-local "jump table (conditionals)"
  SgIfStmt* jump_table = createJumpTable (jump_var,
                                          jumps.begin (),
                                          jumps.end ());
  ROSE_ASSERT (jump_table);
  b_orig->append_statement (jump_table);

  return b_gotos;
}

// eof
