#include "sage3basic.h"
#include "nodeState.h"
#include "compose.h"

#include <boost/make_shared.hpp>

using namespace std;
using namespace dbglog;
namespace fuse {
int nodeStateDebugLevel=2;

// Records that this analysis has initialized its state at this node
void NodeState::initialized(Analysis* init)
{
  initializedAnalyses[(Analysis*)init] = true;
}

// Returns true if this analysis has initialized its state at this node and false otherwise
bool NodeState::isInitialized(Analysis* analysis)
{
  return initializedAnalyses.find((Analysis*)analysis) != initializedAnalyses.end();
}

// Set this node's lattices for this analysis (possibly above or below only, replacing previous mappings).
// The lattices will be associated with the NULL edge
void NodeState::setLattices(Analysis* analysis, vector<Lattice*>& lattices)
{
  map<PartEdgePtr, vector<Lattice*> > tmp;
  
  if(dfInfoAbove.find((Analysis*)analysis) != dfInfoAbove.end())
    dfInfoAbove[(Analysis*)analysis].clear();
  else
    dfInfoAbove[(Analysis*)analysis] = tmp;

  if(dfInfoBelow.find((Analysis*)analysis) != dfInfoBelow.end())
    dfInfoBelow[(Analysis*)analysis].clear();
  else
    dfInfoBelow[(Analysis*)analysis] = tmp;

  // Set dfInfoAbove to lattices
  dfInfoAbove[(Analysis*)analysis][NULLPartEdge] = lattices;
  // copy dfInfoAbove to dfInfoBelow (including copies of all the lattices)
  //dbg << "NodeState::setLattices()"<<endl;
  for(vector<Lattice*>::iterator it = dfInfoAbove[(Analysis*)analysis][NULLPartEdge].begin(); 
      it!=dfInfoAbove[(Analysis*)analysis][NULLPartEdge].end(); it++)
  {
    //indent ind;
    //dbg << (*it)->str()<<endl;
    Lattice* l = (*it)->copy();
    //dbg << "NodeState::setLattices pushing dfInfoBelow: "<<l->str("")<<"\n";
    dfInfoBelow[(Analysis*)analysis][NULLPartEdge].push_back(l);
  }
  
  // Records that this analysis has initialized its state at this node
  initialized((Analysis*)analysis);
}

// Set this node's lattices for this analysis above the node, replacing previous mappings).
// The lattices will be associated with the NULL edge
void NodeState::setLatticeAbove(Analysis* analysis, vector<Lattice*>& lattices)
{
  setLattice_ex(dfInfoAbove, analysis, NULLPartEdge, lattices);
}

void NodeState::setLatticeBelow(Analysis* analysis, vector<Lattice*>& lattices)
{
  setLattice_ex(dfInfoBelow, analysis, NULLPartEdge, lattices);
}

// Set this node's lattices for this analysis, along the given departing edge
void NodeState::setLatticeAbove(Analysis* analysis, PartEdgePtr departEdge, std::vector<Lattice*>& lattices)
{
  setLattice_ex(dfInfoAbove, analysis, departEdge, lattices);
}

// Set this node's lattices for this analysis, along the given departing edge
void NodeState::setLatticeBelow(Analysis* analysis, PartEdgePtr departEdge, std::vector<Lattice*>& lattices)
{
  setLattice_ex(dfInfoBelow, analysis, departEdge, lattices);
}

static vector<Lattice*> emptyLatVec;

// Returns the map containing all the lattices above the node from the given analysis along the all edges
// (read-only access)
const std::map<PartEdgePtr, std::vector<Lattice*> >& NodeState::getLatticeAboveAll(Analysis* analysis) const
{ 
  assert(dfInfoAbove.find(analysis) != dfInfoAbove.end());
  return dfInfoAbove.find(analysis)->second;
}

// Returns the map containing all the lattices below the node from the given analysis along the all edges
// (read-only access)
const std::map<PartEdgePtr, std::vector<Lattice*> >& NodeState::getLatticeBelowAll(Analysis* analysis) const
{ 
  assert(dfInfoBelow.find(analysis) != dfInfoBelow.end());
  return dfInfoBelow.find(analysis)->second;
}

// returns the given lattice from above the node, which owned by the given analysis
Lattice* NodeState::getLatticeAbove(Analysis* analysis, int latticeName) const
{
  return getLattice_ex(dfInfoAbove, analysis, NULLPartEdge, latticeName);
}

// returns the given lattice from below the node, which owned by the given analysis
Lattice* NodeState::getLatticeBelow(Analysis* analysis, int latticeName) const
{
  return getLattice_ex(dfInfoBelow, analysis, NULLPartEdge, latticeName);
}
  
// Returns the given lattice above the node from the given analysis along the given departing edge
Lattice* NodeState::getLatticeAbove(Analysis* analysis, PartEdgePtr departEdge, int latticeName) const
{
  return getLattice_ex(dfInfoAbove, analysis, departEdge, latticeName);
}

// Returns the given lattice below the node from the given analysis along the given departing edge
Lattice* NodeState::getLatticeBelow(Analysis* analysis, PartEdgePtr departEdge, int latticeName) const
{
  return getLattice_ex(dfInfoBelow, analysis, departEdge, latticeName);
}


// Returns the map containing all the lattices above the node from the given analysis along the NULL edge
// (read-only access)
const vector<Lattice*>& NodeState::getLatticeAbove(Analysis* analysis) const
{ return getLattice_ex(dfInfoAbove, analysis, NULLPartEdge); }

// Returns the map containing all the lattices below the node from the given analysis along the NULL edge
// (read-only access)
const std::vector<Lattice*>& NodeState::getLatticeBelow(Analysis* analysis) const
{ return getLattice_ex(dfInfoBelow, analysis, NULLPartEdge); }

// Returns the map containing all the lattices above the node from the given analysis along the given departing edge
// (read-only access)
const std::vector<Lattice*>& NodeState::getLatticeAbove(Analysis* analysis, PartEdgePtr departEdge) const
{ return getLattice_ex(dfInfoAbove, analysis, departEdge); }

// Returns the map containing all the lattices below the node from the given analysis along the given departing edge
// (read-only access)
const std::vector<Lattice*>& NodeState::getLatticeBelow(Analysis* analysis, PartEdgePtr departEdge) const
{ return getLattice_ex(dfInfoBelow, analysis, departEdge); }

// Returns the map containing all the lattices above the node from the given analysis along the all edges
// (read/write access)
std::map<PartEdgePtr, std::vector<Lattice*> >& NodeState::getLatticeAboveAllMod(Analysis* analysis)
{ return dfInfoAbove[analysis]; }

// Returns the map containing all the lattices below the node from the given analysis along the all edges
// (read/write access)
std::map<PartEdgePtr, std::vector<Lattice*> >& NodeState::getLatticeBelowAllMod(Analysis* analysis)
{ return dfInfoBelow[analysis]; }

// Returns the map containing all the lattices above the node from the given analysis along the NULL edge
// (read/write access)
std::vector<Lattice*>& NodeState::getLatticeAboveMod(Analysis* analysis)
{ return getLatticeMod_ex(dfInfoAbove, analysis, NULLPartEdge); }

// Returns the map containing all the lattices above the node from the given analysis along the NULL edge
// (read/write access)
std::vector<Lattice*>& NodeState::getLatticeBelowMod(Analysis* analysis)
{ return getLatticeMod_ex(dfInfoBelow, analysis, NULLPartEdge); }

// Returns the map containing all the lattices above the node from the given analysis along the given departing edge
// (read/write access)
std::vector<Lattice*>& NodeState::getLatticeAboveMod(Analysis* analysis, PartEdgePtr departEdge)
{ return getLatticeMod_ex(dfInfoAbove, analysis, departEdge); }

// Returns the map containing all the lattices above the node from the given analysis along the given departing edge
// (read/write access)
std::vector<Lattice*>& NodeState::getLatticeBelowMod(Analysis* analysis, PartEdgePtr departEdge)
{ return getLatticeMod_ex(dfInfoBelow, analysis, departEdge); }

// Deletes all lattices above this node associated with the given analysis
void NodeState::deleteLatticeAbove(Analysis* analysis)
{ delete_ex(dfInfoAbove, analysis); }

// deletes all lattices below this node associated with the given analysis
void NodeState::deleteLatticeBelow(Analysis* analysis)
{ delete_ex(dfInfoBelow, analysis); }

// General lattice setter function
void NodeState::setLattice_ex(LatticeMap& dfMap, Analysis* analysis, PartEdgePtr departEdge, 
                              std::vector<Lattice*>& lattices)
{
  // If the analysis currently has a mapping in dfInfoB
  std::map<PartEdgePtr, std::vector<Lattice*> >::iterator w;
  if((dfMap.find((Analysis*)analysis)) != dfMap.end() && 
     ((w = dfMap[((Analysis*)analysis)].find(departEdge)) != dfMap[((Analysis*)analysis)].end()))
  {
    // Empty out the current mapping of analysis in dfInfo
    for(vector<Lattice*>::iterator it = w->second.begin(); 
        it != w->second.end(); it++)
    { delete *it; }
    w->second.clear();
    
    // Create the new mapping
    w->second = lattices;
  }
  else
  {
    // Create the new mapping
    dfMap[(Analysis*)analysis][departEdge] = lattices;
  }
  
  // Records that this analysis has initialized its state at this node
  initialized((Analysis*)analysis);
}

// General lattice getter function
Lattice* NodeState::getLattice_ex(const LatticeMap& dfMap, Analysis* analysis, 
                                  PartEdgePtr departEdge, int latticeName) const
{
  LatticeMap::const_iterator a;
  std::map<PartEdgePtr, std::vector<Lattice*> >::const_iterator w;

  if(dfMap.find((Analysis*)analysis) == dfMap.end()) {
    scope reg("NodeState::getLattice_ex: Analysis not found!", scope::medium, 1, 1);
    dbg << "dfMap.find("<<analysis<<")!=dfMap.end() = "<<(dfMap.find((Analysis*)analysis) != dfMap.end())<<" dfMap.size()="<<dfMap.size()<<endl;
    for(LatticeMap::const_iterator i=dfMap.begin(); i!=dfMap.end(); i++)
    { dbg << "i="<<i->first<<endl; }
    assert(0);
  }
  /*if((a=dfMap.find((Analysis*)analysis)) != dfMap.end()) {
    dbg << "a->second.find("<<(departEdge? departEdge->str(): "NULL")<<")!= a->second.end() = "<<(a->second.find(departEdge) != a->second.end())<<endl;
    dbg << "a->second="<<endl;
    indent ind;
    std::map<PartEdgePtr, vector<Lattice*> > b = a->second;
    dbg << str(b)<<endl;
  }*/
  
  // If this analysis has registered some Lattices at this node/edge combination
  if((a=dfMap.find((Analysis*)analysis)) != dfMap.end() && 
     ((w = a->second.find(departEdge)) != a->second.end())) {
      //dbg << "w->second.size()="<<w->second.size()<<" latticeName="<<latticeName<<endl;
    if(w->second.size()>=(unsigned int)latticeName)
      return w->second.at(latticeName);
    else
      return NULL;
  }
  
  return NULL;
}

// General read-only lattice vector getter function
const vector<Lattice*>& NodeState::getLattice_ex(const LatticeMap& dfMap, Analysis* analysis, 
                                                PartEdgePtr departEdge) const
{
  LatticeMap::const_iterator a;
  std::map<PartEdgePtr, std::vector<Lattice*> >::const_iterator w;
  
  // If this analysis has registered some Lattices at this node/edge combination
  if((a=dfMap.find((Analysis*)analysis)) != dfMap.end() && 
     ((w = a->second.find(departEdge)) != a->second.end())) {
      //printf("dfLattices->first=%p, dfLattices->second.size()=%d\n", dfLattices->first, dfLattices->second.size());
    return w->second;
  } else
    return emptyLatVec;
}

// General read/write lattice vector getter function
vector<Lattice*>& NodeState::getLatticeMod_ex(LatticeMap& dfMap, Analysis* analysis, 
                                              PartEdgePtr departEdge)
{
  std::map<PartEdgePtr, std::vector<Lattice*> >::iterator w;
  
  // If this analysis has registered some Lattices at this node/edge combination
  if((dfMap.find((Analysis*)analysis)) != dfMap.end() && 
     ((w = dfMap[((Analysis*)analysis)].find(departEdge)) != dfMap[((Analysis*)analysis)].end())) {
      //printf("dfLattices->first=%p, dfLattices->second.size()=%d\n", dfLattices->first, dfLattices->second.size());
    return w->second;
  } else
    return emptyLatVec;
}


// Deletes all lattices above/below this node associated with the given analysis
void NodeState::delete_ex(LatticeMap& dfMap, Analysis* analysis)
{
  assert(dfMap.find((Analysis*)analysis) != dfMap.end());
  map<PartEdgePtr, vector<Lattice*> > & l = dfMap.find((Analysis*)analysis)->second;
  // Delete the individual lattices associated with this analysis
  for(map<PartEdgePtr, vector<Lattice*> >::iterator e=l.begin(); e!=l.end(); e++)
    for(vector<Lattice*>::iterator l=e->second.begin(); l!=e->second.end(); l++)
      delete *l;

  // Delete the analysis' mapping in dfInfoAbove
  dfMap.erase((Analysis*)analysis);
}

// Returns true if the two lattices vectors contain equivalent information and false otherwise
bool NodeState::equivLattices(const std::vector<Lattice*>& latticesA,
                              const std::vector<Lattice*>& latticesB)
{
  //printf("    latticesA.size()=%d latticesB.size()=%d\n", latticesA.size(), latticesB.size());
  if(latticesA.size() != latticesB.size())
    return false;
  
  vector<Lattice*>::const_iterator itA, itB;
  for(itA = latticesA.begin(), itB = latticesB.begin();
      itA != latticesA.end(), itB != latticesB.end();
      itA++, itB++)
  {
    if(!((*itA)->equiv(*itB))) return false;
  }
  
  return true;
}

// Creates a copy of all the dataflow state (Lattices and Facts) associated with
// analysis srcA and associates this copied state with analysis tgtA.
void NodeState::cloneAnalysisState(Analysis* srcA, Analysis* tgtA)
{
  // Copy srcA's facts
  //const map <int, NodeFact*>& srcFacts = getFacts(srcA);
  const vector<NodeFact*>& srcFacts = getFacts(srcA);
  //printf("srcFacts.size()=%d\n", srcFacts.size());
  
  vector<NodeFact*> tgtFacts;
  for(vector<NodeFact*>::const_iterator f = srcFacts.begin();
      f != srcFacts.end(); f++)
  { tgtFacts.push_back((*f)->copy()); }
  // Associate analysis tgtA with the copied facts
  setFacts(tgtA, tgtFacts);
  
  // Copy srcA's lattices into tgtLatAbv and tgtLatBel
  const map<PartEdgePtr, vector<Lattice*> >& srcLatAbv = getLatticeAboveAll(srcA);
  
  for(map<PartEdgePtr, vector<Lattice*> >::const_iterator e=srcLatAbv.begin(); e!=srcLatAbv.end(); e++) {
    vector<Lattice*> tgtLatAbv;
    for(vector<Lattice*>::const_iterator l=e->second.begin(); l!=e->second.end(); l++)
      tgtLatAbv.push_back((*l)->copy());
    // Associate analysis tgtA with the copied lattices
    setLatticeAbove(tgtA, e->first, tgtLatAbv);
  }
  
  const map<PartEdgePtr, vector<Lattice*> >& srcLatBel = getLatticeBelowAll(srcA);
  for(map<PartEdgePtr, vector<Lattice*> >::const_iterator e=srcLatBel.begin(); e!=srcLatBel.end(); e++) {
    vector<Lattice*> tgtLatBel;
    for(vector<Lattice*>::const_iterator l=e->second.begin(); l!=e->second.end(); l++)
      tgtLatBel.push_back((*l)->copy());
    // Associate analysis tgtA with the copied lattices
    setLatticeBelow(tgtA, e->first, tgtLatBel);
  }
}

// Given a set of analyses, one of which is designated as a master, unions together the 
// lattices associated with each of these analyses. The results are associated on each 
// CFG node with the master analysis.
void NodeState::unionLattices(set<Analysis*>& unionSet, Analysis* master)
{
  //printf("    unionLattices() unionSet.size()=%d, master=%p, this=%p\n", unionSet.size(), master, this);
  //printf("  masterLatAbv.size()=%d, masterLatBel.size()=%d\n", masterLatAbv.size(), masterLatBel.size());
  for(set<Analysis*>::iterator it = unionSet.begin(); it!=unionSet.end(); it++)
  {
    Analysis* curA = *it;
    //printf("  curA=%p\n", curA);
    if(curA != master)
    {
      unionLatticeMaps(getLatticeAboveAllMod(master), getLatticeAboveAll(curA));
      unionLatticeMaps(getLatticeBelowAllMod(master), getLatticeBelowAll(curA));
    }
  }
}

// Unions the dataflow information in Lattices held by the from map into the to map
// Returns true if this causes a change in the lattices in to and false otherwise
bool NodeState::unionLatticeMaps(map<PartEdgePtr, vector<Lattice*> >& to, 
                                 const map<PartEdgePtr, vector<Lattice*> >& from)
{
  scope reg("NodeState::unionLatticeMaps()", scope::medium, 2, nodeStateDebugLevel);
  // All the analyses in unionSet must have the same number of edges
  assert(to.size() == from.size());

  bool modified = false;
  
  map<PartEdgePtr, vector<Lattice*> >::iterator       eTo;
  map<PartEdgePtr, vector<Lattice*> >::const_iterator eFrom;
  for(eTo=to.begin(), eFrom=from.begin(); eTo!=to.end(); eTo++, eFrom++) {
    // All the analyses in unionSet must have the same number of lattices associated with each edge
    assert(eTo->second.size() == eFrom->second.size());

    // Union the Above lattices 
    vector<Lattice*>::iterator       lTo;
    vector<Lattice*>::const_iterator lFrom;
    for(lTo=eTo->second.begin(), lFrom=eFrom->second.begin(); lTo!=eTo->second.end(); lTo++, lFrom++) {
      if(nodeStateDebugLevel>=2) dbg << "(*lTo)->finiteLattice()="<<(*lTo)->finiteLattice()<<endl;
      if((*lTo)->finiteLattice())
        modified = (*lTo)->meetUpdate(*lFrom) || modified;
      else {
        InfiniteLattice* meetResult = dynamic_cast<InfiniteLattice*>((*lTo)->copy());
        meetResult->meetUpdate(*lFrom); 
        // Widen the resulting meet
        modified =  dynamic_cast<InfiniteLattice*>(*lTo)->widenUpdate(meetResult) | modified;
        delete meetResult;
      }
    }
  }
  
  return modified;
}

// associates the given analysis/fact name with the given NodeFact, 
// deleting any previous association (the previous NodeFact is freed)
void NodeState::addFact(Analysis* analysis, int factName, NodeFact* f)
{
  NodeFactMap::iterator factsIt;
  // if this analysis has registered some facts at this node
  if((factsIt = facts.find((Analysis*)analysis)) != facts.end())
  {
    // delete the old fact (if any) and set it to the new fact
    //if(factsIt->second.find(factName) != factsIt->second.end())
    if((unsigned int)factName < factsIt->second.size())
    {
      delete factsIt->second[factName];
      factsIt->second[factName] = f;
    }
    else
    {
      for(int i=factsIt->second.size(); i<(factName-1); i++)
        factsIt->second.push_back(NULL);
      factsIt->second.push_back(f);
    }
  }
  else
  {
    vector<NodeFact*> newVec;
    for(int i=0; i<(factName-1); i++)
      newVec.push_back(NULL);
    newVec.push_back(f);
    facts[(Analysis*)analysis] = newVec;
  }
}

// associates the given analysis with the given map of fact names to NodeFacts
// deleting any previous association (the previous NodeFact is freed)
void NodeState::setFacts(Analysis* analysis, const vector<NodeFact*>& newFacts)
{
  NodeFactMap::iterator factsIt;
  // if this analysis has registered some facts at this node
  if((factsIt = facts.find((Analysis*)analysis)) != facts.end())
  {
    // delete the old facts (if any) and associate the analysis with the new set of facts
    for(vector<NodeFact*>::iterator it = factsIt->second.begin();
        it != factsIt->second.end(); it++)
    //{ delete it->second; }
    { delete *it; }
    factsIt->second.clear();
    factsIt->second = newFacts;
  }
  else
  {
    // Associate newFacts with the analysis
    facts[(Analysis*)analysis] = newFacts;
  }
  
  // Records that this analysis has initialized its state at this node
  initialized((Analysis*)analysis);
}

// returns the given fact, which owned by the given analysis
NodeFact* NodeState::getFact(Analysis* analysis, int factName) const
{
  NodeFactMap::const_iterator factsIt;
  // if this analysis has registered some Lattices at this node
  if((factsIt = facts.find((Analysis*)analysis)) != facts.end())
  {
    vector<NodeFact*>::const_iterator it;
    //printf("NodeState::getFact() factName=%d factsIt->second.size()=%d\n", factName, factsIt->second.size());
    //if((it = factsIt->second.find(factName)) != factsIt->second.end())
    if((unsigned int)factName < factsIt->second.size())
      return (factsIt->second)[factName];
  }
  return NULL;
}

static vector<NodeFact*> emptyFactsMap;
// returns the map of all the facts owned by the given analysis at this NodeState
// (read-only access)
const vector<NodeFact*>& NodeState::getFacts(Analysis* analysis) const
{
  // if this analysis has registered some facts at this node, return their map
  if(facts.find((Analysis*)analysis)!=facts.end())
    return facts.find((Analysis*)analysis)->second;
  else
    // otherwise, return an empty map
    return emptyFactsMap;
}

// returns the map of all the facts owned by the given analysis at this NodeState
// (read/write access)
vector<NodeFact*>& NodeState::getFactsMod(Analysis* analysis)
{
  // if this analysis has registered some facts at this node, return their map
  if(facts.find((Analysis*)analysis)!=facts.end())
    return facts.find((Analysis*)analysis)->second;
  else
    // otherwise, return an empty map
    return emptyFactsMap;
}

// deletes all facts at this node associated with the given analysis
void NodeState::deleteFacts(Analysis* analysis)
{
  vector<NodeFact*>& f = getFactsMod(analysis);
  
  // delete the individual facts associated with this analysis
  for(vector<NodeFact*>::iterator it = f.begin(); it!=f.end(); it++)
    //delete it->second;
    delete *it;

  // delete the analysis' mapping in facts
  facts.erase((Analysis*)analysis);
}

// delete all state at this node associated with the given analysis
void NodeState::deleteState(Analysis* analysis)
{
  deleteLatticeAbove(analysis);
  deleteLatticeBelow(analysis);
  deleteFacts(analysis);
}

// ====== STATIC ======
map<ComposedAnalysis*, map<PartPtr, NodeState*> > NodeState::nodeStateMap;

// Returns the NodeState object associated with the given Part from the given analysis.
NodeState* NodeState::getNodeState(ComposedAnalysis* analysis, PartPtr p)
{
  //dbg << "NodeState::getNodeState() analysis="<<analysis->str()<<" p="<<p->str()<<endl;
  
  if(nodeStateMap.find(analysis)    == nodeStateMap.end() ||
     nodeStateMap[analysis].find(p) == nodeStateMap[analysis].end()) {
    /*dbg << "NodeState::getNodeState() Creating new state. analysis="<<analysis<<"(found="<<(nodeStateMap.find(analysis) != nodeStateMap.end())<< ") #nodeStateMap="<<nodeStateMap.size()<<" p="<<p->str()<<endl;
    dbg << "Analyses = ";
    for(std::map<ComposedAnalysis*, std::map<PartPtr, NodeState*> >::iterator i=nodeStateMap.begin(); i!=nodeStateMap.end(); i++)
      dbg << "("<<i->first<<" | "<<i->first->str()<<") ";
    dbg << endl;
            
    if(nodeStateMap.find(analysis) != nodeStateMap.end()) 
      dbg<<"(found="<<(nodeStateMap[analysis].find(p) == nodeStateMap[analysis].end())<<")"<<endl;
    */
    /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;Creating new state, #nodeStateMap[analysis]="<<nodeStateMap[analysis].size()<<" p="<<p->str()<<endl;
    indent ind;
    for(map<PartPtr, NodeState*>::iterator ns=nodeStateMap[analysis].begin(); ns!=nodeStateMap[analysis].end(); ns++) {
      dbg << ns->get()->str() << endl;
    }*/
    /*for(map<PartPtr, NodeState*>::iterator ns=nodeStateMap[analysis].begin(); ns!=nodeStateMap[analysis].end(); ns++) {
      PartPtr nsf = ns->first;
      dbg << ns->first.get()->str() << " <: "<<(ns->first < p) << " ==: "<<(ns->first == p)  << endl;
    }*/
    
    nodeStateMap[analysis][p] = new NodeState();
    //dbg << "state="<<nodeStateMap[analysis][p]<<endl;
    
  }
  /*dbg << "&nbsp;&nbsp;&nbsp;&nbsp;#nodeStateMap[analysis]="<<nodeStateMap[analysis].size()<<endl;
  indent ind;
  for(map<PartPtr, NodeState*>::iterator ns=nodeStateMap[analysis].begin(); ns!=nodeStateMap[analysis].end(); ns++) {
    PartPtr nsf = ns->first; 
    dbg << nsf->str() << endl;
  }
  for(map<PartPtr, NodeState*>::iterator ns=nodeStateMap[analysis].begin(); ns!=nodeStateMap[analysis].end(); ns++) {
    PartPtr nsf = ns->first;
    dbg << nsf->str() << " <: "<<(ns->first < p)  << endl;
  }
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;state="<<nodeStateMap[analysis][p]->str(analysis, "&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;*/
  
  return nodeStateMap[analysis][p];
}

// Copies from's above lattices for analysis to to's above lattices for the same analysis, both along the NULL edge.
void NodeState::copyLattices_aEQa(Analysis* analysis, NodeState& to, const NodeState& from)
{
  copyLattices_aEQa(analysis, to, NULLPartEdge, from, NULLPartEdge);
}
// Copies along the given departing edges
void NodeState::copyLattices_aEQa(Analysis* analysis, NodeState& to,   PartEdgePtr toDepartEdge, 
                                                const NodeState& from, PartEdgePtr fromDepartEdge)
{
  assert(to.dfInfoAbove.find(analysis)   != to.dfInfoAbove.end());
  assert(from.dfInfoAbove.find(analysis) != from.dfInfoAbove.end());
  copyLattices(to.dfInfoAbove.find(analysis)->second, toDepartEdge, from.dfInfoAbove.find(analysis)->second, fromDepartEdge); 
}

// Copies from's above lattices for analysis to to's below lattices for the same analysis, both along the NULL edge.
void NodeState::copyLattices_bEQa(Analysis* analysis, NodeState& to, const NodeState& from)
{
  copyLattices_bEQa(analysis, to, NULLPartEdge, from, NULLPartEdge);
}
// Copies along the given departing edges
void NodeState::copyLattices_bEQa(Analysis* analysis, NodeState& to,   PartEdgePtr toDepartEdge, 
                                                const NodeState& from, PartEdgePtr fromDepartEdge)
{
  assert(to.dfInfoBelow.find(analysis)   != to.dfInfoBelow.end());
  assert(from.dfInfoAbove.find(analysis) != from.dfInfoAbove.end());
  copyLattices(to.dfInfoBelow.find(analysis)->second, toDepartEdge, from.dfInfoAbove.find(analysis)->second, fromDepartEdge); 
}

// Copies from's below lattices for analysis to to's below lattices for the same analysis, both along the NULL edge.
void NodeState::copyLattices_bEQb(Analysis* analysis, NodeState& to, const NodeState& from)
{
  copyLattices_bEQb(analysis, to, NULLPartEdge, from, NULLPartEdge);
}
// Copies along the given departing edges
void NodeState::copyLattices_bEQb(Analysis* analysis, NodeState& to,   PartEdgePtr toDepartEdge, 
                                                const NodeState& from, PartEdgePtr fromDepartEdge)
{
  assert(to.dfInfoBelow.find(analysis)   != to.dfInfoBelow.end());
  assert(from.dfInfoBelow.find(analysis) != from.dfInfoBelow.end());
  copyLattices(to.dfInfoBelow.find(analysis)->second, toDepartEdge, from.dfInfoBelow.find(analysis)->second, fromDepartEdge);
}


// Copies from's below lattices for analysis to to's above lattices for the same analysis, both along the NULL edge.
void NodeState::copyLattices_aEQb(Analysis* analysis, NodeState& to, const NodeState& from)
{
  copyLattices_aEQb(analysis, to, NULLPartEdge, from, NULLPartEdge);
}
// Copies along the given departing edges
void NodeState::copyLattices_aEQb(Analysis* analysis, NodeState& to,   PartEdgePtr toDepartEdge, 
                                                const NodeState& from, PartEdgePtr fromDepartEdge)
{
  assert(to.dfInfoAbove.find(analysis)   != to.dfInfoAbove.end());
  assert(from.dfInfoBelow.find(analysis) != from.dfInfoBelow.end());
  copyLattices(to.dfInfoAbove.find(analysis)->second, toDepartEdge, from.dfInfoBelow.find(analysis)->second, fromDepartEdge); 
}

// Makes dfInfoTo[*] a copy of dfInfoFrom[*], ensuring that they both have the same structure
void NodeState::copyLattices(map<PartEdgePtr, vector<Lattice*> >& dfInfoTo,
                       const map<PartEdgePtr, vector<Lattice*> >& dfInfoFrom)
{
  assert(dfInfoTo.size() == dfInfoFrom.size());
  
  // Copy lattices
  //dbg << "    copyLattices()\n";
  map<PartEdgePtr, vector<Lattice*> >::iterator eTo;
  map<PartEdgePtr, vector<Lattice*> >::const_iterator eFrom;
  for(eTo=dfInfoTo.begin(), eFrom=dfInfoFrom.begin(); eTo!=dfInfoTo.end(); eTo++, eFrom++) {
    assert(eTo->first == eFrom->first);
    
    vector<Lattice*>::iterator lTo;
    vector<Lattice*>::const_iterator lFrom;
    
    for(lTo=eTo->second.begin(), lFrom=eFrom->second.begin(); lTo!=eTo->second.end(); lTo++, lFrom++) {
      (*lTo)->copy(*lFrom);
    }
  }
}

// Makes dfInfoTo[*] a copy of dfInfoFrom[*]. If dfInfoTo is not initially empty, it is cleared and its 
// Lattices are deallocated.
void NodeState::copyLatticesOW(map<PartEdgePtr, vector<Lattice*> >& dfInfoTo,
                         const map<PartEdgePtr, vector<Lattice*> >& dfInfoFrom)
{
  // First, empty out dfInfoTo if needed
  for(map<PartEdgePtr, vector<Lattice*> >::iterator eTo=dfInfoTo.begin(); eTo!=dfInfoTo.end(); eTo++) {  
    for(vector<Lattice*>::iterator lTo=eTo->second.begin(); lTo!=eTo->second.end(); lTo++) {
      delete *lTo;
    }
  }
  dfInfoTo.clear();
  
  for(map<PartEdgePtr, vector<Lattice*> >::const_iterator eFrom=dfInfoFrom.begin(); eFrom!=dfInfoFrom.end(); eFrom++) {
    for(vector<Lattice*>::const_iterator lFrom=eFrom->second.begin(); lFrom!=eFrom->second.end(); lFrom++) {
      Lattice *lTo = (*lFrom)->copy();
      dfInfoTo[eFrom->first].push_back(lTo);
    }
  }
}

// Makes dfInfoTo[toDepartEdge] a copy of dfInfoFrom[fromDepartEdge]
// If adjustPEdge is true, calls Lattice::setPartEdge() on the copied lattices in dfInfoTo to associate them with this edge.
void NodeState::copyLattices(map<PartEdgePtr, vector<Lattice*> >& dfInfoTo,   PartEdgePtr toDepartEdge, 
                       const map<PartEdgePtr, vector<Lattice*> >& dfInfoFrom, PartEdgePtr fromDepartEdge, bool adjustPEdge)
{
  assert(dfInfoTo.size() == dfInfoFrom.size());
  assert(dfInfoTo.find(toDepartEdge) != dfInfoTo.end());
  assert(dfInfoFrom.find(fromDepartEdge) != dfInfoFrom.end());
  
  // Copy lattices
  //dbg << "    copyLattices()\n";
  map<PartEdgePtr, vector<Lattice*> >::iterator       eTo   = dfInfoTo.find(toDepartEdge);
  map<PartEdgePtr, vector<Lattice*> >::const_iterator eFrom = dfInfoFrom.find(fromDepartEdge);

  vector<Lattice*>::iterator lTo;
  vector<Lattice*>::const_iterator lFrom;

  for(lTo=eTo->second.begin(), lFrom=eFrom->second.begin(); lTo!=eTo->second.end(); lTo++, lFrom++) {
    (*lTo)->copy(*lFrom);
    if(adjustPEdge)
      // Adjust the Lattice's part edge to correspond to its new edge
      (*lTo)->setPartEdge(toDepartEdge);
  }
}

// Makes dfInfoTo[toDepartEdge] a copy of dfInfoFrom[fromDepartEdge]. If dfInfoTo[toDepartEdge] is not initially empty, 
// it is cleared and its Lattices are deallocated.
void NodeState::copyLatticesOW(map<PartEdgePtr, vector<Lattice*> >& dfInfoTo,   PartEdgePtr toDepartEdge,
                         const map<PartEdgePtr, vector<Lattice*> >& dfInfoFrom, PartEdgePtr fromDepartEdge, bool adjustPEdge)
{
  /*scope reg("NodeState::copyLatticesOW()", scope::medium, 1, 1);
  dbg << "toDepartEdge="<<(toDepartEdge?toDepartEdge->str():"NULLPtr")<<" fromDepartEdge="<<(fromDepartEdge?fromDepartEdge->str():"NULLPtr")<<endl;*/
  
  assert(dfInfoFrom.find(fromDepartEdge) != dfInfoFrom.end());
  
  // First, empty out dfInfoTo[toDepartEdge] if needed
  for(vector<Lattice*>::iterator lTo=dfInfoTo[toDepartEdge].begin(); lTo!=dfInfoTo[toDepartEdge].end(); lTo++)
    delete *lTo;
  dfInfoTo[toDepartEdge].clear();
  
  map<PartEdgePtr, vector<Lattice*> >::const_iterator eFrom = dfInfoFrom.find(fromDepartEdge);
  for(vector<Lattice*>::const_iterator lFrom=eFrom->second.begin(); lFrom!=eFrom->second.end(); lFrom++) {
    Lattice* lTo = (*lFrom)->copy();
    if(adjustPEdge)
      // Adjust the Lattice's part edge to correspond to its new edge
      lTo->setPartEdge(toDepartEdge);
    dfInfoTo[toDepartEdge].push_back(lTo);
    indent ind(1, 1);
    //dbg << "lTo="<<lTo->str()<<endl;
  }
}

string NodeState::str(string indent)
{
  ostringstream oss;
  
  oss << "[";
  for(BoolMap::iterator i=initializedAnalyses.begin(); i!=initializedAnalyses.end(); ) {
    oss << str(i->first, indent+"&nbsp;&nbsp;&nbsp;&nbsp;");
    i++;
    if(i!=initializedAnalyses.end())
      oss << endl;
  }
  oss << "]";
  
  return oss.str();
}

string NodeState::str(Analysis* analysis, string indent)
{
  ostringstream analysisName;
  /*if(dynamic_cast<ComposedAnalysis*>(analysis)) analysisName << dynamic_cast<ComposedAnalysis*>(analysis)->str();
  else*/                                          analysisName << analysis;

  ostringstream oss;
  
  // If the analysis has not yet been initialized, say so
  if(initializedAnalyses.find(analysis) == initializedAnalyses.end()) {
    oss << "[NodeState ("<<this<<"): NONE for Analysis"<<analysisName.str()<<"]\n";
    /*for(std::map<Analysis*, bool >::iterator a=initializedAnalyses.begin(); a!=initializedAnalyses.end(); a++)
      oss << "a="<<a->first<<endl;*/
  // If it has been initialized, stringify it
  } else {
    oss << "[NodeState ("<<this<<"): analysis ("<<analysisName.str()<<")\n";
    /*assert(dfInfoAbove.size() == dfInfoBelow.size());
    assert(dfInfoAbove.find(analysis) != dfInfoAbove.end());
    assert(dfInfoBelow.find(analysis) != dfInfoBelow.end());*/
    int i=0;
    
    //assert(dfInfoAbove[analysis].size() == dfInfoBelow[analysis].size());
    oss << indent << "Lattices Above: \n"<<indent<<str(dfInfoAbove[analysis], indent+"&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
    oss << indent << "Lattices Below: \n"<<indent<<str(dfInfoBelow[analysis], indent+"&nbsp;&nbsp;&nbsp;&nbsp;")<<endl;
    
    assert(facts.find(analysis) != facts.end());
    i=0;
    const vector<NodeFact*>& aFacts = facts.find(analysis)->second;
    for(vector<NodeFact*>::const_iterator fact=aFacts.begin(); fact!=aFacts.end(); fact++, i++)
      oss << indent << "    Fact "<<i<<": "<<(*fact)->str(indent+"  ")<<"\n";
    oss << indent << "]";
  }
  
  return oss.str();
}

// Returns the string representation of the Lattices stored in the given map
string NodeState::str(const map<PartEdgePtr, vector<Lattice*> >& dfInfo, string indent)
{
  ostringstream oss;
  
  for(map<PartEdgePtr, vector<Lattice*> >::const_iterator e=dfInfo.begin(); e!=dfInfo.end(); e++) {
    PartEdgePtr edg = e->first;
    oss << indent << "edge "<<(edg? edg->str(): "NULL")<<endl;

    int i=0;
    for(vector<Lattice*>::const_iterator l=e->second.begin(); l!=e->second.end(); l++, i++) {
      oss << indent << "&nbsp;&nbsp;&nbsp;&nbsp;Lattice "<<i<<" = "<<(*l)<<"="<<(*l)->str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<"\n";
    }
  }
  
  return oss.str();
}

}; // namespace fuse
