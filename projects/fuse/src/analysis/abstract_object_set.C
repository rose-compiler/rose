#include "sage3basic.h"
#include "abstract_object_set.h"
#include <ostream>
#include <typeinfo>

using namespace std;
using namespace dbglog;

namespace fuse
{  
int AbstractObjectSetDebugLevel=0;
// returns true only if the list grows
// false implies element is already present
// insert(o) : In both modes we insert o unless o is must-equal to another AbstractObject already inside the set.
//             This ensures that we account for the entire sets of concrete objects that are in the set, only 
//             saving space when two AbstractObjects definitely correspond to the same concrete object (it would also 
//             be ok if the newly added AbstractObject was a subset of a previously added one). In may mode insert 
//             would also be conservative if we replaced multiple AbstractObject with a single AbstractObject that 
//             over-approximates their union. In must mode insert would also be conservative if we removed any 
//             AbstractObject from the set.
bool AbstractObjectSet::insert(AbstractObjectPtr that)
{
  assert(that);
  scope reg(txt()<<"AbstractObjectSet::insert("<<that->str()<<")", scope::medium, AbstractObjectSetDebugLevel, 1);
  
  //dbg << "this="<<str()<<endl;
  
  // Do not insert mappings for dead keys
  if(!that->isLive(latPEdge, comp, analysis)) { 
    if(AbstractObjectSetDebugLevel>=1) dbg << "<b>AbstractObjectSet::insert() WARNING: attempt to insert dead element "<<that->strp(latPEdge)<<"<\b>"<<endl;
    return false;
  }
    
  bool retval = false;
  if(!containsEqualSet(that)) {
    indent(AbstractObjectSetDebugLevel, 1);
    if(AbstractObjectSetDebugLevel>=1) dbg << "Object not contained. Adding..."<<endl;// Inserting into"<<endl<<str("")<<endl;
    
    items.push_back(that);
    retval = true;
  }
  
  // Having inserted the new item we need to clean up the map to ensure that it stays bounded in size
  // Step 1: call isEmpty to check for any keys mapped to empty sets
  isEmpty();
  // Step 2: if the map is larger than some fixed bound, merge some key->value mappings together
  // !!! TODO !!!
  
  /*if(!containsMust(that)) {
    // The similarity between that and each AbstractObject currently in items
    map<AbstractObjectPtr, simType> thatSimilarity;
    
    // Compute the similarity between this item and all the items currently in the set
    for(std::list<AbstractObjectPtr>::iterator it = items.begin(); it != items.end(); it++) {
      if((*it)->mayEqual(that, latPEdge)) thatSimilarity[*it] = mayEqual;
      else                                thatSimilarity[*it] = notMayEqual;
    }
    
    // If adding this item will not push this set beyond its maximum size
    if(items.size()<maxElements) {
      items.push_back(that);
      
      // Incorporate that's similarity information into itemSimilarity
      for(map<AbstractObjectPtr, simType>::iterator item=thatSimilarity.begin(); item!=thatSimilarity.end(); item++) {
        itemSimilarity[*item][that] = mayEqual;
        itemSimilarity[that][*item] = mayEqual;
      }
    // Otherwise, find an existing item to merge this item into
    } else {
      // Records the similarity between each current item and the new item
      map<AbstractObjectPtr, int> simScore;
      for(std::list<AbstractObjectPtr>::iterator it = items.begin(); it != items.end(); it++) {
        simScore[*it]=0;
        for(std::map<AbstractObjectPtr, simType>::iterator itSim=items[*it].begin(); itSim!=items[*it].end(); itSim++) {
          if(itSim->first != that) {
          if(itSim->second == )
        }
      }
    }
    retval = true;
  }*/
  
  return retval;
}

// return true on successfull removal
// throws exception if element not found
// remove(o) : In may mode removes any AbstractObject in the set that is mustEqual to o to ensure that we still keep an 
//             over-approximation of the set of concrete objects in the AbstractObjectSet. In must mode uses mayEqual 
//             to ensure that all concrete objects that may equal to o are removed to keep the AbstractObjectSet an 
//             under-approximation.
bool AbstractObjectSet::remove(const AbstractObjectPtr that)
{
    indent ind(AbstractObjectSetDebugLevel, 1);
    bool retval = false;
    bool found = false;
    assert(that);
    std::list<AbstractObjectPtr>::iterator it = items.begin();
    //dbg << "AbstractObjectSet::remove("<<that->str("")<<")"<<endl;
    for(; it != items.end(); it++) {
        /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;it="<<(*it)->str("")<<" "<<(mode==may? "mustEqual": "mayEqual")<<"="<<((mode==may  && (*it)->mustEqual(that, latPEdge, comp, analysis)) ||
           (mode==must && (*it)->mayEqual(that, latPEdge, comp, analysis)))<<endl;*/
        if((mode==may  && (*it)->mustEqual(that, latPEdge, comp, analysis)) ||
           (mode==must && (*it)->mayEqual(that, latPEdge, comp, analysis))) {
            items.erase(it);
            found = true;
            break; // only one object that mustEqual(that) should be present
        }
    }

