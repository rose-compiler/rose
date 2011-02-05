
#ifndef ___TYPE_ANNOTATION_H
#define ___TYPE_ANNOTATION_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "AnnotDescriptors.h"
#include "SymbolicVal.h"
//! An interface to a single annotation item
template <class TargetInfo>
class  AnnotCollectionBase {
 public:
  //! Check if annotName matches the current annotation type name (TargetInfo)
  // The name is one of the following:
  //    array(is-array?), construct_array(new-array), modify_array, array_optimize,
  //    has_value, alias/allow_alias,
  //    inline, modify, read, restrict_value, ??(inheritable)
  virtual bool read_annot_name( const std::string& annotName) const = 0;
  virtual void read_descriptor( const TargetInfo& target,
                                const std::string& annotName, 
                                std::istream& in) = 0;
  virtual ~AnnotCollectionBase() {}
};

//! An interface to read and store a collection of annotations for a type or an operator
// TargetInfo is result's type, targetsep is begin char, annotend is the end char
// descriptorsep is the delimitor
//e.g. 
//         ReadAnnotCollection<TypeDescriptor,      '{', ';','}'> ReadTypeCollection;
// typedef ReadAnnotCollection<OperatorDeclaration, '{', ';','}'> ReadOperatorCollection;
template <class TargetInfo, char targetsep, char descriptorsep, char annotend>
class ReadAnnotCollection {
  std::vector <AnnotCollectionBase<TargetInfo>*> col;

 public:
  typedef typename std::vector<AnnotCollectionBase<TargetInfo> *>::const_iterator const_iterator;
  typedef typename std::vector<AnnotCollectionBase<TargetInfo> *>::iterator iterator;

  void push_back( AnnotCollectionBase<TargetInfo>* op) { col.push_back(op); }

  const_iterator begin() const { return col.begin(); }
  const_iterator end() const { return col.end(); }
  iterator begin() { return col.begin(); }
  iterator end() { return col.end(); }

  void read( std::istream& in);
};
//! Instance types for type annotations
typedef AnnotCollectionBase<TypeDescriptor> TypeCollectionBase;
typedef ReadAnnotCollection<TypeDescriptor, '{', ';','}'> ReadTypeCollection;

//! A map storing types and their descriptors, the descriptor could be any instantiated types as a template class.
// Descriptor could be either of has_value, array, array_opt annoation types
template <class Descriptor>
class TypeCollection 
{
 protected:
  std::map <std::string, Descriptor> typemap;

 public:
    class const_iterator 
      : public std::map<std::string,Descriptor>::const_iterator 
      {
      public:
        const_iterator( const typename std::map< std::string,Descriptor>::const_iterator& that) 
          : std::map<std::string, Descriptor>::const_iterator(that) {}
        TypeDescriptor get_type() const 
            { return std::map<std::string,Descriptor>::const_iterator::operator*().first; }
        const Descriptor& get_desc() const 
            { return  std::map<std::string,Descriptor>::const_iterator::operator*().second; }
     };

  const_iterator begin() const { return typemap.begin(); }
  const_iterator end() const { return typemap.end(); }
  //Check if a named type 'name' is a type with annotation descriptor records
  bool known_type( const TypeDescriptor &name, Descriptor* desc = 0)  const;
  bool known_type( AstInterface& fa, const AstNodePtr& exp, 
                   Descriptor* desc = 0) const;
  bool known_type( AstInterface& fa, const AstNodeType& exp, 
                   Descriptor* desc = 0) const;
  void Dump() const;
};


template <class Descriptor>
class TypeAnnotCollection 
: public AnnotCollectionBase<TypeDescriptor>, //== TypeCollectionBase
  public TypeCollection<Descriptor>
{  // Derived from TypeCollectionBase
  virtual void read_descriptor( const TypeDescriptor& targetname, 
                                const std::string& annot, std::istream& in);
 protected:
  TypeCollection<Descriptor>::typemap;
 public:
  void add_annot( const TypeDescriptor& name, const Descriptor& d)
      {
    // pmp 08JUN05
    //   was: typemap[name] = d;
       this->typemap[name] = d;
      }
};

#endif
