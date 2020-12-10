/**
 *  \file Check.cc
 *  \brief Verifies that a given outline target meets certain
 *  preconditions.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <iostream>
#include <list>
#include <string>


#include "Outliner.hh"
#include "ASTtools.hh"

// =====================================================================

using namespace std;

// =====================================================================
static
bool
checkType (const SgStatement* s)
{
  if (s)
    switch (s->variantT ())
      {
      case V_SgArithmeticIfStatement:
      case V_SgAssociateStatement:
      case V_SgBasicBlock:
      case V_SgBreakStmt:
      case V_SgClassDeclaration:
      case V_SgCloseStatement:
      case V_SgContinueStmt:
      case V_SgDoWhileStmt:
      case V_SgExprStatement:
      case V_SgFlushStatement:
      case V_SgForStatement:
      case V_SgFortranDo:
      case V_SgFortranNonblockedDo:
      case V_SgGotoStatement:
      case V_SgIfStmt:
      case V_SgOpenStatement:
      case V_SgPrintStatement:
      case V_SgReadStatement:
      case V_SgReturnStmt:
      case V_SgRewindStatement:
      case V_SgProcessControlStatement:
      case V_SgSwitchStatement:
      case V_SgTemplateInstantiationDecl:
      case V_SgTryStmt:
      case V_SgVariableDeclaration:
      case V_SgWaitStatement:
      case V_SgWhereStatement:
      case V_SgWhileStmt:
      case V_SgWriteStatement: 
       return true;
      default:
        break;
      }
  // Default: assume not outlineable.
  return false;
}

bool
isHiddenType (const SgType* type)
{
  if (!type) return false;
  const SgType* base_type = type->findBaseType ();
  if (base_type)
  {
    const SgNamedType* n = isSgNamedType (base_type);
    if (n)
    { // Get the declaration of a named type
      const SgDeclarationStatement* decl = n->get_declaration ();
      if (decl)
      {
        const SgScopeStatement* scope = decl->get_scope ();
        while (!isSgGlobal (scope))
        {
          // Non-global scoped declaration, not within a class or name space, 
          // a hidden type!
          if (!isSgClassDefinition (scope)
              && !isSgNamespaceDefinitionStatement (scope))
            return true; 
          scope = scope->get_scope ();
        }
      }
    }
  }
  return false;
}

/*!
 *  \brief Returns 'true' if the given statement, 's', has a variable
 *  reference or declaration whose type is 'hidden', in the sense
 *  specified by \ref ASTtools::isHiddenType ().
 */
static
bool
doesRefHiddenTypes (const SgStatement* s)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t refs = NodeQuery::querySubTree (const_cast<SgStatement *> (s), V_SgVarRefExp);
  for (NodeList_t::const_iterator i = refs.begin (); i != refs.end (); ++i)
  {
    const SgVarRefExp* ref = isSgVarRefExp (*i);
    ROSE_ASSERT (ref);
    const SgType* type = ref->get_type ();
    if (isHiddenType (type))
      return true;
  }

  refs = NodeQuery::querySubTree (const_cast<SgStatement *> (s),
      V_SgVariableDeclaration);
  for (NodeList_t::const_iterator i = refs.begin (); i != refs.end (); ++i)
  {
    const SgVariableDeclaration* decl = isSgVariableDeclaration (*i);
    ROSE_ASSERT (decl);
    const SgInitializedNamePtrList& names = decl->get_variables ();
    for (SgInitializedNamePtrList::const_iterator v = names.begin ();
        v != names.end (); ++v)
      if (isHiddenType ((*v)->get_type ()))
        return true;
  }

  return false; // No hidden references found
}

/*!
 *  \brief Returns 'true' if the given statement contains any
 *  references to the GNU function, '__builtin_va_start'.
 */
static
bool
doesCall__builtin_va_start (const SgStatement* s)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t refs = NodeQuery::querySubTree (const_cast<SgStatement *> (s), V_SgFunctionRefExp);
  for (NodeList_t::const_iterator i = refs.begin (); i != refs.end (); ++i)
  {
    const SgFunctionRefExp* ref = isSgFunctionRefExp (*i);
    ROSE_ASSERT (ref);
    const SgFunctionSymbol* sym = ref->get_symbol_i ();
    ROSE_ASSERT (sym);
    if (sym->get_name () == "__builtin_va_start")
      return true;
  }
  return false;
}

/*!
 *  \brief Returns 'true' if the statement contains a va_start()
 *  expression.
 *
 *  \note Created this function after 0.8.10a, which introduced new IR
 *  nodes for variable argument objects. This function should make the
 *  function, doesCall__builtin_va_start() (defined in this module),
 *  obsolete.
 */
static
bool
hasVAStart (const SgStatement* s)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t L = NodeQuery::querySubTree (const_cast<SgStatement *> (s), V_SgVarArgStartOp);
  return !L.empty ();
}

/*!
 *  \brief Returns 'true' if the given statement is part of a 'for'
 *  statement's initializer or test statement.
 */
static
bool
isForInitOrTest (const SgStatement* s)
{
  if (!s)
    return false;

  if (isSgForInitStatement (s))
    return true;

  const SgNode* s_par = s->get_parent ();
  // Why is this?, Liao, 4/8/2009
  // I commented it out since we want to outline inner loop sometimes
  //if (isSgForInitStatement (s_par) || isSgForStatement (s_par))
  if (isSgForInitStatement (s_par)) 
    return true;

  const SgForStatement* loop = isSgForStatement (s_par);
  if (loop)
  {
    // Check if the statement is one of the init statements
    const SgStatementPtrList& stmts = loop->get_init_stmt ();
    if (find (stmts.begin (), stmts.end (), s) != stmts.end ())
      return true;
   // Liao, 4/8/2009   
   //Check if the statement is a test statement for a for loop
    if (s == loop->get_test())
       return true;
  }

  return false;
}