    // if element not found issue a notice
    if(!found) {
        //try {
        if(AbstractObjectSetDebugLevel>=1) {
          dbg << "<b>AbstractObjectSet::remove()"<<endl;
          indent ind(AbstractObjectSetDebugLevel, 1);
          dbg << "Cannot find "<<that->str("")<<endl;
          dbg << str("&nbsp;&nbsp;&nbsp;&nbsp;")<<"</b>"<<endl;
        }
        // throw "element not found";
    }
    // end for
    return retval;
}

// Returns true if a mustEqual is present; false otherwise
bool AbstractObjectSet::containsMust(const AbstractObjectPtr that) 
{
  /*dbg << "AbstractObjectSet::containsMust("<<that->str("")<<")"<<endl;
  indent(1, 1);*/
  assert(that);
  bool retval = false;
  std::list<AbstractObjectPtr>::iterator it = items.begin();
  for( ; it != items.end(); it++) {
    //dbg << "it="<<(*it)->str()<<" mustEqual="<<(*it)->mustEqual(that, latPEdge, comp, analysis)<<endl;
    if((*it)->mustEqual(that, latPEdge, comp, analysis)) {
      retval = true;
      break;
    } // end if
  } // end for

  return retval;
}

bool AbstractObjectSet::containsMay(const AbstractObjectPtr that)
{
  assert(that);
  bool retval = false;
  scope reg("AbstractObjectSet::containsMay()", scope::medium, AbstractObjectSetDebugLevel, 2);
  if(AbstractObjectSetDebugLevel>=2) dbg << "that="<<that->str()<<endl;
  indent ind(AbstractObjectSetDebugLevel, 2);
  std::list<AbstractObjectPtr>::iterator it = items.begin();
  for( ; it != items.end(); it++) {
    bool mayEqual = (*it)->mayEqual(that, latPEdge, comp, analysis);
    if(AbstractObjectSetDebugLevel>=2) dbg << "it="<<(*it)->str("")<<") mayEqual="<<mayEqual<<endl;
    if(mayEqual) {
      retval = true;
      break;
    } // end if
  } // end for
  return retval;
}

// Returns true if this set contains an AbstractObject that denotes the same set as that; false otherwise
bool AbstractObjectSet::containsEqualSet(const AbstractObjectPtr that) 
{
  /*dbg << "AbstractObjectSet::containsEqualSet("<<that->str("")<<")"<<endl;
  indent(1, 1);*/
  assert(that);
  bool retval = false;
  std::list<AbstractObjectPtr>::iterator it = items.begin();
  for( ; it != items.end(); it++) {
    /*dbg << "it="<<(*it)->str()<<endl;
    dbg << "equalSet="<<(*it)->equalSet(that, latPEdge, comp, analysis)<<endl;*/
    if((*it)->equalSet(that, latPEdge, comp, analysis)) {
      retval = true;
      break;
    } // end if
  } // end for

  return retval;
}

// Set this Lattice object to represent the set of all possible execution prefixes.
// Return true if this causes the object to change and false otherwise.
bool AbstractObjectSet::setToFull()
{
  bool modified = !setIsFull;
  items.clear();
  setIsFull = true;
  return modified;
}

// Set this Lattice object to represent the of no execution prefixes (empty set).
// Return true if this causes the object to change and false otherwise.
bool AbstractObjectSet::setToEmpty()
{
  bool modified = !items.empty();
  items.clear();
  return modified;
}

