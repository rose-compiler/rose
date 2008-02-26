#include "CPPAnnotation.h"
#include "CPPAstInterface.h"
#include "AnnotExpr.h"
#include <sstream>
#include <list>


using namespace std;

extern bool DebugAnnot();

template <class Descriptor>
string CPPTypeCollection<Descriptor>::
is_known_member_function( CPPAstInterface& fa, const SymbolicVal& exp, AstNodePtr* objp,
                        SymbolicFunction::Arguments* argsp , Descriptor* descp )
{
 std::string op1, op2;
 SymbolicFunction::Arguments arg1,arg2;
 if (!exp.isFunction(op1,&arg1))
   return "";
 if (op1 != "FunctionPtrCall"  ||
     !arg1.front().isFunction(op2,&arg2) || 
     op2 != "." || arg2.size() != 2)
      return "";
 AstNodePtr obj;
 if (!arg2.front().isAstWrap(obj) || !cp->known_type(fa, obj, descp))
     return "";
 if (objp != 0)
    *objp = obj;
 if (argsp != 0) {
    argsp->clear();
    SymbolicFunction::Arguments::const_iterator i = arg1.begin();
    for (++i; i != arg1.end(); ++i) 
       argsp->push_back(*i);
 }
 return arg2.back().toString();
}

template <class Descriptor>
string CPPTypeCollection<Descriptor>::
is_known_member_function( CPPAstInterface& fa, const AstNodePtr& exp,
			  AstNodePtr* objp, AstInterface::AstNodeList* args,
			  Descriptor* desc)
{
  AstNodePtr obj;
  std::string fname;
  if (!fa.IsMemberAccess( exp, &obj, &fname) && 
     !fa.IsMemberFunctionCall(exp, &obj, &fname, 0, args))
     return "";
  if (obj == AST_NULL)
    return "";
  if (cp->known_type( fa, obj, desc)) {
    if (objp != 0)
        *objp = obj;
    return fname;
  }
  return "";
}

template <class Descriptor>
SymbolicVal CPPTypeCollection<Descriptor>::
create_known_member_function( const AstNodePtr& obj, const string& memname,
                                     const SymbolicFunction::Arguments& _args)
{
  SymbolicDotExp func(new SymbolicAstWrap(obj), new SymbolicConst(memname, "memberfunction") );
  SymbolicFunction::Arguments args;
  args.push_back(func);
  for (SymbolicFunction::const_iterator i = _args.begin(); i != _args.end();
        ++i)
     args.push_back(*i);
  return SymbolicFunctionPtrCall( args);
}

template <class Descriptor>
AstNodePtr CPPTypeCollection<Descriptor>::
create_known_member_function( CPPAstInterface& fa, 
                               const AstNodePtr& obj, const string& memname,
                                   const AstInterface::AstNodeList& args)
{
  SymbolicDotExp op(new SymbolicAstWrap(obj), new SymbolicConst(memname, "memberfunction") );
  AstNodePtr func = op.CodeGen(fa);
  return fa.CreateFunctionCall( func, args);
}

