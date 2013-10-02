#include "sage3basic.h"
#include "latticeFull.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;
using namespace dbglog;

namespace fuse {
  
/*****************************
 ********** Lattice **********
 *****************************/
  
// Sets the PartEdge that this Lattice's information corresponds to. 
// Returns true if this causes the edge to change and false otherwise
bool Lattice::setPartEdge(PartEdgePtr latPEdge) { 
  bool modified = this->latPEdge != latPEdge;
  this->latPEdge = latPEdge;
  return modified;
}

// Returns the PartEdge that this Lattice's information corresponds to
PartEdgePtr Lattice::getPartEdge()
{ 
//  Dbg:: << "Lattice::getPartEdge() this->latPEdge="<<this->latPEdge<<endl;
//  Dbg:: << "Lattice::getPartEdge() this->latPEdge="<<this->latPEdge->str()<<endl;
  return this->latPEdge; }
  
/********************************************
 ************** BoolAndLattice **************
 ********************************************/

// returns a copy of this lattice
Lattice* BoolAndLattice::copy() const
{
  return new BoolAndLattice(state, latPEdge);
}

// overwrites the state of this Lattice with that of that Lattice
void BoolAndLattice::copy(Lattice* that)
{
  state = dynamic_cast<BoolAndLattice*>(that)->state;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool BoolAndLattice::meetUpdate(Lattice* that)
{
  int newState = (state > dynamic_cast<BoolAndLattice*>(that)->state ? state : dynamic_cast<BoolAndLattice*>(that)->state);
  bool ret = newState != state;
  state = newState;
  return ret;
}

// computes the meet of this and that and returns the result
/*Lattice* BoolAndLattice::meet(Lattice* that)
{
  int newState = (state > (dynamic_cast<BoolAndLattice*>(that)->state ? state : (dynamic_cast<BoolAndLattice*>(that)->state);
  return new BoolAndLattice(newState);
}*/

bool BoolAndLattice::operator==(Lattice* that)
{
  return state == dynamic_cast<BoolAndLattice*>(that)->state;
}

// returns the current state of this object
bool BoolAndLattice::get() const
{
  if(state<0)
    return false;
  else
    return state;
}

// sets the state of this BoolAndLattice to the given value
// returns true if this causes the BoolAndLattice state to change, false otherwise
bool BoolAndLattice::set(bool state)
{
  if(this->state==-1)
  {
    this->state = state;
    return true;
  }
  else
  {
    bool oldState = state;
    this->state = state;
    return oldState != this->state;
  }
}

// sets the state of this lattice to the conjunction of the BoolAndLattice's current state and the given value
// returns true if this causes the BoolAndLattice state to change, false otherwise
bool BoolAndLattice::andUpd(bool state)
{
  //printf("BoolAndLattice::andUpd(%d) this->state=%d\n", state, this->state);
  if(this->state==-1)
  {
    this->state = state;
    return true;
  }
  else
  {
    bool oldState = state;
    this->state = this->state && state;
    return oldState != this->state;
  }
}

// Set this Lattice object to represent the set of all possible execution prefixes.
// Return true if this causes the object to change and false otherwise.
bool BoolAndLattice::setToFull()
{
  bool modified = (state!=1);
  state = 1;
  return modified;
}

// Set this Lattice object to represent the of no execution prefixes (empty set).
// Return true if this causes the object to change and false otherwise.
bool BoolAndLattice::setToEmpty()
{
  bool modified = (state!=-1);
  state = -1;
  return modified;
}

// Set all the value information that this Lattice object associates with this MemLocObjectPtr to full.
// Return true if this causes the object to change and false otherwise.
bool BoolAndLattice::setMLValueToFull(MemLocObjectPtr ml)
{
  // Since this lattice may contain information for the entire application state,
  // we set the entire lattice to full just in case
  return setToFull();
}

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool BoolAndLattice::isFull()
{ return state==1; }

// Returns whether this lattice denotes the empty set.
bool BoolAndLattice::isEmpty()
{ return state==-1; }

string BoolAndLattice::str(string indent)
{
  ostringstream outs;
  if(state==-1)
    outs << indent << "?";
  else
    outs << indent << (state ? "true" : "false");
  return outs.str();
}

/*******************************************
 ************** IntMaxLattice **************
 *******************************************/
const int IntMaxLattice::infinity = 32768;
  
// returns a copy of this lattice
Lattice* IntMaxLattice::copy() const
{
  return new IntMaxLattice(state, latPEdge);
}

// overwrites the state of this Lattice with that of that Lattice
void IntMaxLattice::copy(Lattice* that)
{
  state = dynamic_cast<IntMaxLattice*>(that)->state;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool IntMaxLattice::meetUpdate(Lattice* that)
{
  int newState = (state > dynamic_cast<IntMaxLattice*>(that)->state ? state : dynamic_cast<IntMaxLattice*>(that)->state);
  bool ret = newState != state;
  state = newState;
  return ret;
}

/*// computes the meet of this and that and returns the result
Lattice* IntMaxLattice::meet(Lattice* that)
{
  int newState = (state > (dynamic_cast<IntMaxLattice*>(that)->state ? state : (dynamic_cast<IntMaxLattice*>(that)->state);
  return new IntMaxLattice(newState);
}*/

bool IntMaxLattice::operator==(Lattice* that)
{
  return state == dynamic_cast<IntMaxLattice*>(that)->state;
}

// widens this from that and saves the result in this
// returns true if this causes this to change and false otherwise
bool IntMaxLattice::widenUpdate(InfiniteLattice* that)
{
  if(state==-1)
  {
    //printf("IntMaxLattice::widenUpdate() widening from %d to %d\n", state, dynamic_cast<IntMaxLattice*>(that)->state);
    state = dynamic_cast<IntMaxLattice*>(that)->state;
    return true;
  }
  else if(dynamic_cast<IntMaxLattice*>(that)->state == -1)
  {
    //printf("IntMaxLattice::widenUpdate() not widening from %d to %d\n", state, dynamic_cast<IntMaxLattice*>(that)->state);
    return false;
  }
  else
  {
    int newState = (state == dynamic_cast<IntMaxLattice*>(that)->state? state : infinity);
    //printf("IntMaxLattice::widenUpdate() widening from %d to %d ==&gt; \n", state, dynamic_cast<IntMaxLattice*>(that)->state, newState);
    bool ret = newState != state;
    state = newState;
    return ret;
  }
}

// returns the current state of this object
int IntMaxLattice::get() const
{
  return state;
}

// sets the state of this lattice to the given value
// returns true if this causes the lattice's state to change, false otherwise
bool IntMaxLattice::set(int state)
{
  // don't set above infinity
  if(state>infinity)
    state = infinity;
    
  int oldState = state;
  this->state = state;
  return oldState != this->state;
}

// increments the state of this lattice by the given value
// returns true if this causes the lattice's state to change, false otherwise
bool IntMaxLattice::incr(int increment)
{
  int oldState = this->state;
  this->state+=increment;
  
  // don't increment past infinity
  if(this->state>infinity)
  {
    //printf("IntMaxLattice::incr(%d) incrementing from %d to infinity\n", increment, oldState);
    this->state = infinity;
  }
  // don't decrement below 0
  else if(this->state<0)
  {
    //printf("IntMaxLattice::incr(%d) incrementing from %d to 0\n", increment, oldState);
    this->state = 0;
  }
  /*else
    printf("IntMaxLattice::incr(%d) incrementing from %d to 0\n", increment, oldState, this->state);*/
  
  return oldState != this->state;
}

// computes the maximum of the given value and the state of this lattice and saves 
//    the result in this lattice
// returns true if this causes the lattice's state to change, false otherwise
bool IntMaxLattice::maximum(int value)
{
  // don't set above infinity
  if(state>infinity)
    state = infinity;
  
  int oldState = state;
  state = (state < value? value: state);
  //printf("maximum: oldState=%d state=%d\n", oldState, state);
  return oldState!=state;
}

// Set this Lattice object to represent the set of all possible execution prefixes.
// Return true if this causes the object to change and false otherwise.
bool IntMaxLattice::setToFull()
{
  bool modified = (state!=infinity);
  state = infinity;
  return modified;
}

// Set this Lattice object to represent the of no execution prefixes (empty set).
// Return true if this causes the object to change and false otherwise.
bool IntMaxLattice::setToEmpty()
{
  bool modified = (state!=-1);
  state = -1;
  return modified;
}

// Set all the value information that this Lattice object associates with this MemLocObjectPtr to full.
// Return true if this causes the object to change and false otherwise.
bool IntMaxLattice::setMLValueToFull(MemLocObjectPtr ml)
{
  // Since this lattice may contain information for the entire application state,
  // we set the entire lattice to full just in case
  return setToFull();
}

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool IntMaxLattice::isFull()
{ return state==infinity; }

// Returns whether this lattice denotes the empty set.
bool IntMaxLattice::isEmpty()
{ return state==-1; }

string IntMaxLattice::str(string indent)
{
  ostringstream outsNum;
  outsNum << state;
  string stateStr = outsNum.str();
  
  ostringstream outs;
  outs << indent << (state==infinity? "infinity" : stateStr);
  return outs.str();
}


/*########################
  ### Utility lattices ###
  ########################*/
  
/**********************
 *** ProductLattice ***
 **********************/

ProductLattice::ProductLattice(const ProductLattice& that) : Lattice(that.latPEdge)
{
  isFinite = true;
  level = that.level;
  init(that.lattices);
}

ProductLattice::ProductLattice(PartEdgePtr pedge) : Lattice(pedge) {
  isFinite = true;
  level = uninitialized;
}

ProductLattice::ProductLattice(const vector<Lattice*>& lattices, PartEdgePtr pedge) : Lattice(pedge) {
  isFinite = true;
  level = uninitialized;
  init(lattices);
}

ProductLattice::~ProductLattice()
{
  // Deallocate all the lattices
  for(vector<Lattice*>::iterator lat=lattices.begin(); lat!=lattices.end(); lat++)
    delete *lat;
}

void ProductLattice::init(const vector<Lattice*>& lattices)
{
  this->lattices = lattices;
  for(std::vector<Lattice*>::const_iterator l=lattices.begin(); l!=lattices.end(); l++)
    if(!((*l)->finiteLattice())) isFinite = false;
}

// initializes this Lattice to its default state
void ProductLattice::initialize()
{
  //cout << "ProductLattice::initialize() level="<<level<<"\n";
  if(level != initialized) {
    level = initialized;
    for(vector<Lattice*>::iterator it = lattices.begin(); it!=lattices.end(); it++) {
      //cout << "ProductLattice::initialize() initializing "<<(*it)->str("    ")<<"\n";
      (*it)->initialize();
    }
  }
}

const vector<Lattice*>& ProductLattice::getLattices()
{
  return lattices;
}

// initializes the given vector with a copy of the lattices vector
void ProductLattice::copy_lattices(vector<Lattice*>& newLattices) const
{
  for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
    newLattices.push_back((*it)->copy());
}

// returns a copy of this lattice
Lattice* ProductLattice::copy() const
{
  return new ProductLattice(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void ProductLattice::copy(Lattice* that_arg)
{
  ProductLattice* that = dynamic_cast<ProductLattice*>(that_arg);
  assert(that);
  level = that->level;
  // Deallocate all the Lattices
  for(vector<Lattice*>::iterator lat=lattices.begin(); lat!=lattices.end(); lat++)
    delete *lat;
  lattices.clear();
  that->copy_lattices(lattices);
}

// Called by analyses to transfer this lattice's contents from across function scopes from a caller function 
//    to a callee's scope and vice versa. If this this lattice maintains any information on the basis of 
//    individual MemLocObjects these mappings must be converted, with MemLocObjects that are keys of the ml2ml 
//    replaced with their corresponding values. If a given key of ml2ml does not appear in the lattice, it must
//    be added to the lattice and assigned a default initial value. In many cases (e.g. over-approximate sets 
//    of MemLocObjects) this may not require any actual insertions.
// The function takes newPEdge, the edge that points to the part within which the values of ml2ml should be 
//    interpreted. It corresponds to the code region(s) to which we are remapping.
// remapML must return a freshly-allocated object.
Lattice* ProductLattice::remapML(const std::set<MLMapping>& ml2ml, PartEdgePtr newPEdge) {
  ProductLattice* pl = new ProductLattice(latPEdge);
  for(std::vector<Lattice*>::iterator l=lattices.begin(); l!=lattices.end(); l++)
    pl->lattices.push_back((*l)->remapML(ml2ml, newPEdge));
  return pl;
}

// Adds information about the MemLocObjects in newL to this Lattice, overwriting any information previously 
//    maintained in this lattice about them.
// Returns true if the Lattice state is modified and false otherwise.
bool ProductLattice::replaceML(Lattice* newL)
{
  ProductLattice* nl = dynamic_cast<ProductLattice*>(newL);
  assert(nl);
  assert(lattices.size() == nl->lattices.size());
  
  bool modified = false;
  for(std::vector<Lattice*>::iterator t=lattices.begin(), n=nl->lattices.begin(); 
      t!=lattices.end(); t++, n++)
    modified = (*t)->replaceML(*n) || modified;
  
  return modified;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool ProductLattice::meetUpdate(Lattice* that_arg)
{
  ProductLattice* that = dynamic_cast<ProductLattice*>(that_arg);
  
  bool modified=false;
  int newLevel = max(level, that->level);
  modified = (newLevel != level) || modified;
  level = newLevel;
  
  vector<Lattice*>::iterator it, itThat;
  for(it = lattices.begin(), itThat = that->lattices.begin(); 
      it!=lattices.end() && itThat!=that->lattices.end(); 
      it++, itThat++)
    modified = (*it)->meetUpdate(*itThat) || modified;
  
  return modified;
}

// Computes the meet of this and that and returns the result
bool ProductLattice::finiteLattice()
{
  return isFinite;
}

bool ProductLattice::operator==(Lattice* that_arg)
{
  ProductLattice* that = dynamic_cast<ProductLattice*>(that_arg);
  if(level != that->level) return false;
  
  vector<Lattice*>::const_iterator it, itThat;
  for(it = lattices.begin(), itThat = that->lattices.begin(); 
      it!=lattices.end() && itThat!=that->lattices.end(); 
      it++, itThat++)
    if((**it) != (**itThat)) return false;
  return true;
}

// Set this Lattice object to represent the set of all possible execution prefixes.
// Return true if this causes the object to change and false otherwise.
bool ProductLattice::setToFull()
{
  bool modified = false;
  for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
      modified = (*it)->setToFull() || modified;
  return modified;
}

// Set this Lattice object to represent the of no execution prefixes (empty set).
// Return true if this causes the object to change and false otherwise.
bool ProductLattice::setToEmpty()
{
  bool modified = false;
  for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
      modified = (*it)->setToEmpty() || modified;
  return modified;
}

// Set all the value information that this Lattice object associates with this MemLocObjectPtr to full.
// Return true if this causes the object to change and false otherwise.
bool ProductLattice::setMLValueToFull(MemLocObjectPtr ml)
{
  bool modified = false;
  for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
      modified = (*it)->setMLValueToFull(ml) || modified;
  return modified;
}


// Returns whether this lattice denotes the set of all possible execution prefixes.
bool ProductLattice::isFull()
{
  // Since we're not sure whether the contents of the product lattice are exhaustive, 
  // we don't bother checking if they are individually full and instead conservatively return false.
  return false;
}
// Returns whether this lattice denotes the empty set.
bool ProductLattice::isEmpty()
{
  // Check if all items are empty
  for(std::vector<Lattice*>::iterator it=lattices.begin(); it!=lattices.end();) {
    // If at least one is not empty, return false
    if(!(*it)->isEmpty()) return false;
    
    // If this item is empty, remove it from the items list
    lattices.erase(it++);
  }
  // If all are empty, return true
  assert(lattices.size()==0);
  return true;
}

// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string ProductLattice::str(string indent)
{
  ostringstream outs;
  outs << indent << "[ProductLattice: level="<<(level==uninitialized ? "uninitialized" : "initialized")<<"\n";
  for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
    outs << (*it)->str(indent+"    ") << "\n";
  outs << indent << "]\n";
  return outs.str();
}

/******************************
 *** InfiniteProductLattice ***
 ******************************/

// Widens this from that and saves the result in this.
// Returns true if this causes this to change and false otherwise.
bool InfiniteProductLattice::widenUpdate(InfiniteLattice* that)
{
  bool modified=false;
  vector<Lattice*>::iterator it, itThat;
  for(it = lattices.begin(), itThat = (dynamic_cast<InfiniteProductLattice*>(that))->lattices.begin(); 
      it!=lattices.end() && itThat!=(dynamic_cast<InfiniteProductLattice*>(that))->lattices.end(); 
      it++, itThat++) {
    /*Lattice* thisLat = *it;
    Lattice* thatLat = *itThat;*/
    modified = (dynamic_cast<InfiniteLattice*>(*it))->widenUpdate(dynamic_cast<InfiniteLattice*>(*itThat)) || modified;
  }
       return modified;
}

}; // using namespace
