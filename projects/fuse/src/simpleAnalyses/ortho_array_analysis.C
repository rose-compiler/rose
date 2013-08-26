#include "sage3basic.h"
#include "ortho_array_analysis.h"
#include "compose.h"
#include <boost/make_shared.hpp>

using namespace std;
using namespace dbglog;

namespace fuse {
int orthoArrayAnalysisDebugLevel=0;

/*********************************
 ***** OrthoIndexVector_Impl *****
 *********************************/

std::string OrthoIndexVector_Impl::str(std::string indent) const // pretty print for the object  
{
  string rt;
  for(std::vector<ValueObjectPtr> ::const_iterator iter = index_vector.begin(); iter != index_vector.end(); iter++)
  {
    ValueObjectPtr current_index_field = *iter;
    rt += current_index_field->str(indent+"    ");
    if(iter != index_vector.begin()) rt += ", ";
  }
  return rt;
}

// Allocates a copy of this object and returns a pointer to it
IndexVectorPtr OrthoIndexVector_Impl::copyIV() const
{
  OrthoIndexVector_ImplPtr newIV = boost::make_shared<OrthoIndexVector_Impl>();
  for (std::vector<ValueObjectPtr>::const_iterator iter = index_vector.begin(); iter != index_vector.end(); iter++)
    newIV->index_vector.push_back((*iter)->copyV());
  return newIV;
}

bool OrthoIndexVector_Impl::mayEqual(IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis)
{
  //dbg << "OrthoIndexVector_Impl::mayEqual()"<<endl;

  OrthoIndexVector_ImplPtr other_impl = boost::dynamic_pointer_cast<OrthoIndexVector_Impl>(other);
  /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;this="<<str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;other="<<other->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;*/

  // If other is not of a compatible type
  if(!other_impl) {
    // Cannot be sure that objects are not equal, so conservatively state they may be equal
    return true;
  }
  bool rt = false;

  bool has_diff_element = false;
  if (this->getSize() == other_impl->getSize()) 
  { // same size, no different element
    for (size_t i =0; i< other_impl->getSize(); i++)
    {
      //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<i<<" : mayEqual="<<this->index_vector[i]->mayEqual(other_impl->index_vector[i], p)<<endl;
      if (!(this->index_vector[i]->mayEqual(other_impl->index_vector[i], pedge, comp, analysis)))
      {
        has_diff_element = true;
        break;
      }
    }
    if (!has_diff_element )
      rt = true;
  }
  //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;"<<(rt ? "MAY-EQUAL": "NOT mayEqual")<<endl;
  return rt; 
}

bool OrthoIndexVector_Impl::mustEqual(IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis)
{
  //dbg << "OrthoIndexVector_Impl::mayEqual()"<<endl;
  
  OrthoIndexVector_ImplPtr other_impl = boost::dynamic_pointer_cast<OrthoIndexVector_Impl>(other);
  /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;this="<<str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;other="<<other->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;*/

  // If other is not of a compatible type
  if(!other_impl) {
    // Cannot be sure that objects must be equal, so conservatively don't claim this
    return false;
  }
  bool rt = false;

  bool has_diff_element = false;
  if (this->getSize() == other_impl->getSize()) 
  { // same size, no different element
    for (size_t i =0; i< other_impl->getSize(); i++)
    {
      //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<i<<" : mustEqual="<<this->index_vector[i]->mustEqual(other_impl->index_vector[i], p)<<endl;
      if (!(this->index_vector[i]->mustEqual(other_impl->index_vector[i], pedge, comp, analysis)))
      {
        has_diff_element = true;
        break;
      }
    }
    if (!has_diff_element )
      rt = true;
  }
  //dbg << "OrthoIndexVector_Impl: "<<(rt ? "MUST-EQUAL": "NOT mustEqual")<<endl;
  return rt;
}

// Returns whether the two abstract IndexVectors denote the same set of concrete vectors
bool OrthoIndexVector_Impl::equalSet(IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis)
{
  //dbg << "OrthoIndexVector_Impl::equalSet()"<<endl;
  
  OrthoIndexVector_ImplPtr other_impl = boost::dynamic_pointer_cast<OrthoIndexVector_Impl>(other);
  /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;this="<<str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;other="<<other->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;*/

  // If other is not of a compatible type
  if(!other_impl) {
    // Cannot be sure that objects must be equal, so conservatively don't claim this
    return false;
  }
  bool rt = false;

  bool has_diff_element = false;
  if (this->getSize() == other_impl->getSize()) 
  { // same size, no different element
    for (size_t i =0; i< other_impl->getSize(); i++)
    {
      //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<i<<" : mustEqual="<<this->index_vector[i]->mustEqual(other_impl->index_vector[i], p)<<endl;
      if (!(this->index_vector[i]->equalSet(other_impl->index_vector[i], pedge, comp, analysis)))
      {
        has_diff_element = true;
        break;
      }
    }
    if (!has_diff_element )
      rt = true;
  }
  //dbg << "OrthoIndexVector_Impl: "<<(rt ? "MUST-EQUAL": "NOT mustEqual")<<endl;
  return rt;
}

// Returns whether this abstract IndexVector denotes a non-strict subset (the sets may be equal) of the set denoted
// by the given abstract IndexVector.
bool OrthoIndexVector_Impl::subSet(IndexVectorPtr that_arg, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis)
{
  //dbg << "OrthoIndexVector_Impl::subSet()"<<endl;
  
  OrthoIndexVector_ImplPtr that = boost::dynamic_pointer_cast<OrthoIndexVector_Impl>(that_arg);
  /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;this="<<str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;other="<<other->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;*/

  // If that is not of a compatible type
  if(!that) {
    // Cannot be sure what the relationship between the objects may be, so conservatively don't claim any
    return false;
  }
  bool rt = false;

  bool has_diff_element = false;
  if (this->getSize() == that->getSize()) 
  { // same size, and each element in this is a sub-set of each element in that
    for (size_t i =0; i< that->getSize(); i++)
    {
      //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<i<<" : mustEqual="<<this->index_vector[i]->mustEqual(that->index_vector[i], p)<<endl;
      if (!(this->index_vector[i]->subSet(that->index_vector[i], pedge, comp, analysis)))
      {
        has_diff_element = true;
        break;
      }
    }
    if (!has_diff_element )
      rt = true;
  }
  //dbg << "OrthoIndexVector_Impl: "<<(rt ? "MUST-EQUAL": "NOT mustEqual")<<endl;
  return rt;
}

bool OrthoIndexVector_Impl::isFull (PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis)
{
  // Return false if any sub-index is not full
  for(vector<ValueObjectPtr>::iterator i=index_vector.begin(); i!=index_vector.end(); i++)
    if(!(*i)->isFull(pedge)) return false;
  // Return true if all sub-indexes are full
  return true;
}

bool OrthoIndexVector_Impl::isEmpty(PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis)
{
  // Return false if any sub-index is not empty
  for(vector<ValueObjectPtr>::iterator i=index_vector.begin(); i!=index_vector.end(); i++)
    if(!(*i)->isEmpty(pedge)) return false;
  // Return true if all sub-indexes are empty
  return true;
}

bool OrthoArrayML::mayEqualML(MemLocObjectPtr other, PartEdgePtr pedge)
{
  scope reg(txt()<<"OrthoArrayML::mayEqualML("<<other->str()<<")", scope::medium, orthoArrayAnalysisDebugLevel, 1);
  OrthoArrayMLPtr that = boost::dynamic_pointer_cast<OrthoArrayML>(other);
  assert(that);
  
  if(orthoArrayAnalysisDebugLevel>=1) 
    dbg << "level="<<(level==empty? "empty": (level==array? "array": (level==notarray? "notarray": (level==full? "full": "???"))))<<", "<<
         "that->level="<<(that->level==empty? "empty": (that->level==array? "array": (that->level==notarray? "notarray": (that->level==full? "full": "???"))))<<", "<<endl;

  // If both objects denote empty sets, they're definitely equal
  if(this->level==empty && that->level==empty) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>May-Equal</b>"<<endl;
    return true;
  // If one is empty and the other is not, they're definitely not equal
  } else if(this->level==empty || that->level==empty) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>Not May-Equal</b>"<<endl;
    return false;
  }
  
  // If either object is full, they may be equal
  if(this->level==full  || that->level==full) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>May-Equal</b>"<<endl;
    return true;
  }
  
  // If the two objects are different types, they're not equal
  if( (this->level==array    && that->level==notarray) ||
      (this->level==notarray && that->level==array)) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>May-Equal</b>"<<endl;
    return false;
  }

  // both are array element
  if(level==array && that->level==array) {
    // making sure both are array elements
    assert(that->p_array); assert(that->p_iv);
    assert(p_array); assert(p_iv);

    // return true only if 
    // array objects mayequals the other and
    // index vector mayequals the other
    bool areMayEqual = (this->p_array)->mayEqual(that->p_array, pedge, oaa->getComposer(), oaa) &&
                       (this->p_iv)->mayEqual(that->p_iv, pedge, oaa->getComposer(), oaa);
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>"<<(areMayEqual?"":"Not ")<<"May-Equal</b>"<<endl;
    return areMayEqual;
  }
  // both ML are not array elements
  else if(level==notarray && that->level==notarray) {
    // making sure both are not array
    assert(p_notarray); assert(that->p_notarray);
    bool areMayEqual = p_notarray->mayEqual(that->p_notarray, pedge, oaa->getComposer(), oaa);
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>"<<(areMayEqual?"":"Not ")<<"May-Equal</b>"<<endl;
    return areMayEqual;
  }
  assert(0);
}

