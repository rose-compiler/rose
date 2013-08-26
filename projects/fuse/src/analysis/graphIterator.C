#include "sage3basic.h"
#include "graphIterator.h"
#include <boost/make_shared.hpp>

#include <list>
#include <vector>
#include <set>
#include <string>
#include <iostream>

using namespace std;
using namespace dbglog;
namespace fuse {
  
/****************************************
 ********** PART_EDGE_ITERATOR **********
 ****************************************/
  
template <class GraphEdgePtr, class GraphNodePtr>
graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::graphEdgeIterator(iterDirection dir, bool initialized): dir(dir), initialized(initialized) {
}

template <class GraphEdgePtr, class GraphNodePtr>
graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::graphEdgeIterator(iterDirection dir, const GraphNodePtr start) : dir(dir)
{
  initialized     = true;
  /*GraphEdgePtr startEdge = Part2PartEdge(start);
  remainingNodes.push_front(startEdge);
  visited.insert(startEdge);*/
  //cout << "start="<<start.get()->str()<<endl;
  add(Part2PartEdge(start));
}

template <class GraphEdgePtr, class GraphNodePtr>
graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::graphEdgeIterator(iterDirection dir, const set<GraphNodePtr>& start) : dir(dir)
{
  initialized     = true;
  for(typename set<GraphNodePtr>::iterator i=start.begin(); i!=start.end(); i++) {
    //GraphEdgePtr startEdge = Part2PartEdge(*i);
    //remainingNodes.push_front(startEdge);
    //visited.insert(startEdge);
    add(Part2PartEdge(*i));
  }
}

template <class GraphEdgePtr, class GraphNodePtr>
void graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::init(const GraphNodePtr start) {
  initialized     = true;
  //GraphEdgePtr startEdge = Part2PartEdge(start);
  //remainingNodes.push_front(startEdge);
  //visited.insert(startEdge);
  add(Part2PartEdge(start));
}

// Given a part returns the wildcard edge pointing into it (for fw traversal) or out of it (for backward traversals
template <class GraphEdgePtr, class GraphNodePtr>
GraphEdgePtr graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::Part2PartEdge(GraphNodePtr part) const
{
  if(dir==fw) return part->inEdgeFromAny();
  else        return part->outEdgeToAny();
}

// Given a PartEdge returns the Part that points in the direction of this iterator's traversal (target for fw, 
// source for bw)
template <class GraphEdgePtr, class GraphNodePtr>
GraphNodePtr graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::PartEdge2DirPart(GraphEdgePtr pedge) const
{
  if(dir==fw) return pedge->target();
  else        return pedge->source();
}

// Given a PartEdge returns the Part that points in the direction opposite to this iterator's traversal (source for fw, 
// target for bw)
template <class GraphEdgePtr, class GraphNodePtr>
GraphNodePtr graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::PartEdge2AntiDirPart(GraphEdgePtr pedge) const
{
  if(dir==fw) return pedge->source();
  else        return pedge->target();
}

// Given a Part returns the list of Edges in the direction of this iterator's traversals (outEdges for fw, inEdges
// for bw);
template <class GraphEdgePtr, class GraphNodePtr>
list<GraphEdgePtr> graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::Part2DirEdges(GraphNodePtr part) const
{
  if(dir==fw) return part->outEdges();
  else        return part->inEdges();
}

// returns true if the given GraphNodePtr  is in the remainingNodes list and false otherwise
template <class GraphEdgePtr, class GraphNodePtr>
bool graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::isRemaining(const GraphEdgePtr n)
{
  assert(initialized);
  for(typename list<GraphEdgePtr>::const_iterator it=remainingNodes.begin(); it!=remainingNodes.end(); it++)
  {
    // if in is currently n remainingNodes, say so
    if(*it == n) return true;
  }
  // n is not in remainingNodes
  return false;
}

// Find its followers (either successors or predecessors, depending on value of fwDir), push back 
// those that have not yet been visited
template <class GraphEdgePtr, class GraphNodePtr>
void graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::pushAllDescendants(GraphEdgePtr pedge)
{
  list<GraphEdgePtr> nextE = Part2DirEdges(PartEdge2DirPart(pedge));
  
  //dbg << "pushAllDescendants(): #nextE="<<nextE.size()<<endl;
  for(typename list<GraphEdgePtr>::iterator it=nextE.begin(); it!=nextE.end(); it++)
  {
//    dbg << "pushAllDescendants(): nextN="<<(*it ? it->get()->str(): "NullPartEdge")<<" visited="<<(visited.find(*it) != visited.end())<<" remaining="<<isRemaining(*it)<<endl;

    // if we haven't yet visited this node and don't yet have it on the remainingNodes list
    if(visited.find(*it) == visited.end() &&
       !isRemaining(*it))
    {
      remainingNodes.push_back(*it);
    }
  }

  // If we still have any nodes left remaining
  if(remainingNodes.size()>0)
  {
    // Take the next node from the front of the list and mark it as visited
    visited.insert(remainingNodes.front());

    //dbg << "remainingNodes.front()=["<<remainingNodes.front().getNode()->unparseToString()<<" | "<<remainingNodes.front().getNode()->class_name()<<"]"<<endl;
  }
  // Since pushAllChildren always = true or = false, we only need to worry about managing visited in the true case
}

// Find its followers (either successors or predecessors, depending on value of fwDir), push back 
// those that have not yet been visited
template <class GraphEdgePtr, class GraphNodePtr>
void graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::pushAllDescendants() {
  pushAllDescendants(remainingNodes.front());
}

// Add the given PartEdge to the iterator's list of edges to follow
template <class GraphEdgePtr, class GraphNodePtr>
void graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::add_internal(GraphEdgePtr next)
{
  // If this dataflow iterator is not initialized, initialize it now since it will now have real state
  if(!initialized) initialized = true;
  
  // If next is not currently in remainingNodes, add it
  if(!isRemaining(next))
  {
    typename set<GraphEdgePtr>::iterator nextLoc = visited.find(next);
    if(nextLoc != visited.end())
      visited.erase(visited.find(next));
    remainingNodes.push_back(next);
  }
}

// Add the given PartEdge to the iterator's list of edges to follow
template <class GraphEdgePtr, class GraphNodePtr>
void graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::add(GraphEdgePtr next) {
  /*dbg << "graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::add() next="<<next->str()<<endl;
  dbg << "PartEdge2DirPart(next)="<<(PartEdge2DirPart(next)? PartEdge2DirPart(next)->str(): "NULL")<<endl;*/
  // If the Part of the next PartEdge in the direction of iteration is not NULL, add it
  if(PartEdge2DirPart(next))
    add_internal(next);
  // Otherwise, if it is NULL (a wildcard), fill in the wildcard with all the edges going in this direction 
  // and add them
  else {
    // At least one side of the edge must be non-NULL
    assert(PartEdge2AntiDirPart(next));
    //dbg << "graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::add() PartEdge2AntiDirPart(next)="<<(PartEdge2AntiDirPart(next)? PartEdge2AntiDirPart(next)->str(): "NULL")<<endl;
    list<GraphEdgePtr> edges = Part2DirEdges(PartEdge2AntiDirPart(next));
    //dbg << "graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::add() #edges="<<edges.size()<<endl;
    for(typename list<GraphEdgePtr>::iterator e=edges.begin(); e!=edges.end(); e++) {
      //dbg << "adding="<<e->get()->str()<<endl;
      add_internal(*e);
    }
  }
}

// Advances this iterator in the direction of motion.
// If pushAllChildren=true, all of the current node's unvisited children (predecessors or successors, 
//    depending on fwDir) are pushed onto remainingNodes
// It is assumed that for a given iterator pushAllChildren either always = true or always = false.
template <class GraphEdgePtr, class GraphNodePtr>
void graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::advance(bool pushAllChildren)
{
  assert(initialized);
  //dbg << "graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::advance(pushAllChildren="<<pushAllChildren<<") #remainingNodes="<<remainingNodes.size()<<" front="<<remainingNodes.front()->str()<<endl;
  /*cout<<"  visited=\n";
  for(typename set<GraphNodePtr>::iterator it=visited.begin(); it!=visited.end(); it++)
    cout << "      ["<<it.getNode()->class_name()<<" | "<<it.getNode()<<" | "<<it.getNode()->unparseToString()<<"]\n";*/
  if(remainingNodes.size()>0)
  {
    // pop the next CFG node from the front of the list
    GraphEdgePtr cur = remainingNodes.front();
    remainingNodes.pop_front();
    //dbg << "#remainingNodes="<<remainingNodes.size()<<" cur="<<(cur? cur->str(): "NULLPart")<<endl;
    
    if(pushAllChildren)
      pushAllDescendants(cur);
  }
}

template <class GraphEdgePtr, class GraphNodePtr>
void graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::operator ++ (int)
{
  assert(initialized);
  advance(true);
}

template <class GraphEdgePtr, class GraphNodePtr>
bool graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::eq(const graphEdgeIterator<GraphEdgePtr, GraphNodePtr>& other_it) const
{
  if(initialized != other_it.initialized) return false;
    
  // If both iterators are not initialized, they're equal
  if(!initialized) return true;
    
  //printf("iterator::eq() remainingNodes.size()=%d  other_it.remainingNodes.size()=%d\n", remainingNodes.size(), other_it.remainingNodes.size());
  if(remainingNodes.size() != other_it.remainingNodes.size()) return false;
  
  typename list<GraphEdgePtr>::const_iterator it1, it2;
  // look to ensure that every CFG node in other_it.remainingNodes appears in remainingNodes
  
  for(it1=remainingNodes.begin(); it1!=remainingNodes.end(); it1++)
  {
    for(it2=other_it.remainingNodes.begin(); it2!=other_it.remainingNodes.end(); it2++)
    {
      // if we found *it1 inside other_it.remainingNodes
      if(*it1 == *it2)
      {
        //printf("        (*it1 == *it2)\n");
        break;
      }
    }
    
    // the two iterators are not equal if ...
    
    // the current node in remainingNodes was not found in other_it.remainingNodes
    if(it2!=other_it.remainingNodes.end()) 
    {
      //printf("        it2!=other_it.remainingNodes.end()\n");
      return false; 
    }
      
    // or the two nodes do not have the same visited status in both iterators
    if((visited.find(*it1) == visited.end()) !=
       (other_it.visited.find(*it1) == other_it.visited.end()))
    {
      //printf("        (visited.find(*it1) == visited.end()) != (other_it.visited.find(*it1) == other_it.visited.end())\n");
      return false;
    }
  }
  //printf("graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::eq: returning true\n");
  
  return true;
}

template <class GraphEdgePtr, class GraphNodePtr>
bool graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::operator==(const graphEdgeIterator& other_it) const
{
  return eq(other_it);
}

template <class GraphEdgePtr, class GraphNodePtr>
bool graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::operator!=(const graphEdgeIterator& it) const
{
  return !(*this == it);
}
  
template <class GraphEdgePtr, class GraphNodePtr>
GraphEdgePtr graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::operator * ()
{
  assert(initialized);
  return remainingNodes.front();
}

// template <class GraphEdgePtr, class GraphNodePtr>
// PartEdge* graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::operator -> ()
// {
//   assert(initialized);
//   return (*(*this)).get();
// }

// Get the PartEdge that the iterator is currently is referring to
template <class GraphEdgePtr, class GraphNodePtr>
GraphEdgePtr graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::getPartEdge() const
{
  assert(initialized);
  return remainingNodes.front();
}

// Get the Part within the current PartEdge in the iterator's direction of motion (target for fw, source for bw)
template <class GraphEdgePtr, class GraphNodePtr>
GraphNodePtr graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::getPart() const
{
  assert(initialized);
  return PartEdge2DirPart(remainingNodes.front());
}

/*graphEdgeIterator graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::begin(const GraphNodePtr n)
{
  return graphEdgeIterator(n);
}*/

template <class GraphEdgePtr, class GraphNodePtr>
graphEdgeIterator<GraphEdgePtr, GraphNodePtr> graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::end()
{
  graphEdgeIterator blank(fw);
  // Make sure that this iterator is initialized even though it is empty
  blank.initialized = true;
  return blank;
}

template <class GraphEdgePtr, class GraphNodePtr>
graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint::checkpoint(const list<GraphEdgePtr>& remainingNodes, const set<GraphEdgePtr>& visited)
{
  this->remainingNodes = remainingNodes;
  this->visited  = visited;
}

template <class GraphEdgePtr, class GraphNodePtr>
graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint::checkpoint(const graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint& that)
{
  this->remainingNodes = that.remainingNodes;
  this->visited  = that.visited;
}

template <class GraphEdgePtr, class GraphNodePtr>
string graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint::str(string indent)
{
  ostringstream outs;
  outs << indent << "[graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint : "<<endl;
  for(typename list<GraphEdgePtr>::iterator it=remainingNodes.begin();
      it!=remainingNodes.end(); )
  {
    outs << indent << "&nbsp;&nbsp;&nbsp;&nbsp;"<<it->str();
    it++;
    if(it!=remainingNodes.end()) outs << endl;
  }
  outs << "]";
  return outs.str();
}

// Returns a checkpoint of this iterator's progress.
template <class GraphEdgePtr, class GraphNodePtr>
typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::getChkpt()
{
  assert(initialized);
  typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint chkpt(remainingNodes, visited);
  return chkpt;
}

// Loads this iterator's state from the given checkpoint.
template <class GraphEdgePtr, class GraphNodePtr>
void graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::restartFromChkpt(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint& chkpt)
{  
  remainingNodes.clear();
  visited.clear();
  
  remainingNodes = chkpt.remainingNodes;
  visited = chkpt.visited;
  // The iterator must become initialized because it is only possible to take a checkpoints of initialized iterators
  initialized = true;
}

template <class GraphEdgePtr, class GraphNodePtr>
string graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::str(string indent)
{  
  ostringstream outs;
  
  if(initialized) {
    outs << "[graphEdgeIterator:"<<endl;
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;remainingNodes(#"<<remainingNodes.size()<<") = "<<endl;
    for(typename list<GraphEdgePtr>::iterator it=remainingNodes.begin(); it!=remainingNodes.end(); it++)
    { outs << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<it->get()->str()<<endl; }
    
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;visited(#"<<visited.size()<<")  = \n";
    for(typename set<GraphEdgePtr>::iterator it=visited.begin(); it!=visited.end(); it++) { 
      outs << "&nbsp;&nbsp;&nbsp;&nbsp;"<<it->get()->str()<<endl;
    }
    
    outs << "]";
  } else {
    outs << "[graphEdgeIterator: Uninitialized]";
  }
    
  return outs.str();
}

/*************************************
 ******* FORWARD_EDGE_ITERATOR *******
 *************************************/

template <class GraphEdgePtr, class GraphNodePtr>
fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw_graphEdgeIterator(bool initialized): 
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, initialized) {}

template <class GraphEdgePtr, class GraphNodePtr>
fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw_graphEdgeIterator(const GraphNodePtr start): 
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, start) { }

template <class GraphEdgePtr, class GraphNodePtr>
fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw_graphEdgeIterator(const std::set<GraphNodePtr>& start): 
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, start) { }

