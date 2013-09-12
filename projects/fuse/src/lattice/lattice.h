#ifndef LATTICE_H
#define LATTICE_H

#include "CallGraphTraverse.h"
//#include "variables.h"
#include "partitions.h"
#include "abstract_object.h"
#include <string>
#include <map>

namespace fuse {
class Lattice : public dbglog::printable
{
  public:
  PartEdgePtr latPEdge;
  //Lattice() {}
  Lattice(PartEdgePtr latPEdge) : latPEdge(latPEdge) {}
  
  public:
  // Sets the PartEdge that this Lattice's information corresponds to. 
  // Returns true if this causes the edge to change and false otherwise
  virtual bool setPartEdge(PartEdgePtr latPEdge);
  
  // Returns the PartEdge that this Lattice's information corresponds to
  virtual PartEdgePtr getPartEdge();
  
  // initializes this Lattice to its default state, if it is not already initialized
  virtual void initialize()=0;
  // returns a copy of this lattice
  virtual Lattice* copy() const=0;
  // overwrites the state of this Lattice with that of that Lattice
  virtual void copy(Lattice* that)
  { latPEdge = that->latPEdge; }
  
  // Called by analyses to transfer this lattice's contents from across function scopes from a caller function 
  //    to a callee's scope and vice versa. If this this lattice maintains any information on the basis of 
  //    individual MemLocObjects these mappings must be converted, with MemLocObjects that are keys of the ml2ml 
  //    replaced with their corresponding values. If a given key of ml2ml does not appear in the lattice, it must
  //    be added to the lattice and assigned a default initial value. In many cases (e.g. over-approximate sets 
  //    of MemLocObjects) this may not require any actual insertions. If the value of a given ml2ml mapping is 
  //    NULL (empty boost::shared_ptr), any information for MemLocObjects that must-equal to the key should be 
  //    deleted.
  // Since the function is called for the scope change across some Part, it needs to account for the fact that
  //    the keys in ml2ml are in scope on one side of Part, while the values on the other side. Specifically, it is
  //    guaranteed that the keys are in scope at fromPEdge while the values are in scope at the edge returned 
  //    by getPartEdge().
  virtual Lattice* remapML(const std::set<MLMapping>& ml2ml, PartEdgePtr fromPEdge) {
    return copy();
  }
  
  // Adds information about the MemLocObjects in newL to this Lattice, overwriting any information previously 
  //    maintained in this lattice about them.
  // Returns true if the Lattice state is modified and false otherwise.
  virtual bool replaceML(Lattice* newL)
  {
    return false;
  }
  
  // Computes the meet of this and that and saves the result in this
  // returns true if this causes this to change and false otherwise
  // The part of this object is to be used for AbstractObject comparisons.
  virtual bool meetUpdate(Lattice* that)=0;
  
  // Returns true if this Lattice implies that lattice (its constraints are equal to or tighter than those of 
  // that Lattice) and false otherwise.
  virtual bool implies(Lattice* that) {
    // this is tighter than that if meeting that into this causes this to change (that contains possibilities 
    // not already in this) but not vice versa (all the possibilities in this already exist in that)
    Lattice* thisCopy = copy();
    if(!thisCopy->meetUpdate(that)) { 
      delete thisCopy;
      return false;
    }
    delete thisCopy;
    
    Lattice* thatCopy = that->copy();
    if(thatCopy->meetUpdate(this)) {
      delete thatCopy;
      return false;
    }
    delete thatCopy;
    return true;
  }
  
  // Returns true if this Lattice is semantically equivalent to that lattice (both correspond to the same set
  // of application executions).
  virtual bool equiv(Lattice* that) {
    // this and that are equivalent if meeting either one with the other causes no changes
    Lattice* thisCopy = copy();
    if(thisCopy->meetUpdate(that)) { 
      delete thisCopy;
      return false;
    }
    delete thisCopy;
    
    Lattice* thatCopy = that->copy();
    if(thatCopy->meetUpdate(this)) {
      delete thatCopy;
      return false;
    }
    delete thatCopy;
    return true;
  }
  
  // Computes the meet of this and that and returns the result
  virtual bool finiteLattice()=0;
  
  virtual bool operator==(Lattice* that) /*const*/=0;
  bool operator!=(Lattice* that) {
    return !(*this == that);
  }
  bool operator==(Lattice& that) {
    return *this == &that;
  }
  bool operator!=(Lattice& that) {
    return !(*this == that);
  }
  
  // Set this Lattice object to represent the set of all possible execution prefixes.
  // Return true if this causes the object to change and false otherwise.
  virtual bool setToFull()=0;
  // Set this Lattice object to represent the of no execution prefixes (empty set).
  // Return true if this causes the object to change and false otherwise.
  virtual bool setToEmpty()=0;
  
  // Set all the value information that this Lattice object associates with this MemLocObjectPtr to full.
  // Return true if this causes the object to change and false otherwise.
  virtual bool setMLValueToFull(MemLocObjectPtr ml)=0;
  
  // Returns whether this lattice denotes the set of all possible execution prefixes.
  virtual bool isFull()=0;
  // Returns whether this lattice denotes the empty set.
  virtual bool isEmpty()=0;
  
  // Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
  //    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
  //    an expression or variable is dead).
  // It is assumed that a newly-added variable has not been added before and that a variable that is being
  //    removed was previously added
  /*virtual void addVar(varID var)=0;
  virtual void remVar(varID var)=0;*/
      
  // The string that represents ths object
  // If indent!="", every line of this string must be prefixed by indent
  // The last character of the returned string should not be '\n', even if it is a multi-line string.
  //virtual string str(string indent="") /*const*/=0;
};

class FiniteLattice : public virtual Lattice
{
  public:
  //FiniteLattice() {}
  FiniteLattice(PartEdgePtr latPEdge) : Lattice(latPEdge) {}
  
  bool finiteLattice()
  { return true;  }
};

class InfiniteLattice : public virtual Lattice
{
  public:
  //InfiniteLattice() {}
  InfiniteLattice(PartEdgePtr latPEdge) : Lattice(latPEdge) {}
  
  bool finiteLattice()
  { return false; }
  
  // widens this from that and saves the result in this
  // returns true if this causes this to change and false otherwise
  virtual bool widenUpdate(InfiniteLattice* that)=0;
};

}; // namespace fuse
#endif
