#include "CtxUnfoldedAnalysis.h"

#include <map>
#include <boost/range/iterator_range.hpp>
#include <sageGeneric.h>

#include "CodeThornLib.h"
#include "CtxSolver0.h"
#include "CtxPrioritySolver.h"
#include "PASolver1.h"
#include "SgNodeHelper.h"
#include "DFAnalysisBase.h"
#include "CtxLattice.h"
#include "CtxAnalysis.h"
#include "CtxReachabilityLattice.h"


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

}

namespace CodeThorn
{
  

struct CtxUnfoldFactory : PropertyStateFactory 
{
  CtxReachabilityLattice*
  create() ROSE_OVERRIDE
  {
    return new CtxReachabilityLattice;
  }
};

struct CtxUnfoldTransfer : DFTransferFunctions 
{
  void transfer(Edge e, Lattice& element) ROSE_OVERRIDE
  {
    Labeler& labeler = SG_DEREF(getLabeler());
    Label    src     = e.source();
    Label    tgt     = e.target();
    std::string mark;
    
    if (labeler.isFunctionCallReturnLabel(src))
      mark = "return";
    else if (labeler.isFunctionCallLabel(src))
      mark = "call";
    
    logWarn() << "Reaching edge"
              << "\n  from: " << labeler.getNode(src)->unparseToString() << "/" << src << "* " << mark 
              << "\n    to: " << labeler.getNode(tgt)->unparseToString() << "/" << tgt << "*"
              << std::endl;

    
    dynamic_cast<CtxReachabilityLattice&>(element).setReachable();
  }
  
  void initializeExtremalValue(Lattice& element)
  {
    dynamic_cast<CtxReachabilityLattice&>(element).setReachable();
  }
};


struct CtxUnfoldAnalysis : CtxAnalysis<FiniteCallString> 
{
    typedef FiniteCallString       context_t;
    typedef CtxAnalysis<context_t> base;
    
    CtxUnfoldAnalysis(CtxUnfoldFactory& factory, CtxUnfoldTransfer& transfer)
    : base(factory, transfer)
    {}
    
    ~CtxUnfoldAnalysis()
    {
      for (Lattice* el : _analyzerDataPreInfo) delete el;
      for (Lattice* el : _analyzerDataPostInfo) delete el;
    }
    
    /// unfolds the context information on the labels 
    ///   and creates a new labeler and CFG
    std::pair<CtxLabeler<context_t>*, std::unique_ptr<Flow> > 
    unfold();

    void initializeSolver() ROSE_OVERRIDE;    
};

}

namespace
{
struct IsNotCalleeCaller
{
  template <class CallContext>
  using MapValue = std::pair<const std::pair<CodeThorn::Label, CallContext>, CodeThorn::Label>;
  
  template <class CallContext>
  bool isCalleeCaller(const MapValue<CallContext>& mapping) const
  {
    const CallContext& callctx = mapping.first.second;
    
    const bool         res = (  (lbl == mapping.first.first)
                             && (callctx.callerOf(retnctx, lbl))
                             );
                             
    //~ logTrace() << callctx << " calls " << retnctx << " (" << lbl << ")? " << res
               //~ << std::endl;
    return res;
  }
  
  template <class CallContext>
  bool operator()(const MapValue<CallContext>& mapping) const
  {
    return !isCalleeCaller(mapping);
  }
  
  CodeThorn::Label   lbl;
  const CallContext& retnctx;
};

#if OBSOLETE_CODE
struct IsCalleeCaller
{
  template <class CallContext>
  using MapValue = std::pair<const std::pair<CodeThorn::Label, CallContext>, CodeThorn::Label>;
  
  template <class CallContext>
  bool operator()(const CallContext& retnctx, const MapValue<CallContext>& mapping)
  {
    const CallContext& callctx = mapping.first.second;
    
    const bool         res = (  (lbl < mapping.first.first)
                             || (  (lbl == mapping.first.first)
                                && (callctx.callerOf(retnctx, lbl))
                                )
                             );
    
    
    
    //~ logTrace() << callctx << " calls " << retnctx << " (" << lbl << ")? " << res
               //~ << std::endl;
    return !res;
  }
  
  CodeThorn::Label lbl;
};
#endif /* OBSOLETE_CODE */

} // anonymous namespace

