#include <AutoTuningInterface.h>
#include <CopyArrayAnal.h>
#include <CommandOptions.h>
#include <ReuseAnalysis.h>
#include <LoopInfoInterface.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

#include <GraphIO.h>

extern bool DebugCopySplit();

static bool DebugCopyRoot()
{
  static int r = 0;
  if (r == 0)
    r = CmdOptions::GetInstance()->HasOption("-debugcopyroot")? 1 : -1;
  return r == 1;
}

static bool DebugCopyRemove()
{
  static int r = 0;
  if (r == 0)
    r = CmdOptions::GetInstance()->HasOption("-debugcopyremove")? 1 : -1;
  return r == 1;
}

static bool DebugCrossGraph()
{
  static int r = 0;
  if (r == 0)
    r = CmdOptions::GetInstance()->HasOption("-debugcrossgraph")? 1 : -1;
  return r == 1;
}

void CopyArrayOperator::operator()
(LoopTreeLocalityAnal& tc, LoopTreeNode* root)
{
   DepCompAstRefGraphCreate refDep;
   refDep.Build(tc, root);

   if (DebugCrossGraph())
      write_graph(refDep, std::cerr, "reuse");
   DepCompCopyArrayCollect collect( root);
   DepCompCopyArrayToBuffer::CollectCopyArray(collect, refDep);
   ModifyCopyArrayCollect(collect, refDep);
   DepCompCopyArrayToBuffer::ApplyCopyArray(collect,refDep);
};

int CopyArrayOperator::
OutmostReuseLevel ( DepCompCopyArrayCollect::CopyArrayUnit& unit,
                       DepCompAstRefGraphCreate& refDep)
{
   int copylevel = unit.copylevel();
   int res = -1;
   for (DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator p1 = unit.refs.begin();
       !p1.ReachEnd(); ++p1) {
      const DepCompAstRefGraphNode* cur = *p1;
      for (int level = copylevel; level < cur->GetInfo().stmt->LoopLevel(); ++level) {
         if (refDep.SelfReuseLevel(cur, level)) {
             if (res <= 0 || res > level)
                res = level;
         }
      }
  }
  if (DebugCopyRoot()) {
     std::cerr << "outmost reuse level for " << IteratorToString2(unit.refs.begin()) << " is " << res << std::endl;
  }
  return res;
}

int CopyArrayOperator::
EnforceCopyDimension( DepCompCopyArrayCollect::CopyArrayUnit& unit,
                       DepCompAstRefGraphCreate& refDep, int copydim,
                       DepCompCopyArrayCollect::CopyArrayUnit::NodeSet* cuts)
{
   int copylevel = unit.copylevel();
   int res = 0;
   for (DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator p1 = unit.refs.begin();
       !p1.ReachEnd(); ++p1) {
      const DepCompAstRefGraphNode* cur = *p1;
      int curdim = 0;
      for (int level = cur->GetInfo().stmt->LoopLevel()-1; level >= copylevel; --level) {
         if (!refDep.SelfReuseLevel(cur, level)) {
            ++curdim;
            if (DebugCopyRoot())
               std::cerr << "processing node " << cur << ":  loop at level " << level << "do not carry reuse \n";
         }
         if (curdim > copydim && cuts != 0) {
            cuts->insert(cur);
            if (unit.refs.size() > 1) {
               DepCompCopyArrayCollect::CopyArrayUnit tmp = unit;
               tmp.refs.erase(cur);
               DepCompCopyArrayToBuffer().EnforceCopyRoot(tmp, refDep, cur, *cuts);
            }
            return -1;
         }
      }
      if (res < curdim)
         res = curdim;
   }
   return res;
}

bool CopyArrayOperator::
IsRedundantCopy( DepCompCopyArrayCollect::CopyArrayUnit& unit,
                 int copydim)
{
   int copylevel = unit.copylevel();
   int dimdiff = 0;
   DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator p = unit.refs.begin();
   for ( ; !p.ReachEnd(); ++p) {
        int curdiff = (*p)->GetInfo().stmt->LoopLevel() - copylevel;
        if (dimdiff < curdiff)
            dimdiff = curdiff;
   }
   if (dimdiff > copydim)
      return false;
   if (unit.refs.size() <= 3)
     return true;

/*
   for (p.Reset(); !p.ReachEnd(); ++p) {
      AstNodePtr ref = (*p)->GetInfo().orig;
      LoopTreeNode* loop = (*p)->GetInfo().stmt->EnclosingLoop();
      for (int i = 0; i < copydim && loop != unit.root; loop = loop->EnclosingLoop(), ++i)
         if (!ReferenceDimension(ref, loop->GetLoopInfo()->GetLoopIndexVar().GetVarName(),i))
            return false;
   }
   return true;
*/
    return false;
}

