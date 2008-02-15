#include <rose.h>
#include <general.h>
#include <CPPAstInterface.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

Boolean CPPAstInterface ::
IsMemberAccess( const AstNodePtr& s,  AstNodePtr* obj, AstNodePtr* field)
{
  switch (s->variantT()) {
  case V_SgDotExp:
      {
        SgDotExp* dot = isSgDotExp(s);
        if (obj != 0)  
          *obj = dot->get_lhs_operand();
        if (field != 0)
          *field = dot->get_rhs_operand();
      }
      break;
  case V_SgArrowExp:
      {
        SgArrowExp* arrow = isSgArrowExp(s);
        if (obj != 0)
          *obj = arrow->get_lhs_operand();
        if (field != 0)
          *field = arrow->get_rhs_operand();
      }
      break;
   default:
      return false;
   }
  return true;
}

Boolean CPPAstInterface :: 
IsMemberFunctionCall( const AstNodePtr& s,  AstNodePtr* obj, AstNodePtr* func,
                           AstNodePtr* access, AstNodeList* _args)
{
  AstNodePtr f;
  AstInterface::AstNodeList args;
  if (!IsFunctionCall(s, &f, &args)) {
    return false;
  }
  switch (f->variantT()) {
  case V_SgMemberFunctionRefExp:
        break;
  default:
       return false;
  }
  if (access != 0) {
     SgNode * dot = f->get_parent();
     assert( dot->variantT() == V_SgDotExp ||
             dot->variantT() == V_SgArrowExp);
     *access = dot;
  }
  if (func != 0)
    *func = f;
  if (obj != 0) {
     assert(args.size() > 0);
     *obj = args.front();
  }
  if (_args != 0) {
     args.erase( args.begin());
     *_args = args;
   }
  return true;
}

Boolean CPPAstInterface :: IsPointerVariable( const AstNodePtr& n)
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

Boolean CPPAstInterface :: IsPlusPlus( const AstNodePtr& s, AstNodePtr* opd)
{
  string fname;
  if ( IsUnaryOp(s, opd, &fname) && 
      (s->variantT() == V_SgPlusPlusOp || fname == "++" || fname == "operator++"))
      return true;
  if ( IsBinaryOp(s, opd, 0, &fname) && 
      (s->variantT() == V_SgPlusPlusOp || fname == "++" || fname == "operator++"))
      return true;
  return false;
}

