#pragma once

#include "virtualCFG.h"
#include "DataflowCFG.h"
#include "partitions.h"

#include <list>
#include <set>
#include <string>

namespace fuse {

// Iterates over PartEdges in a Partition, respecting dependences in the graph.
// Supports both forward and backward iteration.
template <class GraphEdgePtr, class GraphNodePtr>
class graphEdgeIterator
{
  public:
  typedef enum {fw, bw} iterDirection;
  iterDirection dir;
  
  public:
          
  std::list<GraphEdgePtr> remainingNodes;
  std::set<GraphEdgePtr> visited;
  bool initialized;

  public:
  graphEdgeIterator(iterDirection dir, bool initialized=false);
  graphEdgeIterator(iterDirection dir, const GraphNodePtr start);
  graphEdgeIterator(iterDirection dir, const std::set<GraphNodePtr>& start);
  virtual ~graphEdgeIterator() { }
  
  void init(const GraphNodePtr start);

  protected:
  
  // Given a part returns the wildcard edge pointing into it (for fw traversal) or out of it (for backward traversals
  GraphEdgePtr Part2PartEdge(GraphNodePtr part) const;
  
  // Given a PartEdge returns the Part that points in the direction of this iterator's traversal (target for fw, 
  // source for bw)
  GraphNodePtr PartEdge2DirPart(GraphEdgePtr pedge) const;
  
  // Given a PartEdge returns the Part that points in the direction opposite to this iterator's traversal (source for fw, 
  // target for bw)
  GraphNodePtr PartEdge2AntiDirPart(GraphEdgePtr pedge) const;
  
  // Given a Part returns the list of Edges in the direction of this iterator's traversals (outEdges for fw, inEdges
  // for bw);
  std::list<GraphEdgePtr> Part2DirEdges(GraphNodePtr part) const;
  
  // Returns true if the given GraphNodePtr is in the remainingNodes list and false otherwise
  bool isRemaining(const GraphEdgePtr pedge);
  
  // Find the followers of curPart (either successors or predecessors, depending on value of fwDir), push back 
  // those that have not yet been visited
  void pushAllDescendants(GraphEdgePtr pedge);
  
  public:
  void pushAllDescendants();
  
  protected:
  // Add the given PartEdge to the iterator's list of edges to follow
  virtual void add_internal(GraphEdgePtr next);
  
  public:
  // Add the given PartEdge to the iterator's list of edges to follow
  void add(GraphEdgePtr next);
  
  //protected:
  // Advances this iterator in the direction of motion.
  // If pushAllChildren=true, all of the current node's unvisited children (predecessors or successors, 
  //    depending on fwDir) are pushed onto remainingNodes
  void advance(bool pushAllChildren);
  
  public:
  virtual void operator ++ (int);
  
  bool eq(const graphEdgeIterator& other_it) const;
  
  bool operator==(const graphEdgeIterator& other_it) const;
  
  bool operator!=(const graphEdgeIterator& it) const;
          
  virtual GraphEdgePtr operator * ();
  // virtual PartEdge* operator -> ();
  
  // Get the PartEdge that the iterator is currently is referring to
  GraphEdgePtr getPartEdge() const;
  
  // Get the Part within the current PartEdge in the iterator's direction of motion (target for fw, source for bw)
  GraphNodePtr getPart() const;
  
  // Returns an empty iterator that can be compared to any other iterator to 
  // check if it has completed passing over its iteration space
  static graphEdgeIterator end();
  
  // Contains the state of an iterator, allowing iterators to be 
  // checkpointed and restarted.
  class checkpoint
  {
    std::list<GraphEdgePtr> remainingNodes;
    std::set<GraphEdgePtr>  visited;
  
    public:
    checkpoint(const std::list<GraphEdgePtr>& remainingNodes, const std::set<GraphEdgePtr>& visited);
          
    checkpoint(const checkpoint& that);
    
    std::string str(std::string indent="");
    
    friend class graphEdgeIterator;
  };
  
  // Returns a checkpoint of this iterator's progress.
  checkpoint getChkpt();
  
  // Loads this iterator's state from the given checkpoint.
  void restartFromChkpt(checkpoint& chkpt);
  
  std::string str(std::string indent="");
};

typedef graphEdgeIterator<PartEdgePtr, PartPtr> partEdgeIterator;

// Iterates over PartEdges in a Partition, respecting dependences in the graph.
// Supports both forward and backward iteration.
template <class GraphEdgePtr, class GraphNodePtr>
class fw_graphEdgeIterator : public virtual graphEdgeIterator<GraphEdgePtr, GraphNodePtr>
{
  public:
  fw_graphEdgeIterator(bool initialized=false);
  fw_graphEdgeIterator(const GraphNodePtr start);
  fw_graphEdgeIterator(const std::set<GraphNodePtr>& start);
  
  protected:
  
