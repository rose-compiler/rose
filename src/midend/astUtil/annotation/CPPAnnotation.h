
#ifndef ___CPP_ANNOTATION_H
#define ___CPP_ANNOTATION_H

#include "TypeAnnotation.h"
#include "CPPAstInterface.h"
#include "SymbolicVal.h"
#include "CPPAstInterface.h"
#include "AnnotExpr.h"
#include <sstream>
#include <list>
// No CPPAnnotation.C since only template classes are defined here.
// Descriptor could be ArrayDefineDescriptor: is_array annotation
template <class Descriptor>
class CPPTypeCollection 
{
  TypeCollection<Descriptor>* cp;
 public:
  CPPTypeCollection(TypeCollection<Descriptor>* p) : cp(p) {}
  typedef typename TypeCollection<Descriptor>::const_iterator const_iterator;
  //! Check if an expression is a member access or member function call expression of a known array type 
  // (Only check the array type, not the further data/function member name!!)
  // If yes, store the object of the member function, argument list , and the collected descriptor for this class
  std::string is_known_member_function( CPPAstInterface& fa, const AstNodePtr& exp, 
                                   AstNodePtr* obj = 0, 
                                   AstInterface::AstNodeList* args = 0,
                                   Descriptor* desc = 0);                          
  std::string is_known_member_function( CPPAstInterface& fa, const SymbolicVal& exp,
                                   AstNodePtr* obj = 0,
                                   SymbolicFunction::Arguments* args = 0,
                                   Descriptor* desc = 0);
  SymbolicVal create_known_member_function( const AstNodePtr& obj, const std::string& memname,
                                     const SymbolicFunction::Arguments& args);
  AstNodePtr create_known_member_function( CPPAstInterface& fa, const AstNodePtr& obj, 
                                           const std::string& memname,
                                            const AstInterface::AstNodeList& args);
};

extern bool DebugAnnot();

template <class Descriptor>
std::string CPPTypeCollection<Descriptor>::
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
// Check if an expression is an member access/member function call of a known array type.
template <class Descriptor>
std::string CPPTypeCollection<Descriptor>::
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
create_known_member_function( const AstNodePtr& obj, const std::string& memname,
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
                               const AstNodePtr& obj, const std::string& memname,
                                   const AstInterface::AstNodeList& args)
{
  SymbolicDotExp op(new SymbolicAstWrap(obj), new SymbolicConst(memname, "memberfunction") );
  AstNodePtr func = op.CodeGen(fa);
  return fa.CreateFunctionCall( func, args);
}

#endif


