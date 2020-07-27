#include <stdio.h>
#include <CommandOptions.h>
#include <CompSliceDepGraph.h>
#include <DynamicCompSlice.h>
#include <LoopTreeBuild.h>
#include <LoopTreeTransform.h>
#include <DepCompTransform.h>
#include <ProcessAstTree.h>
#include <InterchangeAnal.h>
#include <FusionAnal.h>
#include <BlockingAnal.h>
#include <ParallelizeLoop.h>
#include <CopyArrayAnal.h>
#include <LoopTransformOptions.h>
#include <AutoTuningInterface.h>
#include <GraphIO.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

//#define DEBUG
using namespace std;

void LoopTransformation( LoopTreeDepComp& comp,
                         DependenceHoisting& op,
                         LoopTreeLocalityAnal &anal,
                         int optlevel);

bool OutputDep()
{
  static int r = 0;
  if (r == 0) {
      if (CmdOptions::GetInstance()->HasOption("-outputdep"))
           r = 1;
      else
           r = -1;
  }
  return r == 1;
}

bool DebugDep()
{
  static int r = 0;
  if (r == 0) {
      if (CmdOptions::GetInstance()->HasOption("-debugdep"))
           r = 1;
      else
           r = -1;
  }
  return r == 1;
}
bool DebugLoop()
{
  static int r = 0;
  if (r == 0) {
      if (CmdOptions::GetInstance()->HasOption("-debugloop"))
           r = 1;
      else
           r = -1;
  }
  return r == 1;
}
bool ReportTiming()
{
  return CmdOptions::GetInstance()->HasOption("-tmloop") != 0;
}

bool ApplyLoopSplitting()
{
  return CmdOptions::GetInstance()->HasOption("-splitloop") != 0;
}

int ApplyOptLevel()
{
  static int level = -1;
  if (level < 0) {
    level = 0;
    vector<string>::const_iterator p = CmdOptions::GetInstance()->GetOptionPosition("-opt");
    if (p != CmdOptions::GetInstance()->end()) {
      string str = p->substr(4);
      if (str.empty()) {
        ++p;
        assert (p != CmdOptions::GetInstance()->end());
        str = *p;
      }
      sscanf(str.c_str(), "%d", &level);
    }
    std::cerr << "opt level=" << level << "\n";
  }
  return level;
}

class AstTreeOptimizable : public ProcessAstTree
{
  private:
   AstNodePtr loop, top;
   int succ;
   int optType;

   bool ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
                                      const AstNodePtr& body,
                                      AstInterface::TraversalVisitType t)
   {
        if (DebugLoop())
              std::cerr << "if fun definition ";
        succ = -1;
        return false;
   }

   bool ProcessDecls(AstInterface &fa, const AstNodePtr& s)
     {
        if (fa.IsVariableDecl(s) && fa.GetParent(s) != top) {
           if (DebugLoop())
              std::cerr << "has declaration " << AstInterface::AstToString(s) << "\n";
            succ = -1;
            return false;
        }
        return ProcessAstTree::ProcessDecls(fa, s);
     }
   bool ProcessLoop(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& body,
                        AstInterface::TraversalVisitType t)
    {
        if (succ < 0)
           return false;

        if (!fa.IsFortranLoop(s)) {
           if (DebugLoop()) {
              std::cerr << "not fortran loop " << AstInterface::AstToString(s) << std::endl;
           }
           succ = -1;
           return false;
        }
        if (t == AstInterface::PreVisit) {
           if (loop == 0)
              loop = s;
           else if (loop != 0 && !(optType & LoopTransformOptions::LOOP_DATA_OPT)) {
              if (DebugLoop())
                  std::cerr << "no optimization specified \n";
              succ = -1;
              return false;
           }
        }
        else if (t == AstInterface::PostVisit) {
           if (s == loop) {
              succ = 1;
              return false;
           }
        }
        return ProcessAstTree::ProcessLoop(fa, s, body, t);
     }
  bool ProcessGoto(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& dest)
     {
        if (succ < 0)
           return false;
        loop = AST_NULL;
        return ProcessAstTree::ProcessGoto(fa, s, dest);
     }
  bool ProcessIf(AstInterface &fa, const AstNodePtr& s,
                   const AstNodePtr& cond, const AstNodePtr& truebody,
                   const AstNodePtr& falsebody,
                   AstInterface::TraversalVisitType t)
    {
        if (succ < 0)
           return false;
        if (s == top) return true;
        if (t == AstInterface::PreVisit)
           return ProcessStmt(fa, s);
        return true;
    }

 public:
   AstTreeOptimizable( LoopTransformOptions::OptType t )
     : loop(AST_NULL), succ(0), optType(t) { }
   bool operator()( const AstNodePtr& head)
    {
       loop = AST_NULL; succ = 0;
       top = head;
       if (optType & LoopTransformOptions::LOOP_DATA_OPT) {
          ProcessAstTree::operator()(LoopTransformInterface::getAstInterface(), head);
          if (succ <= 0) {
             optType &= (~LoopTransformOptions::LOOP_DATA_OPT);
          }
       }

       return succ == 1;
    }
    bool PerformTransformation() const
    {
       return optType & LoopTransformOptions::LOOP_DATA_OPT;
    }
    bool PerformLoopTransformation() const
    {
       return optType & LoopTransformOptions::LOOP_OPT;
    }
    bool PerformDataTransformation() const
    {
       return optType & LoopTransformOptions::DATA_OPT;
    }

};

