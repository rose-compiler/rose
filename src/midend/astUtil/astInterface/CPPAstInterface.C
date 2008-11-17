#include "sage3.h"

#include "CPPAstInterface.h"
#include "AstInterface_ROSE.h"
using namespace std;
//! Check if a node is a data member access function for an object, 
// such as dot (object.a) or arrow (objectptr->a)expression
// If yest, return the object, and the data field name
bool CPPAstInterface ::
IsMemberAccess( const AstNodePtr& _s,  AstNodePtr* obj, std::string* field)
{
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  SgVarRefExp* var = 0;
  switch (s->variantT()) {
  case V_SgDotExp:
      {
        SgDotExp* dot = isSgDotExp(s);
        if (obj != 0)  
          *obj = AstNodePtrImpl(dot->get_lhs_operand());
        if (field != 0 && (var = isSgVarRefExp(dot->get_rhs_operand())) != 0)
          *field = var->get_symbol()->get_name().str();
      }
      break;
  case V_SgArrowExp:
      {
        SgArrowExp* arrow = isSgArrowExp(s);
        if (obj != 0)
          *obj = AstNodePtrImpl(arrow->get_lhs_operand());
        if (field != 0 && (var = isSgVarRefExp(arrow->get_rhs_operand())) != 0)
          *field = var->get_symbol()->get_name().str();
      }
      break;
   default:
      return false;
   }
  return true;
}

AstNodePtr CPPAstInterface::
CreateFunctionCall( const AstNodePtr& func, const AstNodeList& args)
{
  return AstNodePtrImpl(impl->CreateFunctionCall(AstNodePtrImpl(func).get_ptr(), args));
}

//Check if a node '_s' is a member function call of an object
// If yes, store the object into 'obj', member function name into 'func', function arguments into '_args'
// and the dot or arrow expressions nodes into 'access'
bool CPPAstInterface :: 
IsMemberFunctionCall( const AstNodePtr& _s,  AstNodePtr* obj, 
                      std::string* func,
                           AstNodePtr* access, AstNodeList* _args)
{
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  SgNode* f;
  AstInterface::AstNodeList args;
  if (!impl->IsFunctionCall(s, &f, &args)) {
    return false;
  }
  //Excluding member function reference expressions.
  switch (f->variantT()) {
    case V_SgMemberFunctionRefExp:
        break;
    default:
       return false;
  }
  //Store member function access nodes: dot, or arrow expressions in ROSE AST
  if (access != 0) {
     AstNodePtrImpl dot = f->get_parent();
     assert( dot->variantT() == V_SgDotExp ||
             dot->variantT() == V_SgArrowExp);
     *access = dot;
  }
  //Store the member function's name
  if (func != 0)
    *func = isSgMemberFunctionRefExp(f)->get_symbol()->get_name().str();

  //Store object from the first argument
  if (obj != 0) {
     assert(args.size() > 0);
     *obj = args.front();
  }
  //Store function call arguments, excluding the firt one
  if (_args != 0) {
     args.erase( args.begin());
     *_args = args;
   }
  return true;
}
//! Check if a node is a variable reference to pointer or C++ reference type variables
bool CPPAstInterface :: IsPointerVariable( const AstNodePtr& _n)
{
  SgNode* n = AstNodePtrImpl(_n).get_ptr();
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

AstNodePtr CPPAstInterface :: GetVarScope( const AstNodePtr& _n)
{
  SgNode* n = AstNodePtrImpl(_n).get_ptr();
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
  return AstNodePtrImpl(scope);
}

bool CPPAstInterface :: IsPlusPlus( const AstNodePtr& _s, AstNodePtr* opd)
{
  AstNodePtrImpl s(_s);
  AstNodePtr op;
  std::string fname;
  AstNodeList args;
  if ( IsFunctionCall(s, &op, &args) && IsVarRef(op,0,&fname) 
       && strstr(fname.c_str(),"operator++") != 0) {
      if (opd != 0) *opd = args.front();
      return true;
  }
  if ( s->variantT() == V_SgPlusPlusOp)  {
      if (opd != 0) *opd = AstNodePtrImpl(isSgPlusPlusOp(s.get_ptr())->get_operand());
      return true;
  }
  return false;
}

