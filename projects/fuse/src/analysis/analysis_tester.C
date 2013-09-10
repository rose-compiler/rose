#include "sage3basic.h"
#include "analysis_tester.h"
#include "factor_trans_system.h"

using namespace std;
using namespace dbglog;

namespace fuse {

int analysisTesterDebugLevel=1;
  
/********************
 ***** HierEdge *****
 ********************/

template <class EdgeTypePtr>  
HierEdge<EdgeTypePtr>::HierEdge(bool singleVisit) : singleVisit(singleVisit){
  l = makePtr<HierEdge_Leaf<EdgeTypePtr> >(singleVisit);
}

template <class EdgeTypePtr>
HierEdge<EdgeTypePtr>::HierEdge(const std::list<EdgeTypePtr>& hPEdge, bool singleVisit) : singleVisit(singleVisit)
{
  l = makePtr<HierEdge_Leaf<EdgeTypePtr> >(singleVisit);
  insert(hPEdge);
}

template <class EdgeTypePtr>
HierEdge<EdgeTypePtr>::HierEdge(const std::set<std::list<EdgeTypePtr> >& hPEdges, bool singleVisit) : singleVisit(singleVisit)
{
  l = makePtr<HierEdge_Leaf<EdgeTypePtr> >(singleVisit);
  for(typename set<list<EdgeTypePtr> >::const_iterator e=hPEdges.begin(); e!=hPEdges.end(); e++)
    insert(*e);
}

template <class EdgeTypePtr>
void HierEdge<EdgeTypePtr>::operator=(HierEdgePtr that) {
  singleVisit = that->singleVisit;
  m = that->m;
  l = that->l;
}

// Stores the given PartEdge (single-level Hierarchical PartEdge) into this map's leaf
template <class EdgeTypePtr>  
void HierEdge<EdgeTypePtr>::insert(EdgeTypePtr pEdge) {
  l->insert(pEdge);
}

// Stores the given PartEdge (single-level Hierarchical PartEdge) into the visited list of this map's leaf
template <class EdgeTypePtr>  
void HierEdge<EdgeTypePtr>::insertVisited(EdgeTypePtr pEdge) {
  l->insertVisited(pEdge);
}

// Stores the given Hierarchical PartEdge into this map
template <class EdgeTypePtr>  
void HierEdge<EdgeTypePtr>::insert(const std::list<EdgeTypePtr>& hPEdge) {
  assert(hPEdge.size()>0);
  //dbg << "HierEdge<EdgeTypePtr>::insert() #key="<<key.size()<<endl;
  //list<list<PartContextPtr> > subKey = key;
  //subKey.pop_front();
  
  //dbg << "#key="<<key.size()<<" #subKey="<<subKey.size()<<endl;
  
  if(hPEdge.size()==1) l->insert(hPEdge.front());
  else {
    EdgeTypePtr next = hPEdge.front();
    list<EdgeTypePtr> rest = hPEdge;
    rest.pop_front();
    if(m.find(next) == m.end()) m[next] = makePtr<HierEdge<EdgeTypePtr> >(singleVisit);
    m[next]->insert(rest);
  }
}

// Given a parent PartEdge and a HierEdge_Leaf that holds its children PartEdges,
// insert all these Hierarchical PartsEdges into this map.
template <class EdgeTypePtr>  
void HierEdge<EdgeTypePtr>::insert(EdgeTypePtr parent, const HierEdge_Leaf<EdgeTypePtr> & hpel) {
  scope reg("HierEdge<EdgeTypePtr>::insert(edge, leaf)", scope::medium, analysisTesterDebugLevel, 2);
  if(analysisTesterDebugLevel>=2) {
    dbg << "hpel="<<const_cast<HierEdge_Leaf<EdgeTypePtr> &>(hpel).str()<<endl;
    dbg << "found="<<(m.find(parent) != m.end())<<endl;
  }
  
  // If there isn't a mapping within this map for parent, create one
  if(m.find(parent) == m.end())
    m[parent] = makePtr<HierEdge<EdgeTypePtr> >(singleVisit);
  
  // Add all the children of parent to its HierEdge object in the map
  for(typename set<EdgeTypePtr>::const_iterator i=hpel.s.begin(); i!=hpel.s.end(); i++) {
    //dbg << "Inserting "<<i->get()->str()<<endl;
    m[parent]->insert(*i);
  }
  
  // Add the visited info of all children of parent to its HierEdge object in the map
  for(typename set<EdgeTypePtr>::const_iterator i=hpel.visited.begin(); i!=hpel.visited.end(); i++) {
    //dbg << "Inserting Visited "<<i->get()->str()<<endl;
    m[parent]->insertVisited(*i);
  }
  
  //dbg << "final this="<<str()<<endl;
}

// Given a parent PartEdge and a HierEdge_Leaf that holds its children PartEdges,
// insert all these Hierarchical PartsEdges into this map.
template <class EdgeTypePtr>  
void HierEdge<EdgeTypePtr>::insert(EdgeTypePtr parent, const HierEdge<EdgeTypePtr> & hpe) {
  // If there isn't a mapping within this map for parent, create one
  if(m.find(parent) == m.end())
    m[parent] = makePtr<HierEdge<EdgeTypePtr> >(singleVisit);
  
  // Add the terminal children of parent (children that have ho children) to its HierEdge object in the map
  /*for(typename set<EdgeTypePtr>::const_iterator i=hpe.l->s.begin(); i!=hpe.l->s.end(); i++) {
    m[parent]->insert(*i);
  }*/
  insert(parent, *(hpe.l.get()));
  
  // Add all the non-terminal children of parent (children that have children) to its HierEdge object in the map
  for(typename map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > >::const_iterator i=hpe.m.begin(); i!=hpe.m.end(); i++) {
    m[parent]->insert(i->first, *(i->second.get()));
  }
}

// Returns the number of leaf edges mapped within this hierarchical edge
template <class EdgeTypePtr> 
int HierEdge<EdgeTypePtr>::size() const {
  int total=0;
  for(typename map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > >::const_iterator i=m.begin(); i!=m.end(); i++) {
    total += i->second->size();
  }
  return total + l->s.size();
}

// Iterates hierarchically through all the sequence of edges from the root of this HierEdge to its leaves.
// Applies the given functor to lists of EdgeTypePtrs, where every element in the list is the parent
// of the subsequent element. The functor may be applied to hierarhical edges from different levels 
// of this map.
template <class EdgeTypePtr> 
void HierEdge<EdgeTypePtr>::mapf(HierEdgeMapFunctor<EdgeTypePtr>& f) const {
  list<EdgeTypePtr> l;
  mapf_rec(f, l);
}

template <class EdgeTypePtr> 
// Recursive component of map, used internally
void HierEdge<EdgeTypePtr>::mapf_rec(HierEdgeMapFunctor<EdgeTypePtr>& f, list<EdgeTypePtr>& hpeL) const {
  // Apply this functor to all the edges in this HierEdge's leaf node
  for(typename set<EdgeTypePtr>::iterator i=l->s.begin(); i!=l->s.end(); i++) {
    scope reg("mapf_rec:L "+i->get()->str(), scope::medium, analysisTesterDebugLevel, 1);
    hpeL.push_back(*i);
    f(hpeL);
    hpeL.pop_back();
  }
  
  // Recurse to the next deepest level of the hierarchy
  for(typename map<EdgeTypePtr, HierEdgePtr>::const_iterator i=m.begin(); i!=m.end(); i++) {
    // Skip empty sub-trees
    if(i->second->size()==0) continue;
    
    scope reg("mapf_rec:M "+i->first.get()->str(), scope::medium, analysisTesterDebugLevel, 1);
    hpeL.push_back(i->first);
    i->second->mapf_rec(f, hpeL);
    hpeL.pop_back();
  }
}

// Advances all the Hierarchical Edges within this Map along the outgoing edges of their target Parts.
// Since the resulting PartEdges may have different parents, the function breaks the outgoing
// edges up by their parent PartEdge. It returns a map where the keys are the parent PartEdges 
// of the outgoing PartEdges and the values are pointers to HierEdge that contain
// of this parent's children Hierarchical PartEdges.
template <class EdgeTypePtr>  
map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > > HierEdge<EdgeTypePtr>::outEdges() {
  scope reg("HierEdge&lt;EdgeTypePtr&gt;::outEdges()", scope::medium, analysisTesterDebugLevel, 2);
  if(analysisTesterDebugLevel>=2) dbg << "this="<<str("    ")<<endl;
  // Maps a given grand-parent PartEdge to the PartEdge->HierEdge mapping of all of its descendants.
  // The grand-parent corresponds the parent HierEdge of this object, and the maps correspond
  // to ancestry level of this object.
  map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > > all;
  
  { scope reg("Leaf", scope::medium, analysisTesterDebugLevel, 2);
  map<EdgeTypePtr, HierEdge_LeafPtr> lOut = l->outEdges();
  if(analysisTesterDebugLevel>=2) dbg << "outEdges #lOut="<<lOut.size()<<endl;
  for(typename map<EdgeTypePtr, HierEdge_LeafPtr>::iterator i=lOut.begin(); i!=lOut.end(); i++) {
    if(analysisTesterDebugLevel>=2) {
      dbg << "    i-&gt;first"<<i->first.get()->str("        ")<<endl;
      dbg << "    i-&gt;second"<<i->second.get()->str("        ")<<endl;
    }
    // If there is no mapping for the grand-parent of PartEdge->HierEdge mapping, create a new HierEdge for it
    if(all.find(i->first->getParent()) == all.end())
      all[i->first->getParent()] = makePtr<HierEdge<EdgeTypePtr> >(singleVisit);
    all[i->first->getParent()]->insert(i->first, *(i->second.get()));
  }
  }
  
  { scope reg("MAP", scope::medium, analysisTesterDebugLevel, 2);
  for(typename map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > >::iterator i=m.begin(); i!=m.end(); i++) {
    map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > > lOut = i->second->outEdges();
    if(analysisTesterDebugLevel>=2) {
      dbg << "i="<<(i->first? i->first.get()->str(): "NULL")<<endl;
      dbg << "outEdges #lOut="<<lOut.size()<<endl;
    }
    for(typename map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > >::iterator j=lOut.begin(); j!=lOut.end(); j++) {
      if(analysisTesterDebugLevel>=2) {
        dbg << "    j-&gt;first="<<(j->first.get()? j->first.get()->str(): "NULL")<<endl;
        if(j->first) dbg << "    j-&gt;first-&gt;getParent()="<<(j->first->getParent()? j->first->getParent()->str(): "NULL")<<endl;
        dbg << "    j-&gt;second="<<j->second.get()->str()<<endl;
      }
      
      // If j->first is not NULL, set its parent appropriately. Otherwise, let its parent be NULL
      EdgeTypePtr parent;
      if(j->first) parent = j->first->getParent();
      
      // If there is no mapping for the grand-parent of PartEdge->HierEdge mapping, create a new HierEdge for it
      if(all.find(parent) == all.end())
        all[parent] = makePtr<HierEdge<EdgeTypePtr> >(singleVisit);
      
      all[parent]->insert(j->first, *(j->second.get()));
    }
  }
  }
  if(analysisTesterDebugLevel>=3) {
    dbg << "all="<<endl;
    for(typename map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > >::iterator i=all.begin(); i!=all.end(); i++)
      dbg << "    " << (i->first? i->first.get()->str():"NULL") << " ==&gt; " << i->second.get()->str("        ")<<endl;
  }
  
  return all;
}

template <class EdgeTypePtr>
void HierEdge<EdgeTypePtr>::advanceOut() {
  map<EdgeTypePtr, HierEdgePtr> out = outEdges();
  assert(out.size()==1);
  assert(out.begin()->first.get()==NULL);
 
  HierEdgePtr parent = out.begin()->second;
  assert(parent->m.size()==1);
  assert(parent->m.begin()->first.get()==NULL);
 
  *this = parent->m.begin()->second;
}

// Returns true if this edge is at the end of the graph and thus cannot advance further
template <class EdgeTypePtr>
bool HierEdge<EdgeTypePtr>::end() const {
  // This level's leaf must be at the end
  if(!l->end()) return false;
  
  // As well as all the lower-level HierEdges
  for(typename map<EdgeTypePtr, HierEdgePtr >::const_iterator i=m.begin(); i!=m.end(); i++)
    if(!i->second->end()) return false;
  
  // For this leve's HierEdge to be at the end
  return true;
}

template <class EdgeTypePtr>  
std::string HierEdge<EdgeTypePtr>::str(std::string indent) {
  ostringstream oss;
  oss << "[HierEdge: "<<endl;
  oss << indent << "    " << l->str(indent+"    ") << endl;
  
  for(typename map<EdgeTypePtr, CompSharedPtr<HierEdge<EdgeTypePtr> > >::iterator i=m.begin(); i!=m.end(); i++) {
    oss << indent << (i->first? i->first.get()->str(indent+"  "): "NULL") << " =&gt; " << endl;
    oss << indent << "    " << i->second->str(indent+"    ") << endl;
  }
  oss << "]";
  return oss.str();
}

template <class EdgeTypePtr>  
HierEdge_Leaf<EdgeTypePtr>::HierEdge_Leaf(EdgeTypePtr pEdge, bool singleVisit) :singleVisit(singleVisit) {
  insert(pEdge);
}

// Stores the given Hierarchical Part into this map
template <class EdgeTypePtr>  
void HierEdge_Leaf<EdgeTypePtr>::insert(EdgeTypePtr pEdge) {
  // If we visit each edge once, only add edges that have not yet been visited
  if(singleVisit && (visited.find(pEdge) != visited.end())) return;
  
  s.insert(pEdge);
}

// Stores the given Hierarchical Part into this map's visited set
template <class EdgeTypePtr>  
void HierEdge_Leaf<EdgeTypePtr>::insertVisited(EdgeTypePtr pEdge) {
  visited.insert(pEdge);
  
  // If we visit each edge once, remove pEdge from s, if it is there
  if(singleVisit)
    s.erase(pEdge);
}

// Advances all the Edges within this Map along the outgoing edges of their target Parts.
// Since the resulting PartEdges may have different parents, the function breaks the outgoing
// edges up by their parent PartEdge. It returns a map where the keys are the parent PartEdges 
// of the outgoing PartEdges and the values are pointers to HierEdge_Leafs that contain
// of this parent's children PartEdges.
template <class EdgeTypePtr>
map<EdgeTypePtr, CompSharedPtr<HierEdge_Leaf<EdgeTypePtr> > > HierEdge_Leaf<EdgeTypePtr>::outEdges() {
  scope reg("HierEdge_Leaf&gt;EdgeTypePtr&gt;::outEdges()", scope::medium, analysisTesterDebugLevel, 2);
  
  map<EdgeTypePtr, HierEdge_LeafPtr> newHPEmap;
  for(typename set<EdgeTypePtr>::iterator i=s.begin(); i!=s.end(); i++) {
    if(analysisTesterDebugLevel>=2) dbg << "i="<<i->get()->str()<<endl;
    
    list<EdgeTypePtr> out = (*i)->target()->outEdges();
    for(typename list<EdgeTypePtr>::iterator o=out.begin(); o!=out.end(); o++) {
      if(analysisTesterDebugLevel>=2) {
        dbg << "o="<<o->get()->str()<<endl;
        dbg << "(*o)-&gt;getParent()="<<((*o)->getParent()? (*o)->getParent()->str(): "NULL")<<endl;
      }
      // Create a new mapping for this edge's parent
      if(newHPEmap.find((*o)->getParent()) == newHPEmap.end())
        newHPEmap[(*o)->getParent()] = makePtr<HierEdge_Leaf<EdgeTypePtr> >(singleVisit);
      newHPEmap[(*o)->getParent()]->insert(*o);
    }
  }
  
  // If we are supposed to visit each leaf edge exactly once, transfer over this HierEdge leaf node's
  // visite edge set as well as its the current edge set to the new leaf nodes' visited set
  if(singleVisit) {
    for(typename set<EdgeTypePtr>::iterator v=visited.begin(); v!=visited.end(); v++) {
      EdgeTypePtr parent = (*v)->getParent();
      // If the new map does not contains an entry for the given parent, create it
      if(newHPEmap.find(parent) == newHPEmap.end())
        newHPEmap[parent] = makePtr<HierEdge_Leaf<EdgeTypePtr> >(singleVisit);
      
      // Copy over the visited information
      if(analysisTesterDebugLevel>=2) dbg << "Visited(V): "<<v->get()->str()<<endl;
      newHPEmap[parent]->insertVisited(*v);
    }
    
    for(typename set<EdgeTypePtr>::iterator v=s.begin(); v!=s.end(); v++) {
      EdgeTypePtr parent = (*v)->getParent();
      // If the new map does not contains an entry for the given parent, create it
      if(newHPEmap.find(parent) == newHPEmap.end())
        newHPEmap[parent] = makePtr<HierEdge_Leaf<EdgeTypePtr> >(singleVisit);
      
      // Copy over the visited information
      if(analysisTesterDebugLevel>=2) dbg << "Visited(S): "<<v->get()->str()<<endl;
      newHPEmap[parent]->insertVisited(*v);
    }
  }
  
  //dbg << "#newHPEmap="<<newHPEmap.size()<<endl;
  
  return newHPEmap;
}

// Returns true if this edge is at the end of the graph and thus cannot advance further
template <class EdgeTypePtr>
bool HierEdge_Leaf<EdgeTypePtr>::end() const {
  return s.size()==0;
}

template <class EdgeTypePtr>  
std::string HierEdge_Leaf<EdgeTypePtr>::str(std::string indent) {
  ostringstream oss;
  oss << "[HierEdge_Leaf: s=";
  for(typename set<EdgeTypePtr>::iterator i=s.begin(); i!=s.end(); i++) {
    oss << endl << indent << i->get()->str(indent+"  ");
  }
  if(analysisTesterDebugLevel>=3) {
    oss << endl << indent << "visited=";
    for(typename set<EdgeTypePtr>::iterator i=visited.begin(); i!=visited.end(); i++) {
      oss << endl << indent << i->get()->str(indent+"  ");
    }
  }
  oss << "]";
  return oss.str();
}

/********************************
 ***** HierEdge self-tester *****
 ********************************/
class IntProgressEdge;
typedef CompSharedPtr<IntProgressEdge> IntProgressEdgePtr;
IntProgressEdgePtr NULLIntProgressEdge;

class IntProgressEdge
{
  set<int> separateFactors;
  list<int> commonFactors;
  int curVal;
  