class CopyDeclarations : public ProcessAstTree
{
  AstNodePtr dest;
 protected:
  virtual bool ProcessLoop(AstInterface &fa, const AstNodePtr& s,
                               const AstNodePtr& init, const AstNodePtr& cond,
                               const AstNodePtr& incr, const AstNodePtr& body,
                               AstInterface::TraversalVisitType t)
     {
       if (t == AstInterface::PreVisit) {
           Skip(s);
           SkipUntil( body);
           SkipUntil( init);
        }
        return true;
     }
  bool ProcessDecls( AstInterface& fa, const AstNodePtr& decl)
  {
    AstNodePtr ndecl = fa.CopyAstTree(decl);
    fa.BlockAppendStmt( dest, ndecl);
    return ProcessAstTree::ProcessDecls( fa, decl);
  }
  bool ProcessStmt(AstInterface &fa, const AstNodePtr& s)
  {
     if (fa.IsVariableDecl(s))
         ProcessDecls(fa, s);
      return true;
  }

 public:
  CopyDeclarations( const AstNodePtr& d) : dest(d) {}
  bool operator ()(AstInterface& fa, const AstNodePtr& top)
     {
        return ReadAstTraverse(fa, top, *this, AstInterface::PreAndPostOrder);
     }
};

extern double GetWallTime();

