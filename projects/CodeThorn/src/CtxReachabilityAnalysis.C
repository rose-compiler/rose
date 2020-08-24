

#include "CtxReachabilityAnalysis.h"

#include "CodeThornLib.h"
#include "CtxSolver0.h"


namespace
{
//
// logging
  
inline
auto logTrace() -> decltype(CodeThorn::logger[Sawyer::Message::TRACE])
{
  return CodeThorn::logger[Sawyer::Message::TRACE];  
}
  
inline
auto logInfo() -> decltype(CodeThorn::logger[Sawyer::Message::INFO])
{
  return CodeThorn::logger[Sawyer::Message::INFO];  
}

inline
auto logWarn() -> decltype(CodeThorn::logger[Sawyer::Message::WARN])
{
  return CodeThorn::logger[Sawyer::Message::WARN];  
}

inline
auto logError() -> decltype(CodeThorn::logger[Sawyer::Message::ERROR])
{
  return CodeThorn::logger[Sawyer::Message::ERROR];  
}
  
inline
auto logFatal() -> decltype(CodeThorn::logger[Sawyer::Message::FATAL])
{
  return CodeThorn::logger[Sawyer::Message::FATAL];  
}

struct IsCalleeCaller
{
  template <class CallContext>
  using MapValue = std::pair<const std::pair<CodeThorn::Label, CallContext>, CodeThorn::Label>;
  
  template <class CallContext>
  bool operator()(const CallContext& retnctx, const MapValue<CallContext>& mapping)
  {
    const CallContext& callctx = mapping.first.second;
    const bool         res = callctx.callerOf(retnctx, lbl);
    
    logTrace() << callctx << " calls " << retnctx << " (" << lbl << ")? " << res
               << std::endl;
    return !res;
  }
  
  CodeThorn::Label lbl;
};

} // anonymous namespace

