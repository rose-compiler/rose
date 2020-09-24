
#ifndef _CTXUNFOLDEDANALYSIS_H
#define _CTXUNFOLDEDANALYSIS_H 1

#include <sage3basic.h>

#include <memory>

#include "DFTransferFunctions.h"
#include "PropertyStateFactory.h"
#include "ProgramAbstractionLayer.h"
#include "CtxCallStrings.h"


namespace CodeThorn
{
  
using CallContext = FiniteCallString; 


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
    Label copyProperty(Label orig, const ContextString& ctx);
    
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
    
    Label getFunctionCallLabelFromReturnLabel(Label retn) ROSE_OVERRIDE;
    Label getFunctionCallReturnLabelFromCallLabel(Label retn) ROSE_OVERRIDE;
    
    const typename RelabelingMap::key_type& 
    originalData(Label lbl) const
    {
      return allLabels.at(lbl.getId() - startlbl)->first;
    } 
    
    context_t 
    originalContext(Label lbl) const
    {
      return originalData(lbl).second;
    }
    
    Label 
    originalLabel(Label lbl) const
    {
      return originalData(lbl).first;
    }
    
    Label 
    getLabel(Label origLabel, const context_t& ctx)
    {
      typename RelabelingMap::key_type       key{origLabel, ctx};
      typename RelabelingMap::const_iterator pos = remapping.find(key);

      ROSE_ASSERT(pos != remapping.end());      
      return pos->second; 
    }
    
    Label 
    getOrMakeLabel(Label origLabel, const context_t& ctx)
    {
      typename RelabelingMap::key_type       key{origLabel, ctx};
      typename RelabelingMap::const_iterator pos = remapping.find(key);

      if (pos != remapping.end()) 
        return pos->second;      

      std::cerr << "make new label" << std::endl; 
      return copyProperty(origLabel, ctx);
    }
    
  
    // returns a 
    boost::iterator_range<typename RelabelingMap::const_iterator>
    labelRange(Label origLabel) const;
    
    boost::iterator_range<typename RelabelingMap::const_iterator>
    returnLabelRange(Label origLabel, const ContextString& calleeContext) const;
    
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


/// call back object to assist in label translation from unfolded to folded lattice elements
struct LabelTranslation
{
  typedef std::function<Label(Label)> Function;
  
  /// renames the labels in @ref original using the mapping function @ref renameLabel
  /// \param  src         the src lattice 
  /// \param  renameLabel the label translation function
  /// \return the translated lattice (ownership is passed to the analysis)
  /// \note - the ownership of the unfolded lattice is passed to the renaming function. The analysis will not
  ///         maintain a pointer to that lattice.
  ///       - the function is invoked at the beginning and end of runUnfoldedAnalysis. At the beginning
  ///         the initial lattice is translated to the new labeling scheme; at the end the new labeling
  ///         scheme is translated back. 
  virtual 
  Lattice* 
  renameLatticeLabels(std::unique_ptr<Lattice> unfolded, Function& renameLabel) = 0;
};

/// uses the reachability analysis to unfold the ICFG and returns
///   a new labeler and new ICFG
std::pair<CtxLabeler<CallContext>*, std::unique_ptr<Flow> > 
unfoldCFG(ProgramAbstractionLayer& pla, SgFunctionDefinition& entryPoint);

/// unfolds the ICFG and runs @ref analysis on it. At the end the results are folded
///   into a CtxLattice returned to the caller.
/// \param pal               the program abstraction layer
/// \param entryPoint        the entry function of the analysis
/// \param factory           the lattice factory
/// \param transfer          the transfer functions
/// \param labelMapping      remaps a lattice between two labelers
/// \param initialLatticeOpt optional argument for the initial lattice 
///                          (if passed, the analysis assumes ownership) 
std::vector<CtxLattice<CallContext>* > 
runUnfoldedAnalysis( ProgramAbstractionLayer& pal, 
                     SgFunctionDefinition& entryPoint, 
                     PropertyStateFactory& factory, 
                     DFTransferFunctions& transfer,
                     LabelTranslation& refolding,
                     Lattice* initialLatticeOpt = nullptr
                   );

} // namespace CodeThorn

#endif /* _CTXUNFOLDEDANALYSIS_H */

