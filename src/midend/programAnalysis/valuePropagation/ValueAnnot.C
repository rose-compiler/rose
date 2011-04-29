
#include <fstream>
#include <iostream>
#include <sstream>
#include <CPPAstInterface.h>
#include <AnnotExpr.h>
#include <ValueAnnot.h>

bool DebugValuePropogate();

void HasValueDescriptor::replace_var( const std::string& name, const SymbolicVal& repl)
{
  for (iterator p = begin(); p != end(); ++p) {
    SymbolicValDescriptor& cur = (*p).second;
    cur.replace_var(name, repl);
  }
}

bool HasValueDescriptor::
has_value( const std::string& name, SymbolicValDescriptor* r) const
    {
      const_iterator p = find(name);
      if (p == end())
        return false;

      if (r != 0)
         {
// DQ (8/30/2009): Debugging ROSE compiling ROSE (this statement does not compile using ROSE).
#ifndef USE_ROSE
           *r = (*p).second;
#endif
         }

      return true;
    }


bool HasValueDescriptor::merge (const HasValueDescriptor& that)
{
  bool change = false;;

  for (const_iterator p = that.begin(); p != that.end(); ++p) {

// DQ (8/30/2009): Debugging ROSE compiling ROSE (this statement does not compile using ROSE).
#ifndef USE_ROSE
    if (operator[]((*p).first).merge((*p).second))
       change = true;
#endif
  }

  return change;
}

std::string HasValueDescriptor :: toString() const
{
  std::stringstream out;
  write(out);
  return out.str();
}

void HasValueDescriptor :: replace_val( MapObject<SymbolicVal, SymbolicVal>& repl)
{
      for (iterator p1 = begin(); p1 != end(); ++p1) {
         SymbolicValDescriptor& r = (*p1).second;
         r.replace_val( repl);
      }
}

void RestrictValueOpDescriptor :: replace_val( MapObject<SymbolicVal, SymbolicVal>& repl) 
{
  for (iterator p = begin(); p != end(); ++p) {
      RestrictValueDescriptor& cur = *p;
      cur.first.replace_val( repl);
      cur.second.replace_val( repl);
  }
}

std::string HasValueCollection::
is_known_member_function( AstInterface& fa,
                        const SymbolicVal& exp, AstNodePtr* objp,
                        SymbolicFunction::Arguments* argsp , 
                        HasValueDescriptor* descp )
{
 std::string op1, op2;
 SymbolicFunction::Arguments arg1, arg2;
 if (!exp.isFunction(op1,&arg1))
      return "";
 if (op1 != "FunctionPtrCall" ||
     !arg1.front().isFunction(op2,&arg2) || op2 != "." || arg2.size() != 2)
      return "";
 AstNodePtr obj;
 if (!arg2.front().isAstWrap(obj) || !known_type(fa, obj, descp))
     return "";
 if (objp != 0)
    *objp = obj;
 if (argsp != 0)
    *argsp = arg1;
 return arg2.back().toString();
}
//! Check if an expression 'exp' is a member function call to an object 'objp'
// If yes, return the member function's name, store function arguments, and 
// store the has_value descriptor of the object (desc)
std::string HasValueCollection::
is_known_member_function( CPPAstInterface& fa, const AstNodePtr& exp,
                          AstNodePtr* objp, AstInterface::AstNodeList* args,
                          HasValueDescriptor* desc)
{
  AstNodePtr obj;
  std::string func;
  // Return a null string if it is not data or function member access expressions to an object.
  if (!fa.IsMemberAccess( exp, &obj, &func) &&
     !fa.IsMemberFunctionCall(exp, &obj, &func, 0, args))
     return "";
  if (obj == AST_NULL)
    return "";
  // Return member function name if the object has a known type  
  if (known_type( fa, obj, desc)) {
    if (objp != 0)
        *objp = obj;
    return func;
  }
  return "";
}


ValueAnnotation* ValueAnnotation::inst = 0;
ValueAnnotation* ValueAnnotation::get_inst()
{
  if (inst == 0)
    inst = new ValueAnnotation();
  return inst;
}

void ValueAnnotation:: register_annot()
{
   ReadAnnotation* op = ReadAnnotation::get_inst();
   op->add_TypeCollection( &values);
   op->add_OperatorCollection(&valueRestrict);
}