  /* // Given a part returns the wildcard edge pointing into it (for fw traversal) or out of it (for backward traversals
  GraphEdgePtr Part2PartEdge(GraphNodePtr part) const;
  
  // Given a PartEdge returns the Part that points in the direction of this iterator's traversal (target for fw, 
  // source for bw)
  GraphNodePtr PartEdge2DirPart(GraphEdgePtr pedge) const;
  
  // Given a PartEdge returns the Part that points in the direction opposite to this iterator's traversal (source for fw, 
  // target for bw)
  GraphNodePtr PartEdge2AntiDirPart(GraphEdgePtr pedge) const;
  
  // Given a Part returns the list of Edges in the direction of this iterator's traversals (outEdges for fw, inEdges
  // for bw);
  std::list<GraphEdgePtr> Part2DirEdges(GraphNodePtr part) const;*/
  
  public:
  // Returns a fresh iterator that starts at node n
  static fw_graphEdgeIterator begin(const GraphNodePtr n);
};

typedef fw_graphEdgeIterator<PartEdgePtr, PartPtr> fw_partEdgeIterator;

template <class GraphEdgePtr, class GraphNodePtr>
class bw_graphEdgeIterator : public virtual graphEdgeIterator<GraphEdgePtr, GraphNodePtr>
{
  public:
  bw_graphEdgeIterator(bool initialized=false);
  bw_graphEdgeIterator(const GraphNodePtr start);
  bw_graphEdgeIterator(const std::set<GraphNodePtr>& start);
  
  protected:
  /* // Given a part returns the wildcard edge pointing into it (for fw traversal) or out of it (for backward traversals
  GraphEdgePtr Part2PartEdge(GraphNodePtr part) const;
  
  // Given a PartEdge returns the Part that points in the direction of this iterator's traversal (target for fw, 
  // source for bw)
  GraphNodePtr PartEdge2DirPart(GraphEdgePtr pedge) const;
  
  // Given a PartEdge returns the Part that points in the direction opposite to this iterator's traversal (source for fw, 
  // target for bw)
  GraphNodePtr PartEdge2AntiDirPart(GraphEdgePtr pedge) const;
  
  // Given a Part returns the list of Edges in the direction of this iterator's traversals (outEdges for fw, inEdges
  // for bw);
  std::list<GraphEdgePtr> Part2DirEdges(GraphNodePtr part) const;*/
  
  public:
  // Returns a fresh iterator that starts at node n
  static bw_graphEdgeIterator begin(const GraphNodePtr n);
  
  // Returns an empty iterator that can be compared to any other iterator to 
  // check if it has completed passing over its iteration space
  //static bw_graphEdgeIterator end();
};
//class bw_graphEdgeIterator<PartEdgePtr, PartPtr>;
typedef bw_graphEdgeIterator<PartEdgePtr, PartPtr> bw_partEdgeIterator;

/*class partIterator: public graphEdgeIterator
{
  GraphEdgePtr operator * ();
  PartEdge* operator -> ();
  GraphNodePtr operator * ();
  Part* operator -> ();
};

class bw_partIterator: public bw_graphEdgeIterator
{
  GraphEdgePtr operator * ();
  PartEdge* operator -> ();
  GraphNodePtr operator * ();
  Part* operator -> ();
};*/

template <class GraphEdgePtr, class GraphNodePtr>
class dataflowGraphEdgeIterator : public virtual graphEdgeIterator<GraphEdgePtr, GraphNodePtr>
{
  protected:
  std::set<GraphNodePtr> terminators;
  public:
  //dataflow(): iterator() {}
  
  dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir);
  dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir, const GraphNodePtr& terminator_arg);
  dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir, const GraphNodePtr& start, const GraphNodePtr& terminator_arg);
  dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir, const std::set<GraphNodePtr>& terminators_arg);
  dataflowGraphEdgeIterator(typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::iterDirection dir, const GraphNodePtr& start, const std::set<GraphNodePtr>& terminators_arg);
  
  void init(const GraphNodePtr& start_arg, const GraphNodePtr& terminator_arg);
  void init(const GraphNodePtr& start_arg, const std::set<GraphNodePtr>& terminators_arg);
  
  // Add a Part to this iterator's set of initial iteration Parts
  void addStart(GraphNodePtr start);

  protected:
  // Add the given PartEdge to the iterator's list of edges to follow
  void add_internal(GraphEdgePtr next);
  
  public:
  void operator ++ (int);
  
  // Contains the state of an dataflow iterator, allowing dataflow 
  // iterators to be checkpointed and restarted.
  class checkpoint/* : public virtual BaseCFG::checkpoint*/
  {
    protected:
    typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint iChkpt;
    std::set<GraphNodePtr> terminators;
    
    public:
    checkpoint(const typename graphEdgeIterator<GraphEdgePtr, GraphNodePtr>::checkpoint& iChkpt, const std::set<GraphNodePtr>& terminators);
    
    checkpoint(const checkpoint &that);
            
    std::string str(std::string indent="");
    
    friend class dataflowGraphEdgeIterator;
  };
  
  // Returns a checkpoint of this dataflow iterator's progress.
  checkpoint getChkpt();
  
  // Loads this dataflow iterator's state from the given checkpoint.
  void restartFromChkpt(checkpoint& chkpt);
  
  std::string str(std::string indent="");
};