template <class GraphEdgePtr, class GraphNodePtr>
fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr> 
  fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::begin(const GraphNodePtr n)
{
  return fw_graphEdgeIterator(n);
}

/**********************************
 ******* bw_EDGE_ITERATOR *******
 *********************************/

template <class GraphEdgePtr, class GraphNodePtr>
bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw_graphEdgeIterator(bool initialized): 
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, initialized) {}

template <class GraphEdgePtr, class GraphNodePtr>
bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw_graphEdgeIterator(const GraphNodePtr start): 
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, start) { }

template <class GraphEdgePtr, class GraphNodePtr>
bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw_graphEdgeIterator(const std::set<GraphNodePtr>& start): 
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, start) { }

/*// Given a part returns the wildcard edge pointing into it (for fw traversal) or out of it (for backward traversals
template <class GraphEdgePtr, class GraphNodePtr>
GraphEdgePtr bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::Part2PartEdge(GraphNodePtr part) const
{
  return part->outEdgeToAny();
}

// Given a PartEdge returns the Part that points in the direction of this iterator's traversal (target for fw, 
// source for bw)
template <class GraphEdgePtr, class GraphNodePtr>
GraphNodePtr bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::PartEdge2DirPart(GraphEdgePtr pedge) const
{
  return pedge->source();
}

// Given a PartEdge returns the Part that points in the direction opposite to this iterator's traversal (source for fw, 
// target for bw)
template <class GraphEdgePtr, class GraphNodePtr>
GraphNodePtr bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::PartEdge2AntiDirPart(GraphEdgePtr pedge) const
{
  return pedge->target();
}

// Given a Part returns the list of Edges in the direction of this iterator's traversals (outEdges for fw, inEdges
// for bw);
template <class GraphEdgePtr, class GraphNodePtr>
list<GraphEdgePtr> bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::Part2DirEdges(GraphNodePtr part) const
{
  return part->inEdges();
}*/