bool LoopTransformation( const AstNodePtr& head, AstNodePtr& result)
{
#ifdef DEBUG
std::cerr << "LoopTransformation1\n";
#endif

 /*CmdOptions *opt =*/ CmdOptions::GetInstance();
 bool reportPhaseTiming = ReportTiming();
 bool debugloop = DebugLoop(), debugdep = DebugDep(), outputdep=OutputDep();

 bool depOnly = debugdep || outputdep; // opt->HasOption("-depAnalOnly");
 LoopTransformOptions *lopt = LoopTransformOptions::GetInstance();
 AstTreeOptimizable sel(lopt->GetOptimizationType());
 if (!depOnly && !sel(head)) {
        return false;
  }

  AstInterface &fa = LoopTransformInterface::getAstInterface();


  if (debugloop) {
    std::cerr <<"----------------------------------------------"<<endl;
    std::cerr << "try applying loop transformation to \n";
    std::cerr << AstInterface::AstToString(head) << std::endl;
  }
  if (reportPhaseTiming) GetWallTime();
  LoopTreeDepCompCreate comp(head);
  if (reportPhaseTiming) std::cerr << "dependence analysis time: " <<  GetWallTime() << "\n";
  if (debugloop) {
     std::cerr <<"----------------------------------------------"<<endl;
    std::cerr << "original LoopTree : \n";
     comp.DumpTree();
  }
  if (debugdep) {
     std::cerr <<"----------------------------------------------"<<endl;
     std::cerr << "LoopTree dependence graph: \n";
     comp.DumpDep();
  }
  if (outputdep) {
     std::cerr <<"----------------------------------------------"<<endl;
     std::cerr << "dependence graph: \n";
     comp.OutputDep();
  }

  if (!depOnly && sel.PerformTransformation()) {
      LoopTreeLocalityAnal loopAnal(comp);
      if (debugdep)
      {
          std::cerr <<"----------------------------------------------"<<endl;
          std::cerr << "LoopTree input dep graph: \n" << GraphToString(*loopAnal.GetInputGraph()) << std::endl;
      }
      if (lopt->DoDynamicTuning()) {
           DynamicSlicing op;
           LoopTransformation( comp, op, loopAnal, ApplyOptLevel());
      }
      else {
           DependenceHoisting op;
           LoopTransformation( comp, op, loopAnal, ApplyOptLevel());
      }
      if (debugloop) {
            std::cerr << "\n final LoopTree : \n";
            comp.DumpTree();
        std::cerr <<"----------------------------------------------"<<endl;
      }
     if (debugdep) {
        std::cerr << "final dependence graph: \n";
        comp.DumpDep();
        std::cerr <<"=============================================="<<endl;
     }
  }

  comp.DetachDepGraph();
  if (ApplyLoopSplitting())
    ApplyLoopSplitting(comp.GetLoopTreeRoot());

  if (debugloop) {
            std::cerr << "\n Before CodeGen : \n";
            comp.DumpTree();
        std::cerr <<"----------------------------------------------"<<endl;
   }

  AstNodePtr r = comp.CodeGen();
  assert (r != 0);

  result = fa.CreateBlock(head);
  CopyDeclarations copyDecl( result);
  copyDecl( fa, head);
  fa.CopyNewVarDecls(result);
  fa.BlockAppendStmt(result, r);
  return true;
}

void RearrangeCompSliceGraph( LoopTreeDepComp &comp,
                        CompSliceDepGraphCreate& graph,
                        CompSliceLocalityRegistry &sliceAnal)
{
 bool debugslice = (CmdOptions::GetInstance()->HasOption("-debugslice") != 0);
 if (debugslice)
   std::cerr << "computation slice graph: \n" << GraphToString(graph) << std::endl;
 LoopTransformOptions *opt = LoopTransformOptions::GetInstance();

 ArrangeNestingOrder *icOp = opt->GetInterchangeSel();
 for (CompSliceDepGraphNodeIterator iter = graph.GetNodeIterator();
         !iter.ReachEnd(); ++iter) {
     CompSliceNest* cur = iter.Current()->GetInfo().GetNest();
     if (cur != 0)
        (*icOp)( &sliceAnal, *cur);
 }

 if (debugslice)
   std::cerr << "after interchange: \n" << GraphToString(graph) << std::endl;

 LoopNestFusion* fsOp = opt->GetFusionSel();
 SliceNestTypedFusion( &sliceAnal, graph, *fsOp);
 SliceNestReverseTypedFusion( &sliceAnal, graph, *fsOp);
 if (debugslice)
   std::cerr << "after fusion: \n" << GraphToString(graph) << std::endl;
}

void Preprocess( LoopTreeDepComp& comp )
{
  LoopTreeNode *root = comp.GetLoopTreeRoot();
  for (LoopTreeNode *n = root->FirstChild(); n != 0; n = n->NextSibling()) {
     if (n->ContainLoop())
         DepCompDistributeLoop()(comp, n);
  }
}

void Postprocess( LoopTreeDepComp& comp)
{
  LoopTreeNode *root = comp.GetLoopTreeRoot();
  assert(root != 0);
  OptimizeLoopTree(root);
}

