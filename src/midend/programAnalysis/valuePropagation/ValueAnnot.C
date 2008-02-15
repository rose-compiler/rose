#include <general.h>
#include <fstream>
#include <iostream>
#include <CPPAstInterface.h>
#include <AnnotExpr.h>
#include <ValueAnnot.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

bool DebugValuePropogate();

void HasValueDescriptor::replace_var( const string& name, const SymbolicVal& repl)
{
  for (iterator p = begin(); p != end(); ++p) {
    SymbolicValDescriptor& cur = (*p).second;
    cur.replace_var(name, repl);
  }
}

bool HasValueDescriptor::
has_value( const string& name, SymbolicValDescriptor* r) const
    {
      const_iterator p = find(name);
      if (p == end())
        return false;
      if (r != 0)
        *r = (*p).second;
      return true;
    }

bool HasValueDescriptor::merge (const HasValueDescriptor& that)
{
  bool change = false;;
  for (const_iterator p = that.begin(); p != that.end(); ++p) {
    const pair<string, SymbolicValDescriptor>& cur = *p;
    if (operator[](cur.first).merge(cur.second))
       change = true;
  }
  return change;
}

void HasValueDescriptor :: Dump() const
{
  write(cerr);
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

bool ValueAnnotation ::
known_type( const AstNodePtr& exp, HasValueDescriptor* r)
{
  if (!values.known_type( exp, r))
    return false;
  if (r != 0)
    r->replace_var( "this", SymbolicAstWrap(exp));
  return true;
}

class ReplaceValue 
  : public MapObject<SymbolicVal, SymbolicVal>, public SymbolicVisitor
{
   MapObject<SymbolicVal, SymbolicVal>* valmap;
   bool succ;

  virtual void VisitAstWrap( const SymbolicAstWrap& v)
  {
     AstNodePtr ast = v.get_ast();
     AstNodeType type;
     if (!AstInterface::IsExpression( ast, &type))
        succ = false;
     else if (!AstInterface::IsScalarType(type))
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
   ReplaceValue( MapObject<SymbolicVal, SymbolicVal>* _valmap = 0) 
         : valmap(_valmap), succ(false) {}
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
is_value_restrict_op( const AstNodePtr& exp, 
                    Collect2Object< AstNodePtr, HasValueDescriptor>* descp,
                     MapObject<SymbolicVal, SymbolicVal>* valMap,
                     Map2Object<AstInterface*, AstNodePtr, AstNodePtr>* astcodegen)
{
  RestrictValueOpDescriptor desc;
  if (!valueRestrict.known_operator( exp, 0, &desc, true, astcodegen))
    return false;
  if (descp == 0)
     return true;
  ReplaceValue repl( valMap);
  for (RestrictValueOpDescriptor::const_iterator p = desc.begin(); 
        p != desc.end(); ++p) {
      RestrictValueDescriptor cur = *p; 
      AstNodePtr curast = cur.first.get_val().ToAst();
      assert(curast != 0);
      HasValueDescriptor curval = cur.second;
      if (repl(curval)) {
         if (DebugValuePropogate()) {
             cerr << "found restrict value : " << AstInterface::AstToString(curast) << ":" << AstInterface::AstToString(exp);
             curval.Dump();
             cerr << endl;
          }
         (*descp)( curast, curval);
      }
      else {
        if (DebugValuePropogate()) {
             cerr << "discard restrict value : " << AstInterface::AstToString(curast) << ":" << AstInterface::AstToString(exp);
             curval.Dump();
             cerr << endl;
          }
      }
  }
  return true;
}

bool ValueAnnotation::
is_access_value( const AstNodePtr& exp, AstNodePtr* obj, string* name,
		 AstInterface::AstNodeList* args, HasValueDescriptor* desc)
{
  string funcname = values.is_known_member_function( exp, obj, args, desc);
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
