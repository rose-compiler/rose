#include "sage3basic.h"
#include <iostream>
#include "abstract_object_map.h"
#include "nodeState.h"
#include "analysis.h"
#include "uniqueNameTraversal.h"
#include "defsAndUsesTraversal.h"
#include "iteratedDominanceFrontier.h"
#include "controlDependence.h"

/**     
 * This is an implementation of the abstract memory object --> lattice map 
 *           
 * author: jisheng zhao (jz10@rice.edu)   
 */

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

using namespace std;
using namespace boost;
using namespace ssa_private;
using namespace dbglog;

namespace fuse {
int AbstractObjectMapDebugLevel=2;

// Set this Lattice object to represent the set of all possible execution prefixes.
// Return true if this causes the object to change and false otherwise.
bool AbstractObjectMap::setToFull()
{
  bool modified = !mapIsFull;
  items.clear();
  mapIsFull = true;
  return modified;
}

// Set this Lattice object to represent the of no execution prefixes (empty set).
// Return true if this causes the object to change and false otherwise.
bool AbstractObjectMap::setToEmpty()
{
  bool modified = !items.empty();
  items.clear();
  return modified;
}

// Set all the information associated Lattice object with this MemLocObjectPtr to full.
// Return true if this causes the object to change and false otherwise.
bool AbstractObjectMap::setMLValueToFull(MemLocObjectPtr ml)
{
  bool modified = false;
  
  // Iterate through all the keys in the items list. If any key is mayEqual(ml) then its associated 
  // value is set to full. Note that this works even if the keys are not MemLobObjectPtrs since in 
  // that case mustEqual will return false.
  for(list<MapElement>::iterator it = items.begin(); it != items.end(); it++) {
    AbstractObjectPtr keyElement = it->first;
    if(keyElement->mayEqual(ml, latPEdge, comp, analysis)) {
      modified = it->second->setToFull() || modified;
    }
  }
  return modified;
}

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool AbstractObjectMap::isFull()
{
  return mapIsFull;
}

// Returns whether this lattice denotes the empty set.
bool AbstractObjectMap::isEmpty()
{
  // Check if all items are empty
  for(std::list<MapElement>::iterator i=items.begin(); i!=items.end();) {
    // If at least one is not empty, return false
    if(!(i->first)->isEmpty(getPartEdge()) && !(i->second)->isEmpty()) return false;
    
    // If this item mapping is empty, remove it from the items list
    items.erase(i++);
  }
  // If all are empty, return true
  assert(items.size()==0);
  return true;
}

std::string AbstractObjectMap::str(std::string indent) {
  return strp(latPEdge, indent);
}

// Variant of the str method that can produce information specific to the current Part.
// Useful since AbstractObjects can change from one Part to another.
std::string AbstractObjectMap::strp(PartEdgePtr pedge, std::string indent)
{
  ostringstream oss;
  oss << "[AbstractObjectMap: "; //("<<pedge->str()<<"): ";
  
  //printf("[AbstractObjectMap: "); fflush(stdout);
  for(list<MapElement>::iterator it = items.begin();
       it != items.end(); it++) {
    //printf("\n%s%p =&gt; %p\n", indent.c_str(), it->first.get(), it->second.get()); fflush(stdout);
    oss << endl;
    oss << indent<<"&nbsp;&nbsp;&nbsp;&nbsp;"<<it->first->strp(pedge, indent+"&nbsp;&nbsp;&nbsp;&nbsp;")<<" =&gt; "<<endl;
    oss << indent<<"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<it->second->str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
  }
  oss << "]";
  return oss.str();
}

// Add a new memory object --> lattice pair to the frontier.
// Return true if this causes the map to change and false otherwise.
// It is assumed that the given Lattice is now owned by the AbstractObjectMap and can be modified and deleted by it.
bool AbstractObjectMap::insert(AbstractObjectPtr o, LatticePtr lattice) {
  scope reg("AbstractObjectMap::insert()", scope::medium, AbstractObjectMapDebugLevel, 1);
  if(AbstractObjectMapDebugLevel>=1) {
    dbg << "&nbsp;&nbsp;&nbsp;&nbsp;o="<<o->strp(latPEdge, "")<<" lattice="<<lattice->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<" mapIsFull="<<mapIsFull<<endl;
    dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<str("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  }
  
  // Do not insert mappings for dead keys
  if(!o->isLive(latPEdge, comp, analysis)) { 
    if(AbstractObjectMapDebugLevel>=1) dbg << "<b>AbstractObjectMap::insert() WARNING: attempt to insert dead mapping "<<o->strp(latPEdge)<<" =&gt; "<<lattice->str()<<"<\b>"<<endl;
    return false;
  }
  
  isFinite = isFinite && lattice->finiteLattice();
  // If this map corresponds to all possible mappings, all insertions are redundant
  if(mapIsFull) { return false; }
  
  bool retVal = false;
  bool insertDone = false;
  // Points to the Lattice mapped to key o after it has been inserted
  LatticePtr insertedLattice;
  bool mustEqualSeen = false;
  
  // First, check if there is a key on the frontier that must-equals o to determine
  // if we can just combine the new lattice with the old mapping
  list<MapElement>::iterator it;
  int i=0;
  for(it = items.begin(); it != items.end(); i++) {
    AbstractObjectPtr keyElement = it->first;
    if(AbstractObjectMapDebugLevel>=2) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;keyElement="<<keyElement->str("            ")<<" mustEqual(o, keyElement, latPEdge)="<<o->mustEqual(keyElement, latPEdge, comp, analysis)<<" insertDone="<<insertDone<<" mustEqualSeen="<<mustEqualSeen<<endl;
    // If we're done inserting, don't do it again
    if(insertDone) {
      // If o is mustEqual to this element and it is not the first match, remove this element
      if(o->mustEqual(keyElement, latPEdge, comp, analysis)) {
        //if(mustEqualSeen) {
          items.erase(it++);
        //} else 
        //  it++;
        mustEqualSeen = true;
      // If o denotes the same set as keyElement and has already been inserted into the map, any get that mayEqual
      // to o is guaranteed to be mayEqual to keyElement. As such, we just meet keyElement's lattice with o's lattice
      // and remove the keyElement's mapping
      } else if(o->equalSet(keyElement, latPEdge, comp, analysis)) {
         retVal = insertedLattice->meetUpdate(it->second.get()) || retVal;
         items.erase(it++);
      } else 
        it++;
      continue;
    }
  
    // If the o-frontier contains an object that must-equal to 
    if(o->mustEqual(keyElement, latPEdge, comp, analysis)) {
      if(AbstractObjectMapDebugLevel==1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;keyElement="<<keyElement->str("            ")<<" mustEqual(o, keyElement, latPEdge)="<<o->mustEqual(keyElement, latPEdge, comp, analysis)<<" insertDone="<<insertDone<<" mustEqualSeen="<<mustEqualSeen<<endl;
      if(AbstractObjectMapDebugLevel>=1) {
        dbg << "&nbsp;&nbsp;&nbsp;&nbsp;Must Equal"<<endl;
        dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;lattice="<<lattice->str("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
        dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;it="<<it->second->str("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
      }

      // If the old and new mappings of o are different,  we remove the old mapping and add a new one 
      if(!it->second->equiv(lattice.get()))
      {
        if(AbstractObjectMapDebugLevel==1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;keyElement="<<keyElement->str("            ")<<" mustEqual(o, keyElement, latPEdge)="<<o->mustEqual(keyElement, latPEdge, comp, analysis)<<" insertDone="<<insertDone<<" mustEqualSeen="<<mustEqualSeen<<endl;
        if(AbstractObjectMapDebugLevel>=1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;Removing i="<<i<<", inserting "<<o->strp(latPEdge, "        ")<<"=&gt;"<<lattice->str("        ")<<endl;
        items.erase(it++);
        items.push_front(MapElement(o, lattice));
        retVal = true;
      } else {
        //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;No Change"<<endl;
        it++;
        // Otherwise, they're identical and thus there is no need to modify the map
        retVal = false;
      }
      insertDone = true;
      insertedLattice = lattice;
      mustEqualSeen = true;
    // If the new element and the original actually denote the same set
    } else if(o->equalSet(keyElement, latPEdge, comp, analysis)) {
      // Meet their respective lattices of the 
      //dbg << "o="<<o->str()<<" <b>equalSet</b> "<<keyElement<<" keyElement="<<keyElement->str()<<endl;
      retVal = it->second->meetUpdate(lattice.get()) || retVal;
      insertedLattice = it->second;
      it++;
      insertDone = true;
    // If the element on the o-frontier may-equals o (their sets overlap) then insert a new o->lattice mapping 
    // since the new lattice cannot be combined with the mapping of *it
    } else if(o->mayEqual(keyElement, latPEdge, comp, analysis)) {
      //dbg << "o="<<o->str()<<" <b>mayEqual</b> "<<keyElement<<" keyElement="<<keyElement->str()<<endl;
      items.push_front(MapElement(o, lattice));
      retVal = true;
      insertedLattice = lattice;
      it++;
      insertDone = true;
    } else
      it++;
  }
  
  if(!insertDone) {
    // There are no objects within this map on the o-frontier. As such, add an o->lattice mapping
    items.push_front(MapElement(o, lattice));
    
    retVal = true;
  }
  
  // Having inserted the new item we need to clean up the map to ensure that it stays bounded in size
  // Step 1: call isEmpty to check for any keys mapped to empty sets
  isEmpty();
  // Step 2: if the map is larger than some fixed bound, merge some key->value mappings together
  // !!! TODO !!!
  
  if(AbstractObjectMapDebugLevel>=1) {
    indent ind(1, 1);
    dbg << "retVal="<<retVal<<" insertDone="<<insertDone<<" mustEqualSeen="<<mustEqualSeen<<endl;
    dbg << str()<<endl;
  }
  return retVal;
};

// Removes the key matching the argument from the frontier.
// Return true if this causes the map to change and false otherwise.
bool AbstractObjectMap::remove(AbstractObjectPtr abstractObjectPtr) {
// If this map corresponds to all possible mappings, all removals are redundant
  if(mapIsFull) { return false; }
  
  for (list<MapElement>::iterator it = items.begin();
       it != items.end(); it++) {
    AbstractObjectPtr keyElement = it->first;
    // For remove operation, we use must equal policy                                                               
    if (abstractObjectPtr->mustEqual(keyElement, latPEdge, comp, analysis)) {
      it = items.erase(it);
      return true;
    }
  }
  return false;
};

// Get all x-frontier for a given abstract memory object                                                            
LatticePtr AbstractObjectMap::get(AbstractObjectPtr abstractObjectPtr) {
  scope reg("AbstractObjectMap::get()", scope::medium, AbstractObjectMapDebugLevel, 1);
  if(AbstractObjectMapDebugLevel>=1) {
    dbg << "&nbsp;&nbsp;&nbsp;&nbsp;o="<<abstractObjectPtr->strp(latPEdge, "&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
    dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<str("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  }
  
  // If this map corresponds to all possible mappings, the only mapping that exists for any object is the full lattice
  if(mapIsFull) { 
    LatticePtr fullLat(defaultLat->copy());
    fullLat->setToFull();
    return fullLat;
  }
  
  LatticePtr ret;
  for (list<MapElement>::iterator it = items.begin();
       it != items.end(); it++) {
    AbstractObjectPtr keyElement = it->first;
    bool eq = abstractObjectPtr->mayEqual(keyElement, latPEdge, comp, analysis);
    if(AbstractObjectMapDebugLevel>=2 || (AbstractObjectMapDebugLevel>=1 && eq)) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;keyElement(equal="<<eq<<")="<<keyElement->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
    if(eq) {
      // If this is the first matching Lattice, copy this Lattice to ret
      if(!ret) ret = boost::shared_ptr<Lattice>(it->second->copy());
      // Otherwise, merge this latice into ret
      else     ret->meetUpdate(it->second.get());
      if(AbstractObjectMapDebugLevel>=1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;ret="<<ret->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
    }
  }
  
  if(AbstractObjectMapDebugLevel>=1) {
    dbg << "ret="<<(ret ? ret->str("&nbsp;&nbsp;&nbsp;&nbsp;"): "NULL")<<endl;
  }
  if(ret) return ret;
  // If there is no match for abstractObjectPtr, return a copy of the default lattice
  return LatticePtr(defaultLat->copy());
};

// initializes this Lattice to its default state, if it is not already initialized
void AbstractObjectMap::initialize()
{
  // Nothing to do here since Peter P's fixes will eliminate the need for lattices to maintain their own initialized state
}
 
// returns a copy of this lattice
Lattice* AbstractObjectMap::copy() const
{ return new AbstractObjectMap(*this); }

// overwrites the state of this Lattice with that of that Lattice
void AbstractObjectMap::copy(Lattice* thatL) {
  Lattice::copy(thatL);
  try {
    AbstractObjectMap * that = dynamic_cast <AbstractObjectMap*> (thatL);
    items = that->items;
  } catch (bad_cast & bc) { 
    assert(false);
  }
}

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
// remapML must return a freshly-allocated object.
Lattice* AbstractObjectMap::remapML(const std::set<MLMapping>& ml2ml, PartEdgePtr fromPEdge)
{
  if(mapIsFull) { return copy(); }
  
  scope reg("AbstractObjectMap::remapML", scope::medium, AbstractObjectMapDebugLevel, 1);
  
  if(AbstractObjectMapDebugLevel>=1) {
    // If either the key or the value of this mapping is dead within its respective part, we skip it.
    // Print notices of this skipping once
    for(std::set<MLMapping>::const_iterator m=ml2ml.begin(); m!=ml2ml.end(); m++) {
      if(!m->from) continue;
      // If either the key or the value of this mapping is dead within its respective part, skip it
      if(!m->from->isLive(fromPEdge, comp, analysis) || (m->to && !m->to->isLive(latPEdge, comp, analysis)))
        dbg << "<b>AbstractObjectMap::remapML() WARNING: Skipping dead ml2ml mapping "<<m->from->strp(fromPEdge)<<"(live="<<m->from->isLive(fromPEdge, comp, analysis)<<") =&gt; "<<(m->to ? m->to->strp(latPEdge) : "NULL")<<"(live="<<(m->to ? m->to->isLive(latPEdge, comp, analysis) : -1)<<")"<<endl
                 << "&nbsp;&nbsp;&nbsp;&nbsp;fromPEdge=["<<fromPEdge->str()<<"]"<<endl
                 << "&nbsp;&nbsp;&nbsp;&nbsp;latPEdge=["<<latPEdge->str()<<"]</b>"<<endl;
    }
  }
  
  if(AbstractObjectMapDebugLevel>=1) {
    scope reg("ml2ml", scope::medium);
    for(std::set<MLMapping>::const_iterator m=ml2ml.begin(); m!=ml2ml.end(); m++) {
      if(!m->from) continue;
      dbg << m->from.get()->str() << " =&gt; " << (m->to? m->to.get()->strp(latPEdge): "NULL") << endl;
    }
    //dbg << "this="<<str()<<endl;
  }
  
  // Copy of this map where the keys in ml2ml have been remapped to their corresponding values
  AbstractObjectMap* newM = new AbstractObjectMap(*this);
  
  // Vector of flags that indicate whether a given key in ml2ml has been added to newM or not
  vector<bool> ml2mlAdded;
  
  // Initialize ml2mlAdded to all false
  for(std::set<MLMapping>::const_iterator m=ml2ml.begin(); m!=ml2ml.end(); m++)
    ml2mlAdded.push_back(false);
  
  if(AbstractObjectMapDebugLevel>=2) dbg << "newM="<<newM->str()<<endl;
  
  // Iterate over all the mappings <key, val> n ml2ml and for each mapping consider each item in newM. If the key 
  // mustEquals to some item newM, that item is replaced by val. If the key mayEquals some item in newM, val is 
  // placed at the front of the list. If the key does not appear in newM at all, val is placed at the front of the list.
  for(std::list<MapElement>::iterator i=newM->items.begin(); i!=newM->items.end(); ) {
    indent ind0(AbstractObjectMapDebugLevel, 1);
    if(AbstractObjectMapDebugLevel>=1) dbg << "i="<<i->first->str()<<endl;
  
    int mIdx=0;
    std::set<MLMapping>::const_iterator m=ml2ml.begin();
    for(; m!=ml2ml.end(); m++, mIdx++) {
      if(!m->from) continue;
      
      indent ind1(AbstractObjectMapDebugLevel, 1);
      if(AbstractObjectMapDebugLevel>=1) dbg << mIdx << ": m-&gt;key="<<m->from->strp(fromPEdge)<<endl;
      
      indent ind2(AbstractObjectMapDebugLevel, 1);
      // If the current item in newM may- or must-equals a key in ml2ml, record this and update newM
      if(AbstractObjectMapDebugLevel>=2) {
        dbg << "i-&gt;first mustEqual m-&gt;from = "<<i->first->mustEqual(m->from, fromPEdge, comp, analysis)<<endl;
        dbg << "i-&gt;first mayEqual m-&gt;from = "<<i->first->mayEqual(m->from, fromPEdge, comp, analysis)<<endl;
      }
      if(i->first->mustEqual(m->from, fromPEdge, comp, analysis) && m->replaceMapping) {
        // If the value of the current ml2ml mapping is not-NULL
        if(m->to) {
          // Replace the current item in newM with the value of the current pair in ml2ml
          *i = make_pair(boost::static_pointer_cast<AbstractObject>(m->to), i->second);

          // Advance onward in newM and remove any items that are must-equal to the value of the current ml2ml mapping
          //scope reg("Deleting items that are must-equal to value", scope::medium, 1, 1);
          std::list<MapElement>::iterator iNext = i; iNext++;
          for(std::list<MapElement>::iterator j=iNext; j!=newM->items.end(); ) {
            if(AbstractObjectMapDebugLevel>=2) {
              dbg << "j="<<j->first<<" => "<<j->second<<endl;
              dbg << mIdx << ": m-&gt;value="<<m->to->strp(fromPEdge)<<endl;
              dbg << "j-&gt;first mustEqual m-&gt;to = "<<j->first->mustEqual(m->to, fromPEdge, comp, analysis)<<endl;
            }
            if(j->first->mustEqual(m->to, fromPEdge, comp, analysis)) {
              if(AbstractObjectMapDebugLevel>=2) dbg << "Erasing j="<<j->first->str()<<" => "<<j->second->str()<<endl;
              j = newM->items.erase(j);
              //break;
            } else
              j++;
          }
        // If the value of the current ml2ml mapping is NULL (i.e. the key is a MemLoc with a lifetime that is limited
        // to a given function and it does not carry over across function boundaries)
        } else {
          // Erase this mapping
          i = newM->items.erase(i);
          break;
        }
        ml2mlAdded[mIdx]=true;
      } else if(i->first->mayEqual(m->from, fromPEdge, comp, analysis)) {
        // Insert the value in the current ml2ml mapping immediately before the current item
        if(AbstractObjectMapDebugLevel>=1) dbg << "Inserting before i: "<<m->to->str()<<" => "<<i->second->str()<<endl;
        newM->items.insert(i, make_pair(boost::static_pointer_cast<AbstractObject>(m->to), i->second));
        ml2mlAdded[mIdx]=true;
      }
    }
    
    // If we broke out early, we must have erased the current element in newM, meaning that we shouldn't advance
    // i again. Otherwise, advance i.
    if(m==ml2ml.end())
      i++;
  }
  
  // Iterate through the false mappings in ml2mlAdded (ml2ml keys that were not mapped to any items in this map)
  // and add to newM a mapping of their values to defaultLat (as long as the values are not NULL)
  int mIdx=0;
  for(std::set<MLMapping>::iterator m=ml2ml.begin(); m!=ml2ml.end(); m++, mIdx++) {
    if(!m->from) continue;
    
    //dbg << "False mapping "<<m->from->str()<<endl;
    // If either the key or the value of this mapping is dead within its respective part, skip it
    if(!m->from->isLive(fromPEdge, comp, analysis) || !(m->to && m->to->isLive(latPEdge, comp, analysis))) continue;
    
    if(!ml2mlAdded[mIdx] && m->to)
      newM->items.push_back(make_pair(m->to, defaultLat->copy()));
  }
  
  return newM;
}

// Adds information about the MemLocObjects in newL to this Lattice, overwriting any information previously 
//    maintained in this lattice about them.
// Returns true if the Lattice state is modified and false otherwise.
bool AbstractObjectMap::replaceML(Lattice* newL)
{
  AbstractObjectMap* calleeAOM = dynamic_cast<AbstractObjectMap*>(newL);
  assert(calleeAOM);
  
  bool modified = false;
  
  for(std::list<MapElement>::iterator i=calleeAOM->items.begin(); i!=calleeAOM->items.end(); i++) {
    // Do not copy over mappings with keys that are dead in this map's host part
    if(!i->first->isLive(latPEdge, comp, analysis)) continue;
    modified = insert(i->first, i->second) || modified;
  }
  
  
  return modified;
}

// Computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
// The part of this object is to be used for AbstractObject comparisons.
bool AbstractObjectMap::meetUpdate(Lattice* thatL)
{
  scope reg("AbstractObjectMap::meetUpdate()", scope::medium, AbstractObjectMapDebugLevel, 2);

  // Both incorporateVars() and meetUpdate currently call merge. This is clearly not
  // right but we'll postpone fixing it until we have the right algorithm for merges
  bool modified = false;
  try {
    AbstractObjectMap * that = dynamic_cast <AbstractObjectMap*> (thatL);
    
    if(AbstractObjectMapDebugLevel>=2) {
      dbg << "latPEdge="<<latPEdge->str()<<endl;
      { scope thisreg("this", scope::medium);
      dbg << str()<<endl; }
      { scope thisreg("that", scope::medium);
      dbg << that->str()<<endl; }
    }
    
    // This algorithm is based on the following insights:
    // Given two AbstractObjectMaps: 
    //                     A: a_0 => a_1 => ... => a_n, and
    //                     B: b_0 => b_1 => ... => b_m
    //    where x => y if x is inserted after y
    // The only non-conservative thing that a merge algorithm can do is take a_i and b_j, where
    //   mustEqual(a_i, b_j) and place one before the other in the merged list. For these cases
    //   it is necessary to insert a single entry a_i -> A[a_i] merge B[b_j].
    // All elements that do not have a mustEqual partner in the same AbstractObjectMap or the other AbstractObjectMap 
    //   can be inserted into the new map in arbitrary order. AbstractObjectMap::insert ensures that no two elements
    //   that are mustEqual to each other may be in the same map. For elements across maps the following
    //   holds. If a_p and b_q are mayEqual to each other, then keeping them in the reverse order from
    //   their correct assignment order is conservative but may lose precision. This is because if 
    //   a_p => b_q but are stored in the opposite order and for some x it is true that mayEqual(x, b_p), mustEqual(x, a_p),
    //   then AbstractObjectMap::get(x) will merge the lattices stored at the two keys whereas the precise result would
    //   be to return the lattice stored under a_p. If a_p and b_q are not mayEqual to each other,
    //   either order is fine with no loss of precision.
    // As an additional enhancement suppose a_i and b_j are not must-equal but denote the same set. In this case, 
    //   although it is conservative to place them in either order, it is also useless since any abstract object
    //   that mayEquals a_i, must also mayEqual b_j. As such, we insert a single entry for a_i -> A[a_i] merge B[b_j]
    //   if a_i and b_j are mustEqual or equalSet. Since mustEqual implies equalSet, we only check equalSet.
    // The algorithm below chooses a simple order that is likely to work well in practice. It connects
    //   the pairs of elements in this->items(A) and that->items(B) that are mustEqual and then
    //   scans over each such pair <a_i, b_j> in the order they appear A, copying all the elements between
    //   the b_j and b_j+1 over to this map between a_i and a_i+1 if they've not already been copied over and 
    //   if they don't have a mustEquals partner in A (these are handled by merging, as described above).
    // For example, 
    //   A: a_0 => a_1 => r => a_2 => s => a_3 => t
    //   B: b_0 =>        r => b_1 =>             t => b_2 => b_3 => s
    //   A.mergeUpdate(B): a_0 => a_1 => b_0 => r => a_2 => b_1 => b_2 => b_3 => s => a_3 => t
    // Further, 
    
    // For each element x in this->items pointers that is mustEqual to an element y in
    // that->items, keeps the triple
    //    - iterator that points to x in this->items
    //    - iterator that points to y in that->items
    //    - index of y in that->items
    // Maintained in order of this->items.
    list<pair<list<MapElement>::iterator, pair<list<MapElement>::iterator, int> > > thisMustEq2thatMustEq;

    // For each element in that->items keeps true if this element is mustEquals to some
    // element in this->items and false otherwise.
    list<bool> thatMustEq;

    // Initialize thatMustEq to all false
    for(list<MapElement>::iterator itThat=that->items.begin(); itThat!=that->items.end(); itThat++)
      thatMustEq.push_back(false);

    if(AbstractObjectMapDebugLevel>=2)  {
      scope thisreg("that->items", scope::medium);
      for(list<MapElement>::iterator itThat=that->items.begin(); itThat!=that->items.end(); itThat++)
      dbg << "that: "<<itThat->first->str()<<" ==&gt; "<<itThat->second->str()<<endl;
    }
    
    // Determine which elements in this->items are mustEqual to elements in that->items
    // and for these pairs merge the lattices from that->items to this->items.
    for(list<MapElement>::iterator itThis=items.begin(); 
       itThis!=items.end(); itThis++) {
      scope thisreg("itThis", scope::medium, AbstractObjectMapDebugLevel, 2);
      if(AbstractObjectMapDebugLevel>=2) dbg << "this: "<<itThis->first->str()<<" ==&gt; "<<itThis->second->str()<<endl;
      
      int i=0;
      list<bool>::iterator thatMEIt=thatMustEq.begin();
      for(list<MapElement>::iterator itThat=that->items.begin(); 
         itThat!=that->items.end(); itThat++, i++, thatMEIt++) {
      
        scope thisreg("itThat", scope::medium, AbstractObjectMapDebugLevel, 2);
        if(AbstractObjectMapDebugLevel>=2) dbg << "that: "<<itThat->first->str()<<" ==&gt; "<<itThat->second->str()<<endl;
        
        // If we've found a pair of keys in this and that that are mustEqual or denote the same set
        //if(mustEqual(itThis->first, itThat->first, latPEdge, comp, analysis)) {
        if(itThis->first->equalSet(itThat->first, latPEdge, comp, analysis)) {
          // Record this pair
          thisMustEq2thatMustEq.push_back(make_pair(itThis, make_pair(itThat, i)));
          *thatMEIt = true;
          
          scope meetreg(txt()<<"Meeting", scope::medium, AbstractObjectMapDebugLevel, 2);
          
          // Update the lattice at *itThis to incorporate information at *itThat
          {
            // First copy the lattice since it may change. We don't deep-copy lattices when we copy
            // AbstractObjectMaps, so multiple maps may contain references to the same lattice.
            // As such, instead of updating lattices in-place (this would update the same lattice
            // in other maps) we first copy them and update into the copy.
            itThis->second = LatticePtr(itThis->second->copy());
            scope meetreg(txt()<<"Meeting "<<itThis->first->str(), scope::medium, AbstractObjectMapDebugLevel, 2);
            if(AbstractObjectMapDebugLevel>=2) { scope befreg("before", scope::low); dbg << itThis->second->str()<<endl; }
            modified = itThis->second->meetUpdate(itThat->second.get()) || modified;
            if(AbstractObjectMapDebugLevel>=2) { scope aftreg("after", scope::low); dbg << itThis->second->str()<<endl; }
          }
        }
      }
    }
    if(AbstractObjectMapDebugLevel>=2) {
      scope eqreg("thisMustEq2thatMustEq", scope::medium, AbstractObjectMapDebugLevel, 2);
      for(list<pair<list<MapElement>::iterator, pair<list<MapElement>::iterator, int> > >::iterator it=thisMustEq2thatMustEq.begin();
          it!=thisMustEq2thatMustEq.end(); it++) {
        dbg << (it->first)->first->str() << " =&gt; " << (it->second).first->second->str() << endl;
      }
    }
    
    { scope insreg("inserting that->this", scope::medium, AbstractObjectMapDebugLevel, 2);
    
    // Copy over the mappings of all the elements in that->items that were not mustEqual
    // to any elements in this->items. Although any order will work for these elements,
    // keep them their order in that->items.
    int thatIdx=0;
    list<MapElement>::iterator thatIt = that->items.begin();
    list<bool>::iterator thatMEIt=thatMustEq.begin();
    for(list<pair<list<MapElement>::iterator, pair<list<MapElement>::iterator, int> > >::iterator meIt=thisMustEq2thatMustEq.begin();
       meIt!=thisMustEq2thatMustEq.end(); meIt++) {
      scope mapreg(txt()<<"mustEqual mapping "<<meIt->second.second<<": "<<(meIt->first)->first->str(), scope::medium, AbstractObjectMapDebugLevel, 2);
      dbg << "this: "<<meIt->first->first->str() << " =&gt; " << meIt->first->second->str() <<endl;
      dbg << "that: "<<(meIt->second).first->first->str() << " =&gt; " << (meIt->second).first->second->str() << endl;
      dbg << "thatIdx="<<thatIdx<<endl;
      
      // Copy over all the mappings from that->items from thatIt to meIt's partner in that->items
      // if they have not already been copied because elements that are mustEqual to each other were ordered
      // differently in this->items and that->items
      if(meIt->second.second >= thatIdx) {
        for(; thatIt!=meIt->second.first; thatIt++, thatIdx++, thatMEIt++) {
          // Copy over the current element from that->items if it doesn't have a mustEqual 
          // partner in this->items (i.e. its already been handled)
          if(!(*thatMEIt)) {
            if(AbstractObjectMapDebugLevel>=2) dbg << "Inserting at meIt->first="<<(meIt->first)->first->str()<<" mapping "<<thatIt->first->str()<<" ==&gt; "<<thatIt->second->str()<<endl;
            // NOTE: we do not currently update the part field in the lattice thatIt->second
            //       to refer to this->latPEdge. Perhaps we should make a copy of it and update it.
            items.insert(meIt->first, *thatIt);
            modified = true;
          } else 
            if(AbstractObjectMapDebugLevel>=2) dbg << "mustEqual partner exists in this"<<endl;
        }
        // Advance thatIt and thatIdx once more to account for the partner in that->items 
        // of the current entry in this->items
        thatIt++;
        thatIdx++;
      }
    }
    
    // Add all the elements from that->items that remain
    for(; thatIt!=that->items.end(); thatIt++) {
      if(AbstractObjectMapDebugLevel>=2) dbg << "Pushing end "<<thatIt->first->str()<<" ==&gt; "<<thatIt->second->str()<<endl;
      // NOTE: we do not currently update the part field in the lattice thatIt->second
      //       to refer to this->latPEdge. Perhaps we should make a copy of it and update it.
      items.push_back(*thatIt);
      modified = true;
    }
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;items.size()="<<items.size()<<"\n";
   }
    
    // Compress all the elements from that are now mustEqual to each other in this->latPEdge.
    // Note: the optimal way to do this is to compress that->mustEqual first and then
    //       merge but we're not allowed to modify that so the compression would need
    //       to be done non-destructively via some additional datastructure. We avoid
    //       this complication for now but should revisit this question if we identify
    //       this code region as a performance bottleneck.
    //dbg << "Before mustEq compression "<<str()<<endl;
    // GB: I don't think we need this compression since we've already performed all the mustEqual
    //     (actually equalSet) matching and therefore should not have any keys that are mustEqual to each other.
    //compressMustEq();
    
    // Remove all the dead keys
    //dbg << "Before dead compression "<<str()<<endl;
    compressDead();
    
    //dbg << "Final "<<str()<<endl;
  } catch (bad_cast & bc) { 
    assert(false);
  }
  return modified;
}

// Identify keys that are must-equal to each other and merge their lattices
// Return true if this causes the object to change and false otherwise.
bool AbstractObjectMap::compressMustEq()
{
  if(mapIsFull) { return false; }
  
  /*dbg << "compressMustEq()"<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;"<<str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;*/
  
  bool modified = false;
  int xIdx=0;
  for(list<MapElement>::iterator x = items.begin(); x != items.end(); x++, xIdx++) {
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;"<<xIdx<<" : x="<<x->first->str("")<<endl;
    // y starts from the element that follows x
    list<MapElement>::iterator y = x;
    y++;
    int yIdx = xIdx+1;
    for(; y != items.end(); yIdx++) {
      //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<yIdx<<" : y="<<y->first->str("")<<endl;
      // If x and y are equal, merge their lattices and remove the later one
      if(x->first->mustEqual(y->first, latPEdge, comp, analysis)) {
        //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;MERGING and REMOVING"<<endl;
        // First copy the lattice since it may change. We don't deep-copy lattices when we copy
        // AbstractObjectMaps, so multiple maps may contain references to the same lattice.
        // As such, instead of updating lattices in-place (this would update the same lattice
        // in other maps) we first copy them and update into the copy.
        x->second = LatticePtr(x->second->copy());
        modified = x->second->meetUpdate(y->second.get()) || modified;
        
        list<MapElement>::iterator tmp = y;
        y++;
        items.erase(tmp);
        
        //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;map="<<str("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
        
        modified = true;
      } else
        y++;
    }
  }

  return modified;
};

// Remove all mappings with dead keys from this map.
// Return true if this causes the object to change and false otherwise.
bool AbstractObjectMap::compressDead()
{
  scope reg("compressDead", scope::low, AbstractObjectMapDebugLevel, 1);
  if(mapIsFull) { return false; }
        
  bool modified = false;
  for(list<MapElement>::iterator i = items.begin(); i != items.end(); ) {
    dbg << "i: "<<i->first.get()->str()<<" ==&gt"<<endl<<
              "          "<<i->second.get()->str()<<endl;
    
    // Remove mappings with dead keys
    if(!(i->first->isLive(latPEdge, comp, analysis))) {
      list<MapElement>::iterator nextI = i;
      nextI++;
      
      dbg << "Erasing "<<i->first.get()->str()<<endl;
      items.erase(i);
      modified = true;
      
      i = nextI;
    } else
      i++;
  }
  
  return modified;
}

bool AbstractObjectMap::finiteLattice()
{
  return isFinite;
}

bool AbstractObjectMap::operator==(Lattice* that)
{
  assert(latPEdge == that->getPartEdge());
  // This will be written once we have the merging algorithm to test
  // these maps' frontiers for semantic equivalence
  return false;
}
}; // namespace fuse