void LoopTransformation( LoopTreeDepComp& comp,
                         DependenceHoisting &op,
                         LoopTreeLocalityAnal &loopAnal,
                         int optlevel)
{
#ifdef DEBUG
std::cerr << "LoopTransformation2\n";
#endif
  LoopTransformOptions *lopt = LoopTransformOptions::GetInstance();
  bool reportPhaseTimings = ReportTiming();

  CopyArrayOperator *cp = lopt->GetCopyArraySel();
  if (!(lopt->GetOptimizationType() & LoopTransformOptions::LOOP_OPT))
  {
     if (cp != 0) (*cp)(loopAnal, comp.GetLoopTreeRoot());
     return;
  }

  LoopPar* par = lopt->GetParSel();
  LoopBlocking* blocking = lopt->GetBlockSel();
  assert (blocking != 0) ;

  if (reportPhaseTimings) GetWallTime();
  LoopTreeTransDepGraphCreate tc(comp.GetDepGraph(), lopt->GetTransAnalSplitLimit());
  Preprocess(comp);
  CompSliceDepGraphCreate sliceGraph(comp, op, &tc);
  if (reportPhaseTimings) std::cerr <<  "slicing analysis timing:" << GetWallTime() << "\n";

  if (DebugDep()) {
     std::cerr << "Transitive dependence graph:\n";
     tc.Dump();
  }
  CompSliceLocalityRegistry sliceAnal(loopAnal, lopt->GetCacheLineSize(), lopt->GetReuseDistance());
  RearrangeCompSliceGraph(comp, sliceGraph, sliceAnal);
  if (reportPhaseTimings)  std::cerr << "transformation analysis timing:" << GetWallTime() << "\n";

  sliceGraph.TopoSort();
  if (DebugLoop()) {
     std::cerr << "*******before xform*****";
     comp.DumpTree();
   }
   if (DebugDep()) {
     std::cerr << "*******before xform*****";
     comp.DumpDep();
   }
  LoopTreeNode* top = comp.GetLoopTreeRoot();
  top = LoopTreeTransform().InsertHandle(top,1);
  for (CompSliceDepGraphCreate::NodeIterator iter = sliceGraph.GetNodeIterator();
       !iter.ReachEnd(); ++iter) {
      CompSliceDepGraphNode *n = iter.Current();
      CompSliceDepGraphNode::NestInfo& nest = n->GetInfo();

      assert(top != 0);
      LoopTreeNode *loopTree_handle = nest.GenXformRoot(top);
      assert (loopTree_handle !=0);
      if (loopTree_handle == top) { top = 0; }

      CompSliceDepGraphNode::FullNestInfo* info = dynamic_cast<CompSliceDepGraphNode::FullNestInfo*>(&nest);
      if (info == 0) continue;

      /* slice is innermost slice being processed*/
      const CompSlice* slice = blocking->SetBlocking(&sliceAnal, *info);
      assert(slice != 0);
      loopTree_handle = blocking->apply(*info,comp,op,loopTree_handle);
      if (par != 0)
          loopTree_handle=par->apply(*info,comp,op,loopTree_handle);
      if (DebugLoop()) {
         std::cerr << "*******after xform*****";
         comp.DumpTree();
      }
      if (DebugDep()) {
         std::cerr << "*******after xform*****";
         comp.DumpDep();
      }
      if (optlevel-- > 0) {
         for (CompSlice::ConstLoopIterator p = slice->GetConstLoopIterator();
              !p.ReachEnd(); ++p) {
            LoopTreeNode *r = p.Current();
            LoopTreeTraverseSelectLoop loops(r, LoopTreeTraverse::PostOrder);
            if (!loops.ReachEnd() && !slice->QuerySliceLoop(loops.Current()) ) {
               r = LoopTreeTransform().InsertHandle(r,1);
               LoopTreeDepCompSubtree scope(comp, r);
               LoopTransformation(scope, op, loopAnal, optlevel);
            }
         }
      }
      if (cp != 0) {
          LoopTreeTransform().InsertHandle(loopTree_handle,-1);
          if (DebugLoop())
             std::cerr << "applying array copy to " << loopTree_handle->TreeToString() << std::endl;
          (*cp)(loopAnal, loopTree_handle->Parent());
      }
   }
   Postprocess(comp);
   if (reportPhaseTimings) std::cerr <<  "slicing transformation timing:" << GetWallTime() << "\n";
}