// Set all the information associated Lattice object with this MemLocObjectPtr to full.
// Return true if this causes the object to change and false otherwise.
// This function does nothing because it is a set of abstract objects rather than a map from some abstract objects
// to others. 
bool AbstractObjectSet::setMLValueToFull(MemLocObjectPtr ml)
{
  return false;
}

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool AbstractObjectSet::isFull()
{
  return setIsFull;
}

// Returns whether this lattice denotes the empty set.
bool AbstractObjectSet::isEmpty()
{
  // Check if all items are empty
  for(std::list<AbstractObjectPtr>::iterator i=items.begin(); i!=items.end();) {
    // If at least one is not empty, return false
    if(!(*i)->isEmpty(getPartEdge(), comp, analysis)) return false;
    
    // If this item is empty, remove it from the items list
    dbg << "AbstractObjectSet::isEmpty() removing "<<(*i)->str()<<endl;
    items.erase(i++);
  }
  // If all are empty, return true
  assert(items.size()==0);
  return true;
}

// debug: prints the elements of set as string
std::string AbstractObjectSet::str(std::string indent)
{
    return strp(latPEdge, indent);
}

// Variant of the str method that can produce information specific to the current Part.
// Useful since AbstractObjects can change from one Part to another.
std::string AbstractObjectSet::strp(PartEdgePtr pedge, std::string indent)
{
    ostringstream oss;
  
    std::list<AbstractObjectPtr>::iterator it = items.begin();
    oss << "[AbstractObjectSet ("<<items.size()<<")"<<endl;
    while(it != items.end()) {
        /*if(it != items.begin()) */oss << indent;
        oss << (*it)->strp(pedge, "&nbsp;&nbsp;&nbsp;&nbsp;");
        
        it++;
        if(it!=items.end())
          oss << std::endl;
    }
    oss << "]";
    
    return oss.str();
}
    
// -----------------
// Lattice methods
// initializes this Lattice to its default state, if it is not already initialized
void AbstractObjectSet::initialize()
{
  // Nothing to do here since Peter P's fixes will eliminate the need for lattices to maintain their own initialized state
}

// returns a copy of this lattice
Lattice* AbstractObjectSet::copy() const
{
  AbstractObjectSet *n = new AbstractObjectSet(*this);
  assert(n);
  return n;
}

