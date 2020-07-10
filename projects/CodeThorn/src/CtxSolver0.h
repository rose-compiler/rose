#ifndef CTXSOLVER0_H
#define CTXSOLVER0_H 1

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


struct CtxSolver0 : DFAbstractSolver
{
    // define your call context
    //~ typedef InfiniteCallString context_t;
    typedef FiniteCallString context_t;
  
    CtxSolver0( WorkListSeq<Edge>& initWorklist,
	        std::vector<Lattice*>& analyzerDataPreInfo,
	        std::vector<Lattice*>& analyzerDataPostInfo,
	        PropertyStateFactory& initialElementFactory,
	        Flow& flow,
	        DFTransferFunctions& transferFunctions,
		Labeler& thelabeler
	      );
	     
    void runSolver() ROSE_OVERRIDE;
    void computeCombinedPreInfo(Label lab, Lattice& inInfo) ROSE_OVERRIDE;
    
    void setTrace(bool) { /* supported in name only */ }

  private:
    //
    // internal types
    typedef WorkListSeq<std::pair<Edge, context_t> > InternalWorklist; 
    typedef CtxPropertyStateFactory<context_t>       StateFactory;
    typedef CtxTransfer<context_t>                   TransferFunction;
  
    //
    // private functions
    
    void 
    computePostInfo(Label lab, Lattice& inInfo);

    /// returns a range of contexts on which ctxpos can be mapped
    /// \details
    ///    this is only interesting at function returns when the context is
    ///    infinite.
    CtxLatticeRange<context_t>
    mappedCtxRange(Label lab, CtxLatticeRange<context_t>::iterator ctxpos);
  
    /// retrieves the preInfoLattice at @ref lab with context @ref ctx
    Lattice&
    preInfoLattice(Label lab, context_t ctx);

    /// retrieves the preInfoLattice at @ref 
    CtxLattice<context_t>&
    preInfoLattice(Label lab);
    
    /// retrieves an iterator for context at label @ref lab and context @ref ctx 
    CtxLatticeRange<context_t>::iterator
    preInfoLatticeIterator(Label lab, context_t ctx);
    
    /// propagates updated state @ref lat at context @ref ctx to @ref tgt, 
    /// and adds all of @ref tgt out-edges to the worklist @ref wkl
    void
    propagate(const context_t& ctx, Lattice& lat, Label tgt, InternalWorklist& wkl);
    
    /// accesses the labeler
    Labeler&
    labeler() { return _labeler; }
    
    /// creates an internal worklist from the initial worklist @ref _workList
    /// e.g., extracts all available lattices and contexts
    InternalWorklist
    preprocessWorklist();

    //
    // data members
    
    WorkListSeq<Edge>&     _workList;
    std::vector<Lattice*>& _analyzerDataPreInfo;
    std::vector<Lattice*>& _analyzerDataPostInfo;
    StateFactory&          _initialElementFactory;
    Flow&                  _flow;
    TransferFunction&      _transferFunctions;
    Labeler&               _labeler;
};

}

#endif /* CTXSOLVER0_H */
