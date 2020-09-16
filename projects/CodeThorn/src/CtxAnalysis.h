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
  size_t             min        = size_t(-1);
  size_t             max        = 0;
  size_t             numNonbot  = 0;
  size_t             numBot     = 0;

  Label              maxLbl   = Label();
  Lattice*           maxLat   = nullptr; 
  
  std::vector<Label> bots;
  
  double             avg        = 0;
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
    : base(), ctxFactory(compFactory), ctxTransfer(compTransfer, *this), botLattice(compFactory)
    {
      _transferFunctions = &ctxTransfer;
      _transferFunctions->setInitialElementFactory(&ctxFactory);
    }

    CtxAnalysis(PropertyStateFactory& compFactory, DFTransferFunctions& compTransfer, const context_lattice_t& init)
    : base(), ctxFactory(compFactory), ctxTransfer(compTransfer, *this, init), botLattice(compFactory)
    {
      _transferFunctions = &ctxTransfer;
      _transferFunctions->setInitialElementFactory(&ctxFactory);
    }

    const context_lattice_t&
    getCtxLattice(Label lbl)
    {
      ROSE_ASSERT(botLattice.size() == 0 && botLattice.isBot());
      
      const Lattice* lat = getPreInfo(lbl);
      const Lattice* res = lat;
      
      if (!res)
      { 
        res = &botLattice;
        std::cerr << "sub bot lattice " << res << std::endl;
      }
      
      ROSE_ASSERT(lat != nullptr || res->isBot());
      return dynamic_cast<const context_lattice_t&>(*res);
    }

    /// retrieves the lattice from the call site
    const CtxLattice<CallContext>&
    getCallSiteLattice(Label lblretn)
    {
      Labeler& labeler = *getLabeler();
      ROSE_ASSERT(labeler.isFunctionCallReturnLabel(lblretn));
      
      return getCtxLattice(labeler.getFunctionCallLabelFromReturnLabel(lblretn));
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
    
    void setProgramAbstractionLayer(ProgramAbstractionLayer& pal)
    {
      _programAbstractionLayer = &pal;
      _programAbstractionLayerOwner = false;
    }

    // debugging support
    
    /*
    Labeler* getLabeler() const
    {
      return const_cast<CtxAnalysis<context_t>*>(this)->base::getLabeler();
    }
    */
    
    CtxStats latticeStats();
    
    
    SgNode& getNode(Label lbl) const
    {
      // MS 7/24/20: added required dereference op
      return *getLabeler()->getNode(lbl);
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
    const CtxLattice<CallContext>      botLattice;
};

template <class CallContext>
CtxStats
CtxAnalysis<CallContext>::latticeStats() 
{
  ProgramAbstractionLayer& pal = SG_DEREF(getProgramAbstractionLayer());
  Labeler& labeler             = SG_DEREF(pal.getLabeler());
  CtxStats res;
  
  for (Label lbl : labeler)
  {
    Lattice* el = getPreInfo(lbl);
    
    //~ std::cerr << el << std::endl;
    //~ if (el) std::cerr << &typeid(*el) << std::endl;
    
    if (el && !el->isBot())
    {
      context_lattice_t& lat = dynamic_cast<context_lattice_t&>(*el);
      const size_t       sz = lat.size();
      
      if (sz < res.min) 
      {
        res.min = sz;
      } 
      
      if (sz > res.max) 
      {
        res.max    = sz;
        res.maxLbl = lbl;
        res.maxLat = el;
      } 
      
      res.avg += sz;
      ++res.numNonbot;
    }
    else
    {
      res.bots.push_back(lbl);
      ++res.numBot;
    }
  }
  
  res.avg = res.avg / res.numNonbot;
  return res;
}

} // namespace CodeThorn

#endif /* CTX_ANALYSIS_H */
