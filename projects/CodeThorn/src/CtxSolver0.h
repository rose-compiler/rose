#ifndef CTXSOLVER0_H
#define CTXSOLVER0_H 1

#include "Labeler.h"
#include "WorkListSeq.h"
#include "Flow.h"
#include "Lattice.h"
#include "CtxPropertyStateFactory.h"
#include "CtxLattice.h"
#include "CtxTransfer.h"
#include "CtxCallStrings.h"
#include "PropertyState.h"
#include "DFTransferFunctions.h"

#include <vector>

namespace CodeThorn {
  
template <class CallString>
struct CtxLatticeRange
{
    typedef typename CtxLattice<CallString>::context_map::const_iterator iterator;

    CtxLatticeRange(iterator first, iterator limit)
    : aa(first), zz(limit)
    {}
    
    iterator begin() const { return aa; }
    iterator end()   const { return zz; }
  
  private:
    iterator aa;
    iterator zz;
    
    CtxLatticeRange() = delete;
};


/// A context-aware solver  
// \note derived from PASolver1
struct CtxSolver0 : DFAbstractSolver
{
    // define your call context
    //~ typedef InfiniteCallString ContextString;
    typedef FiniteCallString      ContextString;
    typedef WorkListSeq<Edge>     InitialWorklist;
    typedef std::vector<Lattice*> LatticeContainer;

  
    CtxSolver0( InitialWorklist& initWorklist,
	              LatticeContainer& analyzerDataPreInfo,
	              LatticeContainer& analyzerDataPostInfo,
	              PropertyStateFactory& initialElementFactory,
	              Flow& flow,
	              DFTransferFunctions& transferFunctions,
		            Labeler& thelabeler
	            );
	     
    void runSolver() ROSE_OVERRIDE;
    void computeCombinedPreInfo(Label lab, Lattice& inInfo) ROSE_OVERRIDE;
    
    void setTrace(bool) ROSE_OVERRIDE { /* supported in name only */ }

  private:
    //
    // internal types
    typedef WorkListSeq<std::pair<Edge, ContextString> > InternalWorklist; 
    typedef CtxPropertyStateFactory<ContextString>       StateFactory;
    typedef CtxTransfer<ContextString>                   TransferFunction;
  
    //
    // private functions
    
    void 
    computePostInfo(Label lab, Lattice& inInfo);

    /// returns a range of contexts on which ctxpos can be mapped
    /// \details
    ///    this is only interesting at function returns when the context is
    ///    infinite.
    CtxLatticeRange<ContextString>
    mappedCtxRange(Label lab, CtxLatticeRange<ContextString>::iterator ctxpos);
  
    /// retrieves the preInfoLattice at @ref lab with context @ref ctx
    Lattice&
    preInfoLattice(Label lab, const ContextString& ctx);

    /// retrieves the preInfoLattice at @ref lbl 
    CtxLattice<ContextString>&
    preInfoLattice(Label lab);
        
    /// propagates updated state @ref lat at context @ref ctx to @ref tgt, 
    /// and adds all of @ref tgt out-edges to the worklist @ref wkl
    void
    propagate(const ContextString&, Lattice& lat, Label tgt, InternalWorklist& wkl);
    
    /// activates a @ref callLbl's return node
    /// \details
    ///    this is necessary b/c when the call context becomes imprecise the call return node
    ///    might receive states while the call label has not been traversed. The context
    ///    mapping will return an empty set and no state will be propagated.
    ///    When, eventually, the traversal of the call node does not yield to new state's
    ///    in the callee, the return node will not be traversed again.
    void
    activateReturnNode(const ContextString& tgtctx, Label callLbl, InternalWorklist& wkl);
    
    /// accesses the labeler
    Labeler&
    labeler() { return _labeler; }
    
    /// creates an internal worklist from the initial worklist @ref _workList
    /// e.g., extracts all available lattices and contexts
    InternalWorklist
    preprocessWorklist();

    //
    // data members    
    InitialWorklist&  _workList;
    LatticeContainer& _analyzerDataPreInfo;
    LatticeContainer& _analyzerDataPostInfo;
    StateFactory&     _initialElementFactory;
    Flow&             _flow;
    TransferFunction& _transferFunctions;
    Labeler&          _labeler;    
};

}

#endif /* CTXSOLVER0_H */
