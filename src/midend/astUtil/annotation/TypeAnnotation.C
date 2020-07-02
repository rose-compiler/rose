
#include "TypeAnnotation.h"
#include "AnnotExpr.h"
#include <sstream>
#include <list>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

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
      // Read in the next identifier as the annotation type name
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
         throw ReadError("unknown annotation: \"" + annot + "\"");
      }
      return true;
   }
};

//! Read a collection of annotations of type TargeInfo (a typedescriptor or an operator declaration)
// They begin after 'sep1', separated by 'sep2', and end with 'e'
template <class TargetInfo, char sep1, char sep2, char e>
void ReadAnnotCollection<TargetInfo,sep1,sep2,e>:: read( istream& in)
{
  TargetInfo target;
  try {
     // Read in the string for a type descriptor or an operator declaration
     target.read(in);
     // Read in the start character for annotation collections
     if (sep1 != 0)
        read_ch(in, sep1);

     ReadAnnotCollectionWrap<TargetInfo,sep1,sep2,e> op(target, *this);
     read_list( in, op, sep2);
     // Read in the end character.
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
  known_type( AstInterface& fa, const AstNodePtr& exp, Descriptor* desc) const
    {
      AstNodeType type;
      if (fa.IsExpression(exp, &type)==AST_NULL)
         return false;
      std::string tname;
      fa.GetTypeInfo(type, 0, &tname);
      return known_type( tname, desc);

    }
template <class Descriptor>
bool TypeCollection<Descriptor>::
  known_type( AstInterface& fa, const AstNodeType& type, Descriptor* desc) const
    {
      std::string tname;
      fa.GetTypeInfo(type, 0, &tname);
      return known_type( tname, desc);
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
