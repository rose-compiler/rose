#ifndef DFSOLVER1_C
#define DFSOLVER1_C

#include "sage3basic.h"

#include <memory>
#include <queue>
#include <limits>

#include "CtxPrioritySolver.h"
#include "CtxLattice.h"
#include "CodeThornLib.h"
#include "TimeMeasurement.h"
#include "TopologicalSort.h"

namespace 
{
  namespace ct = CodeThorn;
  
  //
  // dummy noop output stream
  struct NoStream {};
  
  template <class T>
  inline
  const NoStream& 
  operator<<(const NoStream& nos, const T&) { return nos; }
  
  inline
  const NoStream&
  operator<<(const NoStream& nos, std::ostream& (*)(std::ostream&))
  {
    return nos;
  }
  
  struct LazyToString
  {
    explicit
    LazyToString(ct::Lattice* lat)
    : l(lat)
    {}
    
    explicit
    LazyToString(std::unique_ptr<ct::Lattice>& lat)
    : LazyToString(lat.get())
    {}
    
    explicit
    LazyToString(ct::Lattice& lat)
    : LazyToString(&lat)
    {}
    
    std::string toString() { return l->toString(); }
    
    ct::Lattice* l;
  };
  
  inline
  std::ostream& 
  operator<<(std::ostream& os, LazyToString lat) { return os << lat.toString(); }

  struct LazyEdge
  {
    explicit
    LazyEdge(ct::Edge e, ct::Labeler& l)
    : edge(e), labeler(&l)
    {}
    
    std::string prn(ct::Label lbl) const
    {
      return SG_DEREF(labeler->getNode(lbl)).unparseToString();
    }
    
    std::string source() const { return prn(edge.source()); }
    std::string target() const { return prn(edge.target()); }
    
    ct::Edge     edge;
    ct::Labeler* labeler;
  };
  
  inline
  std::ostream& 
  operator<<(std::ostream& os, const LazyEdge& e) 
  { 
    return os << e.source() << " -> " << e.target(); 
  }
  
  
  // auxiliary wrapper for printing Sg_File_Info objects 
  struct SrcLoc
  {
    explicit
    SrcLoc(SgLocatedNode& n)
    : info(n.get_file_info())
    {}
    
    Sg_File_Info* info;
  };
  
  inline
  std::ostream& operator<<(std::ostream& os, SrcLoc el)
  {
    return os << el.info->get_filenameString() 
              << "@" << el.info->get_line() << ":" << el.info->get_col();
  } 
    
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
  
  inline 
  //~ auto logDbg() -> decltype(logInfo()) { return logInfo(); }
  //~ auto logDbg() -> decltype(logTrace()) { return logTrace(); }
  NoStream logDbg() { return NoStream(); }
}

namespace // auxiliary local functions
{
  template <class Map, class Key>
  typename Map::iterator
  iteratorAt(Map& map, const Key& key)
  {
    typename Map::iterator pos = map.find(key); 
    ROSE_ASSERT(pos != map.end() && pos->second);
    
    return pos;
  }
  
  namespace ct = CodeThorn;
  
  struct IsCalleeCaller
  {
    bool operator()(const ct::FiniteCallString& retctx, const std::pair<const ct::FiniteCallString, ct::Lattice*>& callctx) const
    {
      const bool res = callctx.first.callerOf(retctx, lbl);
      
      logDbg() << callctx.first << " calls " << retctx << " (" << lbl << ")? " << res
               << std::endl;
      return !res;
    }
    
    ct::Label lbl;
  };
  
  constexpr
  bool isInfinite(const ct::FiniteCallString&)   { return false; }

  constexpr
  bool isInfinite(const ct::InfiniteCallString&) { return true; }  
}