bool CopyArrayOperator::
SplitDisconnectedUnit( DepCompCopyArrayCollect& collect,
                        DepCompCopyArrayCollect::CopyArrayUnit& unit,
                        DepCompAstRefGraphCreate& refDep,
                       DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts)
{
  assert(unit.refs.size() > 0);
  DepCompCopyArrayCollect::CopyArrayUnit::InsideGraph insidegraph(&refDep,unit);
  const DepCompAstRefGraphNode* cur = *unit.refs.begin();

  GraphGetNodeReachable<DepCompCopyArrayCollect::CopyArrayUnit::InsideGraph,
                        AppendPtrSet<const DepCompAstRefGraphNode> > op;
  DepCompCopyArrayCollect::CopyArrayUnit::NodeSet innodes;
  AppendPtrSet<const DepCompAstRefGraphNode> col1(innodes), col2(cuts);
  op(&insidegraph, cur, GraphAccess::EdgeIn,col1);
  for (DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator p1 = innodes.begin();
       !p1.ReachEnd(); ++p1) {
     op(&insidegraph, (*p1), GraphAccess::EdgeOut,col2);
  }
  cuts |= innodes;
  return cuts.size() < unit.refs.size();
}

void CopyArrayUnderSizeLimit::
ModifyCopyArrayCollect( DepCompCopyArrayCollect& collect, DepCompAstRefGraphCreate& refDep)
{
   LoopTreeInterface interface;
   if (DebugCopyRoot())
      std::cerr << "copydim = " << copydim << std::endl;
   for (DepCompCopyArrayCollect::iterator arrays = collect.begin();
        arrays != collect.end(); ) {
       DepCompCopyArrayCollect::CopyArrayUnit& unit = *arrays;
       LoopTreeNode* origroot = unit.root;
       if (DebugCopySplit() || DebugCopyRoot())
         std::cerr << " modifying copy unit: " << IteratorToString2(unit.refs.begin()) << " with root = " << ((unit.root == 0)? "null" : unit.root->toString()) << std::endl;
       unit.root = collect.OutmostCopyRoot(unit, refDep, collect.get_tree_root());
       int curdim = -1;
       while (true) {
          DepCompCopyArrayCollect::CopyArrayUnit::NodeSet cuts;
          curdim = EnforceCopyDimension(unit, refDep, copydim, &cuts);
          if (cuts.size() == 0)
               break;
          if (cuts.size() == unit.refs.size()) {
             assert(origroot != unit.root);
             LoopTreeNode* n = origroot, *p = GetEnclosingLoop(n,interface);
             LoopTreeNode* rootloop = (unit.root->GetLoopInfo() == 0)? 0 : unit.root;
             while (n != rootloop && p != rootloop) {
                 n = p;
                 p = GetEnclosingLoop(p, interface);
             }
             if (DebugCopyRoot())
               std::cerr << "resetting copy root to be " << n->toString() << std::endl;
             unit.root = n;
             unit.carrybyroot = true;
             continue;
          }
          if (DebugCopySplit())
             std::cerr << "Enforce copy dimension by removing " << IteratorToString2(cuts.begin()) << std::endl;
          collect.AddCopyArray() =
                DepCompCopyArrayCollect::CopyArrayUnit(cuts, collect.ComputeCommonRoot(cuts));
          unit.refs -= cuts;
          unit.root = origroot = collect.ComputeCommonRoot(unit.refs);
          unit.root = collect.OutmostCopyRoot(unit, refDep, collect.get_tree_root());
       }
       DepCompCopyArrayCollect::CopyArrayUnit::NodeSet cuts;
       if (SplitDisconnectedUnit(collect, unit, refDep,cuts))  {
          origroot = collect.ComputeCommonRoot(cuts);
          DepCompCopyArrayCollect::CopyArrayUnit::NodeSet left = unit.refs;
          if (DebugCopySplit() || DebugCopyRoot())
             std::cerr << " Spliting disconnected refs: removing " << IteratorToString2(cuts.begin()) << std::endl;
          left -= cuts;
          collect.AddCopyArray() = DepCompCopyArrayCollect::CopyArrayUnit(left,collect.ComputeCommonRoot(left));
          unit.refs = cuts;
          curdim = EnforceCopyDimension(unit, refDep, copydim);
          assert(curdim <= (int)copydim);
       }
       if (origroot != 0 && unit.root != origroot) {
           int reuselevel = OutmostReuseLevel( unit, refDep);
           int copylevel = unit.copylevel();
           if (reuselevel > copylevel) {
              LoopTreeNode *cur = origroot;
              for (int curlevel = origroot->LoopLevel(); reuselevel  <= curlevel;
                   cur = GetEnclosingLoop(cur, interface), --curlevel);
              if (DebugCopyRoot())
                  std::cerr << "After reuse anal, resetting copy root to be " << cur->toString() << std::endl;
              unit.root = cur;
              curdim -= (reuselevel - copylevel+1);
           }
           else if (DebugCopyRoot())
                std::cerr << "do not reset copy root because copylevel = " << copylevel << " and copy root = " << unit.root->toString() << std::endl;
       }
       DepCompCopyArrayCollect::iterator tmp = arrays;
       ++arrays;
       if (IsRedundantCopy( unit, curdim)) {
         if (DebugCopyRemove()) {
             std::cerr << "remove redundant copy " <<  IteratorToString2(unit.refs.begin()) << " with root = " << unit.root->toString() << std::endl;
         }
         collect.RemoveCopyArray(tmp);
       }
   }
}