bool OrthoArrayML::mustEqualML(MemLocObjectPtr other, PartEdgePtr pedge)
{
  scope reg(txt()<<"OrthoArrayML::mustEqualML("<<other->str()<<")", scope::medium, orthoArrayAnalysisDebugLevel, 1);
  OrthoArrayMLPtr that = boost::dynamic_pointer_cast<OrthoArrayML>(other);
  
  // If both objects denote empty sets, they're definitely equal
  if(this->level==empty && that->level==empty) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>Must-Equal</b>"<<endl;
    return true;
  // If one is empty and the other is not, they're definitely not equal
  } else if(this->level==empty || that->level==empty) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>Not Must-Equal</b>"<<endl;
    return false;
  }
  
  // If either object is full, they may or may not be equal
  if(this->level==full  || that->level==full) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>Not Must-Equal</b>"<<endl;
    return false;
  }
  
  // if its not an array object, we know they are not equal
  // compare only if two objects are same types
  if( (this->level==array    && that->level==notarray) ||
      (this->level==notarray && that->level==array) ) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>Not Must-Equal</b>"<<endl;
    return false;
  }
  
  // both are array element
  if(level==array && that->level==array) {
    // making sure both are array elements
    assert(that->p_array); assert(that->p_iv);
    assert(p_array); assert(p_iv);

    // return true only if 
    // array objects mayequals the other and
    // index vector mayequals the other
    bool areMustEqual = (this->p_array)->mustEqual(that->p_array, pedge, oaa->getComposer(), oaa) && 
                        (this->p_iv)->mustEqual(that->p_iv, pedge, oaa->getComposer(), oaa);
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>"<<(areMustEqual?"":"Not ")<<"Must-Equal</b>"<<endl;
    return areMustEqual;
  }
  // both ML are not array elements
  else if(level==notarray && that->level==notarray) {
    // making sure both are not array
    assert(p_notarray); assert(that->p_notarray);
    bool areMustEqual = p_notarray->mustEqual(that->p_notarray, pedge, oaa->getComposer(), oaa);
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>"<<(areMustEqual?"":"Not ")<<"Must-Equal</b>"<<endl;
    return areMustEqual;
  }
  assert(0);
}

