
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
#include <cstdio>

using namespace std;

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

void ReportTime(bool doit, const char* msg)
{
   if (doit) {
     struct timeb tb;
     ftime (&tb);
     std::cout << msg << ": " << tb.time << " : " << tb.millitm << std::endl;
    }
}

int OptLevel()
{
  static int level = -1;
  if (level < 0) {
    level = 0;
    vector<string>::const_iterator p = CmdOptions::GetInstance()->GetOptionPosition("-opt");
    if (p != CmdOptions::GetInstance()->opts.end()) {
      string str = p->substr(4);
      if (str.empty()) {
        ++p;
        assert (p != CmdOptions::GetInstance()->opts.end());
        str = *p;
      }
      sscanf(str.c_str(), "%d", &level);
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
              std::cerr << "has declaration " << AstToString(s) << "\n";
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
              std::cerr << "not fortran loop " << AstToString(s) << std::endl;
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
        if (t == AstInterface::PreVisit) 
           return ProcessStmt(fa, s); 
        return true;
    }

 public:
   AstTreeOptimizable( LoopTransformInterface& _la, 
                        LoopTransformOptions::OptType t ) 
     : la(_la), loop(AST_NULL), succ(0), optType(t) { }
   bool operator()( LoopTransformInterface &fa, const AstNodePtr& head)
    {  
       loop = AST_NULL; succ = 0;
       top = head;
       if (optType & LoopTransformOptions::LOOP_DATA_OPT) {
          ProcessAstTree::operator()(fa, head);
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
        bool r = ReadAstTraverse(fa, top, *this, AstInterface::PreAndPostOrder);
        fa.AddNewVarDecls(dest, top);
        return r;
     }
};


bool LoopTransformation( LoopTransformInterface &la, const AstNodePtr& head, AstNodePtr& result)
{
 CmdOptions *opt = CmdOptions::GetInstance();
 bool depOnly = opt->HasOption("-depAnalOnly");
 LoopTransformOptions *lopt = LoopTransformOptions::GetInstance();
 AstTreeOptimizable sel(la,lopt->GetOptimizationType());
 if (!depOnly && !sel(la, head)) {
        return false;
  }

  bool reportPhaseTimings = ReportTiming();
  bool debugloop = DebugLoop(), debugdep = DebugDep();

  if (debugloop) {
    std::cerr <<"----------------------------------------------"<<endl;
    std::cerr << "try applying loop transformation to \n";
    std::cerr << AstToString(head) << std::endl;
  }
  ReportTime(reportPhaseTimings, "Start constructing dependence graph." );
  LoopTreeDepCompCreate comp(la, head);
  ReportTime(reportPhaseTimings, "Finish constructing dependence graph." );
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

  if (!depOnly && sel.PerformTransformation()) {
      LoopTreeLocalityAnal loopAnal(la, comp);
      CopyArrayOperator *cp = LoopTransformOptions::GetInstance()->GetCopyArraySel();
      if (debugdep) 
      {
          std::cerr <<"----------------------------------------------"<<endl;
          std::cerr << "LoopTree input dep graph: \n" << GraphToString(*loopAnal.GetInputGraph()) << std::endl;
      }    
      if (lopt->DoDynamicTuning()) {
           DynamicSlicing op;
           LoopTransformation( la, comp, op, loopAnal, OptLevel(), cp);
      }
      else {
           DependenceHoisting op;
           LoopTransformation( la, comp, op, loopAnal, OptLevel(), cp);
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

  AstInterface &fa = la;
  AstNodePtr r = comp.CodeGen(la);

  result = fa.CreateBlock(head);
  CopyDeclarations copyDecl( result);
  copyDecl( fa, head); 
  assert (r != 0);
  fa.BlockAppendStmt(result, r);

  return true;
}

void RearrangeCompSliceGraph( LoopTransformInterface &fa, LoopTreeDepComp &comp,
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
     (*icOp)( &sliceAnal, iter.Current()->GetInfo());
 }

 if (debugslice) 
   std::cerr << "after interchange: \n" << GraphToString(graph) << std::endl;

 LoopNestFusion* fsOp = opt->GetFusionSel();
 SliceNestTypedFusion( &sliceAnal, graph, *fsOp);
 SliceNestReverseTypedFusion( &sliceAnal, graph, *fsOp);
 if (debugslice) 
   std::cerr << "after fusion: \n" << GraphToString(graph) << std::endl;
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
        std::cerr << "\n from\n";
        top->DumpTree();
  }
  AstInterface& fa = la;
  for ( int j; (j = selSlice.CurrentIndex()) >= 0; selSlice++)  {
    top = op.Transform( la, comp, g[j], top);
    if (debugloop) {
        std::cerr << "\n transforming into\n";
        if (head == 0)
           top->DumpTree();
        else
           head->DumpTree();
    }
    SymbolicVal b = selSlice.GetBlockSize();
    if (!(b == 1)) {
      LoopTreeNode *n = LoopTreeBlockLoop()( top, SymbolicVar(fa.NewVar(fa.GetType("int")), AST_NULL), b);
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
  LoopTransformOptions *lopt = LoopTransformOptions::GetInstance();
  bool reportPhaseTimings = ReportTiming();

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
                   LoopTreeTraverseSelectLoop loops(r, LoopTreeTraverse::PostOrder);
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
                std::cerr << "applying array copy to " << h->TreeToString() << std::endl;
             (*cp)(fa, loopAnal, h->Parent());
         }
      }
      Postprocess(comp);
      ReportTime(reportPhaseTimings, "Finish slicing transformation." );
   }
   else if (cp != 0)
         (*cp)(fa, loopAnal, comp.GetLoopTreeRoot());
}
