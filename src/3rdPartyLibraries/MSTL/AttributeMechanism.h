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

// #ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 
   #include "AttributeMechanism.C" 
// #endif 

#endif