void ParameterizeCopyArray:: ApplyXform(
                DepCompCopyArrayCollect::CopyArrayUnit& curarray,
                CopyArrayConfig& config, LoopTreeNode* repl,
                LoopTreeNode* init, LoopTreeNode* save)
{
  AutoTuningInterface* tuning = LoopTransformInterface::getAutoTuningInterface();
  assert(tuning != 0);

  tuning->CopyArray(config, repl);
}

bool ParameterizeCopyArray::
CanBeBlocked(
     DepCompCopyArrayCollect::CopyArrayUnit& unit, int copydim)
{
   if (copydim == 0) return false;

   int k = 0; /* number of sweeping loop dimensions */
   for (int i = 0; i < copydim; ++i) {
      bool sweeping = false; /*QY: any sweeping loops*/
      for (DepCompCopyArrayCollect::CopyArrayUnit::NodeSet::const_iterator
              p = unit.refs.begin(); !p.ReachEnd(); ++p) {
          int j = 0; /*number of sweeping loop dimensions per array dimension*/
          for (LoopTreeNode* loop = (*p)->GetInfo().stmt->EnclosingLoop();
               loop!=0 && loop != unit.root; loop = loop->EnclosingLoop()) {
            AstNodePtr ref = (*p)->GetInfo().orig;
            if (!ReferenceDimension(ref, loop->GetLoopInfo()->GetVar().GetVarName(),i)) { ++j; sweeping=true; break; }
          }
          if (j > 1) return false; /*QY: multiple loops sweeping a dimension*/
      }
      if (sweeping) ++k;
   }
   if (k == 0) return false; /* QY: no sweeping dimensions*/
   return true;
}

void ParameterizeCopyArray::
ModifyCopyArrayCollect(
            DepCompCopyArrayCollect& collect, DepCompAstRefGraphCreate& refDep)
{
  CopyArrayUnderSizeLimit::ModifyCopyArrayCollect(collect,refDep);
#ifndef NDEBUG
  AutoTuningInterface* tuning = LoopTransformInterface::getAutoTuningInterface();
  assert(tuning != 0);
#else
  LoopTransformInterface::getAutoTuningInterface();     // side effects?
#endif

      for (DepCompCopyArrayCollect::iterator arrays = collect.begin();
           arrays != collect.end(); ++arrays) {
          DepCompCopyArrayCollect::CopyArrayUnit& unit = *arrays;
          if (!CanBeBlocked(unit, copydim)) continue;
          if (DebugCopyRoot())
            std::cerr << " modifying copy unit: " << IteratorToString2(unit.refs.begin()) << " with root = " << ((unit.root == 0)? "null" : unit.root->toString()) << std::endl;
          unit.root = collect.get_tree_root();
       }
}
