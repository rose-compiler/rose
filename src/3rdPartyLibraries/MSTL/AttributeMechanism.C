// Author: Markus Schordan
// $Id: AttributeMechanism.C,v 1.5 2008/01/08 02:56:19 dquinlan Exp $

#ifndef ATTRIBUTEMECHANISM_C
#define ATTRIBUTEMECHANISM_C

// DQ (4/23/2006): Required for g++ 4.1.0!
#include "assert.h"

#include "AttributeMechanism.h"

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
  iterator iter=find(name);
  return iter!=end();
}

template<class Key, class Value>
void 
AttributeMechanism<Key,Value>::add(Key name, Value data)
   {
     if (!exists(name))
        {
          insert(MapValueType(name,data));
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
  iterator iter=find(name);
  if(iter!=end()) {
    erase(name); // map erase
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
	 << "does not exist." << std::endl;
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
