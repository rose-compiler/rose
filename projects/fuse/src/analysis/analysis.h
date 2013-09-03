#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "graphIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"

namespace fuse {
class Analysis;
};

#include "lattice.h"
#include "nodeState.h"
//#include "variables.h"
#include "partitions.h"
//#include "varSets.h"
#include <vector>
#include <set>
#include <map>
#include <list>
#include <iostream>

namespace fuse {

extern int analysisDebugLevel;

// Returns a boost shared pointers to variables that should be
// deallocated when the shared_ptr's reference count drops. Useful
// for passing unmanaged memory to code that takes boost shared 
// pointers as arguments.
void dummy_dealloc(int *);
#ifdef ANALYSIS_C
void dummy_dealloc(int *) {} // Deallocator that does nothing
#endif
template <class RefType>
boost::shared_ptr<RefType> shared_ptr_for_static(RefType& o) 
{
  return boost::shared_ptr<RefType>(&o, dummy_dealloc);
}

class Analysis
{
  public:
    // a filter function to decide which raw CFG node to show (if return true) or hide (otherwise)  
    // This is required to support custom filters of virtual CFG
    // Custom filter is set inside the analysis.
    // Inter-procedural analysis will copy the filter from its analysis during the call to its constructor.
    bool (*filter) (CFGNode cfgn); 
    Analysis(bool (*f)(CFGNode) = defaultFilter):filter(f) {}

    // Runs the analysis. Returns true if the function's NodeState gets modified as a result and false otherwise
    virtual void runAnalysis();
    
    ~Analysis();
};

/********************************
 *** UnstructuredPassAnalyses ***
 ********************************/
class ComposedAnalysis;

// A driver class which simply iterates through all CFG nodes of a specified function
class UnstructuredPassAnalysis : virtual public Analysis
{
  public:
  ComposedAnalysis* analysis;
  
  UnstructuredPassAnalysis(ComposedAnalysis* analysis) : analysis(analysis) {}
  
  // Runs the analysis. Returns true if the function's NodeState gets modified as a result and false otherwise
  void runAnalysis();
  
  virtual void visit(PartPtr p, NodeState& state)=0;
};

/*************************
 *** Dataflow Analyses ***
 *************************/

/// Apply an analysis A's transfer function at a particular AST node type
class DFTransferVisitor : public ROSE_VisitorPatternDefaultBase
{
  protected:
  // Common arguments to the underlying transfer function
  PartPtr part;
  CFGNode cn;
  NodeState &nodeState;
  std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo;

  public:
  DFTransferVisitor(PartPtr p, CFGNode cn, NodeState &s, 
                         std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)
    : part(p), cn(cn), nodeState(s), dfInfo(dfInfo)
  { }

  virtual bool finish() = 0;
  virtual ~DFTransferVisitor() { }
};

class Dataflow : virtual public Analysis
{
  public:

  // the transfer function that is applied to every node
  // part - The Part that is being processed
  // state - the NodeState object that describes the state of the node, as established by earlier
  //   analysis passes
  // dfInfo - The Lattices that this transfer function operates on. The function take a map of lattices, one for
  //   each edge that departs from this part (outgoing for forward analyses and incoming for backwards)
  //   as input and overwrites them with the result of the transfer.
  // Returns true if any of the input lattices changed as a result of the transfer function and
  //    false otherwise.
  virtual bool transfer(PartPtr part, CFGNode cn, NodeState& state, 
                        std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)=0;

  class DefaultTransfer : public DFTransferVisitor
  {
    bool modified;
    Dataflow *analysis;
    public:
    DefaultTransfer(PartPtr part, CFGNode cn, NodeState& state, 
        std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, Dataflow *a)
      : DFTransferVisitor(part, cn, state, dfInfo), modified(false), analysis(a)
      { }

    void visit(SgNode *n) { modified = analysis->transfer(part, cn, nodeState, dfInfo); }
    bool finish() { return modified; }
  };


  // \todo \pp IMO. the function getTransferVisitor is not necessary and can be removed.
  //     Users wanting to write the analysis based on visitors can do so
  //     in the transfer function. (This safes one memory allocation, deallocation,
  //     and boost::shared_pointer management overhead per transfer).
  //     A transfer function using the visitor would look like (if desired this can be
  //     simplified by providing a convenience function taking a visitor as argument):
  // \code
  //     virtual bool transfer(const Function& func, PartPtr p, NodeState& state, const std::vector<Lattice*>& dfInfo, std::vector<Lattice*>** retState, bool fw)
  //     {
  //       MyTransferVisitor visitor(myarguments, func, n, ...);
  //       n.getNode().accept(visitor);
  //       return visitor.finish();
  //     }
  // \endcode
  virtual boost::shared_ptr<DFTransferVisitor> getTransferVisitor(
                PartPtr part, CFGNode cn,
                NodeState& state, 
                std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)
  { return boost::shared_ptr<DFTransferVisitor>(new DefaultTransfer(part, cn, state, dfInfo, this)); }
};

}; // namespace fuse
#endif