template <class GraphEdgePtr, class GraphNodePtr>
bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr> 
  bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::begin(const GraphNodePtr n)
{
  return bw_graphEdgeIterator(n);
}

/*
template <class GraphEdgePtr, class GraphNodePtr>
graphEdgeIterator graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::end()
{
  graphEdgeIterator blank;
  // Make sure that this iterator is initialized even though it is empty
  blank.initialized = true;
  return blank;
}*/
/***********************************
 ********** PART_ITERATOR **********
 *********************************** /

// The PartEdge-specific access operators should not be used with this iterator
GraphEdgePtr partIterator::operator * ()
{ assert(0); }

PartEdge* partIterator::operator -> ()
{ assert(0); }

GraphNodePtr partIterator::operator * ()
{
  assert(initialized);
  return PartEdge2DirPart(remainingNodes.front());
}

Part* partIterator::operator -> ()
{
  assert(initialized);
  return (*(*this))->get();
}*/

/****************************************
 ********** bw_PART_ITERATOR **********
 **************************************** /

// The PartEdge-specific access operators should not be used with this iterator
GraphEdgePtr bw_partIterator::operator * ()
{ assert(0); }

PartEdge* bw_partIterator::operator -> ()
{ assert(0); }

GraphNodePtr bw_partIterator::operator * ()
{
  assert(initialized);
  return PartEdge2DirPart(remainingNodes.front());
}

Part* bw_partIterator::operator -> ()
{
  assert(initialized);
  return (*(*this))->get();
}*/