void ValueAnnotation :: Dump() const
{
   values.Dump();
   valueRestrict.Dump();
}
// Check if an expression representing an class object has a known class type
// Store its has_value descriptor into "r" if the descriptor exists, also 
// replace its corresonding has_value annotations' "this" pointer with the concrete object references 'exp'
bool ValueAnnotation ::
known_type( AstInterface& fa, const AstNodePtr& exp, HasValueDescriptor* r)
{
  // values is HasValueCollection, which derives from TypeCollection and has access to known_type() of the base class
  if (!values.known_type(fa, exp, r))
    return false;
  if (r != 0)
    r->replace_var( "this", SymbolicAstWrap(exp));
  return true;
}

class ReplaceValue 
  : public MapObject<SymbolicVal, SymbolicVal>, public SymbolicVisitor
{
   MapObject<SymbolicVal, SymbolicVal>* valmap;
   AstInterface& fa;
   bool succ;

  virtual void VisitAstWrap( const SymbolicAstWrap& v)
  {
     AstNodePtr ast = v.get_ast();
     AstNodeType type;
     if (fa.IsExpression( ast, &type)==AST_NULL)
        succ = false;
     else if (!fa.IsScalarType(type))
        succ = false;
  }
  virtual SymbolicVal operator()( const SymbolicVal& v)
  {
      SymbolicVal r; 
      if (valmap != 0)
         r = (*valmap)(v);
      if (r.IsNIL()) { 
         v.Visit(this);
       }
       return r;
  }

 public:
   ReplaceValue( AstInterface& _fa, 
                 MapObject<SymbolicVal, SymbolicVal>* _valmap = 0) 
         : valmap(_valmap), fa(_fa), succ(false)  {}
   bool operator()( HasValueDescriptor& desc) 
   {
      bool onesucc = false;
      for (HasValueDescriptor::iterator p = desc.begin(); p != desc.end(); ++p) {
         SymbolicValDescriptor& r = (*p).second;
         succ = true;
         r.replace_val(*this);
         if (!succ)
           r = SymbolicVal();
         else
            onesucc = true;
      }
      return onesucc;
   }
};


bool ValueAnnotation::
is_value_restrict_op( AstInterface& fa, const AstNodePtr& exp, 
                    Collect2Object< AstNodePtr, HasValueDescriptor>* descp,
                     MapObject<SymbolicVal, SymbolicVal>* valMap,
                     Map2Object<AstInterface*, AstNodePtr, AstNodePtr>* astcodegen)
{
  RestrictValueOpDescriptor desc;
  if (!valueRestrict.known_operator( fa, exp, 0, &desc, true, astcodegen))
    return false;
  if (descp == 0)
     return true;
  ReplaceValue repl( fa, valMap);
  for (RestrictValueOpDescriptor::const_iterator p = desc.begin(); 
        p != desc.end(); ++p) {
      RestrictValueDescriptor cur = *p; 
      AstNodePtr curast;
      if (!cur.first.get_val().isAstWrap(curast))
         assert(false);
      HasValueDescriptor curval = cur.second;
      if (repl(curval)) {
         if (DebugValuePropogate()) {
             std::cerr << "found restrict value : " << AstToString(curast) << ":" << AstToString(exp);
             curval.Dump();
             std::cerr << std::endl;
          }
         (*descp)( curast, curval);
      }
      else {
        if (DebugValuePropogate()) {
             std::cerr << "discard restrict value : " << AstToString(curast) << ":" << AstToString(exp);
             curval.Dump();
             std::cerr << std::endl;
          }
      }
  }
  return true;
}

bool ValueAnnotation::
is_access_value( CPPAstInterface& fa, const AstNodePtr& exp, AstNodePtr* obj, std::string* name,
                 AstInterface::AstNodeList* args, HasValueDescriptor* desc)
{
  std::string funcname = values.is_known_member_function( fa, exp, obj, args, desc);
  if (funcname != "") {
    if (name != 0)
      *name = funcname;
    return true;
  }
  return false;
}

#ifndef TEMPLATE_ONLY
#define TEMPLATE_ONLY
#include <TypeAnnotation.C>
#include <AnnotDescriptors.C>
template class TypeAnnotCollection<HasValueDescriptor>;
template class TypeCollection<HasValueDescriptor>;
#endif


