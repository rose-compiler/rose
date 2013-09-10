#ifndef VIRTUAL_CFG_ITERATOR
#define VIRTUAL_CFG_ITERATOR

#include "virtualCFG.h"
#include "DataflowCFG.h"
#include "partitions.h"
//#include "baseCFGIterator.h"

#include <list>
#include <set>
#include <string>

namespace fuse {

// Iterates over Parts in a VirtualCFG, respecting dependences in the graph.
// Supports both forward and backward iteration.
class CFGIterator
{
  //protected:
  public:
          
  std::list<CFGNode> remainingNodes;
  std::set<CFGNode> visited;
  bool initialized;

  public:
  CFGIterator();
  
  CFGIterator(const CFGNode start);
  virtual ~CFGIterator() { }
  
  void init(const CFGNode start);

  protected:
  // returns true if the given CFGNode is in the remainingNodes list and false otherwise
  bool isRemaining(const CFGNode p);
          
  // advances this CFGIterator in the given direction. Forwards if fwDir=true and backwards if fwDir=false.
  // if pushAllChildren=true, all of the current node's unvisited children (predecessors or successors, 
  //    depending on fwDir) are pushed onto remainingNodes
  void advance(bool fwDir, bool pushAllChildren);
  
  public:
  virtual void operator ++ (int);
  
  bool eq(const CFGIterator& other_it) const;
  
  bool operator==(const CFGIterator& other_it) const;
  
  bool operator!=(const CFGIterator& it) const;
          
  CFGNode operator * ();
  CFGNode* operator ->();
  
  // Returns a fresh CFGIterator that starts at node n
  static CFGIterator begin(const CFGNode n);
  
  // Returns an empty CFGIterator that can be compared to any other CFGIterator to 
  // check if it has completed passing over its iteration space
  static CFGIterator end();
  
  // Contains the state of an CFGIterator, allowing CFGIterators to be 
  // checkpointed and restarted.
  class checkpoint/* : public virtual BaseCFG::CFGIterator::checkpoint*/
  {
    std::list<CFGNode> remainingNodes;
    std::set<CFGNode>  visited;
  
    public:
    checkpoint(const std::list<CFGNode>& remainingNodes, const std::set<CFGNode>& visited);
          
    checkpoint(const checkpoint& that);
    
    std::string str(std::string indent="");
    
    friend class CFGIterator;
  };
  
  // Returns a checkpoint of this CFGIterator's progress.
  checkpoint getChkpt();
  
  // Loads this CFGIterator's state from the given checkpoint.
  void restartFromChkpt(checkpoint& chkpt);
  
  std::string str(std::string indent="");
};

class back_CFGIterator : public virtual CFGIterator
{
  public:
  back_CFGIterator(): CFGIterator() {}
  
  back_CFGIterator(const CFGNode end): CFGIterator(end) { }
  
  void operator ++ (int);
};

class dataflowCFGIterator : public virtual CFGIterator
{
  protected:
  CFGNode terminator;
  public:
  
  dataflowCFGIterator(const CFGNode terminator_arg);
  
  dataflowCFGIterator(const CFGNode start, const CFGNode terminator_arg);
  
  void init(const CFGNode start_arg, const CFGNode terminator_arg);
    
  void add(const CFGNode next);
  
  void operator ++ (int);
  
  // Contains the state of an dataflow iterator, allowing dataflow 
  // iterators to be checkpointed and restarted.
  class checkpoint/* : public virtual BaseCFG::dataflow::checkpoint*/
  {
          protected:
          CFGIterator::checkpoint iChkpt;
          CFGNode terminator;
          
          public:
          checkpoint(const CFGIterator::checkpoint& iChkpt, const CFGNode terminator);
          
          checkpoint(const checkpoint &that);
                  
          std::string str(std::string indent="");
          
          friend class dataflowCFGIterator;
  };
  
  // Returns a checkpoint of this dataflow CFGIterator's progress.
  checkpoint getChkpt();
  
  // Loads this dataflow CFGIterator's state from the given checkpoint.
  void restartFromChkpt(checkpoint& chkpt);
  
  std::string str(std::string indent="");
};

class back_dataflowCFGIterator: /*public virtual BaseCFG::back_dataflow,*/ public virtual dataflowCFGIterator
{
  public: 
  back_dataflowCFGIterator(const CFGNode terminator_arg) : dataflowCFGIterator(terminator_arg) {}
  back_dataflowCFGIterator(const CFGNode end, const CFGNode terminator_arg);
          
  void operator ++ (int);
  
  std::string str(std::string indent="");
};
} // namespace fuse
#endif