  public:
  IntProgressEdge(const set<int>& separateFactors, const list<int>& commonFactors, int initVal): 
    separateFactors(separateFactors), commonFactors(commonFactors) 
  {
    curVal=initVal;
  }
  
  void operator=(const IntProgressEdge& that) {
    separateFactors = that.separateFactors;
    commonFactors = that.commonFactors;
    curVal = that.curVal;
  }
  
  void operator=(IntProgressEdgePtr that) {
    separateFactors = that->separateFactors;
    commonFactors = that->commonFactors;
    curVal = that->curVal;
  }
  
  bool operator==(const IntProgressEdgePtr& that) const {
    /*scope reg("IntProgressEdge::==", scope::medium, 1, 1);
    dbg << "this="<<const_cast<IntProgressEdge*>(this)->str()<<endl;
    dbg << "that="<<that.get()->str()<<endl;
    dbg << ((separateFactors == that->separateFactors &&
           commonFactors   == that->commonFactors &&
            curVal         == that->curVal)?
             "EQUAL": "NOT EQUAL")<<endl;*/
    
    return separateFactors == that->separateFactors &&
           commonFactors   == that->commonFactors &&
            curVal         == that->curVal;
  }
  
  bool operator<(const IntProgressEdgePtr& that) const {
    /*scope reg("IntProgressEdge::<", scope::medium, 1, 1);
    dbg << "this="<<const_cast<IntProgressEdge*>(this)->str()<<endl;
    dbg << "that="<<that.get()->str()<<endl;
    dbg << "separateFactors < that->separateFactors: "<<(separateFactors < that->separateFactors)<<endl;
    dbg << "separateFactors == that->separateFactors: "<<(separateFactors == that->separateFactors)<<endl;
    dbg << "commonFactors < that->commonFactors: "<<(commonFactors < that->commonFactors)<<endl;
    dbg << "commonFactors == that->commonFactors: "<<(commonFactors == that->commonFactors)<<endl;
    dbg << "curVal("<<curVal<<" < that->curVal("<<that->curVal<<"): "<<(curVal < that->curVal)<<endl;
    
    dbg << ((separateFactors < that->separateFactors) ||
           (separateFactors == that->separateFactors && commonFactors < that->commonFactors) ||
           (separateFactors == that->separateFactors && commonFactors == that->commonFactors && curVal < that->curVal)?
             "LESS": "NOT LESS")<<endl;*/
    
    return (separateFactors < that->separateFactors) ||
           (separateFactors == that->separateFactors && commonFactors < that->commonFactors) ||
           (separateFactors == that->separateFactors && commonFactors == that->commonFactors && curVal < that->curVal);
  }
  
