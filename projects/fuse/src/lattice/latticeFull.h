#ifndef LATTICE_FULL_H
#define LATTICE_FULL_H

#include "cfgUtils.h"
//#include "variables.h"
#include "nodeState.h"
#include "lattice.h"
#include "partitions.h"
#include <string>
#include <map>
#include <vector>

/******************************
 *** Commonly used lattices ***
 ******************************/
namespace fuse {
class BoolAndLattice : public FiniteLattice
{
  // state can be:
  // -1 : unset (default value)
  // 0 : false
  // 1 : true
  int state;
  
  public:
  BoolAndLattice(PartEdgePtr pedge) : Lattice(pedge), FiniteLattice(pedge)
  { state = -1; }
  
  public:
  BoolAndLattice(bool state, PartEdgePtr pedge) : Lattice(pedge), FiniteLattice(pedge)
  { this->state = state; }
  
  // initializes this Lattice to its default state
  void initialize()
  { state = -1; }
  
  // returns a copy of this lattice
  Lattice* copy() const;
  
  // overwrites the state of this Lattice with that of that Lattice
  void copy(Lattice* that);
  
  // computes the meet of this and that and saves the result in this
  // returns true if this causes this to change and false otherwise
  bool meetUpdate(Lattice* that);
  
  // computes the meet of this and that and returns the result
  //Lattice* meet(Lattice* that);
    
  bool operator==(Lattice* that);
  
  // returns the current state of this object
  bool get() const;
  
  // sets the state of this BoolAndLattice to the given value
  // returns true if this causes the BoolAndLattice state to change, false otherwise
  bool set(bool state);
  
  // sets the state of this lattice to the conjunction of the BoolAndLattice's current state and the given value
  // returns true if this causes the BoolAndLattice state to change, false otherwise
  bool andUpd(bool state);
  
  // Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
  //    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
  //    an expression or variable is dead).
  // It is assumed that a newly-added variable has not been added before and that a variable that is being
  //    removed was previously added
  // These are empty in this lattice since it is now explicitly aware of variables.
  /*void addVar(varID var) {};
  void remVar(varID var) {};*/
  
  // Set this Lattice object to represent the set of all possible execution prefixes.
  // Return true if this causes the object to change and false otherwise.
  bool setToFull();
  // Set this Lattice object to represent the of no execution prefixes (empty set).
  // Return true if this causes the object to change and false otherwise.
  bool setToEmpty();
  
  // Set all the value information that this Lattice object associates with this MemLocObjectPtr to full.
  // Return true if this causes the object to change and false otherwise.
  bool setMLValueToFull(MemLocObjectPtr ml);
  
  // Returns whether this lattice denotes the set of all possible execution prefixes.
  bool isFull();
  // Returns whether this lattice denotes the empty set.
  bool isEmpty();
  
  std::string str(std::string indent="");
};

class IntMaxLattice : public InfiniteLattice
{
  int state;
  
  public:
  static const int infinity;// = 32768;
  
  IntMaxLattice(PartEdgePtr pedge) : Lattice(pedge), InfiniteLattice(pedge)
  {
    state = -1;
  }
  
  IntMaxLattice(int state, PartEdgePtr pedge) : Lattice(pedge), InfiniteLattice(pedge)
  {
    this->state = state;
  }
  
  // initializes this Lattice to its default state
  void initialize()
  {
    state = -1;
  }
  
  // returns a copy of this lattice
  Lattice* copy() const;
  
  // overwrites the state of this Lattice with that of that Lattice
  void copy(Lattice* that);
  
  // computes the meet of this and that and saves the result in this
  // returns true if this causes this to change and false otherwise
  bool meetUpdate(Lattice* that);
  
  // computes the meet of this and that and returns the result
  //Lattice* meet(Lattice* that);
  
  bool operator==(Lattice* that);
  
  // widens this from that and saves the result in this
  // returns true if this causes this to change and false otherwise
  bool widenUpdate(InfiniteLattice* that);
  
  // returns the current state of this object
  int get() const;
  
  // sets the state of this lattice to the given value
  // returns true if this causes the lattice's state to change, false otherwise
  bool set(int state);
  
  // increments the state of this lattice by the given value
  // returns true if this causes the lattice's state to change, false otherwise
  bool incr(int increment);
  
  // computes the maximum of the given value and the state of this lattice and saves 
  //    the result in this lattice
  // returns true if this causes the lattice's state to change, false otherwise
  bool maximum(int value);
    
  // Set this Lattice object to represent the set of all possible execution prefixes.
  // Return true if this causes the object to change and false otherwise.
  bool setToFull();
  // Set this Lattice object to represent the of no execution prefixes (empty set).
  // Return true if this causes the object to change and false otherwise.
  bool setToEmpty();
  
  // Set all the value information that this Lattice object associates with this MemLocObjectPtr to full.
  // Return true if this causes the object to change and false otherwise.
  bool setMLValueToFull(MemLocObjectPtr ml);
  
  // Returns whether this lattice denotes the set of all possible execution prefixes.
  bool isFull();
  // Returns whether this lattice denotes the empty set.
  bool isEmpty();
  