/***********************************
 ********** DATAFLOW **********
 ***********************************/

template <class GraphEdgePtr, class GraphNodePtr>
dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir) : 
    graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(dir, true)
{ }

template <class GraphEdgePtr, class GraphNodePtr>
dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir, const GraphNodePtr& terminator_arg) : 
    graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(dir, true)
{
  terminators.insert(terminator_arg);
  // Record that the terminator has been visited to ensure that it is never analyzed
  //visited.insert(terminator_arg);
}

template <class GraphEdgePtr, class GraphNodePtr>
dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir, const GraphNodePtr& start, const GraphNodePtr& terminator_arg): 
    graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(dir, start)
{
  terminators.insert(terminator_arg);
  // Record that the terminator has been visited to ensure that it is never analyzed
  //visited.insert(terminator_arg);
  
  assert(start!=terminator_arg);
}

template <class GraphEdgePtr, class GraphNodePtr>
dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir, const set<GraphNodePtr>& terminators_arg) :
      graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(dir, true), terminators(terminators_arg)
{
  // Record that the terminator has been visited to ensure that it is never analyzed
  /*for(typename set<GraphNodePtr>::iterator i=terminators.begin(); i!=terminators.end(); i++)
    visited.insert(*i);*/
}

template <class GraphEdgePtr, class GraphNodePtr>
dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir, const GraphNodePtr& start, const set<GraphNodePtr>& terminators_arg): 
    graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(dir, start), terminators(terminators_arg)
{
/*  // Record that the terminator has been visited to ensure that it is never analyzed
  for(typename set<GraphNodePtr>::iterator i=terminators.begin(); i!=terminators.end(); i++)
    visited.insert(*i);*/
  
  assert(terminators.find(start) == terminators.end());
}

