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
#include <string>

namespace AbstractMemoryObject
{
  //Users should provide a concrete node implementation by default
  // We try not to use pure virtual functions here to allow  
  // users to reuse the default implementation as much as possible
  class ObjSet
  {
    public:
      virtual ~ObjSet();
      // True if this set includes memory objects what May be the same
      virtual bool maySet();

      // True if this set includes memory objects what Must be the same
      virtual bool mustSet();

      // Return the number of real memory objects that this object set corresponds to. 
      // Returns a concrete number if it is known or the constant unboundedSize if no static bound is known.
      static const size_t unboundedSize=UINT_MAX;  
      virtual size_t objCount(); // the number of real memory locations the abstract ObjSet corresponds to
      // TODO should we have an interface function to return the starting address of the mem object?

      //TODO not in use for now
      virtual size_t memSize(); // the number of bytes of this objset takes up within memory

      // Type of memory
      // The types that all elements of the set may/must have. 
      // If the set of types this abstract object may have is unknown or very large, returned list contains an object of SgTypeUnknown. 
      // If the set of types the object must have is unknown or very large, returns an empty list.
      virtual std::set<SgType*> getType(); 


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
      virtual bool operator == (const ObjSet& o2); 

      // Total order relations (implemented by interface)
      // General comparison operators. Provide a total order among different object types (scalar, labeledAggregate, array or pointer). 
      // These operators have no semantic meaning and are primarily intended to be used to put all objects into a total order, 
      // which makes it possible to use them in data structures such as std::map.
      virtual bool operator < ( const ObjSet& o2);
      virtual bool operator <= ( const ObjSet& o2);
      virtual bool operator > ( const ObjSet& o2);
      virtual bool operator >= ( const ObjSet& o2);


      virtual std::string toString(); // pretty print for the object
  }; // end class ObjSet

  //memory object that has no internal structure
  class Scalar : public ObjSet
  {
    public:
      // Equality relations:
      // Returns true if this object and that object may/must refer to the same scalar memory object.
      bool operator == (const Scalar& that) const;
      // Total order relations (implemented by interface):
      // Comparison operator to make it possible to provide a total order among all scalar objects.
      bool operator < (const Scalar& that) const; 
  };

  class LabeledAggregate;
  class LabeledAggregateField
  {
    public:
      virtual std::string getName(); // field name
      virtual size_t getIndex(); // The field's index within its parent object. The first field has index 0.

      virtual ObjSet* getField(); // Pointer to an abstract description of the field
      virtual void setField( ObjSet* f); // Pointer to an abstract description of the field

      virtual LabeledAggregate* getParent(); // the parent obj this field belongs to
      virtual void setParent(LabeledAggregate* p) ; // the parent obj this field belongs to

      virtual std::string toString(); // pretty print for the object
  };

  // a memory object that contains a finite number of explicitly labeled memory objects, such as structs, classes and bitfields
  class LabeledAggregate: public ObjSet
  {
    public:
      // number of fields
      virtual size_t fieldCount();

      // Returns a list of field
      virtual std::vector<LabeledAggregateField*> getElements() const; 
      // Returns true if this object and that object may/must refer to the same labeledAggregate memory object.
      virtual bool operator == (const LabeledAggregate& that) const;
      //Total order relations (implemented by interface)
      virtual bool operator < (const LabeledAggregate& that) const;
  };

  // represents d-dimensional integral vectors. It encapsulates a variety of abstract representations for such vectors 
  // such as polyhedral constraints and strided indexes.
  // TODO: we support a single multi-dimensional index for now
  class IndexVector
  {
    public:
      // the index vector's length
      size_t getSize();
    
  };

  class Array: public ObjSet
  {
    public:
      // Returns a memory object that corresponds to all the elements in the given array
      ObjSet* getElements();
      // Returns the memory object that corresponds to the elements described by the given abstract index, 
      // which represents one or more indexes within the array
      ObjSet* getElements(IndexVector* ai);

      // number of dimensions of the array
      size_t getNumDims();
      bool operator == (const Array & that) const;
      bool operator < (const Array & that) const;
  };

  class Pointer: public ObjSet
  {
    public:
      // used for a pointer to non-array
      ObjSet* getDereference () const;
      // used for a pointer to an array
      ObjSet * getElements() const;
      // Returns true if this pointer refers to the same abstract object as that pointer.
      bool equalPoints(const Pointer & that);
      // Returns true if this object and that object may/must refer to the same pointer memory object.
      bool operator == (const Pointer & that) const;
      bool operator < (const Pointer & that) const;
  };

  // A factory method for creating instances of ObjSet of a given abstraction
  class ObjSetFactory
  {
    public:
      //TODO smart pointer ?
      static ObjSet* createObjSet(SgNode*); 
  };


  // Used to store the union of ObjSet
  class MultiObjSet: public ObjSet
  {
    
  };

} // end namespace
#endif //abstract_memory_object_INCLUDED