// Returns whether the two abstract objects denote the same set of concrete objects
bool OrthoArrayML::equalSet(AbstractObjectPtr that_arg, PartEdgePtr pedge)
{
  OrthoArrayMLPtr that = boost::dynamic_pointer_cast<OrthoArrayML>(that_arg);
  
  // If both objects denote empty sets, they're definitely equal
  if(this->level==empty && that->level==empty) return true;
  // If one is empty and the other is not, they're definitely not equal
  else if(this->level==empty || that->level==empty) return false;
  
  // If either object is full, they denote the same set of all MemLocObjects
  if(this->level==full  || that->level==full) return true;
  
  // If its not an array object, we know that the two objects denote the same set only if they are same types
  if( (this->level==array    && that->level==notarray) ||
      (this->level==notarray && that->level==array) ) return false;
  
  // both are array element
  if(level==array && that->level==array) {
    // making sure both are array elements
    assert(that->p_array); assert(that->p_iv);
    assert(p_array); assert(p_iv);

    // return true only if 
    // array objects denote the same set and
    // index vector denote the same set 
    return(this->p_array)->equalSet(that->p_array, pedge, oaa->getComposer(), oaa) && 
          (this->p_iv)->equalSet(that->p_iv, pedge, oaa->getComposer(), oaa);
  }
  // both ML are not array elements
  else if(level==notarray && that->level==notarray) {
    // making sure both are not array
    assert(p_notarray); assert(that->p_notarray);
    return p_notarray->equalSet(that->p_notarray, pedge, oaa->getComposer(), oaa);
  }
  assert(0);
}

