
#ifndef ___TYPE_ANNOTATION_H
#define ___TYPE_ANNOTATION_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "AnnotDescriptors.h"
#include "SymbolicVal.h"

template <class TargetInfo>
class  AnnotCollectionBase {
 public:
  virtual bool read_annot_name( const std::string& annotName) const = 0;
  virtual void read_descriptor( const TargetInfo& target,
				const std::string& annotName, 
				std::istream& in) = 0;
  virtual ~AnnotCollectionBase() {}
};

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

typedef AnnotCollectionBase<TypeDescriptor> TypeCollectionBase;
typedef ReadAnnotCollection<TypeDescriptor, '{', ';','}'> ReadTypeCollection;

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
  
  bool known_type( const TypeDescriptor &name, Descriptor* desc = 0)  const;
  bool known_type( AstInterface& fa, const AstNodePtr& exp, 
		   Descriptor* desc = 0) const;
  bool known_type( AstInterface& fa, const AstNodeType& exp, 
		   Descriptor* desc = 0) const;
  void Dump() const;
};

template <class Descriptor>
class TypeAnnotCollection 
: public AnnotCollectionBase<TypeDescriptor>, 
  public TypeCollection<Descriptor>
{
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
