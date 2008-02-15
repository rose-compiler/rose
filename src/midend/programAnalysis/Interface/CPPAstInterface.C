#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rose.h>
#include <CPPAstInterface.h>

bool CPPAstInterface :: IsMemberFunctionCall( const AstNodePtr& s)
{
  SgExprStatement *n = isSgExprStatement(s);
  SgNode *exp = (n != 0)? n->get_the_expr_i() : s;
  if (exp->variantT() == V_SgAssignInitializer)
       exp = isSgAssignInitializer(exp)->get_operand();
  SgFunctionCallExp *fs = isSgFunctionCallExp(exp);
  if (fs == 0)
    return false;
  SgExpression *func = fs->get_function();
  return func->variantT() == V_SgDotExp;
}

AstNodePtr CPPAstInterface :: GetMemberFunctionCallObject( const AstNodePtr& s)
{
  SgExprStatement *n = isSgExprStatement(s);
  SgNode *exp = (n != 0)? n->get_the_expr_i() : s;
  if (exp->variantT() == V_SgAssignInitializer)
       exp = isSgAssignInitializer(exp)->get_operand();
  SgFunctionCallExp *fs = isSgFunctionCallExp(exp);
  assert(fs != 0);
  SgExpression *func = fs->get_function();
  SgDotExp *dot = isSgDotExp(func);
  assert(dot != 0);
  return dot->get_lhs_operand();
}

bool CPPAstInterface :: IsPointerVariable( const AstNodePtr& n)
{
  SgVarRefExp *var = isSgVarRefExp(n);
  if (var == 0)
    return false;
  SgVariableSymbol *varsym = var->get_symbol();
  SgType *t = varsym->get_type();
  if (t->variantT() == V_SgPointerType || t->variantT() == V_SgReferenceType) {
    return true;
  }
  return false;
}

AstNodePtr CPPAstInterface :: GetVarScope( const AstNodePtr& n)
{
  SgInitializedName *decl = isSgInitializedName(n);
  if (decl == 0) {
     SgVarRefExp *var = isSgVarRefExp(n);
     assert (var != 0); 
     SgVariableSymbol *varsym = var->get_symbol();
     decl = varsym->get_declaration();
  }
  SgStatement* vardecl = isSgStatement(decl->get_parent());
  assert(vardecl != 0);
  SgScopeStatement *scope = vardecl->get_scope();
  return scope;
}

AstNodePtr CPPAstInterface :: GetLoopInit( const AstNodePtr& s)
{
  SgForStatement *fs = isSgForStatement(s);
  if (fs != 0) {
    SgStatementPtrList &init = fs->get_init_stmt();
    assert(init.size() == 1);
    return init.front();
  }
  return 0;
}

AstNodePtr CPPAstInterface :: GetLoopTest( const AstNodePtr& s)
{
  switch (s->variantT()) {
  case V_SgForStatement:

    // DQ (12/13/2006): A SgForStatement should always have a valid 
    // test expression (SgExpressionStmt containing an expression).
    ROSE_ASSERT(isSgForStatement(s)->get_test_expr() != NULL);

    return isSgForStatement(s)->get_test_expr();
  case V_SgWhileStmt:

    // DQ (12/13/2006): A SgForStatement should always have a valid 
    // test expression (SgExpressionStmt containing an expression).
    ROSE_ASSERT(isSgWhileStmt(s)->get_condition() != NULL);

    return isSgWhileStmt(s)->get_condition();
  case V_SgDoWhileStmt:

    // DQ (12/13/2006): A SgForStatement should always have a valid 
    // test expression (SgExpressionStmt containing an expression).
    ROSE_ASSERT(isSgDoWhileStmt(s)->get_condition() != NULL);

    return isSgDoWhileStmt(s)->get_condition();

  default:
    assert(false);
  }
  return 0;
}

AstNodePtr CPPAstInterface :: GetLoopIncr( const AstNodePtr& s)
{
  SgForStatement *fs = isSgForStatement(s);
  if (fs != 0) {
    return fs->get_increment_expr();
  }
  return 0;
}

bool CPPAstInterface :: IsPlusPlus( const AstNodePtr& s)
{
  if (s->variantT() == V_SgPlusPlusOp)
     return true;
  return IsInvocation(s) && GetInvocationName(s) == "operator++";
}

AstNodePtr CPPAstInterface :: GetPlusPlusOpd( const AstNodePtr& s)
{
  if (s->variantT() == V_SgPlusPlusOp)
      return isSgPlusPlusOp(s)->get_operand();
  return GetOverloadOperatorOpd1(s);
}