namespace CodeThorn
{

CtxPrioritySolver::CtxPrioritySolver( InitialWorklist& workList,
                                      LatticeContainer& analyzerDataPreInfo,
                                      LatticeContainer& analyzerDataPostInfo,
                                      PropertyStateFactory& initialElementFactory,
                                      Flow& flow,
                                      DFTransferFunctions& transferFunctions,
                                      Labeler& thelabeler
                                    )
: _workList(workList),
  _analyzerDataPreInfo(analyzerDataPreInfo),
  _analyzerDataPostInfo(analyzerDataPostInfo),
  _initialElementFactory(dynamic_cast<StateFactory&>(initialElementFactory)),
  _flow(flow),
  _transferFunctions(dynamic_cast<TransferFunction&>(transferFunctions)),
  _labeler(thelabeler)
  //~ _trace(false)
{}

void
CtxPrioritySolver::computeCombinedPreInfo(Label lab, Lattice& info) 
{
  if (!_flow.contains(lab)) 
  {
    // schroder3 (2016-07-07): If the label does not exist in the CFG, then
    //  it does not have predecessors and the given pre-info therefore does
    //  not change. This check is necessary if Flow::pred(Label) uses the
    //  Sawyer graph as underlying datastructure because Flow::pred then
    //  expects that the given label exists in the CFG.
    return;
  }
  
  for (Label predlab : _flow.pred(lab)) 
  {
    std::unique_ptr<Lattice> predInfo{cloneLattice(_initialElementFactory, preInfoLattice(predlab))};
    
    computePostInfo(predlab, *predInfo);
    info.combine(*predInfo);
  }
}

void
CtxPrioritySolver::computePostInfo(Label lab,Lattice& info) 
{
  _transferFunctions.transfer(lab, info);
}

CtxLatticeRange<CtxPrioritySolver::ContextString>
CtxPrioritySolver::mappedCtxRange(Label lab, CtxLatticeRange<ContextString>::iterator ctxpos)
{
  typedef CtxLatticeRange<ContextString>::iterator Iterator;
  typedef CtxLatticeRange<ContextString>           ResultType;
  
  ROSE_ASSERT(labeler().isFunctionCallReturnLabel(lab));
  
  // if it is an infinite context or not a function call,
  //   the mapping is trivially 1:1.
  if (isInfinite(ctxpos->first))
  {
    // \todo this requires more thought
    ROSE_ASSERT(false);
    return ResultType(ctxpos, std::next(ctxpos));
  }
  
  // if the call context does not match the call label associated with the return
  //   return an empty range. Nothing needs to be propagated.
  if (!ctxpos->first.isValidReturn(labeler(), lab))
    return ResultType(ctxpos, ctxpos);
  
  logDbg() << labeler().getNode(lab)->unparseToString() 
           << std::endl;
      
  Label                      callLab = labeler().getFunctionCallLabelFromReturnLabel(lab); 
  CtxLattice<ContextString>& pre     = preInfoLattice(callLab);
  ContextString              retctx  = ctxpos->first;
  
  //~ logDbg() << "retctx: " << retctx << " / " << lab << std::endl;
  //~ logDbg() << "pre: " << pre.toString() << std::endl;
  retctx.callReturn(labeler(), lab);
  
  // find lower bound using the return context 
  Iterator aa = pre.lower_bound(retctx);
  Iterator zz = pre.end();
  
  // iterate to find last element with overlapping call context
  zz = std::upper_bound(aa, zz, ctxpos->first, IsCalleeCaller{callLab});
  
  //~ logDbg() << "found " << std::distance(aa, zz)
           //~ << " return contexts in pre lattice of size " << pre.size() 
           //~ << std::endl;  
  //~ logDbg() << "retctx: " << retctx << std::endl;
  //~ logDbg() << "calllat: " << pre.toString() << std::endl;           
  return ResultType(aa, zz);
}

    
CtxLattice<CtxPrioritySolver::ContextString>&
CtxPrioritySolver::preInfoLattice(Label lab)
{
  return dynamic_cast<CtxLattice<ContextString>&>(*_analyzerDataPreInfo[lab.getId()]);
}
    
    
Lattice&
CtxPrioritySolver::preInfoLattice(Label lab, const ContextString& ctx)
{
  CtxLattice<ContextString>& all = preInfoLattice(lab);
  Lattice*&                  sub = all[ctx];
  
  if (sub == NULL)
    sub = _initialElementFactory.componentFactory().create();
  
  return *sub; 
}



struct PriorityComparator
{
  template <class T, class PriorityT>
  bool
  operator()(const std::pair<T, PriorityT>& lhs, const std::pair<T, PriorityT>& rhs) const
  {
    return lhs.second > rhs.second;
  } 
};

namespace
{
  inline
  Edge edge(Edge e) { return e; }