namespace CodeThorn
{

void
CtxReachabilityAnalysis::initializeSolver()
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


template <class ContextString>
void expandEdges( CtxLabeler<ContextString>& ctxlabeler, Flow& ctxflow, Edge e,
                  const typename CtxLabeler<ContextString>::RelabelingMap::value_type& mapping
                )
{
  Label                ctxsrc = mapping.second;
  const ContextString& ctx    = mapping.first.second;
  Label                ctxtgt = ctxlabeler.getLabel(e.target(), ctx);
  
  ctxflow.insert(Edge(ctxsrc, e.types(), ctxtgt));
}

template <class ContextString>
void expandCallEdges( CtxLabeler<ContextString>& ctxlabeler, Flow& ctxflow, Edge e,
                      const typename CtxLabeler<ContextString>::RelabelingMap::value_type& mapping
                    )
{
  ContextString ctx    = mapping.first.second;
  
  ctx.callInvoke(ctxlabeler.baseLabeler(), mapping.first.first /* original call label */);
  
  Label         ctxtgt = ctxlabeler.getLabel(e.target(), ctx);
  Label         ctxsrc = mapping.second;
  
  ctxflow.insert(Edge(ctxsrc, e.types(), ctxtgt));
}

template <class ContextString>
void expandReturnEdges( CtxLabeler<ContextString>& ctxlabeler, Flow& ctxflow, Edge e,
                        const typename CtxLabeler<ContextString>::RelabelingMap::value_type& mapping
                      )
{
  Label                ctxsrc = mapping.second;
  const ContextString& ctx    = mapping.first.second;
  
  for (const typename CtxLabeler<ContextString>::RelabelingMap::value_type& ctxelem : ctxlabeler.returnLabelRange(e.target(), ctx))
  {
    Label ctxtgt = ctxelem.second;
    
    ctxflow.insert(Edge(ctxsrc, e.types(), ctxtgt));
  }
}


std::pair<CtxLabeler<CtxReachabilityAnalysis::context_t>*, Flow*>  
CtxReachabilityAnalysis::unfold()
{
  using ContextLabeler = CtxLabeler< CtxReachabilityAnalysis::context_t >;
  using RelabelElem    = ContextLabeler::RelabelingMap::value_type;
  using ExpFn          = void (*) (ContextLabeler& ctxlabeler, Flow& ctxflow, Edge e, const RelabelElem&);
  
  Labeler&        labeler    = SG_DEREF(getLabeler());
  ContextLabeler& ctxlabeler = SG_DEREF(new ContextLabeler(labeler));
  Flow&           flow       = SG_DEREF(getFlow());    
  Flow&           ctxflow    = SG_DEREF(new Flow);    
  
  // expand labels for unfolded CFG
  for (Label lbl : labeler)
  {
    const CtxLattice<context_t>& ctxlat = getCtxLattice(lbl);
    
    if (!ctxlat.isBot())
      ctxlabeler.expandLabels(lbl, ctxlat);
  }
  
  logWarn() << "Generated " << ctxlabeler.numberOfLabels() 
            << " new labels from " << labeler.numberOfLabels()
            << std::endl;

  // unfold control flow edges
  for (const ContextLabeler::RelabelingMap::value_type& mapping: ctxlabeler.relabelMap())
  {
    ExpFn       fnExpandEdges = expandEdges;
    
    if (ctxlabeler.isFunctionCallReturnLabel(mapping.second)) fnExpandEdges = expandReturnEdges;
    else if (ctxlabeler.isFunctionCallLabel(mapping.second))  fnExpandEdges = expandCallEdges;

    for (Edge e : flow.outEdges(mapping.first.first /* original label */))
    {
      ROSE_ASSERT(e.source() == mapping.first.first /* original label */);
      
      fnExpandEdges(ctxlabeler, ctxflow, e, mapping);
    }
  }
    
  return std::make_pair(&ctxlabeler, &ctxflow);
}

std::pair<CtxLabeler<FiniteCallString>*, Flow*> 
unfoldCFG(ProgramAbstractionLayer& pla, SgFunctionDefinition& entryPoint)
{
  CtxReachabilityFactory  factory;
  CtxReachabilityTransfer transfer;
  CtxReachabilityAnalysis analysis{factory, transfer};
  
  analysis.initialize(nullptr, &pla);
  analysis.initializeTransferFunctions();
  analysis.initializeGlobalVariables(pla.getRoot());
  analysis.determineExtremalLabels(&entryPoint);
  //~ analysis.setNoTopologicalSort(!USE_TOPOLOGICAL_SORTED_WORKLIST);
  
  analysis.run();
  return analysis.unfold();
}


//
// CtxLabeler template member-function implementations  

template <class ContextString>
void 
CtxLabeler<ContextString>::expandLabels(Label orig, const CtxLattice<ContextString>& lat)
{
  using RelabelKey   = typename RelabelingMap::key_type;
  using RelabelValue = typename RelabelingMap::value_type;
  
  logWarn() << "expand -> " << std::distance(lat.begin(), lat.end());
  
  for (const std::pair<context_t, Lattice*>& el : lat)
  {
    RelabelKey   key{orig, el.first};
    const size_t num = startlbl + allLabels.size() + 1;
    
    registerLabel(original.getProperty(orig));
    auto         res = remapping.insert(RelabelValue(key, Label(num)));
    
    ROSE_ASSERT(res.second);
    allLabels.push_back(res.first);
    
    //~ ROSE_ASSERT(allLabels.size() == original.mappingLabelToLabelProperty.size());
  }
}

template <class ContextString>
Label 
CtxLabeler<ContextString>::getCallForReturnLabel(Label retn) 
{
  using Iterator = typename RelabelingMap::const_iterator;
  
  Iterator retnIter = allLabels.at(retn.getId() - startlbl);      
  Label    origRetn = retnIter->first.first;
  Label    origCall = original.getCallForReturnLabel(origRetn);
  Iterator callIter = remapping.find(std::make_pair(origCall, retnIter->first.second));
  
  ROSE_ASSERT(callIter != remapping.end());
  ROSE_ASSERT(callIter->second >= startlbl);
  return callIter->second;
}


template <class ContextString>
boost::iterator_range<typename CtxLabeler<ContextString>::RelabelingMap::const_iterator>
CtxLabeler<ContextString>::labelRange(Label origLabel) const
{
  using ResultType    = boost::iterator_range<typename CtxLabeler<ContextString>::RelabelingMap::const_iterator>;
  using KeyType       = typename RelabelingMap::key_type;
  using ConstIterator = typename RelabelingMap::const_iterator;
  
  const ContextString emptyctx;                
  KeyType             lbkey{origLabel, emptyctx};
  ConstIterator       lb = remapping.lower_bound(lbkey); 
  
  KeyType             ubkey{Label(origLabel.getId()+1), emptyctx};
  ConstIterator       ub = remapping.lower_bound(ubkey);

  return ResultType(lb, ub);
}
    
        
template <class ContextString>
boost::iterator_range<typename CtxLabeler<ContextString>::RelabelingMap::const_iterator>
CtxLabeler<ContextString>::returnLabelRange(Label origRetnLabel, ContextString retctx) const
{
  using ResultType = boost::iterator_range<typename CtxLabeler<ContextString>::RelabelingMap::const_iterator>;
  using KeyType       = typename RelabelingMap::key_type;
  using ConstIterator = typename RelabelingMap::const_iterator;
  
  Label origCallLabel = original.getCallForReturnLabel(origRetnLabel);
  
  retctx.callReturn(original, origCallLabel);
  
  KeyType       lbkey{origRetnLabel, retctx};
  ConstIterator lb = remapping.lower_bound(lbkey); 
  ConstIterator zz = remapping.end(); 
  ConstIterator ub = std::upper_bound(lb, zz, retctx, IsCalleeCaller{origCallLabel});
    
  return ResultType(lb, ub);
}
  

//
// instantiations for the CtxLabeler<CtxReachabilityAnalysis::context_t>

template 
void 
CtxLabeler<CtxReachabilityAnalysis::context_t>::expandLabels(Label, const CtxLattice<CtxReachabilityAnalysis::context_t>&);

template 
Label 
CtxLabeler<CtxReachabilityAnalysis::context_t>::getCallForReturnLabel(Label); 

template 
boost::iterator_range<CtxLabeler<CtxReachabilityAnalysis::context_t>::RelabelingMap::const_iterator>
CtxLabeler<CtxReachabilityAnalysis::context_t>::labelRange(Label) const;

template 
boost::iterator_range<CtxLabeler<CtxReachabilityAnalysis::context_t>::RelabelingMap::const_iterator>
CtxLabeler<CtxReachabilityAnalysis::context_t>::returnLabelRange(Label, CtxReachabilityAnalysis::context_t) const;

}
