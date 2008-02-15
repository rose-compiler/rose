
#include <general.h>
#include <sys/timeb.h>

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
#include <CopyArrayAnal.h>
#include <LoopTransformOptions.h>
#include <GraphIO.h>

// DQ (7/31/2006): Added to support gettimeofday
#include <sys/time.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

void LoopTransformation( LoopTransformInterface &fa, LoopTreeDepComp& comp,
                         DependenceHoisting& op, LoopTreeLocalityAnal &anal,
                         int optlevel, CopyArrayOperator* cp = 0);

bool DebugDep()
{
  static int r = 0;
  if (r == 0) {
      if (CmdOptions::GetInstance()->HasOption("-debugdep"))
           r = 1;
      else
           r = -1;
  }
  return 1 /* r == 1 */;
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
  return 1 /* r == 1 */;
}
bool ReportTiming()
{
  return CmdOptions::GetInstance()->HasOption("-tmloop");
}

bool ApplyLoopSplitting()
{
  return CmdOptions::GetInstance()->HasOption("-splitloop");
}

void ReportTime(Boolean doit, const char* msg)
{
   if (doit) {

  // DQ (7/31/2006): replaced ftime with gettimeofday
  // since ftime is now deprecated (even on linux) and 
  // replaced by gettimeofday (required for Mac OSX portability)
  // struct timeb tb;
  // ftime (&tb);
  // cout << msg << ": " << tb.time << " : " << tb.millitm << endl;
     struct timeval tp;
     struct timezone* tzp = NULL;
     int status = gettimeofday (&tp,tzp);
     assert(status == 0);
     cout << msg << ": " << tp.tv_sec << " : " << tp.tv_usec << endl;
    }
}

int OptLevel()
{
  static int level = -1;
  if (level < 0) {
    level = 0;
    const vector<string>& opts = CmdOptions::GetInstance()->GetOptions();
    unsigned int p = CmdOptions::GetInstance()->HasOption("-opt");
    if (p != 0) {
       // p now points to the NEXT argument
       if (p != opts.size()) {
	 sscanf(opts[p].c_str(), "%d", &level);
       }
    }
  }
  return level;
}

class AstTreeOptimizable : public ProcessAstTree
{
  private:
   LoopTransformInterface& la;
   AstNodePtr loop, top;
   int succ;
   int optType;

   Boolean ProcessFunctionDefinition( AstInterface &fa, const AstNodePtr& s,
                                      const AstNodePtr& body,
                                      AstInterface::TraversalVisitType t) 
   {
        succ = -1;
        return false;
   }