// Returns whether this abstract IndexVector denotes a non-strict subset (the sets may be equal) of the set denoted
// by the given abstract IndexVector.
bool OrthoArrayML::subSet(AbstractObjectPtr that_arg, PartEdgePtr pedge)
{
  OrthoArrayMLPtr that = boost::dynamic_pointer_cast<OrthoArrayML>(that_arg);
  
  // If both objects denote empty sets, they're definitely equal
  if(this->level==empty && that->level==empty) return true;
  // If one is empty and the other is not, then this is a subset of that
  else if(this->level==empty || that->level==empty) return false;
  
  // If either object is full, they denote the same set of all MemLocObjects
  if(this->level==full  || that->level==full) return true;
  
  // If its not an array object, we know that this object is a subset of that only if they are same types
  if( (this->level==array    && that->level==notarray) ||
      (this->level==notarray && that->level==array) ) return false;
  
  // Both are array elements
  if(level==array && that->level==array) {
    // Baking sure both are array elements
    assert(that->p_array); assert(that->p_iv);
    assert(p_array); assert(p_iv);

    // Return true only if 
    // this->array is a subset of that->array and
    // this->index is a subset of that->index
    return(this->p_array)->subSet(that->p_array, pedge, oaa->getComposer(), oaa) && 
          (this->p_iv)->subSet(that->p_iv, pedge, oaa->getComposer(), oaa);
  }
  // Both ML are not array elements
  else if(level==notarray && that->level==notarray) {
    // Making sure both are not array
    assert(p_notarray); assert(that->p_notarray);
    return p_notarray->subSet(that->p_notarray, pedge, oaa->getComposer(), oaa);
  }
  assert(0);
}

//NOTE: Do we have to always re-implement this for every analysis
bool OrthoArrayML::isLiveML(PartEdgePtr pedge)
{
  scope reg(txt()<<"OrthoArrayML::isLiveML() "<<(level==empty? "empty": (level==array? "array": (level==notarray? "notarray": (level==full? "full": "???")))), scope::medium, orthoArrayAnalysisDebugLevel, 1);
  // if the array is live, element is live
  if(level==array) {
    /*bool live = oaa->getComposer()->OperandIsLiveMemLoc(array_ref, isSgPntrArrRefExp(array_ref)->get_lhs_operand(), p_array, pedge, oaa) ||
                p_array->isLive(pedge, oaa->getComposer(), oaa);
    //dbg << "OrthoArrayML::isLive() = "<<live<<endl;
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>"<<(live?"LIVE":"DEAD")<<"</b>"<<endl;
    return live;*/
    
    // Always return live since OrthogonalArrayAnalysis knows nothing about whether a memory location is live
    //   or not. Forwarding the question to prior analyses seems like the reasonable option but in fact is wrong.
    //   For example, if this ML denotes array[i], what do we learn from the fact that the memory block pointed to 
    //   by array is live at the edge where this OrthoArrayML object was created? 
    // In fact, the way liveness should
    //   work is that some liveness analysis filters out all the dead MemLocs so that subsequent analyses can
    //   only reason about live MemLocs and construct their MemLocs based on only on the live ones. If additional
    //   liveness filtering is needed we should run another liveness analysis to compute it based on actual defs
    //   and uses.
    return true;
    
  } else if(level==notarray) {
    //dbg << "OrthoArrayML::isLiveML() p_notarray: "<<str()<<endl;
    bool live = p_notarray->isLive(pedge, oaa->getComposer(), oaa);
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>"<<(live?"LIVE":"DEAD")<<"</b>"<<endl;
    return live;
  } else if(level==full) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>LIVE</b>"<<endl;
    return true;
  } else if(level==empty) {
    if(orthoArrayAnalysisDebugLevel>=1) dbg << "<b>DEAD</b>"<<endl;
    return false;
  }
  assert(0);
}

// Computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool OrthoArrayML::meetUpdateML(MemLocObjectPtr o, PartEdgePtr pedge)
{
  OrthoArrayMLPtr that = boost::dynamic_pointer_cast<OrthoArrayML>(o);
  assert(that);
  
  if(level==array && that->level==array) {
    // If needed, copy p_array and p->iv before updating them in-place
    if(origML) {
      p_array = p_array->copyML();
      p_iv    = p_iv->copyIV();
      origML = false;
    }
    bool modified = p_array->meetUpdate(that->p_array, pedge, oaa->getComposer(), oaa);
    return p_iv->meetUpdate(that->p_iv, pedge, oaa->getComposer(), oaa) || modified;
  } else if(level==notarray && that->level==notarray) {
    // If needed, copy p_notarray before updating it in-place
    if(origML) {
      p_notarray = p_notarray->copyML();
      origML = false;
    }
    return p_notarray->meetUpdate(that->p_notarray, pedge, oaa->getComposer(), oaa);
  // If this object denotes the empty set, the meet is equal to that object
  } else if(level==empty) {
    if(that->level == empty) return false;
    else {
      level      = that->level;
      p_array    = that->p_array;
      p_iv       = that->p_iv;
      p_notarray = that->p_notarray;
      origML     = that->origML;
      // it is ok to overwrite array_ref with that->array_ref since it is used only in the call to isLiveML
      // and it is true that since this object was originally the empty set, the union of this and that 
      // is live iff that is live
      array_ref  = that->array_ref;
      return true;
    }
  // if that object denotes the empty set or this object denotes the full set, the meet is just this object
  } else if(that->level==empty || level == full) {
    return false;
  // If that object is full or this and that denote different object types, the meet is full
  } else if(that->level==full || 
            (level==array && that->level==notarray) || 
            (level==notarray && that->level==array)) {
    return setToFull();
  }
  assert(0);
}

// Returns whether this AbstractObject denotes the set of all possible execution prefixes.
bool OrthoArrayML::isFull(PartEdgePtr pedge)
{
  /*// The array reference refers to the set of all array references if both the array and the index sets are full
  return p_array->isFull(pedge, oaa->getComposer(), oaa) &&
         p_iv->isFull(pedge, oaa->getComposer(), oaa);*/
  // Array references cannot denote the set of all MemLocs since some MemLocs are not array references.
  if(level==array) return false;
  // MemLocs that are not specifically array expressions may be anything, so if p_notarray is full, then this really
  // means that it denotes the set of all MemLocs
  else if(level==notarray) {
    // Check whether p_notarray is full and if so, update this object's state and return 
    if(p_notarray->isFull(pedge)) {
      setToFull();
      return true;
    } else
      return false;
  } else
    return level==full;
}

// Returns whether this AbstractObject denotes the empty set.
bool OrthoArrayML::isEmpty(PartEdgePtr pedge)
{
  if(level==array) {
    // The array reference refers to the empty set if either the array or the index are empty
    if((level==array && (p_array->isEmpty(pedge, oaa->getComposer(), oaa) ||
                         p_iv->isEmpty(pedge, oaa->getComposer(), oaa)))
       || 
       // If this is non-array, then forward the request to its isEmpty method
       ((level==notarray) && (p_notarray->isEmpty(pedge, oaa->getComposer(), oaa)))) {
      // If this object is empty, update its state accordingly
      setToEmpty();
      return true;
    } else
      return false;
  } else
    return level==empty;
}

// Set this object to represent the set of all possible MemLocs
// Return true if this causes the object to change and false otherwise.
bool OrthoArrayML::setToFull()
{
  bool modified = (level != full);
  level      = full;
  p_array.reset();
  p_iv.reset();
  p_notarray.reset();
  array_ref  = NULL;
  origML     = true;
  return modified;
}
// Set this Lattice object to represent the empty set of MemLocs.
// Return true if this causes the object to change and false otherwise.
bool OrthoArrayML::setToEmpty()
{
  bool modified = (level != empty);
  level      = empty;
  p_array.reset();
  p_iv.reset();
  p_notarray.reset();
  array_ref  = NULL;
  origML     = true;
  return modified;
}