// overwrites the state of this Lattice with that of that Lattice
void AbstractObjectSet::copy(Lattice* thatL)
{
  Lattice::copy(thatL);
  
  try {
    AbstractObjectSet *that = dynamic_cast <AbstractObjectSet*> (thatL);
    setIsFull = that->setIsFull;
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
// In must mode for each MemLocObject o in the set, if there exist any pairs <old, new> in ml2ml such that 
//    o mustEquals old, then new will be included in the final set.
// May mode is the same, except if for some pair <old, new> old mayEquals o but not mustEquals o then new is 
//    included in the final set but o is not removed.
Lattice* AbstractObjectSet::remapML(const std::set<MLMapping>& ml2ml, PartEdgePtr fromPEdge)
{
  scope remReg("AbstractObjectSet::remapML", scope::medium, AbstractObjectSetDebugLevel, 1);
  dbg << "latPEdge="<<latPEdge->str()<<endl;
  dbg << "getPartEdge()="<<getPartEdge()->str()<<endl;
  if(AbstractObjectSetDebugLevel>=1) {
    // If either the key or the value of this mapping is dead within its respective part, we skip it.
    // Print notices of this skipping once
    for(std::set<MLMapping>::const_iterator m=ml2ml.begin(); m!=ml2ml.end(); m++) {
      if(!m->from) continue;
      // If either the key or the value of this mapping is dead within its respective part, skip it
      if(!m->from->isLive(fromPEdge, comp, analysis) || (m->to && !m->to->isLive(latPEdge, comp, analysis)))
        dbg << "AbstractObjectSet::remapML() WARNING: Skipping dead ml2ml mapping "<<m->from->strp(fromPEdge)<<"(live="<<m->from->isLive(fromPEdge, comp, analysis)<<") =&gt; "<<(m->to ? m->to->strp(latPEdge) : "NULL")<<"(live="<<(m->to ? m->to->isLive(latPEdge, comp, analysis) : -1)<<")"<<endl;
    }
  }
  
  scope reg("AbstractObjectSet::remapML", scope::medium, AbstractObjectSetDebugLevel, 1);
  
  if(AbstractObjectSetDebugLevel>=1) {
    dbg << "AbstractObjectSet::remapML"<<endl;
    for(std::set<MLMapping>::const_iterator m=ml2ml.begin(); m!=ml2ml.end(); m++) {
      if(!m->from) continue;
      dbg << m->from.get()->str() << " =&gt; " << (m->to? m->to.get()->strp(latPEdge): "NULL") << endl;
    }
    dbg << "this="<<str()<<endl;
  }
  
  AbstractObjectSet* newS = new AbstractObjectSet(latPEdge, comp, analysis, mode);
  // Set of ml2ml values that need to be added to newS because they match (may-equal or must-equal)
  // MemLocObjects currently in items
  set<MemLocObjectPtr> vals2add;
  
  for(std::list<AbstractObjectPtr>::iterator i=items.begin(); i!=items.end(); i++) {
    scope regI(txt() << "item "<<i->get()->str(), scope::medium, AbstractObjectSetDebugLevel, 2);
    // Flags that indicate whether the current item is mayEqual or mustEqual any keys in ml2ml
    bool existsMustEqual=false;
    bool existsMayEqual=false;

    for(std::set<MLMapping>::const_iterator m=ml2ml.begin(); m!=ml2ml.end(); m++) {
      if(!m->from) continue;
      scope regM(txt() << "m="<<m->str(), scope::low, AbstractObjectSetDebugLevel, 2);
      dbg << "m->from->isLive = "<<m->from->isLive(fromPEdge, comp, analysis)<<endl;
      dbg << "m->to->isLive = "<<(m->to? m->to->isLive(latPEdge, comp, analysis): -1)<<endl;
      // If either the key or the value of this mapping is dead within its respective part, skip it
      if(!m->from->isLive(fromPEdge, comp, analysis) || (m->to && !m->to->isLive(latPEdge, comp, analysis))) continue;
      
      // If the current item in this set may- or must-equals a key in ml2ml, record this and add the corresponding
      // value in ml2ml to be added to newS
      if((*i)->mustEqual(m->from, fromPEdge, comp, analysis)) {
        if(AbstractObjectSetDebugLevel>=2) dbg << "mustEqual"<<endl;
        existsMustEqual = true;
        // Insert the corresponding value in ml2ml if it is not NULL
        if(m->to) vals2add.insert(m->to);
      } else if(mode == may && (*i)->mayEqual(m->from, fromPEdge, comp, analysis)) {
        if(AbstractObjectSetDebugLevel>=2) dbg << "mayEqual"<<endl;
        existsMayEqual = true;
        // Insert the corresponding value in ml2ml if it is not NULL
        vals2add.insert(m->to);
      }
    }
    if(AbstractObjectSetDebugLevel>=2) dbg << "existsMustEqual="<<existsMustEqual<<" existsMayEqual="<<existsMayEqual<<endl;
    
    // If this item is not must-equal to some key(s) in ml2ml, copy it over to newS
    // This check ensures that if there are mappings in ml2ml from valid MemLocs to NULL (denote MemLocs the lifetime
    // of which is limited to a given function), any elements in items that are must-equal to them are removed.
    if(!existsMustEqual) {
      // Skip items that are dead in latPEdge
      if(!(*i)->isLive(latPEdge, comp, analysis)) continue;
      newS->items.push_back(*i);
    }
    // Otherwise, we skip this item since it will be replaced by the value(s) of the key(s) it was must-equal to
  }
  
  // Now add the values of all the keys in ml2ml that got matched to this set's items
  if(AbstractObjectSetDebugLevel>=1) dbg << "vals2add=" << endl;
  for(set<MemLocObjectPtr>::iterator v=vals2add.begin(); v!=vals2add.end(); v++) {
    indent ind(AbstractObjectSetDebugLevel,1);
    if(AbstractObjectSetDebugLevel>=1) dbg << (*v)->str("") << endl;
    newS->items.push_back(*v);
  }
  
  return newS;
}

// Adds information about the MemLocObjects in newL to this Lattice, overwriting any information previously 
//    maintained in this lattice about them.
// Returns true if the Lattice state is modified and false otherwise.
bool AbstractObjectSet::replaceML(Lattice* newL)
{
  AbstractObjectSet* calleeAOS = dynamic_cast<AbstractObjectSet*>(newL);
  assert(calleeAOS);
  
  bool modified = false;
  
  for(std::list<AbstractObjectPtr>::iterator i=calleeAOS->items.begin(); i!=calleeAOS->items.end(); i++) {
    MemLocObjectPtr ml = boost::dynamic_pointer_cast<MemLocObject>(*i);
    assert(ml);
    // Do not copy over mappings with keys that are dead in this set's host part
    if(!ml->isLive(latPEdge, comp, analysis)) continue;
    
    modified = insert(ml) || modified;
  }
  
  return modified;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
// The part of this object is to be used for AbstractObject comparisons.
// meet(s1, s2) : In may mode uses insert() to add AbstractObjects from both s1 and s2 to the meet. In must mode only
//             inserts objects into the meet that must exist in both AbstractObjectSets.
bool AbstractObjectSet::meetUpdate(Lattice* thatL)
{
  try {
    scope reg(txt()<<"AbstractObjectSet::meetUpdate("<<(mode==may? "may": "must")<<")", scope::medium, AbstractObjectSetDebugLevel, 2);
    AbstractObjectSet *that = dynamic_cast <AbstractObjectSet*> (thatL);
    if(AbstractObjectSetDebugLevel>=2) {
      { scope thisScope("this", scope::low);
      dbg << str()<<endl; }
      { scope thisScope("that", scope::low);
      dbg << that->str()<<endl; }
    }
    if(setIsFull) return false;
    if(that->setIsFull) {
      setToFull();
      return true;
    }
    
    // Copy over from that all the elements that don't already exist in this
    bool modified = false;
    if(mode == may) {
      if(AbstractObjectSetDebugLevel>=2) dbg << "latPEdge="<<latPEdge->str()<<endl;
      if(AbstractObjectSetDebugLevel>=2) dbg << "that->items("<<that->items.size()<<"="<<endl;
      indent ind(AbstractObjectSetDebugLevel, 2);
      for(std::list<AbstractObjectPtr>::iterator it=that->items.begin(); it!=that->items.end(); it++) {
        bool itLive = (*it)->isLive(latPEdge, comp, analysis);
        if(AbstractObjectSetDebugLevel>=2) dbg << "it(live="<<itLive<<")="<<it->get()->str()<<endl;
        
        // Do not copy over mappings with keys that are dead in this set's host part
        if(!itLive) continue;
        
        modified = insert(*it) || modified;
        if(AbstractObjectSetDebugLevel>=2) dbg << "modified = "<<modified<<endl;
      }
    } else if(mode==must) {
      // Remove all the AbstractObjects in this that do not also appear in that
      for(std::list<AbstractObjectPtr>::iterator it=that->items.begin(); it!=that->items.end(); it++) {
        // Do not copy over mappings with keys that are dead in this set's host part
        if(!(*it)->isLive(latPEdge, comp, analysis)) continue;
        
        if(!containsMust(*it))
          modified = remove(*it) || modified;
      }
    }

    return modified;
  } catch (bad_cast & bc) { 
    assert(false);
  }
}

bool AbstractObjectSet::operator==(Lattice* thatL)
{
  assert(latPEdge == thatL->getPartEdge());
  try {
    AbstractObjectSet *that = dynamic_cast <AbstractObjectSet*> (thatL);
    assert(comp     == that->comp);
    assert(analysis == that->analysis);
    // GB: This is a quadratic time comparison. Can make it linear if we sort the objects somehow.
    
    // Iterate through this->items and confirm that all its elements are in that->items
    for(std::list<AbstractObjectPtr>::iterator it=items.begin(); it!=items.end(); it++)
    if(!that->containsMust(*it)) return false;
    
    // Iterate through that->items and confirm that all its elements are in this->items
    for(std::list<AbstractObjectPtr>::iterator it=that->items.begin(); it!=that->items.end(); it++)
    if(!that->containsMust(*it)) return false;

    // Everything aligns perfectly
    return true;
  } catch (bad_cast & bc) { 
    assert(false);
  }
}

}; // end namespace