  const IntProgressEdge* target() const { return this; }
  
  list<IntProgressEdgePtr> outEdges() const {
    list<IntProgressEdgePtr> edges;
    scope reg("IntProgressEdge::outEdges", scope::medium, analysisTesterDebugLevel, 2);
    if(analysisTesterDebugLevel>=2) dbg << "#separateFactors="<<separateFactors.size()<<endl;
    for(set<int>::iterator i=separateFactors.begin(); i!=separateFactors.end(); i++) {
      edges.push_back(makePtr<IntProgressEdge>(separateFactors, commonFactors, curVal * *i));
      if(analysisTesterDebugLevel>=2) dbg << "curVal="<<curVal<<" * *i="<<(*i)<<" = "<<(curVal * *i)<<endl;
    }
    return edges;
  }
  
  IntProgressEdgePtr getParent() const {
    if(commonFactors.size()>1) {
      list<int> parentCF = commonFactors;
      parentCF.pop_back();
      return makePtr<IntProgressEdge>(separateFactors, parentCF, curVal);
    } else {
      return NULLIntProgressEdge;
    }
  }
  
  std::string str(std::string indent="") {
    ostringstream oss;
    oss << "[Int: ";
    int v=curVal;
    for(list<int>::iterator i=commonFactors.begin(); i!=commonFactors.end(); i++)
      v *= *i;
    oss << v;
    /*oss << endl << indent << "commonFactors=";
    for(list<int>::iterator i=commonFactors.begin(); i!=commonFactors.end(); i++)
      oss << " "<<*i;
    oss << endl << indent << "separateFactors=";
    for(set<int>::iterator i=separateFactors.begin(); i!=separateFactors.end(); i++)
      oss << " "<<*i;*/
    oss << "]";
    
    return oss.str();
  }
};

void AnalysisTester_selfTest() {
  /*set<int> allSeparateFactors;
  allSeparateFactors.insert(2);
  allSeparateFactors.insert(3);
  allSeparateFactors.insert(5);
  
  list<int> allCommonFactors;
  allCommonFactors.push_back(2);
  allCommonFactors.push_back(2);
  
  list<IntProgressEdgePtr> initStates;
  for(list<int>::iterator i=allCommonFactors.begin(); i!=allCommonFactors.end(); i++) {
    list<int> commonFactors(i, allCommonFactors.end());
    initStates.push_front(makePtr<IntProgressEdge>(allSeparateFactors, commonFactors, 1));
  }
  
  cout << "initStates:"<<endl;
  for(list<IntProgressEdgePtr>::iterator i=initStates.begin(); i!=initStates.end(); i++) {
    cout << "    " << i->get()->str("        ")<<endl;
  }
  
  HierEdge<IntProgressEdgePtr> he(initStates);
  cout << "he="<<he.str("    ")<<endl;
  
  for(int j=0; j<5; j++) {
    cout << "------------------------------"<<endl;
    / *map<IntProgressEdgePtr, CompSharedPtr<HierEdge<IntProgressEdgePtr> > > out = he.outEdges();
    assert(out.size()==0);
    assert(out.begin()->first.get()==NULL);
    
    cout << "out="<<out.begin()->second.get()->str("    ")<<endl;
    he = out.begin()->second;* /
    
    he.advanceOut();
    cout << "out="<<he.str("    ")<<endl;
  }*/
  
  std::set<int> transitionFactors;
  transitionFactors.insert(3);
  transitionFactors.insert(5);
  transitionFactors.insert(7);

  std::set<int> subGraphTransFactors;
  subGraphTransFactors.insert(11);
  subGraphTransFactors.insert(13);

  list<ComposedAnalysis*> subAnalyses;

  for(int i=0; i<2; i++) {
    subAnalyses.push_back(new FactorTransSystemAnalysis(i==0, // firstAnalysis
                                                        transitionFactors,
                                                        //2, // myCommonFactor
                                                        5, // maxSteps,
                                                        4, // numRollbackSteps
                                                        3, // maxNumStepsInDerivedGraph
                                                        subGraphTransFactors
                          ));
  }
  ChainComposer* cc = new ChainComposer(subAnalyses, NULL, true, false);
  cc->runAnalysis();
  
  //dbg << "Start="<<cc->GetStartAState_Spec()->str()<<endl;
  set<PartPtr> startStates = cc->GetStartAStates_Spec();
  set<list<PartEdgePtr> > startEdges;
  
  for(set<PartPtr>::iterator s=startStates.begin(); s!=startStates.end(); s++) {
    PartEdgePtr hpEntry = /*cc->GetStartAState_Spec()*/s->get()->inEdgeFromAny();
  
    //PartEdgePtr lpEntry = lowPrecision->GetStartAState_Spec()->inEdgeFromAny();

    // Create a Hierarchical edge that corresponds to the entry edge into the abstract transition system on which 
    // the high-precision instance of analysisToTest runs.
    PartEdgePtr cur = hpEntry;
    list<PartEdgePtr> entry;
    while(cur) {
      dbg << "cur="<<cur->str()<<endl;
      entry.push_front(cur);
      cur = cur->getParent();
    }
    startEdges.insert(entry);
  }
  
  for(HierEdge<PartEdgePtr> he(startEdges, true); !he.end(); he.advanceOut()) {
    scope reg("Current Iterator State", scope::medium, 1, 1);
    dbg << "out="<<he.str("    ")<<endl;
    cout << "." << endl;
  }
}

/**************************************
 ***** ComposedAnalysisSelfTester *****
 **************************************/

ComposedAnalysisSelfTester::ComposedAnalysisSelfTester(
        ComposedAnalysisPtr analysisToTest, 
        const std::set<ComposedAnalysisPtr>& precisionAnalyses,
        ComposedAnalysisPtr stxAnalysis) : 
   analysisToTestLP(analysisToTest), analysisToTestHP(analysisToTest->copy()), 
   precisionAnalyses(precisionAnalyses), stxAnalysis(stxAnalysis)
{
  assert(analysisToTestLP->implementsExpr2Val()     == analysisToTestHP->implementsExpr2Val());
  assert(analysisToTestLP->implementsExpr2MemLoc()  == analysisToTestHP->implementsExpr2MemLoc());
  assert(analysisToTestLP->implementsExpr2CodeLoc() == analysisToTestHP->implementsExpr2CodeLoc());
  assert(analysisToTestLP->implementsPartGraph()    == analysisToTestHP->implementsPartGraph());
}
 
// Creates a Composer that creates an Abstract Transition System using the analyses in precisionAnalyses
// and runs analysisToTest on top of it
ChainComposer* ComposedAnalysisSelfTester::createTestComposer(
        ComposedAnalysisPtr analysisToTest, 
        const std::set<ComposedAnalysisPtr>& precisionAnalyses) {
  list<ComposedAnalysis*> analysisChain;
  for(set<ComposedAnalysisPtr>::iterator i=precisionAnalyses.begin(); i!=precisionAnalyses.end(); i++)
    analysisChain.push_back(i->get());
  analysisChain.push_back(analysisToTest.get());
  
  //checkDataflowInfoPass* cdip = new checkDataflowInfoPass();
  return new ChainComposer(analysisChain, /*cdip*/NULL, true, stxAnalysis.get());
}

class printHE : public HierEdgeMapFunctor<PartEdgePtr> {
  public:
  void operator()(const std::list<PartEdgePtr>& hpe) {
    scope reg("printHE: edge", scope::medium, 1, 1);
    for(std::list<PartEdgePtr>::const_iterator i=hpe.begin(); i!=hpe.end(); i++)
      dbg << "    " << i->get()->str("    ")<<endl;
  }
};

class compareHE : public HierEdgeMapFunctor<PartEdgePtr> {
  Composer* lowPrecision;
  Composer* highPrecision;
  ComposedAnalysisPtr analysisToTestLP;
  ComposedAnalysisPtr analysisToTestHP;
  