std::string OrthoArrayML::str(std::string indent) const
{
  ostringstream oss;
  oss << "[OrthoArrayML: ";
  if(level==array) {
    oss << p_array->str(indent) << ", ";
    oss << p_iv->str(indent) << " ";
  } else if(level==notarray){
    oss << p_notarray->str(indent);
  } else if(level==full)
    oss << "full";
  else if(level==empty) 
    oss << "empty";
  oss << "]";
  return oss.str();
}


/**********************************
 ***** OrthoArrayMemLocObject *****
 ********************************** /
OrthoArrayMemLocObject::OrthoArrayMemLocObject(MemLocObjectPtr array, OrthoIndexVector_ImplPtr iv, PartPtr p)
  : part(p)
{ 
  dbg << "OrthoArrayMemLocObject::OrthoArrayMemLocObject()"<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;array="<<array->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;iv="<<iv->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  
  // Since this is a constructor for a[i] objects, array must be an array object
  // GB: what happens if array is a pointer? Our API doesn't support indexing into pointers
  ArrayPtr a = isArray(array);
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;a="<<a<<"="<<a.get()<<endl;
  assert(a);
  arrayElt = a->getElements(iv);
}

OrthoArrayMemLocObject::OrthoArrayMemLocObject(MemLocObjectPtr notArray, PartPtr p)
  : notArray(notArray), part(p)
{ }

OrthoArrayMemLocObject::OrthoArrayMemLocObject(const OrthoArrayMemLocObject& that): arrayElt(that.arrayElt), notArray(that.notArray), part(that.part)
{}

bool OrthoArrayMemLocObject::mayEqual(MemLocObjectPtr o, PartPtr part) const
{
  OrthoArrayMemLocObjectPtr that = boost::dynamic_pointer_cast<OrthoArrayMemLocObject>(o);
  dbg << "OrthoArrayMemLocObject::mayEqual()"<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;this="<<str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;that=("<<that.get()<<")="<<o->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  
  // If that is of the right type
  if(that) {
    // And this and that are both array elements
    if(arrayElt && that->arrayElt) {
      dbg << "&nbsp;&nbsp;&nbsp;&nbsp;arrayElt->mayEqual(that->arrayElt, part)="<<arrayElt->mayEqual(that->arrayElt, part)<<endl;
      return arrayElt->mayEqual(that->arrayElt, part);
    // And this and that are both not array elements
    } else if(notArray && that->notArray) {
      dbg << "&nbsp;&nbsp;&nbsp;&nbsp;notArray->mayEqual(that->notArray, part)"<<notArray->mayEqual(that->notArray, part)<<endl;
      return notArray->mayEqual(that->notArray, part);
    }
    else
      return false;
  } else
    return false;
}

bool OrthoArrayMemLocObject::mustEqual(MemLocObjectPtr o, PartPtr part) const
{
  OrthoArrayMemLocObjectPtr that = boost::dynamic_pointer_cast<OrthoArrayMemLocObject>(o);
  dbg << "OrthoArrayMemLocObject::mustEqual()"<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;this="<<str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;that=("<<that.get()<<")="<<o->str("&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
  
  // If that is of the right type
  if(that) {
    // And this and that are both array elements
    if(arrayElt && that->arrayElt) {
      dbg << "&nbsp;&nbsp;&nbsp;&nbsp;arrayElt->mustEqual(that->arrayElt, part)="<<arrayElt->mustEqual(that->arrayElt, part)<<endl;
      return arrayElt->mustEqual(that->arrayElt, part);
    // And this and that are both not array elements
    } else if(notArray && that->notArray) {
      dbg << "&nbsp;&nbsp;&nbsp;&nbsp;notArray->mustEqual(that->notArray, part)"<<notArray->mustEqual(that->notArray, part)<<endl;
      return notArray->mustEqual(that->notArray, part);
    } else
      return false;
  } else
    return false;
}

// pretty print for the object
std::string OrthoArrayMemLocObject::str(std::string indent) const
{
    return strp(part, indent);
}

std::string OrthoArrayMemLocObject::strp(PartPtr part, std::string indent) const
{
  ostringstream oss;
  oss << "[OrthoArrayMemLocObject: "<<(arrayElt? "arrayElt: ": "")<<(arrayElt? arrayElt->str("&nbsp;&nbsp;&nbsp;&nbsp;"): "")<<
                                      (notArray? "notArray: ": "")<<(notArray? notArray->str("&nbsp;&nbsp;&nbsp;&nbsp;"): "")<<"]";
  return oss.str();
}

// Allocates a copy of this object and returns a pointer to it
MemLocObjectPtr OrthoArrayMemLocObject::copyML() const
{
  return boost::make_shared<OrthoArrayMemLocObject>(*this);
}*/