typedef dataflowGraphEdgeIterator<PartEdgePtr, PartPtr> dataflowPartEdgeIterator;

template <class GraphEdgePtr, class GraphNodePtr>
class fw_dataflowGraphEdgeIterator: public virtual dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>, 
                                    public virtual fw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>
{
  public: 
  fw_dataflowGraphEdgeIterator();
  fw_dataflowGraphEdgeIterator(const GraphNodePtr& terminator_arg);
  fw_dataflowGraphEdgeIterator(const std::set<GraphNodePtr>& terminators_arg);
  fw_dataflowGraphEdgeIterator(const GraphNodePtr end, const GraphNodePtr& terminator_arg);
  fw_dataflowGraphEdgeIterator(const GraphNodePtr end, const std::set<GraphNodePtr>& terminators_arg);
          
  //void operator ++ (int);
  
  //std::string str(std::string indent="");
};

typedef fw_dataflowGraphEdgeIterator<PartEdgePtr, PartPtr> fw_dataflowPartEdgeIterator;

template <class GraphEdgePtr, class GraphNodePtr>
class bw_dataflowGraphEdgeIterator: public virtual dataflowGraphEdgeIterator<GraphEdgePtr, GraphNodePtr>, 
                                    public virtual bw_graphEdgeIterator<GraphEdgePtr, GraphNodePtr>
{
  public: 
  bw_dataflowGraphEdgeIterator();
  bw_dataflowGraphEdgeIterator(const GraphNodePtr& terminator_arg);
  bw_dataflowGraphEdgeIterator(const std::set<GraphNodePtr>& terminators_arg);
  bw_dataflowGraphEdgeIterator(const GraphNodePtr end, const GraphNodePtr& terminator_arg);
  bw_dataflowGraphEdgeIterator(const GraphNodePtr end, const std::set<GraphNodePtr>& terminators_arg);
          
  //void operator ++ (int);
  
  //std::string str(std::string indent="");
};

typedef bw_dataflowGraphEdgeIterator<PartEdgePtr, PartPtr> bw_dataflowPartEdgeIterator;


/*template <class GraphEdgePtr, class GraphNodePtr>
std::map<GraphNodePtr, std::set<GraphNodePtr> > computeDominators() {
  set<PartPtr> startingStates = GetStartAStates(client);
  fw_dataflowPartEdgeIterator worklist;
  for(set<PartPtr>::iterator s=startingStates.begin(); s!=startingStates.end(); s++) {
    worklist->addStart(*s);
    
    set<PartPtr> onlyS; onlyS.insert(*s);
    dominators[s] = onlyS;
  }
  
  while(*worklist!=dataflowPartEdgeIterator::end()) {
    PartPtr part = worklist->getPart();
    allParts.insert(part);
    
    // There must be a dominator record for each part before we visit it
    assert(dominators.find(part) != dominators.end());
    
    // Intersect this part's dominator set with the dominator sets of all of its successors
    list<PartEdgePtr> descendants = part->outEdges();
    for(list<PartEdgePtr>::iterator de=descendants.begin(); de!=descendants.end(); de++) {
      // If the current descendant does not have a mapping in the graph, its current dominator set must be 
      // all the parts
      PartPtr target = (*de)->target();
      if(dominators.find(target) == dominators.end()) {
        // The intersection of all parts with dominators[part] is dominators[part]
        dominators[target] = dominators[part];
      } else {
        //set_intersection(dominators[target].begin(), dominators[target].end(), dominators[part].begin(), dominators[part].end(), .begin());
        set<PartPtr>::iterator partIt = dominators[part].begin(),
                               descIt = dominators[target].begin();
        while(partIt!=dominators[part].end() && descIt!=dominators[target].end()) {
          if(*partIt < *descIt) partIt++;
          else if(*partId > *descIt) descIt = dominators[target].erase(descIt);
          else if(*partId == *descIt) {
            partIt++;
            descIt++;
          }
        }
        while(descIt!=dominators[target].end()))
          descIt = dominators[target].erase(descIt);
      }
    } 
  }*/

extern template class graphEdgeIterator<PartEdgePtr, PartPtr>;
extern template class fw_graphEdgeIterator<PartEdgePtr, PartPtr>;
extern template class bw_graphEdgeIterator<PartEdgePtr, PartPtr>;
extern template class dataflowGraphEdgeIterator<PartEdgePtr, PartPtr>;
extern template class fw_dataflowGraphEdgeIterator<PartEdgePtr, PartPtr>;
extern template class bw_dataflowGraphEdgeIterator<PartEdgePtr, PartPtr>;


}; // namespace fuse
