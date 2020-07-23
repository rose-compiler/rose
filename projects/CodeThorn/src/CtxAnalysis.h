#ifndef CTX_ANALYSIS_H
#define CTX_ANALYSIS_H 1

/// \author Peter Pirkelbauer

#include "CtxLattice.h"
#include "CtxTransfer.h"
#include "CtxAttribute.h"
#include "CtxPropertyStateFactory.h"
#include "CtxSolver0.h"

namespace CodeThorn
{

struct CtxStats
{
  size_t min        = size_t(-1);
  size_t max        = 0;
  size_t numNonbot  = 0;
  size_t numBot     = 0;

  Label    maxLbl   = Label();
  Lattice* maxLat   = nullptr; 
  
  double avg        = 0;
};


/// analysis class that wraps a context-sensitive analysis around
///   a non-context-sensitive forward analysis.
template <class CallContext>
struct CtxAnalysis : DFAnalysisBase
{
    typedef DFAnalysisBase        base;
    typedef CallContext           context_t;
    typedef CtxLattice<context_t> context_lattice_t;
    
    CtxAnalysis(PropertyStateFactory& compFactory, DFTransferFunctions& compTransfer)
    : base(), ctxFactory(compFactory), ctxTransfer(compTransfer, *this)
    {
      _transferFunctions = &ctxTransfer;
      _transferFunctions->setInitialElementFactory(&ctxFactory);
    }

    CtxAnalysis(PropertyStateFactory& compFactory, DFTransferFunctions& compTransfer, const context_lattice_t& init)
    : base(), ctxFactory(compFactory), ctxTransfer(compTransfer, *this, init)
    {
      _transferFunctions = &ctxTransfer;
      _transferFunctions->setInitialElementFactory(&ctxFactory);
    }

    const CtxLattice<CallContext>&
    getCtxLattice(Label lbl)
    {
      return dynamic_cast<context_lattice_t&>(SG_DEREF(getPreInfo(lbl)));
    }

    /// retrieves the lattice from the call site
    const CtxLattice<CallContext>&
    getCallSiteLattice(const SgStatement& stmt)
    {
      SgStatement& call    = const_cast<SgStatement&>(stmt);
      Labeler&     labeler = *getLabeler();

      //~ std::cerr << call.unparseToString() << std::endl;
      return getCtxLattice( labeler.functionCallLabel(&call) );
    }

    /// retrieves the lattice from the call site
    const CtxLattice<CallContext>&
    getCallSiteLattice(const SgCallExpression& exp)
    {
      return getCallSiteLattice(sg::ancestor<SgStatement>(exp));
    }

    /// retrieves the lattice from the call site
    const CtxLattice<CallContext>&
    getCallSiteLattice(Label lblret)
    {
      Labeler& labeler = *getLabeler();
      ROSE_ASSERT(labeler.isFunctionCallReturnLabel(lblret) && (lblret.getId() > 0));
      
      Label    lblcall(lblret.getId()-1);
      
      return getCtxLattice(lblcall);
    }

    CtxPropertyStateFactory<context_t>& factory()  { return ctxFactory;  }
    CtxTransfer<context_t>&             transfer() { return ctxTransfer; }
    
    void initializeSolver() ROSE_OVERRIDE
    {
      _solver = new CtxSolver0( _workList,
                                _analyzerDataPreInfo,
                                _analyzerDataPostInfo,
                                SG_DEREF(getInitialElementFactory()),
                                SG_DEREF(getFlow()),
                                SG_DEREF(_transferFunctions),
                                SG_DEREF(getLabeler())
                              );
    }

    // debugging support
    
    /*
    Labeler* getLabeler() const
    {
      return const_cast<CtxAnalysis<context_t>*>(this)->base::getLabeler();
    }
    */
    
    CtxStats latticeStats() ;
    
    
    SgNode& getNode(Label lbl)
    {
      return getLabeler()->getNode(lbl);
    }
    
    
  protected:
    CtxAttribute<CallContext>*
    createDFAstAttribute(Lattice* elem) ROSE_OVERRIDE
    {
      context_lattice_t* lat = dynamic_cast<context_lattice_t*>(elem);

      return new CtxAttribute<CallContext>(sg::deref(lat));
    }

  private:
    CtxPropertyStateFactory<context_t> ctxFactory;
    CtxTransfer<context_t>             ctxTransfer;
};

template <class CallContext>
CtxStats
CtxAnalysis<CallContext>::latticeStats() 
{
  Labeler& labeler = *getLabeler();
  CtxStats res;
  
  for (Label lbl : labeler)
  {
    Lattice& el = SG_DEREF(getPreInfo(lbl));
    
    if (!el.isBot())
    {
      context_lattice_t& lat = dynamic_cast<context_lattice_t&>(el);
      const size_t       sz = lat.size();
      
      if (sz < res.min) res.min = sz; 
      
      if (sz > res.max) 
      {
        res.max    = sz;
        res.maxLbl = lbl;
        res.maxLat = &el;
      } 
      
      res.avg += sz;
      ++res.numNonbot;
    }
    else
    {
      ++res.numBot;
    }
  }
  
  res.avg = res.avg / res.numNonbot;
  return res;
}

} // namespace CodeThorn

#endif /* CTX_ANALYSIS_H */
