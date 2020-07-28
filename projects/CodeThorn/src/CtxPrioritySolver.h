#ifndef CTXPRIORITYSOLVER_H
#define CTXPRIORITYSOLVER_H 1

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
#include "CtxSolver0.h"

#include <vector>

namespace CodeThorn {
  
/// A context-aware solver  
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

    /// retrieves the preInfoLattice at @ref 
    CtxLattice<ContextString>&
    preInfoLattice(Label lab);
    
    /// retrieves an iterator for context at label @ref lab and context @ref ctx 
    CtxLatticeRange<ContextString>::iterator
    preInfoLatticeIterator(Label lab, const ContextString& ctx);
    
    /// propagates updated state @ref lat at context @ref ctx to @ref tgt, 
    /// and adds all of @ref tgt out-edges to the worklist @ref wkl
    /// \tparam W the worklist type
    template <class W>
    void
    propagate(const ContextString& ctx, Lattice& lat, Label tgt, W& wkl);
    
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

}

#endif /* CTXPRIORITYSOLVER_H */
