//#include "variables.h"
#include "sage3basic.h"
#include "VirtualCFGIterator.h"
#include <boost/make_shared.hpp>

#include <list>
#include <vector>
#include <set>
#include <string>
#include <iostream>

using namespace std;

namespace fuse{
  
/******************************
 ********** ITERATOR **********
 ******************************/
  
CFGIterator::CFGIterator() {
  initialized     = false;
}

CFGIterator::CFGIterator(const CFGNode start) 
{
  initialized     = true;
  remainingNodes.push_front(start);
  visited.insert(start);
}

void CFGIterator::init(const CFGNode start) {
  initialized     = true;
  remainingNodes.push_front(start);
  visited.insert(start);
}


// returns true if the given CFGNode  is in the remainingNodes list and false otherwise
bool CFGIterator::isRemaining(const CFGNode n)
{
  assert(initialized);
  for(list<CFGNode>::const_iterator it=remainingNodes.begin(); it!=remainingNodes.end(); it++)
  {
    // if in is currently n remainingNodes, say so
    if(*it == n) return true;
  }
  // n is not in remainingNodes
  return false;
}

// advances this CFGIterator in the given direction. Forwards if fwDir=true and backwards if fwDir=false.
// if pushAllChildren=true, all of the current node's unvisited children (predecessors or successors, 
//    depending on fwDir) are pushed onto remainingNodes
// It is assumed that for a given CFGIterator pushAllChildren either always = true or always = false.
void CFGIterator::advance(bool fwDir, bool pushAllChildren)
{
  assert(initialized);
  /*cout << "CFGIterator::advance(fwDir="<<fwDir<<", pushAllChildren="<<pushAllChildren<<") #remainingNodes="<<remainingNodes.size()<<endl;
  cout<<"  visited=\n";
  for(set<CFGNode>::iterator it=visited.begin(); it!=visited.end(); it++)
    cout << "      "<<cfgUtils::CFGNode2Str(*it)<<"\n";*/
  if(remainingNodes.size()>0)
  {
    // pop the next CFG node from the front of the list
    CFGNode cur = remainingNodes.front();
    remainingNodes.pop_front();
    //cout << "#remainingNodes="<<remainingNodes.size()<<" cur="<<cfgUtils::CFGNode2Str(cur)<<endl;
    
    if(pushAllChildren)
    {
      // find its followers (either successors or predecessors, depending on value of fwDir), push back 
      // those that have not yet been visited
      vector<CFGEdge> nextE;
      if(fwDir) nextE = cur.outEdges();
      else      nextE = cur.inEdges();
      //cout << "    #nextE="<<nextE.size()<<endl;
      for(vector<CFGEdge>::iterator it=nextE.begin(); it!=nextE.end(); it++)
      {
        CFGNode nextN = (fwDir ? it->target() : nextN = it->source());
        /*cout << "    nextN="<<cfgUtils::CFGNode2Str(nextN)<<endl;
        cout << "      CFGIterator::advance "<<(fwDir?"descendant":"predecessor")<<": "<<
               "visited="<<(visited.find(nextN) != visited.end())<<
               " remaining="<<isRemaining(nextN)<<"\n";*/
        
        // if we haven't yet visited this node and don't yet have it on the remainingNodes list
        if(visited.find(nextN) == visited.end() &&
           !isRemaining(nextN))
        {
          //printf("       pushing back node <%s: 0x%x: %s> visited=%d\n", nextN.getNode()->class_name().c_str(), nextN.getNode(), nextN.getNode()->unparseToString().c_str(), visited.find(nextN)!=visited.end());
          remainingNodes.push_back(nextN);
        }
      }
    
      // if we still have any nodes left remaining
      if(remainingNodes.size()>0)
      {
        // take the next node from the front of the list and mark it as visited
        visited.insert(remainingNodes.front());
      
        //cout << "        remainingNodes.front()=["<<remainingNodes.front().getNode()->unparseToString()<<" | "<<remainingNodes.front().getNode()->class_name()<<"]"<<endl;
      }
      // Since pushAllChildren always = true or = false, we only need to worry about managing visited in the true case
    }
  }
}

void CFGIterator::operator ++ (int)
{
  assert(initialized);
  advance(true, true);
}

bool CFGIterator::eq(const CFGIterator& other_it) const
{
  if(initialized != other_it.initialized) return false;
    
  // If both CFGIterators are not initialized, they're equal
  if(!initialized) return true;
    
  //printf("CFGIterator::eq() remainingNodes.size()=%d  other_it.remainingNodes.size()=%d\n", remainingNodes.size(), other_it.remainingNodes.size());
  if(remainingNodes.size() != other_it.remainingNodes.size()) return false;
  
  list<CFGNode>::const_iterator it1, it2;
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
    
    // the two CFGIterators are not equal if ...
    
    // the current node in remainingNodes was not found in other_it.remainingNodes
    if(it2!=other_it.remainingNodes.end()) 
    {
      //printf("        it2!=other_it.remainingNodes.end()\n");
      return false; 
    }
      
    // or the two nodes do not have the same visited status in both CFGIterators
    if((visited.find(*it1) == visited.end()) !=
       (other_it.visited.find(*it1) == other_it.visited.end()))
    {
      //printf("        (visited.find(*it1) == visited.end()) != (other_it.visited.find(*it1) == other_it.visited.end())\n");
      return false;
    }
  }
  //printf("CFGIterator::eq: returning true\n");
  
