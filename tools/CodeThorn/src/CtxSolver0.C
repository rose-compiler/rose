#ifndef DFSOLVER1_C
#define DFSOLVER1_C

#include "sage3basic.h"

#include <memory>

#include "CtxSolver0.h"
#include "CtxLattice.h"
#include "CodeThornLib.h"
#include "TimeMeasurement.h"

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

  template <class T>
  struct LazyToString
  {
    explicit
    LazyToString(T* tp)
    : obj(tp)
    {}

    explicit
    LazyToString(std::unique_ptr<T>& tp)
    : LazyToString(tp.get())
    {}

    explicit
    LazyToString(T& tref)
    : LazyToString(&tref)
    {}

    std::string toString() { return obj->toString(); }

    T* obj;
  };

  template <class T>
  LazyToString<T> lazyToString(T* tp)
  {
    return LazyToString<T>(tp);
  }

  template <class T>
  inline
  std::ostream&
  operator<<(std::ostream& os, LazyToString<T> lat) { return os << lat.toString(); }


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
  //~ auto logDbg() -> decltype(msgInfo()) { return msgInfo(); }
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

CtxSolver0::CtxSolver0( WorkListSeq<Edge>& workList,
                        std::vector<Lattice*>& analyzerDataPreInfo,
                        std::vector<Lattice*>& analyzerDataPostInfo,
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
CtxSolver0::computeCombinedPreInfo(Label lab, Lattice& info)
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
CtxSolver0::computePostInfo(Label lab,Lattice& info)
{
  _transferFunctions.transfer(lab, info);
}

CtxLatticeRange<CtxSolver0::ContextString>
CtxSolver0::mappedCtxRange(Label lab, CtxLatticeRange<ContextString>::iterator ctxpos)
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


CtxLattice<CtxSolver0::ContextString>&
CtxSolver0::preInfoLattice(Label lab)
{
  return dynamic_cast<CtxLattice<ContextString>&>(*_analyzerDataPreInfo[lab.getId()]);
}


Lattice&
CtxSolver0::preInfoLattice(Label lab, const ContextString& ctx)
{
  CtxLattice<ContextString>&  all = preInfoLattice(lab);
  Lattice*&               sub = all[ctx];

  if (sub == NULL)
    sub = _initialElementFactory.componentFactory().create();

  return *sub;
}


CtxSolver0::InternalWorklist
CtxSolver0::preprocessWorklist()
{
  InternalWorklist res;

  for (Edge edge : _workList)
  {
    CtxLattice<ContextString>& lat = preInfoLattice(edge.source());

    for (const CtxLattice<ContextString>::value_type& el : lat)
      res.add(InternalWorklist::value_type(edge, el.first));
  }

  return res;
}

void
CtxSolver0::propagate(const ContextString& tgtctx, Lattice& state, Label tgt, InternalWorklist& wkl)
{
  typedef std::pair<Edge, ContextString> WorkListElem;

  Lattice&   tgtstate  = preInfoLattice(tgt, tgtctx);
  const bool returnLbl = labeler().isFunctionCallReturnLabel(tgt);
  const bool subsumed  = (!returnLbl) && state.approximatedBy(tgtstate);

  if (subsumed)
  {
    logDbg() << "mapping not necessary (already approximated): " << tgt.getId()
             << std::endl;
    return;
  }

  logDbg() << "mapping transfer result to: " << tgt << " / " << tgtctx << ": " << lazyToString(&tgtstate)
           << std::endl;

  tgtstate.combine(state);

  logDbg() << "new df value : " << tgt << " / " << tgtctx << ": " << lazyToString(&tgtstate)
           << std::endl;

  const size_t oldsz = wkl.size();

  for (Edge e : _flow.outEdges(tgt))
  {
    wkl.add(WorkListElem(e, tgtctx));
  }

  logDbg() << "added : " << (wkl.size() - oldsz) << " edges."
           << std::endl;
}

void
CtxSolver0::activateReturnNode(const ContextString& ctx, Label callLbl, InternalWorklist& wkl)
{
  typedef CtxLattice<ContextString>       context_lattice_t;
  typedef std::pair<Edge, ContextString>  WorkListElem;

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


// runs until worklist is empty
void
CtxSolver0::runSolver()
{
  typedef CtxLattice<ContextString>                context_lattice_t;
  typedef std::pair<Edge, ContextString>           WorkListElem;
  typedef CtxLatticeRange<ContextString>::iterator Iterator;

  constexpr uint64_t REPORT_INTERVAL = (1 << 12);

  TimeMeasurement     solverTimer;
  uint64_t            nodeCounter = 0;
  double              splitTime   = 0;
  InternalWorklist    worklist    = preprocessWorklist();

  msgInfo() << "CtxSolver0 started" << std::endl;

  if (worklist.isEmpty())
    msgWarn() << "initial worklist size is empty." << std::endl;

  solverTimer.start();

  while (!worklist.isEmpty())
  {
    WorkListElem             el   = worklist.take();
    Edge                     edge = el.first;
    const ContextString&     ctx  = el.second;
    Label                    lab0 = edge.source();
    Label                    lab1 = edge.target();

    logDbg() << "computing edge " << lab0 << "->" << lab1 << ": " << lazyToString(&edge)
             << std::endl;

    context_lattice_t&       ctxlat = preInfoLattice(lab0);
    const Iterator           preIt  = iteratorAt(ctxlat, ctx);
    std::unique_ptr<Lattice> info{cloneLattice(_initialElementFactory.componentFactory(), *preIt->second)};

    if (!info->isBot())
    {
      logDbg() << "computing transfer function: " << lab0 << " / " << ctx << ": " << LazyToString<Lattice>(info)
               << std::endl;

      _transferFunctions.componentTransfer().transfer(edge, *info);

      logDbg() << "transfer function result: " << lab0 << " / " << ctx << ": " << LazyToString<Lattice>(info)
               << std::endl;

      // propagate the state to the respective context in lab1
      //   and add the outflowing edges to the worklist.
      // Three cases are distinguished: "normal" edge, a call edge,
      //   and return edge.
      if (labeler().isFunctionCallLabel(lab0))
      {
        // update the context by appending the lbl (1:1 mapping)

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

      msgInfo() << static_cast<size_t>(REPORT_INTERVAL / (splitTime-oldSplitTime)) << " nodes/s - "
                << nodeCounter << '/' << splitTime << '.'
                << std::endl;
    }
  }

  TimeDuration endTime = solverTimer.getTimeDurationAndStop();

  msgInfo() << "Solver 1 finished after " << static_cast<size_t>(endTime.milliSeconds()) << "ms."
            << std::endl
            << "  " << nodeCounter << " nodes analyzed (" << static_cast<size_t>(nodeCounter / endTime.seconds())
            << " nodes/s)"
            << std::endl;
}

}

#endif