template <class GraphEdgePtr, class GraphNodePtr>
void dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::init(const GraphNodePtr& start_arg, const GraphNodePtr& terminator_arg)
{
  terminators.insert(terminator_arg);
  // Use the init method to initialize the starting point to make sure that the object is recorded as being initialized
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::init(start_arg);

  assert(terminators.find(start_arg) == terminators.end());  
}

template <class GraphEdgePtr, class GraphNodePtr>
void dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::init(const GraphNodePtr& start_arg, const set<GraphNodePtr>& terminators_arg)
{
  terminators = terminators_arg;
  
  // Use the init method to initialize the starting point to make sure that the object is recorded as being initialized
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::init(start_arg);

  assert(terminators.find(start_arg) == terminators.end());  
}

// Add a Part to this iterator's set of initial iteration Parts
template <class GraphEdgePtr, class GraphNodePtr>
void dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::addStart(GraphNodePtr start)
{
  add(Part2PartEdge(start));
}

// Add the given PartEdge to the iterator's list of edges to follow
template <class GraphEdgePtr, class GraphNodePtr>
void dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::add_internal(GraphEdgePtr next)
{
  // Never add a terminator state
  if(terminators.find(PartEdge2DirPart(next)) != terminators.end()) return;
  
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::add_internal(next);
}