  return true;
}

bool CFGIterator::operator==(const CFGIterator& other_it) const
{
  return eq(other_it);
}

bool CFGIterator::operator!=(const CFGIterator& it) const
{
  return !(*this == it);
}
  
CFGNode CFGIterator::operator * ()
{
  assert(initialized);
  return remainingNodes.front();
}

CFGNode* CFGIterator::operator ->()
{
  assert(initialized);
  return &(remainingNodes.front());
}

CFGIterator CFGIterator::begin(const CFGNode n)
{
  CFGIterator newIter(n);
  return newIter;
}

CFGIterator CFGIterator::end()
{
  CFGIterator blank;
  // Make sure that this CFGIterator is initialized even though it is empty
  blank.initialized = true;
  return blank;
}

CFGIterator::checkpoint::checkpoint(const list<CFGNode>& remainingNodes, const set<CFGNode>& visited)
{
  this->remainingNodes = remainingNodes;
  this->visited  = visited;
}

CFGIterator::checkpoint::checkpoint(const CFGIterator::checkpoint& that)
{
  this->remainingNodes = that.remainingNodes;
  this->visited  = that.visited;
}

string CFGIterator::checkpoint::str(string indent)
{
  ostringstream outs;
  outs << indent << "[VirtualCFG::CFGIterator::checkpoint : "<<endl;
  for(list<CFGNode>::iterator it=remainingNodes.begin();
      it!=remainingNodes.end(); )
  {
    outs << indent << "&nbsp;&nbsp;&nbsp;&nbsp;["<<it->getNode()->unparseToString()<<" | "<<it->getNode()->class_name()<<"]";
    it++;
    if(it!=remainingNodes.end()) outs << endl;
  }
  outs << "]";
  return outs.str();
}

// Returns a checkpoint of this CFGIterator's progress.
CFGIterator::checkpoint CFGIterator::getChkpt()
{
  assert(initialized);
  CFGIterator::checkpoint chkpt(remainingNodes, visited);
  return chkpt;
  //return new CFGIterator::checkpoint::checkpoint(remainingNodes, visited);
}

// Loads this CFGIterator's state from the given checkpoint.
void CFGIterator::restartFromChkpt(CFGIterator::checkpoint& chkpt)
{  
  remainingNodes.clear();
  visited.clear();
  
  remainingNodes = chkpt.remainingNodes;
  visited = chkpt.visited;
  // The CFGIterator must become initialized because it is only possible to take a checkpoints of initialized CFGIterators
  initialized = true;
}

