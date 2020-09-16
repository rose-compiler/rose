#ifndef CTXPRIORITYSOLVER_H
#define CTXPRIORITYSOLVER_H 1

#include <vector>

#include "Labeler.h"
#include "WorkListSeq.h"
#include "Flow.h"
#include "Lattice.h"
#include "CtxLattice.h"
#include "CtxAnalysis.h"
#include "CtxTransfer.h"
#include "CtxCallStrings.h"
#include "PropertyState.h"
#include "DFTransferFunctions.h"
//~ #include "CtxSolver0.h"


namespace CodeThorn {
  
/// A context and priority aware solver  
// \note derived from PASolver1
struct CtxPrioritySolver : DFAbstractSolver
{
    // define your call context
    typedef FiniteCallString      ContextString;
    typedef WorkListSeq<Edge>     InitialWorklist;
    typedef std::vector<Lattice*> LatticeContainer;
  
    CtxPrioritySolver( InitialWorklist& initWorklist,
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
    typedef CtxPropertyStateFactory<ContextString> StateFactory;
    typedef CtxTransfer<ContextString>             TransferFunction;
  
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

    /// retrieves the preInfoLattice at @ref lab
    CtxLattice<ContextString>&
    preInfoLattice(Label lab);
        
    /// propagates updated state @ref lat at context @ref ctx to @ref tgt, 
    /// and adds all of @ref tgt out-edges to the worklist @ref wkl
    /// \tparam W the worklist type
    template <class W>
    void
    propagate(const ContextString& ctx, Lattice& lat, Label tgt, W& wkl);
    
    /// activates a @ref callLbl's return node
    /// \details
    ///    this is necessary b/c when the call context becomes imprecise the call return node
    ///    might receive states while the call label has not been traversed. The context
    ///    mapping will return an empty set and no state will be propagated.
    ///    When, eventually, the traversal of the call node does not yield to new state's
    ///    in the callee, the return node will not be traversed again.
    template <class W>
    void
    activateReturnNode(const ContextString& tgtctx, Label callLbl, W& wkl);

    
    /// accesses the labeler
    Labeler&
    labeler() { return _labeler; }
    
    /// creates an internal worklist from the initial worklist @ref _workList
    /// e.g., extracts all available lattices and contexts
    //~ InternalWorklist
    //~ preprocessWorklist();

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

/// A priority-aware solver  
// \note derived from PASolver1
struct SeqPrioritySolver : DFAbstractSolver
{
    // define your call context
    typedef WorkListSeq<Edge>     InitialWorklist;
    typedef std::vector<Lattice*> LatticeContainer;
  
    SeqPrioritySolver( InitialWorklist& initWorklist,
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
    
    typedef PropertyStateFactory StateFactory;
    typedef DFTransferFunctions  TransferFunction;
    
    //
    // private functions
    
    void 
    computePostInfo(Label lab, Lattice& inInfo);
    
    /// retrieves the preInfoLattice at @ref lab
    Lattice&
    preInfoLattice(Label lab);

    
    /// propagates updated state @ref lat at context @ref ctx to @ref tgt, 
    /// and adds all of @ref tgt out-edges to the worklist @ref wkl
    /// \tparam W the worklist type
    template <class W>
    void
    propagate(Lattice& lat, Label tgt, W& wkl);
          
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

#endif /* CTXPRIORITYSOLVER_H */
