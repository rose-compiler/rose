
#ifndef _CTXREACHABILITYANALYSIS_H
#define _CTXREACHABILITYANALYSIS_H 1

#include <sage3basic.h>
#include <sageGeneric.h>
#include <map>

#include <boost/range/iterator_range.hpp>

#include "DFAnalysisBase.h"
#include "ProgramAbstractionLayer.h"
#include "CtxLattice.h"
#include "CtxAnalysis.h"
#include "CtxCallStrings.h"
#include "CtxReachabilityTransfer.h"
#include "CtxReachabilityFactory.h"
//~ #include "CtxReachabilityLattice.h"

namespace CodeThorn
{
  
template <class CallContextComparator>
struct CtxLabelComparator
{
  template <class ContextString>
  bool operator()(const std::pair<Label, ContextString>& lhs, const std::pair<Label, ContextString>& rhs) const
  {
    if (lhs.first < rhs.first) return true;
    if (rhs.first < lhs.first) return false;
    
    return CallContextComparator()(lhs.second, rhs.second); 
  }
};


template <class ContextString>
struct CtxLabeler : Labeler
{
    typedef ContextString                                                   context_t;
    typedef std::pair<Label, context_t>                                     ContextedLabel;               
    typedef CtxLabelComparator<typename context_t::comparator>              RelabelComparator;
    
    // the relabeling map uses the same ordering as the contexts in the lattice to use
    //   similar call return logic.
    typedef std::map<std::pair<Label, context_t>, Label, RelabelComparator> RelabelingMap;
    typedef std::vector<typename RelabelingMap::const_iterator>             RelabeledSequence;
  
    explicit
    CtxLabeler(Labeler& orig)  
    : Labeler(), original(orig), remapping(), startlbl(0)
      // startlbl(orig.numberOfLabels())
    {
      // \todo initialize startlbl with orig.numberOfLabels() 
      // we would like to use a different start offset for this labeler
      //   so that we can distinguish different label sets.
      //   however, that requires to override many access functions
      //   to adjust for a non-zero offset. Thus this is left out for now.
    }
  
    void expandLabels(Label orig, const CtxLattice<context_t>& lat);
    
    bool areCallAndReturnLabels(Label call, Label retn) ROSE_OVERRIDE
    {
      typename RelabelingMap::const_iterator callIter = allLabels.at(call.getId() - startlbl);
      typename RelabelingMap::const_iterator retnIter = allLabels.at(retn.getId() - startlbl);
      
      // two "ctxlabels" are call and return label, if they have the same context
      //   and if their base labels are call and return labels.
      return (  (callIter->first.second == retnIter->first.second)
             && original.areCallAndReturnLabels(callIter->first.first, retnIter->first.first)
             );
    }
    
    Label getCallForReturnLabel(Label retn) ROSE_OVERRIDE;
    
    context_t context(Label lbl) const
    {
      return allLabels.at(lbl.getId() - startlbl)->first.second;
    }
    
    Label getLabel(Label origLabel, const ContextString& ctx)
    {
      typename RelabelingMap::key_type       key{origLabel, ctx};
      typename RelabelingMap::const_iterator pos = remapping.find(key);
      ROSE_ASSERT(pos != remapping.end());
      
      return pos->second; 
    }
    
    // returns a 
    boost::iterator_range<typename RelabelingMap::const_iterator>
    labelRange(Label origLabel) const;
    
    boost::iterator_range<typename RelabelingMap::const_iterator>
    returnLabelRange(Label origLabel, ContextString calleeContext) const;
    
    Labeler& baseLabeler() const
    {
      return original;
    }

    const RelabelingMap& relabelMap() const
    {
      return remapping;
    }
    
  
  private:
    Labeler&          original;
    RelabelingMap     remapping;
    RelabeledSequence allLabels;
    const size_t      startlbl;  ///< first label (\todo should be original.last+1)
};

struct CtxReachabilityAnalysis : CtxAnalysis<FiniteCallString> 
{
    typedef FiniteCallString       context_t;
    typedef CtxAnalysis<context_t> base;
    
    CtxReachabilityAnalysis(CtxReachabilityFactory& reachabilityFactory, CtxReachabilityTransfer& reachabilityTransfer)
    : base(reachabilityFactory, reachabilityTransfer)
    {}
    
    /// unfolds the context information on the labels 
    ///   and creates a new labeler and CFG
    std::pair<CtxLabeler<context_t>*, Flow*> 
    unfold();

    void initializeSolver() ROSE_OVERRIDE;    
};


/// uses the reachability analysis to unfold the ICFG and returns
///   a new labeler and new ICFG
std::pair<CtxLabeler<FiniteCallString>*, Flow*> 
unfoldCFG(ProgramAbstractionLayer& pla, SgFunctionDefinition& entryPoint);


} // namespace CodeThorn

#endif /* _CTXREACHABILITYANALYSIS_H */