// =====================================================================

/*!
 *  \brief Check whether the candidate statement satisfies the
 *  conditions of an outlineable statement.
 */
bool
Outliner::isOutlineable (const SgStatement* s, bool verbose)
{
  if (verbose)
  {
    cerr << "=== Checking Outliner preconditions for "
      << (const void *)s;
    if (s)
    {
      cerr << ':' << s->class_name ()
        << ' ' << ASTtools::toStringFileLoc (s);
    }
    cerr << "... ===" << endl;
  }

  if (!s)
  {
    if (verbose)
      cerr << "*** Statement must not be NULL. ***" << endl;
    return false;
  }
#if 1
  if (isSgVariableDeclaration (s))
  {
    if (verbose)
      cerr << "*** Can't outline a variable declaration by itself. ***" << endl;
    return false;
  }
#endif
  const SgFunctionDeclaration* decl = 
    SageInterface::getEnclosingFunctionDeclaration (const_cast<SgStatement* >(s));
  if (!decl)
  {
    if (verbose)
      cerr << "*** Statement must appear in an enclosing function declaration. ***" << endl;
    return false;
  }

  // New option: C++ template is enabled or not for outlining.
  if (!enable_template)
  {
    if (isSgTemplateInstantiationFunctionDecl (decl)
        || isSgTemplateInstantiationMemberFunctionDecl (decl))
    {
      // \todo Fix the template instantiation case (see Cxx_tests/test2004_75.C)
      if (verbose)
        cerr << "*** Can't outline template instantiations yet. ***" << endl;
      return false;
    }
    // Liao 12/20/2012. New EDG 4.4 version has better template support. We store original template declarations instead of instantiations in AST now

    if (isSgTemplateFunctionDeclaration(decl)
        || isSgTemplateMemberFunctionDeclaration(decl))
    {
      // \todo Fix the template case (see Cxx_tests/test2004_75.C)
      if (verbose)
        cerr << "*** Can't outline things within templates yet. ***" << endl;
      return false;
    }
  }

  if (isSgDeclarationStatement (s) && !isSgVariableDeclaration (s))
  {
    if (verbose)
    {
      cerr << "*** Statement must not be a declaration statement, unless it is a variable declaration. ***"  << endl
        << "    (Statement: " << (const void *)s << ":<" << s->class_name () << ">)" << " " << ASTtools::toStringFileLoc (s) << endl;
    }
    return false;
  }

  if (!checkType (s))
  {
    if (verbose)
      cerr << "*** Statement must have the correct type. ***" << endl;
    return false;
  }

  if (doesRefHiddenTypes (s))
  {
    if (verbose)
      cerr << "*** Statement must not reference hidden types. ***" << endl;
    return false;
  }

  if (doesCall__builtin_va_start (s))
  {
    if (verbose)
      cerr << "*** __builtin_va_start() calls not supported. ***" << endl;
    return false;
  }

  // Liao, 8/20/2009 TODO why do we need this at all?
  // A minor excuse is that if the BB is the body of a function definition, 
  // replacing it with a call statement will cause some problem.
  // But we can easily extend SageInterface::replaceStatement() to handle it 
  if (isSgBasicBlock (s) && (!isSgBasicBlock (s->get_parent ())&& !isSgOmpBodyStatement(s->get_parent ())))
  {
    if (verbose)
      cerr << "*** Basic blocks must be secondary (nested) or body of SgOmpBodyStatement. ***" << endl;
    return false;
  }

  if (isForInitOrTest (s))
  {
    if (verbose)
      cerr << "*** Can't outline for-init or for-test statements. ***" << endl;
    return false;
  }

  if (hasVAStart (s))
  {
    if (verbose)
      cerr << "*** Can't outline va_start(). ***" << endl;
    return false;
  }

  if (ASTtools::isIfCond (s)
      || ASTtools::isWhileCond (s)
      || ASTtools::isSwitchCond (s))
  {
    if (verbose)
      cerr << "*** Can't outline the condition of an 'if', 'while', or 'switch' statement. ***" << endl;
    return false;
  }
  // Liao, 12/26/2007, Masked array assignment in Fortran 90 and later is not outlineable
  // SgWhereStatement-> SgBasicBlock->SgExprStatement ->SgAssignOp
  // SgElseWhereStatement-> SgBasicBlock->SgExprStatement ->SgAssignOp
  if (isSgExprStatement(s))
  {
    const SgExprStatement *exp1 = isSgExprStatement(s);
    SgAssignOp *assign1 = isSgAssignOp(exp1->get_expression());
    if((assign1)&&(isSgBasicBlock(s->get_parent())))
    { SgNode * grandpa = s->get_parent()->get_parent();
      if((isSgWhereStatement(grandpa))||(isSgElseWhereStatement(grandpa)))
      {
        if (verbose)
          cerr << "*** Can't outline array assignment statement within a WHERE block***" << endl;
        return false;
      }
    }
  }
  //SgWhereStatement could be nested within another SgWhereStatement
  // SgWhereStatement-> SgBasicBlock->SgWhereStatement
  // It is not outlineable in this case
  if (isSgWhereStatement(s))
  {
    const SgWhereStatement *where1 = isSgWhereStatement(s);
    if (isSgBasicBlock(where1->get_parent()))
      if (isSgWhereStatement(where1->get_parent()->get_parent()))
      {
        if (verbose)
          cerr << "*** Can't outline WHERE stmt nested within another WHERE stmt***" << endl;
        return false;
      }
  }

  // Passes all checks
  return true;
}

// eof