  inline
  Edge edge(const std::pair<Edge, CtxPrioritySolver::ContextString>& el) { return el.first; }
  
  inline
  int edgePriorityIdx(const Edge& edge) { return edge.target().getId(); }  

  inline
  int edgePriorityIdx(const std::pair<Edge, CtxPrioritySolver::ContextString>& el) { return edgePriorityIdx(edge(el)); }  
}    
    

template <class T>
struct PriorityWorklist 
{
    typedef T                                                                      value_type;  
    typedef std::pair<value_type, int>                                             element_type;
    typedef std::vector<element_type>                                              queue_base_type;
    typedef std::priority_queue<element_type, queue_base_type, PriorityComparator> queue_type;
    typedef std::vector<int>                                                       priority_map;
    
    PriorityWorklist(priority_map&& priority_mapping, Labeler& lbler)
    : priorities(std::move(priority_mapping)), container(), labeler(&lbler)
    {}
    
    PriorityWorklist(PriorityWorklist&&)                 = default;
    PriorityWorklist& operator=(PriorityWorklist&&)      = default;
    
    size_t size()    const { return container.size(); }
    bool   isEmpty() const { return size() == 0; }
    
    value_type take() 
    { 
      element_type elem = std::move(container.top());

      logDbg() << "take " << LazyEdge(edge(elem.first), *labeler) 
               << ": " << elem.second
               << std::endl;  
      
      container.pop();
      return std::move(elem.first);
    }
    
    void add(value_type&& elem) 
    { 
      const int priority = priorities.at(edgePriorityIdx(elem));

      logDbg() << "put " << LazyEdge(edge(elem), *labeler) 
               << ": " << edge(elem).target().getId() << " @ " << priority
               << std::endl;  
      
      container.emplace(std::move(elem), priority); 
    }
    
    void add(const value_type& elem) 
    { 
      const int priority = priorities.at(edgePriorityIdx(elem));

      logDbg() << "put " << LazyEdge(edge(elem), *labeler) 
               << ": " << edge(elem).target().getId() << " @ " << priority
               << std::endl;  
      
      container.push(element_type(elem, priority)); 
    }
  
  private:  
    const priority_map priorities;
    queue_type         container;
    Labeler*           labeler;       
    