   Boolean ProcessDecls(AstInterface &fa, const AstNodePtr& s)
     {
        if (fa.IsVariableDecl(s) && fa.GetParent(s) != top) {
            succ = -1;
            return false;
        } 
        return ProcessAstTree::ProcessDecls(fa, s);
     }
   Boolean ProcessLoop(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& body,
                        AstInterface::TraversalVisitType t) 
    {
        if (DebugLoop()) {
          cerr << "starting ProcessLoop " << succ << " " << AstInterface::AstToString(s) << endl;
        }
        if (succ < 0)
           return false;
        if (!la.IsFortranLoop(s)) {
           if (DebugLoop()) {
              cerr << "not fortran loop ";
              fa.DumpAst(s);
              cerr << endl;
           }
           succ = -1;
           return false;
        }
        if (t == AstInterface::PreVisit) {
           if (loop == 0)
              loop = s;
           else if (loop != 0 && !(optType & LoopTransformOptions::LOOP_DATA_OPT)) {
              if (DebugLoop())
                  cerr << "no optimization specified \n";
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
  Boolean ProcessGoto(AstInterface &fa, const AstNodePtr& s, const AstNodePtr& dest)
     {  
        if (succ < 0)
           return false;
        loop = 0; 
        return ProcessAstTree::ProcessGoto(fa, s, dest);
     }
  void ProcessIf(AstInterface &fa, const AstNodePtr& s, 
                   const AstNodePtr& cond, const AstNodePtr& truebody,
                   const AstNodePtr& falsebody, 
                   AstInterface::TraversalVisitType t) 
    { 
        if (succ < 0)
           return;
        if (t == AstInterface::PreVisit) {
           ProcessStmt(fa, s); 
	   return;
	}
        return;
    }

 public:
   AstTreeOptimizable( LoopTransformInterface& _la, 
                        LoopTransformOptions::OptType t ) 
     : la(_la), loop(0), succ(0), optType(t) { }
   Boolean operator()( LoopTransformInterface &fa, const AstNodePtr& head)
    {  
       loop = 0; succ = 0;
       top = head;
       if (optType & LoopTransformOptions::LOOP_DATA_OPT) {
          ProcessAstTree::operator()(fa, head);
          if (succ <= 0) {
             optType &= (~LoopTransformOptions::LOOP_DATA_OPT);
          }
       }
       
       return succ == 1;
    }
    Boolean PerformTransformation() const
    {
       return optType & LoopTransformOptions::LOOP_DATA_OPT;
    }
    Boolean PerformLoopTransformation() const
    {
       return optType & LoopTransformOptions::LOOP_OPT;
    }
    Boolean PerformDataTransformation() const
    {
       return optType & LoopTransformOptions::DATA_OPT;
    }

};

class CopyDeclarations : public ProcessAstTree
{
  AstNodePtr dest;
 protected:
  virtual Boolean ProcessLoop(AstInterface &fa, const AstNodePtr& s,
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
  Boolean ProcessDecls( AstInterface& fa, const AstNodePtr& decl)
  {
    AstNodePtr ndecl = fa.CopyAstTree(decl);
    fa.BasicBlockAppendStmt( dest, ndecl);
    return ProcessAstTree::ProcessDecls( fa, decl);
  }
  void ProcessStmt(AstInterface &fa, const AstNodePtr& s)
  {
     if (fa.IsVariableDecl(s)) 
         ProcessDecls(fa, s);
  }

 public:
  CopyDeclarations( const AstNodePtr& d) : dest(d) {}
  Boolean operator ()(AstInterface& fa, const AstNodePtr& top)
     {
        return ReadAstTraverse(fa, top, *this, AstInterface::PreAndPostOrder);
     }
};


Boolean LoopTransformation( LoopTransformInterface &la, const AstNodePtr& head, AstNodePtr& result)
{
  Boolean debugloop = DebugLoop(), debugdep = DebugDep();
  if (debugloop) {
    cerr << "Starting LoopTransformation on " << AstInterface::AstToString(head) << endl;
  }
  // CmdOptions *opt = CmdOptions::GetInstance();
  LoopTransformOptions *lopt = LoopTransformOptions::GetInstance();
  AstTreeOptimizable sel(la,lopt->GetOptimizationType());
  if (!sel(la, head)) {
    if (debugloop) {
      cerr << "This loop is not optimizable" << endl;
    }
    return false;
  }

  Boolean reportPhaseTimings = ReportTiming();

  if (debugloop) {
    cerr << "try applying loop transformation to \n";
    AstInterface::DumpAst(head);
    cerr << endl;
  }
  ReportTime(reportPhaseTimings, "Start constructing dependence graph." );
  LoopTreeDepCompCreate comp(la, head);
  ReportTime(reportPhaseTimings, "Finish constructing dependence graph." );
  if (debugloop) {
    cerr << "original LoopTree : \n";
     comp.DumpTree();
  }
  if (debugdep) {
     cerr << "LoopTree dependence graph: \n";
     comp.DumpDep();
  }

  if (sel.PerformTransformation()) {
      LoopTreeLocalityAnal loopAnal(la, comp);
      CopyArrayOperator *cp = LoopTransformOptions::GetInstance()->GetCopyArraySel();
      if (debugdep) 
          cerr << "LoopTree input dep graph: \n" << GraphToString(*loopAnal.GetInputGraph()) << endl;
      if (lopt->DoDynamicTuning()) {
           DynamicSlicing op;
           LoopTransformation( la, comp, op, loopAnal, OptLevel(), cp);
      }
      else {
           DependenceHoisting op;
           LoopTransformation( la, comp, op, loopAnal, OptLevel(), cp);
      }
      if (debugloop) {
            cerr << "\n final LoopTree : \n";
            comp.DumpTree();
      }
     if (debugdep) {
        cerr << "final dependence graph: \n";
        comp.DumpDep();
     }
  }
     
  comp.DetachDepGraph();
  if (ApplyLoopSplitting())
    ApplyLoopSplitting(comp.GetLoopTreeRoot());

  cerr << "Before LoopTransformation call to CodeGen" << endl;
  AstInterface &fa = la;
  AstNodePtr r = comp.CodeGen(la);
  cerr << "Got " << r << " back from CodeGen" << endl;

  cerr << "Creating basic block from " << head << endl;
  result = fa.CreateBasicBlock(head);
  cerr << "Created basic block is " << result << endl;
  CopyDeclarations copyDecl( result);
  copyDecl( fa, head); 
  assert (r != 0);
  fa.BasicBlockAppendStmt(result, r);

  std::cerr << "After copy in LoopTransformation: head = " << head << ", result = " << result << std::endl;

  return true;
}

void RearrangeCompSliceGraph( LoopTransformInterface &fa, LoopTreeDepComp &comp,
                        CompSliceDepGraphCreate& graph,
                        CompSliceLocalityRegistry &sliceAnal)
{
 bool debugslice = CmdOptions::GetInstance()->HasOption("-debugslice");
 if (debugslice) 
   cerr << "computation slice graph: \n" << GraphToString(graph) << endl;
 LoopTransformOptions *opt = LoopTransformOptions::GetInstance();

 ArrangeNestingOrder *icOp = opt->GetInterchangeSel();
 for (CompSliceDepGraphNodeIterator iter = graph.GetNodeIterator();
         !iter.ReachEnd(); ++iter) {
     (*icOp)( &sliceAnal, iter.Current()->GetInfo());
 }

 if (debugslice) 
   cerr << "after interchange: \n" << GraphToString(graph) << endl;

 LoopNestFusion* fsOp = opt->GetFusionSel();
 SliceNestTypedFusion( &sliceAnal, graph, *fsOp);
 SliceNestReverseTypedFusion( &sliceAnal, graph, *fsOp);
 if (debugslice) 
   cerr << "after fusion: \n" << GraphToString(graph) << endl;
}

class SelectSlice
{
  LoopBlockingAnal* anal;
  CompSliceNest& g;
  int block;
  int index, first;
  int SetIndex( int num)
     {
        if (block > 1) {
          for ( ; num >=0; --num) {
            if (anal != 0 && !(anal->GetBlockSize( num) == 1))
              break;
          }
          if (num < 0) {
            block = 1;
            num = g.NumberOfEntries()-1;
          }
        }
        if (block <= 1) {
           for ( ; num >=0; num --) {
             if (anal == 0 || anal->GetBlockSize(num) == 1)
                break;
           }
        }
        return num;
      }
 public:
  SelectSlice( CompSliceNest& _g, LoopBlockingAnal* a = 0)
     : anal(a), g(_g)
    { block = (a == 0)? 1 : 2; first = index = SetIndex(g.NumberOfEntries()-1); }

  int CurrentIndex() { return index; }
  int FirstIndex() { return first; }
  void operator ++(int)
      {  if (index >= 0) { index = SetIndex( index - 1); } }
  SymbolicVal GetBlockSize() 
   { 
      if (anal != 0 && block > 1) 
	  return anal->GetBlockSize(index);
       else
	  return 1; 
   }
} ;

// return the root of blocked loops (excluding block enumerating loops)
LoopTreeNode* SliceNestTransform( LoopTransformInterface &la, LoopTreeDepComp& comp, 
                         DependenceHoisting &op,
                         LoopTreeNode *top, CompSliceNest& g, SelectSlice &selSlice)
{
  if (g.NumberOfEntries() <= 0) 
          return top;

  bool debugloop = DebugLoop();

  LoopTreeNode *head = 0;
  if (debugloop) {
        cerr << "\n from\n";
        top->DumpTree();
  }
  AstInterface& fa = la;
  for ( int j; (j = selSlice.CurrentIndex()) >= 0; selSlice++)  {
    top = op.Transform( la, comp, g[j], top);
    if (debugloop) {
        cerr << "\n transforming into\n";
        if (head == 0)
           top->DumpTree();
        else
           head->DumpTree();
    }
    SymbolicVal b = selSlice.GetBlockSize();
    if (!(b == 1)) {
      LoopTreeNode *n = LoopTreeBlockLoop()( top, SymbolicVar(fa.NewVar(fa.GetType("int")), 0), b);
      if (head == 0)
          head = n;
      else {
        while (n->FirstChild() != head)
           LoopTreeSwapNodePos()( n->Parent(), n);
      }
    }
  }
  return top;
}

void Preprocess( LoopTreeDepComp& comp )
{
  LoopTreeNode *root = comp.GetLoopTreeRoot();
  for (LoopTreeNode *n = root->FirstChild(); n != 0; n = n->NextSibling()) {
     if (n->ContainLoop())
         DepCompDistributeLoop()(comp, n);
  } 
}

void Postprocess( LoopTreeDepComp& comp )
{
  LoopTreeNode *root = comp.GetLoopTreeRoot();
     OptimizeLoopTree(root);
}

void LoopTransformation( LoopTransformInterface &fa, LoopTreeDepComp& comp,
                         DependenceHoisting &op, LoopTreeLocalityAnal &loopAnal,
                         int optlevel, CopyArrayOperator *cp)
{
  if (DebugLoop()) {
    cerr << "Starting LoopTransformation/6" << endl;
  }
  LoopTransformOptions *lopt = LoopTransformOptions::GetInstance();
  Boolean reportPhaseTimings = ReportTiming();

  if (lopt->GetOptimizationType() & LoopTransformOptions::LOOP_OPT) {
     LoopBlockingAnal* block = lopt->GetBlockSel();

     ReportTime(reportPhaseTimings, "Start slicing analysis..." );
     LoopTreeTransDepGraphCreate tc(comp.GetDepGraph(), lopt->GetTransAnalSplitLimit());

     Preprocess(comp);
     CompSliceDepGraphCreate sliceGraph(comp, op, &tc);
     ReportTime(reportPhaseTimings, "Finish slicing analysis");

     ReportTime(reportPhaseTimings, "Start Transformation analysis..." );
     CompSliceLocalityRegistry sliceAnal(loopAnal, lopt->GetCacheLineSize(), lopt->GetReuseDistance());
     RearrangeCompSliceGraph(fa, comp, sliceGraph, sliceAnal);
     ReportTime(reportPhaseTimings, "Finish transformation analysis." );

     ReportTime(reportPhaseTimings, "Start slicing transformation." );
     sliceGraph.TopoSort();
     LoopTreeTransform().InsertHandle(comp.GetLoopTreeRoot(),1);
     for (CompSliceDepGraphCreate::NodeIterator iter = sliceGraph.GetNodeIterator();
          !iter.ReachEnd(); ++iter) {
         CompSliceDepGraphNode *n = iter.Current();
         LoopTreeNode *h = LoopTreeDistributeNode()(n->GetSliceRootIterator());

         CompSliceNest& g = n->GetInfo();
         int num = g.NumberOfEntries();
         if (num > 0) {
             if (block != 0) 
                block->SetBlocking(fa, &sliceAnal, g);
             SelectSlice selSlice(g, block);
             h = SliceNestTransform( fa, comp, op, h, g, selSlice);
             if (optlevel-- > 0) {
                const CompSlice *slice = g[selSlice.FirstIndex()];
                for (CompSlice::ConstLoopIterator p = slice->GetConstLoopIterator();
                     !p.ReachEnd(); ++p) {
                   LoopTreeNode *r = p.Current();
                   LoopTreeTraverseSelectLoop loops( LoopTreeTraverse(r, LoopTreeTraverse::PostOrder));
                   if (!slice->QuerySliceLoop(loops.Current()) ) {
                      r = LoopTreeTransform().InsertHandle(r,1);
                      LoopTreeDepCompSubtree scope(comp, r);
                      LoopTransformation(fa, scope, op, loopAnal, optlevel); 
                   }
                }
             }
         }
         if (cp != 0) {
             LoopTreeTransform().InsertHandle(h,-1);
             if (DebugLoop())
                cerr << "applying array copy to " << h->TreeToString() << endl;
             (*cp)(fa, loopAnal, h->Parent());
         }
      }
      Postprocess(comp);
      ReportTime(reportPhaseTimings, "Finish slicing transformation." );
   }
   else if (cp != 0)
         (*cp)(fa, loopAnal, comp.GetLoopTreeRoot());
}