string CFGIterator::str(string indent)
{
  
  ostringstream outs;
  
  if(initialized) {
    outs << "[CFGIterator:"<<endl;
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;remainingNodes(#"<<remainingNodes.size()<<") = "<<endl;
    for(list<CFGNode>::iterator it=remainingNodes.begin(); it!=remainingNodes.end(); it++)
    { outs << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;["<<it->getNode()->unparseToString()<<" | "<<it->getNode()->class_name()<<"]"<<endl; }
    
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;visited(#"<<visited.size()<<")  = \n";
    for(set<CFGNode>::iterator it=visited.begin(); it!=visited.end(); it++)
    { outs << "&nbsp;&nbsp;&nbsp;&nbsp;["<<it->getNode()->unparseToString()<<" | "<<it->getNode()->class_name()<<"]"<<endl; }
    
    outs << "]";
  } else {
    outs << "[CFGIterator: Uninitialized]";
  }
    
  return outs.str();
}


/*****************************
 ******* BACK_ITERATOR *******
 *****************************/
  
void back_CFGIterator::operator ++ (int)
{
  assert(initialized);
  advance(false, true);
}

/******************************
 ********** DATAFLOW **********
 ******************************/

dataflowCFGIterator::dataflowCFGIterator(const CFGNode terminator_arg) : terminator(terminator_arg)
{
  // Record that the terminator has been visited to ensure that it is never analyzed
  visited.insert(terminator);
}

dataflowCFGIterator::dataflowCFGIterator(const CFGNode start, const CFGNode terminator_arg): 
    CFGIterator(start), terminator(terminator_arg)
{
  // Record that the terminator has been visited to ensure that it is never analyzed
  visited.insert(terminator);
  
  assert(start!=terminator);
}

void dataflowCFGIterator::init(const CFGNode start_arg, const CFGNode terminator_arg)
{
  assert(start_arg!=terminator_arg);
  // Use the init method to initialize the starting point to make sure that the object is recorded as being initialized
  CFGIterator::init(start_arg);
  terminator = terminator_arg;
}
 
void dataflowCFGIterator::add(const CFGNode next)
{
  // If this dataflow CFGIterator is not initialized, initialize it now since it will now have real state
  if(!initialized) initialized = true;
  
  // never add the terminator node
  if(next==terminator)
    return;
  
  // if next is not currently in remainingNodes, add it
  if(!isRemaining(next))
  {
    set<CFGNode>::iterator nextLoc = visited.find(next);
    if(nextLoc != visited.end())
      visited.erase(visited.find(next));
    remainingNodes.push_back(next);
  }
}

void dataflowCFGIterator::operator ++ (int)
{
  assert(initialized);
  advance(true, false);
}

dataflowCFGIterator::checkpoint::checkpoint(const CFGIterator::checkpoint& iChkpt, const CFGNode terminator): 
  iChkpt(iChkpt), terminator(terminator) {}

dataflowCFGIterator::checkpoint::checkpoint(const dataflowCFGIterator::checkpoint &that): 
  iChkpt(that.iChkpt), terminator(that.terminator) {}

string dataflowCFGIterator::checkpoint::str(string indent)
{
  ostringstream outs;
  outs << indent << "[VirtualCFG::dataflowCFGIterator::checkpoint : \n"; 
  outs << indent << "&nbsp;&nbsp;&nbsp;&nbsp;CFGIterator = \n"<<iChkpt.str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;")<<"\n";
  outs << indent << "&nbsp;&nbsp;&nbsp;&nbsp;terminator = ["<<terminator.getNode()->unparseToString()<<" | "<<terminator.getNode()->class_name()<<"]"<<endl;
  return outs.str();
}

// Returns a checkpoint of this CFGIterator's progress.
dataflowCFGIterator::checkpoint dataflowCFGIterator::getChkpt()
{
  assert(initialized);
  dataflowCFGIterator::checkpoint chkpt(CFGIterator::getChkpt(), terminator);
  return chkpt;
  //return new dataflowCFGIterator::checkpoint::checkpoint(CFGIterator::getChkpt(), terminator);
}

// Loads this CFGIterator's state from the given checkpoint.
void dataflowCFGIterator::restartFromChkpt(dataflowCFGIterator::checkpoint& chkpt)
{
  CFGIterator::restartFromChkpt(chkpt.iChkpt);
  terminator = chkpt.terminator;
}

string dataflowCFGIterator::str(string indent)
{
  ostringstream outs;
  
  if(initialized) {
    outs << "[dataflowCFGIterator:\n";
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;CFGIterator = "<<CFGIterator::str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;")<<"\n";
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;terminator = ["<<terminator.getNode()->unparseToString()<<" | "<<terminator.getNode()->class_name()<<"]"<<endl;
  } else {
    outs << "[dataflowCFGIterator: Uninitialized]";
  }
    
  return outs.str();
}


/*****************************
******* BACK_DATAFLOW *******
*****************************/
back_dataflowCFGIterator::back_dataflowCFGIterator(const CFGNode end, const CFGNode terminator_arg): 
    CFGIterator(end), dataflowCFGIterator(end, terminator_arg) {
}

void back_dataflowCFGIterator::operator ++ (int)
{
  advance(false, false);
}

string back_dataflowCFGIterator::str(string indent)
{
  ostringstream outs;
  
  if(initialized) {
    outs << "[back_dataflowCFGIterator:\n";
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;CFGIterator = "<<CFGIterator::str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;")<<"\n";
    outs << "&nbsp;&nbsp;&nbsp;&nbsp;terminator = ["<<terminator.getNode()->unparseToString()<<" | "<<terminator.getNode()->class_name()<<"]"<<endl;
  } else {
    outs << "[back_dataflowCFGIterator: Uninitialized]";
  }
    
  return outs.str();
}

} // namespace fuse