  std::string str(std::string indent="");
};

/*########################
  ### Utility lattices ###
  ########################*/

class ProductLattice : public virtual Lattice
{
  public:
  // The different levels of this lattice
  static const int uninitialized=0; 
  static const int initialized=1; 
  // This object's current level in the lattice: (uninitialized or initialized)
  short level;
  
  bool isFinite;
  
  protected:
  std::vector<Lattice*> lattices;
  
  public:
  //ProductLattice();
  ProductLattice(PartEdgePtr pedge);
  ProductLattice(const ProductLattice& that);
  ProductLattice(const std::vector<Lattice*>& lattices, PartEdgePtr pedge);
  ~ProductLattice();
  
  void init(const std::vector<Lattice*>& lattices);
  
  // initializes this Lattice to its default state
  void initialize();
  
  const std::vector<Lattice*>& getLattices();
  
  // initializes the given vector with a copy of the lattices vector
  void copy_lattices(std::vector<Lattice*>& newLattices) const;
  
  // Returns a copy of this lattice
  Lattice* copy() const;
        
  // Overwrites the state of this Lattice with that of that Lattice
  void copy(Lattice* that);
  
  // Called by analyses to transfer this lattice's contents from across function scopes from a caller function 
  //    to a callee's scope and vice versa. If this this lattice maintains any information on the basis of 
  //    individual MemLocObjects these mappings must be converted, with MemLocObjects that are keys of the ml2ml 
  //    replaced with their corresponding values. If a given key of ml2ml does not appear in the lattice, it must
  //    be added to the lattice and assigned a default initial value. In many cases (e.g. over-approximate sets 
  //    of MemLocObjects) this may not require any actual insertions.
  // The function takes newPEdge, the edge that points to the part within which the values of ml2ml should be 
  //    interpreted. It corresponds to the code region(s) to which we are remapping.
  // remapML must return a freshly-allocated object.
  Lattice* remapML(const std::set<MLMapping>& ml2ml, PartEdgePtr newPEdge);

  // Adds information about the MemLocObjects in newL to this Lattice, overwriting any information previously 
  //    maintained in this lattice about them.
  // Returns true if the Lattice state is modified and false otherwise.
  bool replaceML(Lattice* newL);
  
  // computes the meet of this and that and saves the result in this
  // returns true if this causes this to change and false otherwise
  bool meetUpdate(Lattice* that);
  
  // Computes the meet of this and that and returns the result
  virtual bool finiteLattice();
  
  bool operator==(Lattice* that);
  
  int getLevel() { return level; }
  
  // Set this Lattice object to represent the set of all possible execution prefixes.
  // Return true if this causes the object to change and false otherwise.
  bool setToFull();
  // Set this Lattice object to represent the of no execution prefixes (empty set).
  // Return true if this causes the object to change and false otherwise.
  bool setToEmpty();
  
  // Set all the value information that this Lattice object associates with this MemLocObjectPtr to full.
  // Return true if this causes the object to change and false otherwise.
  bool setMLValueToFull(MemLocObjectPtr ml);
  
  // Returns whether this lattice denotes the set of all possible execution prefixes.
  bool isFull();
  // Returns whether this lattice denotes the empty set.
  bool isEmpty();
  
  // The string that represents this object
  // If indent!="", every line of this string must be prefixed by indent
  // The last character of the returned string should not be '\n', even if it is a multi-line string.
  std::string str(std::string indent="");
};

class FiniteProductLattice : public virtual ProductLattice
{
  public:

  FiniteProductLattice(PartEdgePtr pedge) : Lattice(pedge), ProductLattice(pedge)
  {}
  
  FiniteProductLattice(const std::vector<Lattice*>& lattices, PartEdgePtr pedge) : Lattice(pedge), ProductLattice(lattices, pedge)
  {
    verifyFinite();
  }
  
  FiniteProductLattice(const FiniteProductLattice& that) : Lattice(that.latPEdge), ProductLattice(that.lattices, that.latPEdge)
  {
    verifyFinite();
  }
  
  void verifyFinite()
  {
    for(std::vector<Lattice*>::iterator it = lattices.begin(); it!=lattices.end(); it++)
        assert((*it)->finiteLattice());
  }
  
  bool finiteLattice()
  { return true;  }
  
  // Returns a copy of this lattice
  Lattice* copy() const
  {
    return new FiniteProductLattice(*this);
  }
};

class InfiniteProductLattice : public virtual ProductLattice
{
  public:
  
  InfiniteProductLattice(PartEdgePtr pedge) : Lattice(pedge), ProductLattice(pedge)
  {}
  
  InfiniteProductLattice(const std::vector<Lattice*>& lattices, PartEdgePtr pedge) : Lattice(pedge), ProductLattice(lattices, pedge)
  {}
  
  InfiniteProductLattice(const InfiniteProductLattice& that) : Lattice(that.latPEdge), ProductLattice(that.lattices, that.latPEdge)
  {}
  
  bool finiteLattice()
  { return false; }
  
  // returns a copy of this lattice
  Lattice* copy() const
  {
    return new InfiniteProductLattice(*this);
  }
  
  // Widens this from that and saves the result in this.
  // Returns true if this causes this to change and false otherwise.
  bool widenUpdate(InfiniteLattice* that);
};

}; // namespace fuse
#endif