namespace CodeThorn
{

void
CtxUnfoldAnalysis::initializeSolver()
{
  _solver = new CtxPrioritySolver( _workList,
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
  
  ROSE_ASSERT(eq(ctxlabeler.baseLabeler().getProperty(e.source()), ctxlabeler.getProperty(ctxsrc)));
  ROSE_ASSERT(eq(ctxlabeler.baseLabeler().getProperty(e.target()), ctxlabeler.getProperty(ctxtgt)));
  
  logWarn() << "Insert CF edge"
            << "\n  from: " << ctxlabeler.getNode(ctxsrc)->unparseToString() << "/" << ctxsrc << "(" << e.source() << ")"
            << "\n    to: " << ctxlabeler.getNode(ctxtgt)->unparseToString() << "/" << ctxtgt << "(" << e.target() << ")"
            << std::endl;
  
  ctxflow.insert(Edge(ctxsrc, e.types(), ctxtgt));
}

template <class ContextString>
void expandCallEdges( CtxLabeler<ContextString>& ctxlabeler, Flow& ctxflow, Edge e,
                      const typename CtxLabeler<ContextString>::RelabelingMap::value_type& mapping
                    )
{
  ContextString ctx    = mapping.first.second;
    
  ROSE_ASSERT(mapping.first.first == e.source());
  ctx.callInvoke(ctxlabeler.baseLabeler(), mapping.first.first /* original call label */);
  
  Label         ctxtgt = ctxlabeler.getLabel(e.target(), ctx);
  Label         ctxsrc = mapping.second;

  ROSE_ASSERT(eq(ctxlabeler.baseLabeler().getProperty(e.source()), ctxlabeler.getProperty(ctxsrc)));
  ROSE_ASSERT(eq(ctxlabeler.baseLabeler().getProperty(e.target()), ctxlabeler.getProperty(ctxtgt)));

  logWarn() << "Insert CF call edge"
            << "\n  from: " << ctxlabeler.getNode(ctxsrc)->unparseToString() << "/" << ctxsrc << "(" << e.source() << " @" << mapping.first.second << ")"
            << "\n    to: " << ctxlabeler.getNode(ctxtgt)->unparseToString() << "/" << ctxtgt << "(" << e.target() << " @" << ctx << ")"
            << std::endl;
  
  ctxflow.insert(Edge(ctxsrc, e.types(), ctxtgt));
}

template <class ContextString>
void expandReturnEdges( CtxLabeler<ContextString>& ctxlabeler, Flow& ctxflow, Edge e,
                        const typename CtxLabeler<ContextString>::RelabelingMap::value_type& mapping
                      )
{
  Label                ctxsrc = mapping.second;
  const ContextString& ctx    = mapping.first.second;
  
  //~ logTrace() << "e: " << e.source() << " -> " << e.target() 
            //~ << "\n  map: " << mapping.first.first << " / " << ctx << " = " << ctxsrc  
            //~ << std::endl;
  
  for (const typename CtxLabeler<ContextString>::RelabelingMap::value_type& ctxelem : ctxlabeler.returnLabelRange(e.source(), ctx))
  {
    //~ Label ctxtgt = ctxelem.second;
    Label ctxtgt = ctxlabeler.getLabel(e.target(), ctxelem.first.second);

    //~ logTrace() << "Insert CF return edge"
              //~ << "\n  from: " << ctxlabeler.getNode(ctxsrc)->unparseToString() << "/" << ctxsrc << "(" << e.source() << ")"
              //~ << "\n    to: " << ctxlabeler.getNode(ctxtgt)->unparseToString() << "/" << ctxtgt << "(" << e.target() << ")"
              //~ << std::endl;
    
    ROSE_ASSERT(eq(ctxlabeler.baseLabeler().getProperty(e.source()), ctxlabeler.getProperty(ctxsrc)));
    ROSE_ASSERT(eq(ctxlabeler.baseLabeler().getProperty(e.target()), ctxlabeler.getProperty(ctxtgt)));
    ctxflow.insert(Edge(ctxsrc, e.types(), ctxtgt));
  }
}

std::pair<CtxLabeler<CtxUnfoldAnalysis::context_t>*, std::unique_ptr<Flow> >  
CtxUnfoldAnalysis::unfold()
{
  using ContextLabeler = CtxLabeler< CtxUnfoldAnalysis::context_t >;
  using RelabelElem    = ContextLabeler::RelabelingMap::value_type;
  using ExpFn          = void (*) (ContextLabeler& ctxlabeler, Flow& ctxflow, Edge e, const RelabelElem&);
  
  ProgramAbstractionLayer& pal        = SG_DEREF(getProgramAbstractionLayer());    
  Labeler&                 labeler    = SG_DEREF(getLabeler());
  ContextLabeler&          ctxlabeler = SG_DEREF(new ContextLabeler(labeler));
  Flow&                    flow       = SG_DEREF(getFlow());
  std::unique_ptr<Flow>    ctxflow{ new Flow };    
  auto                     globalVarDecls = SgNodeHelper::listOfGlobalVars(pal.getRoot());

  // copy all global labels  
  for (SgVariableDeclaration* var : globalVarDecls) 
    ctxlabeler.copyProperty(labeler.getLabel(var), context_t());

  logInfo() << "Generated " << ctxlabeler.numberOfLabels() 
            << " global variables labels"
            << std::endl;

  // expand all reachable labels for unfolded CFG
  for (Label lbl : labeler)
  {
    const CtxLattice<context_t>& ctxlat = getCtxLattice(lbl);
    
    if (!ctxlat.isBot())
      ctxlabeler.expandLabels(lbl, ctxlat);
  }
  
  logInfo() << "Generated " << ctxlabeler.numberOfLabels() 
            << " new labels from " << labeler.numberOfLabels()
            << std::endl;

  // unfold control flow edges between reachable labels
  for (const ContextLabeler::RelabelingMap::value_type& mapping: ctxlabeler.relabelMap())
  {
    if (!flow.contains(mapping.first.first))
    {
      //~ logTrace() << "NO Flow from " << mapping.first.first << std::endl;
      continue;
    }
    
    ExpFn       fnExpandEdges = expandEdges;
    
    if (ctxlabeler.isFunctionCallReturnLabel(mapping.second)) fnExpandEdges = expandReturnEdges;
    else if (ctxlabeler.isFunctionCallLabel(mapping.second))  fnExpandEdges = expandCallEdges;

    size_t num = 0;
    for (Edge e : flow.outEdges(mapping.first.first /* original label */))
    {
      ROSE_ASSERT(e.source() == mapping.first.first /* original label */);
      
      fnExpandEdges(ctxlabeler, *ctxflow.get(), e, mapping);
      ++num;
    }
    
    if (!num)
    {
      logWarn() << "Zero Edges from " << mapping.first.first << std::endl;
    }
  }
    
  return std::make_pair(&ctxlabeler, std::move(ctxflow));
}

namespace // anonymous
{
void printStats(CtxUnfoldAnalysis& analysis)
{
  typedef CtxLattice<CtxUnfoldAnalysis::context_t> lattice_t;
  
  CtxStats stats = analysis.latticeStats();

  logInfo() << "avg(|callstring|) = "    << stats.avg 
            << "\n min(|callstring|) = " << stats.min 
            << "\n max(|callstring|) = " << stats.max
            << "\n |isBot| = "           << stats.numBot
            << "\n |!isBot| = "          << stats.numNonbot
            << std::endl;
            
  Labeler&     labeler = SG_DEREF(analysis.getLabeler());

/*  
  for (Label botlbl : stats.bots)
  {
    SgNode&      sgbot   = SG_DEREF(labeler.getNode(botlbl));          
    logInfo() << "bot: " << sgbot.unparseToString() 
              << " (" << botlbl << "): "
              << labeler.labelToString(botlbl)
              << std::endl;
  }
*/

  SgNode&      sgnode  = SG_DEREF(labeler.getNode(stats.maxLbl));          
  lattice_t&   maxLat  = dynamic_cast<lattice_t&>(SG_DEREF(stats.maxLat));
  
  logInfo() << "max: " << sgnode.unparseToString() 
            << " (" << stats.maxLbl << "): "
            << labeler.labelToString(stats.maxLbl)
            << std::endl; 
  dbgPrintCtx(logInfo(), *analysis.getLabeler(), maxLat);
}

} // anonymous

std::vector<CtxLattice<CallContext>* >
foldResults( std::vector<Lattice*> results, 
             CtxLabeler<CallContext>& labeler, 
             PropertyStateFactory& factory, 
             LabelTranslation& labelTranslation 
           )
{
  typedef CtxLattice<CallContext> ContextLattice;
  
  std::vector<ContextLattice*> res;
  std::function<Label(Label)>  dict = [&labeler](Label unfolded) -> Label
                                      {
                                        Label zyx = labeler.originalLabel(unfolded);
                                        //~ logWarn() << zyx << " <-t-- " << unfolded << std::endl;
                                        return zyx;
                                      };
  
  for (auto mapping : labeler.relabelMap())
  {
    const size_t     numFolded   = mapping.first.first.getId();
    const size_t     numUnfolded = mapping.second.getId();
    Lattice*         unfoldedLattice = results.at(numUnfolded);
    ROSE_ASSERT(unfoldedLattice);
    
    if (unfoldedLattice->isBot()) continue;
    
    if (numFolded >= res.size())
      res.resize(numFolded+1, nullptr);
    
    ContextLattice*& entry = res.at(numFolded);
      
    if (entry == nullptr)
      entry = new ContextLattice(factory);
      
    Lattice*&        foldedLattice = (*entry)[mapping.first.second];
  
    ROSE_ASSERT(foldedLattice == nullptr);
    
    //~ RDLattice&       testUnfolded = dynamic_cast<RDLattice&>(SG_DEREF(unfoldedLattice));
    //~ const size_t     szUnfolded   =  testUnfolded.size();
    //~ const size_t     botUnfolded  =  testUnfolded.isBot();
    
    foldedLattice = labelTranslation.renameLatticeLabels(std::unique_ptr<Lattice>{ unfoldedLattice }, dict);
    
    //~ RDLattice& testFolded   = dynamic_cast<RDLattice&>(SG_DEREF(foldedLattice));
    
    //~ if (numFolded == 903)
    //~ {    
      //~ logError() << "*** folding at " << numFolded << " / " << mapping.first.second 
                 //~ << "\n  sizes = " << szUnfolded << " <> " << testFolded.size()
                 //~ << "\n  bots = " << botUnfolded << " <> " << testFolded.isBot()
                 //~ << "\n" << testFolded.toString()
                 //~ << std::endl;
    //~ }
  }
  
  return res;
}


std::pair<CtxLabeler<CtxUnfoldAnalysis::context_t>*, std::unique_ptr<Flow> >
unfoldCFG(ProgramAbstractionLayer& pal, SgFunctionDefinition& entryPoint)
{
  CtxUnfoldFactory  factory;
  CtxUnfoldTransfer transfer;
  CtxUnfoldAnalysis analysis{factory, transfer};
  
  analysis.initialize(nullptr, &pal);
  analysis.initializeTransferFunctions();
  
  transfer.setProgramAbstractionLayer(&pal);
  analysis.initializeGlobalVariables(pal.getRoot());
  analysis.determineExtremalLabels(&entryPoint);
  
  //~ analysis.setNoTopologicalSort(!USE_TOPOLOGICAL_SORTED_WORKLIST);
  
  analysis.run();
  
  printStats(analysis);
  
  return analysis.unfold();
  //~ return std::make_pair(nullptr, nullptr);
}

namespace // anonymous
{
  struct SimpleAnalysis : DFAnalysisBase
  {
      typedef DFAnalysisBase base;
      
      SimpleAnalysis(PropertyStateFactory& factory, DFTransferFunctions& transfer)
      : base()
      {
        _transferFunctions = &transfer;
        setInitialElementFactory(&factory);
        setNoTopologicalSort(true);
      }
      
      ~SimpleAnalysis()
      {
        for (Lattice* el : _analyzerDataPreInfo) delete el;
        for (Lattice* el : _analyzerDataPostInfo) delete el;
      }
            
      void initializeSolver() ROSE_OVERRIDE
      {
        PASolver1* theSolver = new PASolver1( _workList,
                                              _analyzerDataPreInfo,
                                              _analyzerDataPostInfo,
                                              SG_DEREF(getInitialElementFactory()),
                                              SG_DEREF(getFlow()),
                                              SG_DEREF(_transferFunctions)
                                            );
        theSolver->setTrace(false);
        theSolver->setLabeler(SG_DEREF(getLabeler()));
        
        _solver = theSolver;
      }
      
      std::vector<Lattice*> 
      preInfoResults() && 
      { 
        return std::move(_analyzerDataPreInfo); 
      }     
      
      LabelSet extremalLabels() const            
      { 
        return _extremalLabels;  
      }
      
      void setGlobalVariablesState(Lattice& lat)
      {
        _globalVariablesState = &lat;
        initializeAnalyzerDataInfo();
      }
  };

} // anonymous


std::vector<CtxLattice<CtxUnfoldAnalysis::context_t>* > 
runUnfoldedAnalysis( ProgramAbstractionLayer& pal, 
                     SgFunctionDefinition& entryPoint, 
                     PropertyStateFactory& factory, 
                     DFTransferFunctions& transfer, 
                     LabelTranslation& labelTranslate,
                     Lattice* initialLat
                   )
{
  typedef CtxUnfoldAnalysis::context_t                       ContextString;
  typedef CtxLabeler<ContextString>                          ContextLabeler;
  typedef std::pair<ContextLabeler*, std::unique_ptr<Flow> > unfolded_t;
  
  unfolded_t                      unfolded{ unfoldCFG(pal, entryPoint)};
  ROSE_ASSERT(unfolded.first);
  
  std::unique_ptr<ContextLabeler> labeler{unfolded.first};
  ProgramAbstractionLayer         unfoldedPal{pal};
  
  unfoldedPal.clearCFAnalyzer(); // required so that unfoldedPal does not free the CFAnalyzer upon destruction
  unfoldedPal.setLabeler(labeler.get());
  unfoldedPal.setForwardFlow(*unfolded.second.get());
  
  SimpleAnalysis analysis{factory, transfer};
  
  analysis.initialize(nullptr, &unfoldedPal);
  analysis.initializeTransferFunctions();
  
  if (!initialLat)
  { 
    initialLat = analysis.initializeGlobalVariables(unfoldedPal.getRoot());
  }
  else
  {
    LabelTranslation::Function dict = [&labeler](Label original) -> Label
                                      {
                                        Label xyz = labeler->getOrMakeLabel(original, ContextString());
                                        //~ logWarn() << original << " --t-> " << xyz << std::endl;
                                        return xyz;
                                      };
                                        
    initialLat = labelTranslate.renameLatticeLabels(std::unique_ptr<Lattice>{ initialLat }, dict);
  }
  
  analysis.setGlobalVariablesState(SG_DEREF(initialLat));
  analysis.determineExtremalLabels(&entryPoint);
  
  //~ logWarn() << "|extremalLabels| = " << analysis.extremalLabels().size() << std::endl;
  
  analysis.run();
  
  // printStats(analysis);
  return foldResults(std::move(analysis).preInfoResults(), *labeler.get(), factory, labelTranslate);
}



//
// CtxLabeler template member-function implementations  

bool eq(LabelProperty lhs, LabelProperty rhs)
{
  const bool res = lhs.toString() == rhs.toString();
  
  if (!res) 
    logError() << " x> " << lhs.toString() << "\n y> " << rhs.toString() 
               << std::endl;
                     
  return res;
}

template <class ContextString>
void 
CtxLabeler<ContextString>::expandLabels(Label orig, const CtxLattice<ContextString>& lat)
{
  //~ logError() << "expand" << orig << " " << lat.size() << std::endl;
  
  for (const std::pair<ContextString, Lattice*>& el : lat)
    copyProperty(orig, el.first);
}

template <class ContextString>
Label 
CtxLabeler<ContextString>::copyProperty(Label orig, const ContextString& ctx)
{
  using RelabelKey   = typename RelabelingMap::key_type;
  using RelabelValue = typename RelabelingMap::value_type;
  
  RelabelKey   key{orig, ctx};
  const size_t num = startlbl + allLabels.size();
  
  registerLabel(original.getProperty(orig));
  auto         res = remapping.insert(RelabelValue(key, Label(num)));
  
  ROSE_ASSERT(res.second);
  allLabels.push_back(res.first);
  
  //~ ROSE_ASSERT(eq(original.getProperty(orig), getProperty(Label(num))));
  return Label(num); 
}


template <class ContextString>
Label 
CtxLabeler<ContextString>::getFunctionCallLabelFromReturnLabel(Label retn) 
{
  using Iterator = typename RelabelingMap::const_iterator;
  
  Iterator retnIter = allLabels.at(retn.getId() - startlbl);      
  Label    origRetn = retnIter->first.first;
  Label    origCall = original.getFunctionCallLabelFromReturnLabel(origRetn);
  Iterator callIter = remapping.find(std::make_pair(origCall, retnIter->first.second));
  
  ROSE_ASSERT(callIter != remapping.end());
  ROSE_ASSERT(callIter->second >= startlbl);
  return callIter->second;
}

template <class ContextString>
Label 
CtxLabeler<ContextString>::getFunctionCallReturnLabelFromCallLabel(Label call) 
{
  using Iterator = typename RelabelingMap::const_iterator;
  
  Iterator callIter = allLabels.at(call.getId() - startlbl);      
  Label    origCall = callIter->first.first;
  Label    origRetn = original.getFunctionCallReturnLabelFromCallLabel(origCall);
  Iterator retnIter = remapping.find(std::make_pair(origRetn, callIter->first.second));
  
  ROSE_ASSERT(retnIter != remapping.end());
  ROSE_ASSERT(retnIter->second >= startlbl);
  return retnIter->second;
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
CtxLabeler<ContextString>::returnLabelRange(Label origRetnLabel, const ContextString& retnctx) const
{
  using ResultType    = boost::iterator_range<typename CtxLabeler<ContextString>::RelabelingMap::const_iterator>;
  using KeyType       = typename RelabelingMap::key_type;
  using ConstIterator = typename RelabelingMap::const_iterator;
  
  if (!retnctx.isValidReturn(original, origRetnLabel))
  {
    //~ logError() << "Invalid Return" << std::endl;
    return ResultType(remapping.end(), remapping.end());
  }
  
  ROSE_ASSERT(original.isFunctionCallReturnLabel(origRetnLabel));
  Label         origCallLabel = original.getFunctionCallLabelFromReturnLabel(origRetnLabel);
  ContextString retnctxLowerBound{retnctx};

  ROSE_ASSERT(original.isFunctionCallLabel(origCallLabel));
  retnctxLowerBound.callReturn(original, origRetnLabel);
  
  KeyType       lbkey{origCallLabel, retnctxLowerBound};
  ConstIterator lb = remapping.lower_bound(lbkey); 
  ConstIterator zz = remapping.end(); 
  
  //~ if (lb != zz)
    //~ logWarn() << "  lb = " << lb->first.first << " ~ " << lb->first.second 
              //~ << " @" << retnctx
              //~ << std::endl;
  //~ else
    //~ logWarn() << "  lb = zz" << std::endl;

  //~ ConstIterator ub = std::upper_bound(lb, zz, retctx, IsCalleeCaller{origCallLabel});
  ConstIterator ub = std::find_if(lb, zz, IsNotCalleeCaller{origCallLabel, retnctx});
    
  //~ if (ub != zz)
    //~ logWarn() << "  ub = " << ub->first.first << " ~ " << ub->first.second << std::endl;
  //~ else
    //~ logWarn() << "  ub = zz" << std::endl;
    
  //~ logInfo() << "Valid Returns: #" << std::distance(lb, ub) << std::endl;  
  return ResultType(lb, ub);
}
  

//
// instantiations for the CtxLabeler<CtxUnfoldAnalysis::context_t>

template 
void 
CtxLabeler<CtxUnfoldAnalysis::context_t>::expandLabels(Label, const CtxLattice<CtxUnfoldAnalysis::context_t>&);

template 
Label 
CtxLabeler<CtxUnfoldAnalysis::context_t>::getFunctionCallLabelFromReturnLabel(Label); 

template 
Label 
CtxLabeler<CtxUnfoldAnalysis::context_t>::getFunctionCallReturnLabelFromCallLabel(Label); 

template 
boost::iterator_range<CtxLabeler<CtxUnfoldAnalysis::context_t>::RelabelingMap::const_iterator>
CtxLabeler<CtxUnfoldAnalysis::context_t>::labelRange(Label) const;

template 
boost::iterator_range<CtxLabeler<CtxUnfoldAnalysis::context_t>::RelabelingMap::const_iterator>
CtxLabeler<CtxUnfoldAnalysis::context_t>::returnLabelRange(Label, const CtxUnfoldAnalysis::context_t&) const;

}
