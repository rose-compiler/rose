/*!
 * An interface to represent all memory locations as a set of abstract objects.
 * Greg Bronevetsky drafted the original design document, with some help from me.
 *
 * It is part of the work to support the Compiled MPI project 
 *
 *  Goal: We only define interfaces and abstract implementation.
 *        Any particular instantiation should provide more details.
 *
 * Liao 7/20/2011
 * */
#ifndef abstract_memory_object_INCLUDED
#define abstract_memory_object_INCLUDED

#include "sage3basic.h"
#include <climits>
#include <set>

namespace AbstractMemoryObject
{
  //Users should provide a concrete node implementation by default
  // We try not to use pure virtual functions here to allow 
  // users to reuse the default implementation as much as possible
  class objSet
  {
    // True if this set includes memory objects what May be the same
    bool maySet();

    // True if this set includes memory objects what Must be the same
    bool mustSet();

    // Return the number of real memory objects that this object set corresponds to. 
    // Returns a concrete number if it is known or the constant unboundedSize if no static bound is known.
    static const size_t unboundedSize=UINT_MAX;  
    size_t objCount(); // the number of real memory locations the abstract objSet corresponds to
    size_t memSize(); // the number of bytes of this objset takes up within memory

    // Type of memory
    // The types that all elements of the set may/must have. 
    // If the set of types this abstract object may have is unknown or very large, returned list contains an object of SgTypeUnknown. 
    // If the set of types the object must have is unknown or very large, returns an empty list.
    std::set<SgType*> getType(); 


    //   Object overlap relations: TODO skipped for the first implementation
    // Returns true if this object set and that object set may/must overlap. 
    // A separate version of this method is provided for each possible type of memory object set to make sure that 
    // if we add more memory object types, all implementations of the memory object abstraction will not 
    // type-check unless they provide explicit support for the new types.
    //bool overlaps(const scalar& that);
    //bool overlaps(const labeledAggregate& that);
    //bool overlaps(const array& that);
    //bool overlaps(const pointer& that);

    // Equality relations (implemented by interface)
    // Returns true if this object set and that object set may/must refer to the same memory objects.
    bool operator == (const objSet& o2);

    // Total order relations (implemented by interface)
    // General comparison operators. Provide a total order among different object types (scalar, labeledAggregate, array or pointer). 
    // These operators have no semantic meaning and are primarily intended to be used to put all objects into a total order, 
    // which makes it possible to use them in data structures such as std::map.
    bool operator < ( const objSet& o2);
    bool operator <= ( const objSet& o2);
    bool operator > ( const objSet& o2);
    bool operator >= ( const objSet& o2);
  }; // end class objSet

  //memory object that has no internal structure
  class scalar : public objSet
  {
    // Equality relations:
    // Returns true if this object and that object may/must refer to the same scalar memory object.
    bool operator == (const scalar& that) const;
    // Total order relations (implemented by interface):
    // Comparison operator to make it possible to provide a total order among all scalar objects.
    bool operator < (const scalar& that) const; 
  };

  class labeledAggregateField
  {
    public:
    std::string name; // field name
    size_t index; // The field's index within its parent object. The first field has index 0.
    objSet* field; // Pionter to an abstract description of the field
  };

  // a memory object that contains a finite number of explicitly labeled memory objects, such as structs, classes and bitfields
  class labeledAggregate: public objSet
  {
    // number of fields
    size_t fieldCount();

    // Returns a list of field
    std::list<labeledAggregateField*> getElements() const; 
    // Returns true if this object and that object may/must refer to the same labeledAggregate memory object.
    bool operator == (const labeledAggregate& that) const;
    //Total order relations (implemented by interface)
    bool operator < (const labeledAggregate& that) const;
  };

  // represents d-dimensional integral vectors. It encapsulates a variety of abstract representations for such vectors 
  // such as polyhedral constraints and strided indexes.
  class abstractIndex
  {
    
  };

  class array: public objSet
  {
    // Returns a memory object that corresponds to all the elements in the given array
    objSet* getElements();
    // Returns the memory object that corresponds to the elements described by the given abstract index, 
    // which represents one or more indexes within the array
    objSet* getElements(abstractIndex* ai);
    
    // number of dimensions of the aray
    size_t numDims();
    bool operator == (const array & that) const;
    bool operator < (const array & that) const;
  };

  class pointer: public objSet
  {
    // used for a pointer to non-array
    objSet* getDereference () const;
    // used for a pointer to an array
    objSet * getElements() const;
    // Returns true if this pointer refers to the same abstract object as that pointer.
    bool equalPoints(const pointer & that);
    // Returns true if this object and that object may/must refer to the same pointer memory object.
    bool operator == (const pointer & that) const;
    bool operator < (const pointer & that) const;
  };

  // A factory method for creating instances of objSet of a given abstraction
  class objSetFactory
  {
    //TODO smart pointer
    static objSet* createObjSet(SgNode*); 
  };

} // end namespace
#endif //abstract_memory_object_INCLUDED