  public:
  compareHE(Composer* lowPrecision, Composer* highPrecision, 
            ComposedAnalysisPtr analysisToTestLP, ComposedAnalysisPtr analysisToTestHP) :
    lowPrecision(lowPrecision), highPrecision(highPrecision), 
     analysisToTestLP(analysisToTestLP), analysisToTestHP(analysisToTestHP) {}
  
  void operator()(const std::list<PartEdgePtr>& hpe) {
    scope reg("compareHE: edge", scope::medium, 1, 1);
    
    int idx=0;
    for(std::list<PartEdgePtr>::const_iterator i=hpe.begin(); i!=hpe.end(); i++, idx++) {
      indent ind(txt()<<idx<":   ");
      dbg << i->get()->str()<<endl;
    }
    
    PartEdgePtr lpPEdge = *hpe.begin();
    PartPtr lpPart = lpPEdge->source();
    PartEdgePtr hpPEdge = *hpe.rbegin();
    PartPtr hpPart = hpPEdge->source();
    if(!lpPart && !hpPart) return;
    assert(lpPart && hpPart);
    //assert(lpPart->CFGNodes() == hpPart->CFGNodes());
    
    if(SgExpression* expr = hpPart->maySgNodeAny<SgExpression>()) {
      assert(lpPart->maySgNodeAny<SgExpression>());
      
      if(analysisToTestLP->implementsExpr2Val()) {
        ValueObjectPtr lpVal = lowPrecision->Expr2Val(expr, lpPEdge);
        ValueObjectPtr hpVal = highPrecision->Expr2Val(expr, hpPEdge);
        
        //dbg << "expr="<<CFGNode2Str(expr)<<endl;
        //dbg << "lpVal="<<lpVal->str()<<", lpVal->isConcrete()="<<lpVal->isConcrete()<<", type="<<(lpVal->isConcrete()? SgNode2Str(lpVal->getConcreteType()): "NONE")<<", val="<<(lpVal->isConcrete()? SgNode2Str(lpVal->getConcreteValue().get()): "NONE")<<endl;
        //dbg << "hpVal="<<hpVal->str()<<", hpVal->isConcrete()="<<hpVal->isConcrete()<<", type="<<(hpVal->isConcrete()? SgNode2Str(hpVal->getConcreteType()): "NONE")<<", val="<<(hpVal->isConcrete()? SgNode2Str(hpVal->getConcreteValue().get()): "NONE")<<endl;
        
        //assert(hpVal->subSet(lpVal, hpPEdge, highPrecision));
        if(!highPrecision->subSet(hpVal, lpVal, hpPEdge, NULL)) {
          scope reg("ERROR!", scope::medium, 1, 1);
          dbg << "lpPEdge="<<lpPEdge->str()<<endl;
          dbg << "hpPEdge="<<hpPEdge->str()<<endl;
          dbg << "lpVal="<<lpVal->str()<<endl;
          dbg << "hpVal="<<hpVal->str()<<endl;
          assert(0);
        // Check a violation of the sub-set property in another way, based on the concrete contents of the two sets
        } else if(lpVal->isConcrete() || hpVal->isConcrete()) {
          // If both sets are concrete, ensure that the set from hpVal is a subset of the set from lpVal
          if(lpVal->isConcrete() && hpVal->isConcrete()) {
            set<boost::shared_ptr<SgValueExp> > hpConcreteSet = hpVal->getConcreteValue();
            set<boost::shared_ptr<SgValueExp> > lpConcreteSet = lpVal->getConcreteValue();

            // Set of concrete values that are in the concrete set of the high-precision analysis but not in
            // the set of the low-precision analysis
            set<boost::shared_ptr<SgValueExp> > missedConcreteVals;

            for(set<boost::shared_ptr<SgValueExp> >::iterator h=hpConcreteSet.begin(); h!=hpConcreteSet.end(); h++) {
              set<boost::shared_ptr<SgValueExp> >::iterator l=lpConcreteSet.begin();
              for(; l!=lpConcreteSet.end(); l++)
                // If we've found the concrete value from the high-precision analysis in the set of the 
                // low-precision analysis, stop
                if(ValueObject::equalValueExp(h->get(), l->get())) break;

              // If we failed to find the current concrete value from the high-precision analysis in the set of the 
              // low-precision analysis, we've proven that the hp-set is not a subset of the lp-set
              if(l==lpConcreteSet.end())
                missedConcreteVals.insert(*h);
            }

            // If we found that the hp-set is not a subset of the lp-set, print out the error with the evidence
            if(missedConcreteVals.size()>0) {
              scope reg("ERROR!", scope::medium, 1, 1);
              dbg << "lpPEdge="<<lpPEdge->str()<<endl;
              dbg << "hpPEdge="<<hpPEdge->str()<<endl;
              dbg << "lpVal="<<lpVal->str()<<endl;
              dbg << "hpVal="<<hpVal->str()<<endl;
              dbg << "Values in the set from the high-precision analysis that are not in the set from the low-precision analysis!"<<endl;
              indent ind;
              for(set<boost::shared_ptr<SgValueExp> >::iterator v=missedConcreteVals.begin(); 
                  v!=missedConcreteVals.begin(); v++) {
                dbg << SgNode2Str((*v).get()) << endl;
              }
              assert(0);
            }
          }
        }
      }
      
      if(analysisToTestLP->implementsExpr2MemLoc()) {
        MemLocObjectPtr lpML = lowPrecision->Expr2MemLoc(expr, lpPEdge);
        MemLocObjectPtr hpML = highPrecision->Expr2MemLoc(expr, hpPEdge);
        if(hpML->subSet(lpML, hpPEdge, highPrecision)) {
          scope reg("ERROR!", scope::medium, 1, 1);
          dbg << "lpPEdge="<<lpPEdge->str()<<endl;
          dbg << "hpPEdge="<<hpPEdge->str()<<endl;
          dbg << "lpML="<<lpML->str()<<endl;
          dbg << "hpML="<<hpML->str()<<endl;
          assert(0);
        }
      }
      
      /*if(analysisToTestLP->implementsExpr2CodeLoc()) {
        CodeLocObjectPtr lpCL = lowPrecision->Expr2CodeLoc(expr, lpPEdge);
        dbg << "lpCL="<<lpCL->str()<<endl;
        CodeLocObjectPtr hpCL = highPrecision->Expr2CodeLoc(expr, hpPEdge);
        dbg << "hpCL="<<hpCL->str()<<endl;
        assert(hpCL->subSet(lpCL, hpPEdge, highPrecision));
      }*/
    }
  }
};

// Perform the self-test, return true on success, false on failure
bool ComposedAnalysisSelfTester::testAnalysis()
{
  
  ChainComposer* highPrecision = createTestComposer(analysisToTestHP, precisionAnalyses);
  { scope reghp("Test High Precision", scope::medium, analysisTesterDebugLevel, 1);
  highPrecision->runAnalysis();
  }
  set<ComposedAnalysisPtr> noAnalyses;
  ChainComposer* lowPrecision = createTestComposer(analysisToTestLP, noAnalyses);
  { scope reghp("Test Low Precision", scope::medium, analysisTesterDebugLevel, 1);
  lowPrecision->runAnalysis();
  }
  
  assert(analysisToTestLP->implementsExpr2Val()     == analysisToTestHP->implementsExpr2Val());
  assert(analysisToTestLP->implementsExpr2MemLoc()  == analysisToTestHP->implementsExpr2MemLoc());
  assert(analysisToTestLP->implementsExpr2CodeLoc() == analysisToTestHP->implementsExpr2CodeLoc());
  assert(analysisToTestLP->implementsPartGraph()    == analysisToTestHP->implementsPartGraph());
  
  scope reg("testAnalysis", scope::medium, 1, 1);
  set<PartPtr> hpEntry = highPrecision->GetStartAStates_Spec();
  { scope("hpEntry", scope::low, analysisTesterDebugLevel, 1);
    for(set<PartPtr>::iterator e=hpEntry.begin(); e!=hpEntry.end(); e++)
      dbg << e->get()->str()<<endl; }
  set<PartPtr> lpEntry = lowPrecision->GetStartAStates_Spec();
  { scope("lpEntry", scope::low, analysisTesterDebugLevel, 1);
    for(set<PartPtr>::iterator e=lpEntry.begin(); e!=lpEntry.end(); e++)
      dbg << e->get()->str()<<endl; }
  
  // Create a Hierarchical edge that corresponds to the entry edge into the abstract transition system on which 
  // the high-precision instance of analysisToTest runs.
  set<list<PartEdgePtr> > hpEntryEdges;
  for(set<PartPtr>::iterator hpE=hpEntry.begin(); hpE!=hpEntry.end(); hpE++) {
    PartEdgePtr cur = hpE->get()->inEdgeFromAny();
    list<PartEdgePtr> entry;
    //{ scope reg("entry", scope::medium, analysisTesterDebugLevel, 1);
    while(cur) {
      entry.push_front(cur);
      //dbg << ":"<<cur->str() << endl;
      cur = cur->getParent();
    //}
    //dbg << "size="<<entry.size()<<endl; 
    }
    hpEntryEdges.insert(entry);
  }
  //assert(entry.front() == lpEntry);
  
  for(HierEdge<PartEdgePtr> he(hpEntryEdges, true); !he.end(); he.advanceOut()) {
    //dbg << "out="<<he.str("    ")<<endl;
    scope reg("Current Iterator State", scope::medium, 1, 1);
    compareHE f(lowPrecision, highPrecision, analysisToTestLP, analysisToTestHP);
    he.mapf(f);
  }
  
  return true;
}

}; // namespace fuse
