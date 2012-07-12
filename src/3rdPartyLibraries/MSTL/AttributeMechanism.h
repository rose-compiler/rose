// Author: Markus Schordan
// $Id: AttributeMechanism.h,v 1.4 2006/04/24 00:21:27 dquinlan Exp $

#ifndef ATTRIBUTEMECHANISM_H
#define ATTRIBUTEMECHANISM_H

#include <map>
#include <string>
#include <iostream>
#include <set>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

/*! \class AttributeMechanism AttributeMechanism.h
     \ingroup multiSupportTemplateLibrary
     \brief This class defines a mapping mechanism, to map keys (names) to a representation of attributes.
     \authors Markus Schordan
     \version 0.5
     \date Dec 16th 2002
     \bug No known bugs.

The attribute mechanism allows to add/remove/replace/access
attributes. An attribute is refered to by 'key'. The value of
an attribute is of type 'Value'. 

The interface is designed to be restrictive in how attributes can be
added and removed. Check the respective documentation for each method.

For example, let 'attribute' be of type AttributeMechanism and a data
member of 'mynodeobject'. Then the existence of an attribute can be
checked with mynodeobject->attribute.exists("attributename"). It can
be accessed using mynodeobject->attribute["attributename"] - this
access failes if the attribute does not exist.

In general this class does not throw exceptions if one of the
operations fails - it asserts that the performed operation
succeeds. Therefore, if it is possible that an attribute may not exist
(yet) it can be checked using method 'exists' and accessed afterwards.

\internal This class is implemented by using an STL map.

*/

template<class Key,class Value>
class AttributeMechanism : protected std::map<Key,Value>
   {
     public:
          typedef std::set<Key> AttributeIdentifiers; 
          std::map<Key,Value>::end;
          std::map<Key,Value>::begin;

          AttributeMechanism();
         ~AttributeMechanism();

       //! test if attribute "name" exists (i.e. has been added with add or set)
          bool exists(Key name);

       //! add a new attribute. If attribute already exists, fail.
          void add(Key name, Value data);

       //! replace an existing attribute "name", fail if the attribute does not exist
          void replace(Key name, Value data);

       //! remove an existing attribute name, fail if the attribute does not exist
          void remove(Key name);

       //! Set a value data for attribute name. If the attribute exists
       //! it is replaced with replace(name,data). If the attribute does
       //! not exist it is added with add(name, data).
          void set(Key name, Value data);

       //! get the set of all attribute identifiers/names
          typename AttributeMechanism<Key,Value>::AttributeIdentifiers getAttributeIdentifiers();

       //! access the value of attribute "name". Fails if the attribute does not exist.
          Value operator[](Key name);

       // DQ (1/2/2006): Added member function to return number of attributes in the container
          int size()
             {
               return std::map<Key,Value>::size();
             }
       
      protected:
        typedef std::map<Key,Value> MapType;
        typedef typename MapType::value_type MapValueType;
      // Liao, 2/26/2008, support iterator
      public:
         typedef typename MapType::iterator iterator;
         iterator begin()
            {  return std::map<Key,Value>::begin();}
         iterator end()
            {  return std::map<Key,Value>::end();}

   };

template<class Key, class Value> 
AttributeMechanism<Key,Value>::AttributeMechanism() {
}

template<class Key, class Value> 
AttributeMechanism<Key,Value>::~AttributeMechanism()
   {
#if 0
  // DQ (12/5/2004): This is an error now that we have ROSE generated destructors 
  // (error is in AST rewrite test codes (replace statements)). Since an AttributeMechanism 
  // is now part of a SgNode now has an attribute (ASTAttributeMechanism) the error 
  // occurs when a SgName destructor is called.

  // MS: The following code is not backward compatible with gcc 2.96,
  // therefore I make it dependent on the compiler version
#if __GNUC__ >= 3
     typename AttributeMechanism<Key,Value>::AttributeIdentifiers names=getAttributeIdentifiers();
     for(typename AttributeMechanism<Key,Value>::AttributeIdentifiers::iterator nameit=names.begin(); nameit != names.end(); nameit++)
#else 
     AttributeMechanism<Key,Value>::AttributeIdentifiers names=getAttributeIdentifiers();
     for(AttributeMechanism<Key,Value>::AttributeIdentifiers::iterator nameit=names.begin(); nameit != names.end(); nameit++)
#endif
        {
          remove(*nameit);
        }
#endif
   }

template<class Key, class Value>
bool 
AttributeMechanism<Key,Value>::exists(Key name) {
  iterator iter=std::map<Key,Value>::find(name);
  return iter!=end();
}

template<class Key, class Value>
void 
AttributeMechanism<Key,Value>::add(Key name, Value data)
   {
     if (!exists(name))
        {
          std::map<Key,Value>::insert(MapValueType(name,data));
        }
       else
        {
          std::cerr << "Error: add failed. Attribute: " << name << " exists already." << std::endl;
          assert(false);
        }
   }

template<class Key, class Value>
void 
AttributeMechanism<Key,Value>::replace(Key name, Value data) {
  if(exists(name)) {
    remove(name);
    add(name,data);
  } else {
    std::cerr << "Error: replace failed. Attribute: " << name << " exists already." << std::endl;
    assert(false);
  }
}

template<class Key, class Value>
void 
AttributeMechanism<Key,Value>::remove(Key name) {
  iterator iter=std::map<Key,Value>::find(name);
  if(iter!=end()) {
    std::map<Key,Value>::erase(name); // map erase
  } else {
    std::cerr << "Error: remove failed. Attribute: " << name 
	 << "exists already." << std::endl;
    assert(false);
  }
}

template<class Key, class Value>
void 
AttributeMechanism<Key,Value>::set(Key name, Value data) {
  if(exists(name)) {
    replace(name,data);
  } else {
    add(name,data);
  }
}

template<class Key, class Value>
Value 
AttributeMechanism<Key,Value>::operator[](Key name) {
  if(exists(name)) {
    return MapType::operator[](name);
  } else {
    std::cerr << "Error: access [" << name << "] failed. Attribute: " << name 
	 << " does not exist. Please check if it exists before getting it." << std::endl;
    assert(false);
  }
}

template<class Key, class Value>
typename AttributeMechanism<Key,Value>::AttributeIdentifiers
AttributeMechanism<Key,Value>::getAttributeIdentifiers() {
  typename AttributeMechanism<Key,Value>::AttributeIdentifiers idents;
  for(iterator iter=begin();
      iter!=end();
      iter++) {
    idents.insert((*iter).first);
  }
  return idents;
}

#endif