template <class GraphEdgePtr, class GraphNodePtr>
void dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::operator ++ (int)
{
  if(!graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::initialized) { assert(0); }
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::advance(false);
}

template <class GraphEdgePtr, class GraphNodePtr>
dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::
  checkpoint::
    checkpoint(const typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint& iChkpt, 
               const set<GraphNodePtr>& terminators): 
  iChkpt(iChkpt), terminators(terminators) {}

template <class GraphEdgePtr, class GraphNodePtr>
dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint::checkpoint(const dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint &that): 
  iChkpt(that.iChkpt), terminators(that.terminators) {}

template <class GraphEdgePtr, class GraphNodePtr>
string dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint::str(string indent)
{
  ostringstream outs;
  outs << indent << "[dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint : \n"; 
  outs << indent << "&nbsp;&nbsp;&nbsp;&nbsp;iterator = \n"<<iChkpt.str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")<<"\n";
  outs << indent << "&nbsp;&nbsp;&nbsp;&nbsp;terminators = ";
  for(typename set<GraphNodePtr>::iterator i=terminators.begin(); i!=terminators.end(); i++) {
    if(i!=terminators.begin()) outs << ", ";
    outs << i->get()->str();
  }
  outs<<endl;
  return outs.str();
}

// Returns a checkpoint of this iterator's progress.
template <class GraphEdgePtr, class GraphNodePtr>
typename dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::getChkpt()
{
  if(!graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::initialized) { assert(0); }
  typename dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint 
    chkpt(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::getChkpt(), terminators);
  return chkpt;
}

// Loads this iterator's state from the given checkpoint.
template <class GraphEdgePtr, class GraphNodePtr>
void dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::restartFromChkpt(dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint& chkpt)
{
  graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::restartFromChkpt(chkpt.iChkpt);
  terminators = chkpt.terminators;
}