/*******************************
 ***** OrthogonalArrayAnalysis *****
 *******************************/

// Maps the given SgNode to an implementation of the MemLocObject abstraction.
MemLocObjectPtr OrthogonalArrayAnalysis::Expr2MemLoc(SgNode* n, PartEdgePtr pedge)
{
  // If this is a top-most array index expression
  if(isSgPntrArrRefExp(n) && 
     (!isSgPntrArrRefExp (n->get_parent()) || !isSgPntrArrRefExp (isSgPntrArrRefExp (n->get_parent())->get_lhs_operand()))) {
    SgExpression* arrayNameExp = NULL;
    std::vector<SgExpression*>* subscripts = new std::vector<SgExpression*>;
    SageInterface::isArrayReference(isSgPntrArrRefExp(n), &arrayNameExp, &subscripts);
    // MemLocObjectPtrPair array = composer->Expr2MemLoc(arrayNameExp, pedge, this);
    //MemLocObjectPtr array = composer->Expr2MemLoc(arrayNameExp, pedge, this);
    MemLocObjectPtr array = composer->OperandExpr2MemLoc(n, arrayNameExp, pedge, this);

    OrthoIndexVector_ImplPtr iv = boost::make_shared<OrthoIndexVector_Impl>();
    
    /*dbg << "Predecessor Nodes #("<<p.inEdges().size()<<")="<<endl;
    indent ind(1,1);
    for(std::vector<DataflowEdge>::const_iterator in=p.inEdges().begin(); in!=p.inEdges().end(); in++)
      dbg << "["<<((*in).source().getNode() ? (*in).source().getNode()->unparseToString() : "NULL")<<" | "<<
                       ((*in).source().getNode() ? (*in).source().getNode()->class_name()      : "NULL")<<" | "<<(*in).source().getIndex()<<"]"<<endl;*/
    
    for (std::vector<SgExpression*>::iterator iter = subscripts->begin(); iter != subscripts->end(); iter++) {
      iv->index_vector.push_back(composer->OperandExpr2Val(n, *iter, pedge, this));
    }
    
    // MemLocObjectPtr tmp = array.mem ? array.mem->isArray()->getElements(iv, pedge) :
    //                                   array.expr->isArray()->getElements(iv, pedge);
    // assert(array->isArray());
    // MemLocObjectPtr tmp = array->isArray()->getElements(iv, pedge);
    assert(array); assert(iv);
    MemLocObjectPtr tmp = boost::make_shared<OrthoArrayML>(n, array, iv, this);

    // GB: Do we need to deallocate subscripts???
    if(orthoArrayAnalysisDebugLevel>=1) {
      dbg << "OrthogonalArrayAnalysis::Expr2MemLoc() "<<endl;
      dbg << "&nbsp;&nbsp;&nbsp;&nbsp;"<<tmp->str("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
    }
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;result="<<tmp<<"="<<tmp.get()<<endl;
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;result="<<tmp->str("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
    return tmp;
    //return boost::make_shared<OrthoArrayML>(array, iv, p);
  } else {    
    MemLocObjectPtr notArray = composer->Expr2MemLoc(n, pedge, this);

    //NOTE: if the analysis does not handle some expressions,
    // it must wrap them with its own memory/value object to
    // ensure consistent wrapping by the composer.
    return boost::make_shared<OrthoArrayML>(n, notArray, this);
  }
}

}; // namespace fuse