    PriorityWorklist()                                   = delete;
    PriorityWorklist(const PriorityWorklist&)            = delete;
    PriorityWorklist& operator=(const PriorityWorklist&) = delete;
};


template <class T>
PriorityWorklist<T>
createWorklist(CtxPrioritySolver::InitialWorklist& wkl, Labeler& l, Flow& flow)
{
  typedef typename PriorityWorklist<T>::priority_map PriorityMap;
  
  static constexpr int LOWEST_PRIORITY = std::numeric_limits<int>::max();
  
  const Label      origin = wkl.examine().source();
  
  flow.setStartLabel(origin);
  
  TopologicalSort  topSort(l, flow);
  //~ PriorityWorklist res(topSort.labelToPriorityMap(), l);
  PriorityMap      priority_mapping;
  int              priority = 0;
    
  for (auto labelPriority : topSort.labelToPriorityMap())
  {
    const size_t lblid = labelPriority.first.getId();
    
    if (lblid >= priority_mapping.size())
      priority_mapping.resize(lblid+1, LOWEST_PRIORITY);
    
    int& entry = priority_mapping.at(lblid);     
    
    if (entry > priority) entry = ++priority;
  }
  
  return PriorityWorklist<T>{ std::move(priority_mapping), l };  
}

template <class W>
void
CtxPrioritySolver::propagate(const ContextString& tgtctx, Lattice& state, Label tgt, W& wkl)
{
  typedef typename W::value_type WorkListElem;
  
  Lattice&   tgtstate = preInfoLattice(tgt, tgtctx);
  const bool subsumed = state.approximatedBy(tgtstate);
  
  if (subsumed) 
  {
    logDbg() << "mapping not necessary (already approximated): " << tgt.getId()
             << std::endl;
    
    return;
  }  
  
  logDbg() << "mapping transfer result to: " << tgt << " / " << tgtctx << ": " << LazyToString(tgtstate)
           << std::endl;

  tgtstate.combine(state);

  logDbg() << "new df value : " << tgt << " / " << tgtctx << ": " << LazyToString(tgtstate)
           << std::endl;
           
  const size_t oldsz = wkl.size();

  for (Edge e : _flow.outEdges(tgt)) 
  {
    wkl.add(WorkListElem(e, tgtctx));
  }

  logDbg() << "added : " << (wkl.size() - oldsz) << " edges."
           << std::endl;
}

template <class W>
void
CtxPrioritySolver::activateReturnNode(const ContextString& ctx, Label callLbl, W& wkl)
{
  typedef CtxLattice<ContextString>      context_lattice_t;
  typedef std::pair<Edge, ContextString> WorkListElem;

  Label              retnLbl = labeler().getFunctionCallReturnLabelFromCallLabel(callLbl);
  context_lattice_t& ctxlat = preInfoLattice(retnLbl);
  
  if (ctxlat.find(ctx) != ctxlat.end())
  {
    for (Edge e : _flow.outEdges(retnLbl)) 
    {
      wkl.add(WorkListElem(e, ctx));
    }
  }
}


namespace
{

template <class W, class Fn>
void
addInitialWorklistItems(W& wkl, CtxPrioritySolver::InitialWorklist& orig, Fn fn, bool singleSource = false)
{
  bool        done = orig.isEmpty();                        
  const Label src  = done ? Label() : orig.examine().source();
  
  while (!done)
  {
    fn(wkl, orig.take());
    
    done = orig.isEmpty() || (singleSource && (src != orig.examine().source()));
  } 

  orig.clear();
}    

}    
    
// runs until worklist is empty
void
CtxPrioritySolver::runSolver() 
{
  typedef CtxLattice<ContextString>                context_lattice_t;
  typedef std::pair<Edge, ContextString>           WorkListElem;
  typedef PriorityWorklist<WorkListElem>           CtxPriorityWorklist;
  typedef CtxLatticeRange<ContextString>::iterator Iterator;
  
  constexpr uint64_t REPORT_INTERVAL = (1 << 12);
  
  TimeMeasurement     solverTimer;
  uint64_t            nodeCounter = 0;
  double              splitTime   = 0;
  CtxPriorityWorklist worklist    = createWorklist<WorkListElem>(_workList, _labeler, _flow);
  
  addInitialWorklistItems( worklist, _workList, 
                           [this](CtxPriorityWorklist& w, Edge e) -> void
                           {
                             const Lattice*           lat = _analyzerDataPreInfo.at(e.source().getId());
                             const context_lattice_t& ctxlat = dynamic_cast<const context_lattice_t&>(*lat); 
    
                             for (const context_lattice_t::value_type& el : ctxlat)
                               w.add(WorkListElem(e, el.first));    
                           },
                           true
                         );
  
  logInfo() << "CtxPrioritySolver started" << std::endl;
  
  if (worklist.isEmpty())
    logWarn() << "initial worklist size is empty." << std::endl;
    
  solverTimer.start();
  
  while (!worklist.isEmpty()) 
  {
    WorkListElem             el   = worklist.take();
    Edge                     edge = el.first;
    const ContextString&     ctx  = el.second;
    Label                    lab0 = edge.source();
    Label                    lab1 = edge.target();

    //~ logDbg() << "computing edge " << LazyEdge(edge, labeler()) << std::endl;
    
    context_lattice_t&       ctxlat = preInfoLattice(lab0);
    const Iterator           preIt  = iteratorAt(ctxlat, ctx);
    std::unique_ptr<Lattice> info{cloneLattice(_initialElementFactory.componentFactory(), *preIt->second)};
    
    if (!info->isBot()) 
    {
      logDbg() << "computing transfer function: " << lab0 << " / " << ctx << ": " << LazyToString(info)
               << std::endl;
      
      _transferFunctions.componentTransfer().transfer(edge, *info);
      
      logDbg() << "transfer function result: " << lab0 << " / " << ctx << ": " << LazyToString(info)
               << std::endl;
               
      // Propagate the state to the respective context in lab1 
      //   and add the outflowing edges to the worklist.
      // Three cases are distinguished: "normal" edge, a call edge, 
      //   and return edge.
      if (labeler().isFunctionCallLabel(lab0))
      {
        // create a copy for modification
        ContextString callctx(ctx);
        
        //~ callctx.callLosesPrecision(preIt); 
        
        callctx.callInvoke(labeler(), lab0);
        propagate(callctx, *info, lab1, worklist);
        activateReturnNode(callctx, lab0, worklist);
      }
      else if (labeler().isFunctionCallReturnLabel(lab0))
      {
        // update the context to reflect function return (could be 1:n mapping)
        
        for (const context_lattice_t::context_map::value_type& ctxelem : mappedCtxRange(lab0, preIt))
          propagate(ctxelem.first, *info, lab1, worklist);
      }
      else
      {
        // unchanged context (1:1 mapping)
        
        propagate(ctx, *info, lab1, worklist);      
      }
    }
    else 
    {
      logDbg() << "computing transfer function: " << lab0 << ": bot"
               << "\n  => cancel (because of bot)"
               << std::endl;
      // do nothing (non-reachable code)
    } 
    
    if (((++nodeCounter) % REPORT_INTERVAL) == 0)
    {
      const double oldSplitTime = splitTime;
       
      splitTime = solverTimer.getTimeDurationAndKeepRunning().seconds();
      
      logInfo() << static_cast<size_t>(REPORT_INTERVAL / (splitTime-oldSplitTime)) << " nodes/s - "
                << nodeCounter << '/' << splitTime << '.' 
                << std::endl; 
    }
  }
  
  TimeDuration endTime = solverTimer.getTimeDurationAndStop();
  
  logInfo() << "CtxPrioritySolver finished after " << static_cast<size_t>(endTime.milliSeconds()) << "ms." 
            << std::endl
            << "  " << nodeCounter << " nodes analyzed (" << static_cast<size_t>(nodeCounter / endTime.seconds())
            << " nodes/s)" 
            << std::endl; 
}


//
// ---

SeqPrioritySolver::SeqPrioritySolver( InitialWorklist& workList,
                                      LatticeContainer& analyzerDataPreInfo,
                                      LatticeContainer& analyzerDataPostInfo,
                                      PropertyStateFactory& initialElementFactory,
                                      Flow& flow,
                                      DFTransferFunctions& transferFunctions,
                                      Labeler& thelabeler
                                    )
: _workList(workList),
  _analyzerDataPreInfo(analyzerDataPreInfo),
  _analyzerDataPostInfo(analyzerDataPostInfo),
  _initialElementFactory(dynamic_cast<StateFactory&>(initialElementFactory)),
  _flow(flow),
  _transferFunctions(dynamic_cast<TransferFunction&>(transferFunctions)),
  _labeler(thelabeler)
  //~ _trace(false)
{}



void
SeqPrioritySolver::computeCombinedPreInfo(Label lab,Lattice& info) 
{
  if(!_flow.contains(lab)) {
    // schroder3 (2016-07-07): If the label does not exist in the CFG, then
    //  it does not have predecessors and the given pre-info therefore does
    //  not change. This check is necessary if Flow::pred(Label) uses the
    //  Sawyer graph as underlying datastructure because Flow::pred then
    //  expects that the given label exists in the CFG.
    return;
  }
  LabelSet pred=_flow.pred(lab);
  for(LabelSet::iterator i=pred.begin();i!=pred.end();++i) {
    Lattice* predInfo=_initialElementFactory.create();
    predInfo->combine(*_analyzerDataPreInfo[(*i).getId()]); // clone info of predecessor
    computePostInfo(*i,*predInfo);
    info.combine(*predInfo);
    delete predInfo;
  }
}

void
SeqPrioritySolver::computePostInfo(Label lab,Lattice& info) 
{
  _transferFunctions.transfer(lab,info);
}

Lattice&
SeqPrioritySolver::preInfoLattice(Label lab)
{
  return SG_DEREF(_analyzerDataPreInfo.at(lab.getId()));
}




template <class W>
void
SeqPrioritySolver::propagate(Lattice& state, Label tgt, W& wkl)
{
  Lattice&   tgtstate = preInfoLattice(tgt);
  const bool subsumed = state.approximatedBy(tgtstate);
  
  if (subsumed) 
  {
    logDbg() << "mapping not necessary (already approximated): " << tgt.getId()
             << std::endl;
    
    return;
  }  
  
  logDbg() << "mapping transfer result to: " << tgt << ": " << LazyToString(tgtstate)
           << std::endl;

  tgtstate.combine(state);

  logDbg() << "new df value : " << tgt << ": " << LazyToString(tgtstate)
           << std::endl;
           
  const size_t oldsz = wkl.size();

  for (Edge e : _flow.outEdges(tgt)) 
  {
    wkl.add(e);
  }

  logDbg() << "added : " << (wkl.size() - oldsz) << " edges."
           << std::endl;
}



// runs until worklist is empty
void
SeqPrioritySolver::runSolver() 
{
  typedef Edge                                     WorkListElem;
  typedef PriorityWorklist<WorkListElem>           PriorityWorkQ;
  
  constexpr uint64_t REPORT_INTERVAL = (1 << 12);
  
  TimeMeasurement     solverTimer;
  uint64_t            nodeCounter = 0;
  double              splitTime   = 0;
  PriorityWorkQ       worklist    = createWorklist<WorkListElem>(_workList, _labeler, _flow);
  
  addInitialWorklistItems( worklist, _workList,
                           [](PriorityWorkQ& lst, Edge e)
                           {
                             lst.add(e);
                           },
                           false
                         );

  logInfo() << "PrioritySolver started" << std::endl;
  
  if (worklist.isEmpty())
    logWarn() << "initial worklist size is empty." << std::endl;
    
  solverTimer.start();
  
  while (!worklist.isEmpty()) 
  {
    WorkListElem             edge = worklist.take();
    Label                    lab0 = edge.source();
    Label                    lab1 = edge.target();

    //~ logDbg() << "computing edge " << LazyEdge(edge, labeler()) << std::endl;
    
    Lattice&                 state = preInfoLattice(lab0);
    std::unique_ptr<Lattice> stateTransfered{cloneLattice(_initialElementFactory, state)};
    
    if (!stateTransfered->isBot()) 
    {
      logDbg() << "computing transfer function: " << lab0 << ": " << LazyToString(state)
               << std::endl;
      
      _transferFunctions.transfer(edge, *stateTransfered);
      
      logDbg() << "transfer function result: " << lab0 << ": " << LazyToString(state)
               << std::endl;
               
      propagate(*stateTransfered, lab1, worklist);
    }
    else 
    {
      logDbg() << "computing transfer function: " << lab0 << ": bot"
               << "\n  => cancel (because of bot)"
               << std::endl;
      // do nothing (non-reachable code)
    } 
    
    if (((++nodeCounter) % REPORT_INTERVAL) == 0)
    {
      const double oldSplitTime = splitTime;
       
      splitTime = solverTimer.getTimeDurationAndKeepRunning().seconds();
      
      logInfo() << static_cast<size_t>(REPORT_INTERVAL / (splitTime-oldSplitTime)) << " nodes/s - "
                << nodeCounter << '/' << splitTime << '.' 
                << std::endl; 
    }
  }
  
  TimeDuration endTime = solverTimer.getTimeDurationAndStop();
  
  logInfo() << "PrioritySolver finished after " << static_cast<size_t>(endTime.milliSeconds()) << "ms." 
            << std::endl
            << "  " << nodeCounter << " nodes analyzed (" << static_cast<size_t>(nodeCounter / endTime.seconds())
            << " nodes/s)" 
            << std::endl; 
}


}

#endif
