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
      virtual bool maySet(); //TODO remove it

      // True if this set includes memory objects what Must be the same
      virtual bool mustSet(); //TODO remove it

      // Return the number of real memory objects that this object set corresponds to. 
      // Returns a concrete number if it is known or the constant unboundedSize if no static bound is known.
      static const size_t unboundedSize=UINT_MAX;  
// TODO remove it
      virtual size_t objCount(); // the number of real memory locations the abstract ObjSet corresponds to 
//TODO Long-term isFinite
//TODO Long-term getElements

// TODO should we have an interface function to return the starting address of the mem object?

      //TODO not in use for now
      //virtual size_t memSize(); // the number of bytes of this objset takes up within memory

      // Type of memory
      // The types that all elements of the set may/must have. 
      // If the set of types this abstract object may have is unknown or very large, returned list contains an object of SgTypeUnknown. 
      // If the set of types the object must have is unknown or very large, returns an empty list.
      virtual std::set<SgType*> getType() ;  //TODO remove it


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
      virtual bool operator== (const ObjSet& o2) const;  //TODO remove this

      // Return true if there is a slightly chance to be equal
      virtual bool mayEqual (const ObjSet& o2) const ; 
      // Return true only if we are absolutely certain they are equal in all conditions
      virtual bool mustEqual (const ObjSet& o2) const ; 

      // Total order relations (implemented by interface)
      // General comparison operators. Provide a total order among different object types (scalar, labeledAggregate, array or pointer). 
      // These operators have no semantic meaning and are primarily intended to be used to put all objects into a total order, 
      // which makes it possible to use them in data structures such as std::map.
      virtual bool operator < ( const ObjSet& o2) const; //TODO remove this
//      virtual bool operator <= ( const ObjSet& o2) const;
//     virtual bool operator > ( const ObjSet& o2) const;
//     virtual bool operator >= ( const ObjSet& o2) const;


      virtual std::string toString(); // pretty print for the object
      //TODO virtual std::string toString(string indent); // pretty print for the object
  }; // end class ObjSet

  //memory object that has no internal structure
  class Scalar : public ObjSet
  {
    public:
      // Equality relations:
      // Returns true if this object and that object may/must refer to the same scalar memory object.
      //virtual bool operator== (const Scalar& that) const;
      // Total order relations (implemented by interface):
      // Comparison operator to make it possible to provide a total order among all scalar objects.
      //virtual bool operator < (const Scalar& that) const; 
  };

  
  // memory object to model function objects in memory
  class Function: public ObjSet
  {
  public:  
    
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
      //virtual bool operator== (const LabeledAggregate& that) const;
      //Total order relations (implemented by interface)
      //virtual bool operator < (const LabeledAggregate& that) const;
  };

  // represents d-dimensional integral vectors. It encapsulates a variety of abstract representations for such vectors 
  // such as polyhedral constraints and strided indexes.
  // TODO: we support a single multi-dimensional index for now
  class IndexVector
  {
    public:
      // the index vector's length
      size_t getSize();
      virtual std::string toString();
      // equal operator
      virtual bool operator== (const IndexVector & other) const;
      virtual bool mayEqual (const IndexVector & other) const;
      virtual bool mustEqual (const IndexVector & other) const;
      virtual bool operator != (const IndexVector & other) const;
    
  };

#if 0 // Still not clear if users will get confused by this class
  // A class to summarize overlapped semantics for C pointers and Arrays
  // Recommended to be used in C/C++ where pointers and arrays sometimes can be used interchangeably. 
  class PointerOrArray: public ObjSet
  {
    public:
    // Array side of interfaces ---------
      // Returns a memory object that corresponds to all the elements in the given array
      virtual ObjSet* getElements();
      // Returns the memory object that corresponds to the elements described by the given abstract index, 
      // which represents one or more indexes within the array
      virtual ObjSet* getElements(IndexVector* ai);

      // number of dimensions of the array
      virtual size_t getNumDims();

   // Pointer side of interfaces ---------
       // used for a pointer to non-array
      virtual ObjSet* getDereference () const;
      // Returns true if this pointer refers to the same abstract object as that pointer.
      virtual bool equalPoints(const PointerOrArray & that);
    
      //virtual bool operator== (const ObjSet & that) const;
      //virtual bool operator < (const ObjSet & that) const;
  };
#endif 
  // Some programming languages don't have the concept of pointers. We provide explicit support for Array
  class Array: public ObjSet
  {
    public:
      // Returns a memory object that corresponds to all the elements in the given array
      virtual ObjSet* getElements() ;
      // Returns the memory object that corresponds to the elements described by the given abstract index, 
      // which represents one or more indexes within the array
      virtual ObjSet* getElements(IndexVector* ai) ;

      // number of dimensions of the array
      virtual size_t getNumDims();

      //--- pointer like semantics
      // support dereference of array object, similar to the dereference of pointer
      // Return the element object: array[0]
      virtual ObjSet* getDereference () ;
      //virtual bool operator== (const ObjSet & that) const;
      //virtual bool operator < (const ObjSet & that) const;
  };

  class Pointer: public ObjSet
  {
    public:
      virtual ObjSet* getDereference () ;
      // Returns true if this pointer refers to the same abstract object as that pointer.
      virtual bool equalPoints(Pointer & that);
      // Returns true if this object and that object may/must refer to the same pointer memory object.
      //virtual bool operator== (const Pointer & that) const;
      //virtual bool operator < (const Pointer & that) const;
      //
      //--- array like semantics
       // Returns a memory object that corresponds to all the elements in the given array
      //virtual ObjSet* getElements() ;
      // which represents one or more indexes within the array
      //virtual ObjSet* getElements(IndexVector* ai) ;
      // number of dimensions of the array
      //virtual size_t getNumDims() ;
  };

  // A factory method for creating instances of ObjSet of a given abstraction
  class ObjSetFactory
  {
    public:
      //TODO smart pointer ?
      static ObjSet* createObjSet(SgNode*); 
  };


  // TODO Used to store the union of ObjSet
  // Basically we use the composite design pattern to hold ObjSet and MultiObjSet, both are derived
  // class from ObjSet
  class MultiObjSet: public ObjSet
  {
  private:  
    // Do we want to dictate the implementation of the internal container?
    // vector <ObjSet* > elements;
   public:
  //    void add (ObjSet* );
  //    MultiObjSet & union (const MultiObjSet& other);
  //    MultiObjSet & operator+(const MultiObjSet& other); // same as union
  //    MultiObjSet & intersection(const MultiObjSet& other);
  //    MultiObjSet & operator-(const MultiObjSet& other); // difference
  };

} // end namespace
#endif //abstract_memory_object_INCLUDED