template <class GraphEdgePtr, class GraphNodePtr>
string dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::str(string indent)
{
  ostringstream outs;
  
  if(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::initialized) {
    outs << "[dataflowGraphEdgeIterator:\n";
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;iterator = "<<graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;")<<"\n";
    outs << indent << "&nbsp;&nbsp;&nbsp;&nbsp;terminators = ";
    for(typename set<GraphNodePtr>::iterator i=terminators.begin(); i!=terminators.end(); i++) {
      if(i==terminators.begin()) outs << ", ";
      outs << i->get()->str();
    }
  } else {
    outs << "[dataflowGraphEdgeIterator: Uninitialized]";
  }
    
  return outs.str();
}


/*****************************
******* DATAFLOW *******
*****************************/

template <class GraphEdgePtr, class GraphNodePtr>
fw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw_dataflowGraphEdgeIterator() : 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw), 
        fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(true) {}
template <class GraphEdgePtr, class GraphNodePtr>
fw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw_dataflowGraphEdgeIterator(const GraphNodePtr& terminator_arg) : 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, terminator_arg) {}
template <class GraphEdgePtr, class GraphNodePtr>
fw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw_dataflowGraphEdgeIterator(const set<GraphNodePtr>& terminators_arg) : 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, terminators_arg) {}
template <class GraphEdgePtr, class GraphNodePtr>
fw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw_dataflowGraphEdgeIterator(const GraphNodePtr end, const GraphNodePtr& terminator_arg): 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, end, terminator_arg), 
        fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(end) {}
template <class GraphEdgePtr, class GraphNodePtr>
fw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw_dataflowGraphEdgeIterator(const GraphNodePtr end, const set<GraphNodePtr>& terminators_arg): 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::fw, end, terminators_arg), 
        fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(end) {}

/*void dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::operator ++ (int)
{
  advance(false);
}*/

/*****************************
******* bw_DATAFLOW *******
*****************************/

template <class GraphEdgePtr, class GraphNodePtr>
bw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw_dataflowGraphEdgeIterator() : 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw), 
        bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(true) {}
template <class GraphEdgePtr, class GraphNodePtr>
bw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw_dataflowGraphEdgeIterator(const GraphNodePtr& terminator_arg) : 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, terminator_arg) {}
template <class GraphEdgePtr, class GraphNodePtr>
bw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw_dataflowGraphEdgeIterator(const set<GraphNodePtr>& terminators_arg) : 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, terminators_arg) {}
template <class GraphEdgePtr, class GraphNodePtr>
bw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw_dataflowGraphEdgeIterator(const GraphNodePtr end, const GraphNodePtr& terminator_arg): 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, end, terminator_arg), 
        bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(end) {}
template <class GraphEdgePtr, class GraphNodePtr>
bw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw_dataflowGraphEdgeIterator(const GraphNodePtr end, const set<GraphNodePtr>& terminators_arg): 
        graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, true), 
        dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>(graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::bw, end, terminators_arg), 
        bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>(end) {}

/*void bw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::operator ++ (int)
{
  advance(false);
}*/

/*string bw_dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>::str(string indent)
{
  ostringstream outs;
  
  if(initialized) {
    outs << "[bw_dataflowGraphEdgeIterator:\n";
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;iterator = "<<graphEdgeIterator::str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;")<<"\n";
    outs << indent << "&nbsp;&nbsp;&nbsp;&nbsp;terminators = ";
    for(typename set<GraphNodePtr>::iterator i=terminators.begin(); i!=terminators.end(); i++) {
      if(i==terminators.begin()) outs << ", ";
      outs << i->get()->str();
    }
  } else {
    outs << "[bw_dataflowGraphEdgeIterator: Uninitialized]";
  }
    
  return outs.str();
}*/

template class graphEdgeIterator<PartEdgePtr, PartPtr>;
template class fw_graphEdgeIterator<PartEdgePtr, PartPtr>;
template class bw_graphEdgeIterator<PartEdgePtr, PartPtr>;
template class dataflowGraphEdgeIterator<PartEdgePtr, PartPtr>;
template class fw_dataflowGraphEdgeIterator<PartEdgePtr, PartPtr>;
template class bw_dataflowGraphEdgeIterator<PartEdgePtr, PartPtr>;
}; // namespace fuse
