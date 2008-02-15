#include <general.h>

#include <TypeAnnotation.h>
#include <CPPAstInterface.h>
#include <AnnotExpr.h>
#include <sstream>
#include <list>


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

extern bool DebugAnnot();

template <class TargetInfo, char sep1, char sep2, char end>
class ReadAnnotCollectionWrap {
  const TargetInfo& target;
  ReadAnnotCollection<TargetInfo, sep1, sep2, end>& op;

 public:
  ReadAnnotCollectionWrap(const TargetInfo& t, 
			  ReadAnnotCollection<TargetInfo,sep1,sep2,end>& _op) 
        : target(t), op(_op) {}
  bool read (istream& in)
   { 
      if (peek_ch(in) == end)
          return false;
      string annot = read_id(in);
      if (annot == "")
         return false;
      AnnotCollectionBase<TargetInfo> * cur = 0;
      for (typename ReadAnnotCollection<TargetInfo,sep1,sep2,end>::iterator 
	     p = op.begin(); 
	   p != op.end(); ++p) {
         cur = *p;
         if (cur->read_annot_name(annot)) {
	   cur->read_descriptor( target, annot, in);
	   break;
         }
         cur = 0;
      }
      if (cur == 0) {
         throw ReadError("unknown annotation: " + annot );
      }
      return true;
   }
};


template <class TargetInfo, char sep1, char sep2, char e>
void ReadAnnotCollection<TargetInfo,sep1,sep2,e>:: read( istream& in)
{
  TargetInfo target;
  try {
     target.read(in);
     if (sep1 != 0)
        read_ch(in, sep1);
     ReadAnnotCollectionWrap<TargetInfo,sep1,sep2,e> op(target, *this);
     read_list( in, op, sep2);
     if (e != 0)
       read_ch(in, e);
  }
  catch (ReadError m) {
     cerr << "error reading annotation: ";
     target.write(cerr);
     cerr << endl << m.msg << endl;
     throw(m);
  }

}

template <class Descriptor>
void TypeCollection<Descriptor>:: Dump() const
{
  for (const_iterator p = begin(); p != end(); ++p) {
    cerr << " ";
    p.get_type().Dump();
    cerr << " : ";
    p.get_desc().Dump();
    cerr << endl;
  }
}

template <class Descriptor>
bool TypeCollection<Descriptor>::
   known_type( const TypeDescriptor &name, Descriptor* desc)  const
     { 
       const_iterator p = typemap.find(name); 
       if (p != typemap.end()) {
         if (desc != 0)
            *desc = (*p).second;
	 if (DebugAnnot()) 
	    cerr << "recognized type: " << name.get_string() << endl;
         return true;
       }
       if (DebugAnnot()) 
	    cerr << "not recognize type: " << name.get_string() << endl;
       return false;
     }
template <class Descriptor>
bool TypeCollection<Descriptor>:: 
  known_type( const AstNodePtr& exp, Descriptor* desc) const
    {
      AstNodeType type;
      if (!AstInterfaceBase::IsExpression(exp, &type))
         return false;
      string tname = AstInterfaceBase::GetTypeName(type);
      return known_type( tname, desc);
    }
template <class Descriptor>
bool TypeCollection<Descriptor>:: 
  known_type( const AstNodeType& type, Descriptor* desc) const
    {
      string tname = AstInterfaceBase::GetTypeName(type);
      return known_type( tname, desc);
    }
template <class Descriptor>
string TypeCollection<Descriptor>::
is_known_member_function( const SymbolicVal& exp, AstNodePtr* objp,
                        SymbolicFunction::Arguments* argsp , Descriptor* descp )
{
 SymbolicFunction r1, r2;
 if (!ToSymbolicFunction()(exp, r1))
      return "";
 if (!ToSymbolicFunction()(r1.GetOp(), r2) || r2.GetOp().ToString() != "." || r2.NumOfArgs() != 2)
      return "";
 AstNodePtr obj = r2.GetArg(0).ToAst();
 if (obj == 0 || !known_type(obj, descp))
     return "";
 if (objp != 0)
    *objp = obj;
 if (argsp != 0)
    *argsp = r1.GetArgs();
 return r2.GetArg(1).ToString();
}

template <class Descriptor>
string TypeCollection<Descriptor>::
is_known_member_function( const AstNodePtr& exp,
			  AstNodePtr* objp, AstInterface::AstNodeList* args,
			  Descriptor* desc)
{
  AstNodePtr func, obj;
  if (!CPPAstInterface::IsMemberAccess( exp, &obj, &func) && 
     !CPPAstInterface::IsMemberFunctionCall(exp, &obj, &func, 0, args))
     return "";
  if (obj == 0)
    return "";
  if (known_type( obj, desc)) {
    if (objp != 0)
        *objp = obj;
    return AstInterface::GetFunctionName(func);
  }
  return "";
}

template <class Descriptor>
SymbolicVal TypeCollection<Descriptor>::
create_known_member_function( const AstNodePtr& obj, const string& memname,
                                     const SymbolicFunction::Arguments& args)
{
  SymbolicDotExp func(new SymbolicAstWrap(obj), new SymbolicConst(memname, "memberfunction") );
  return SymbolicFunction( func, args);
}

template <class Descriptor>
AstNodePtr TypeCollection<Descriptor>::
create_known_member_function( AstInterface& fa, const AstNodePtr& obj, const string& memname,
                                   const AstInterface::AstNodeList& args)
{
  SymbolicDotExp op(new SymbolicAstWrap(obj), new SymbolicConst(memname, "memberfunction") );
  AstNodePtr func = op.CodeGen(fa);
  return fa.CreateFunctionCall( func, args);
}

template <class Descriptor>
void TypeAnnotCollection<Descriptor>:: 
read_descriptor( const TypeDescriptor& name, const string& annotName, 
		 istream& in)
{
   Descriptor d;
   d.read(in);
   add_annot( name, d );
}

#ifndef TEMPLATE_ONLY
#define TEMPLATE_ONLY
#include <AnnotDescriptors.h>
template class ReadAnnotCollection<TypeDescriptor, '{', ';','}'>;
#endif
