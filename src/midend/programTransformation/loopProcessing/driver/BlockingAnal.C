#include <BlockingAnal.h>
#include <LoopTreeTransform.h>
#include <AutoTuningInterface.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

static int SliceNestReuseLevel(CompSliceLocalityRegistry *anal, const CompSliceNest& n)
     {
       unsigned num = n.NumberOfEntries();
       size_t reuseLevel = 0;
       for (; reuseLevel < num; ++reuseLevel) {
          if (anal->TemporaryReuses(n[reuseLevel]) > 0 || anal->SpatialReuses(n[reuseLevel]) > 0)
             break;
       }
       return reuseLevel;
    }

static SymbolicVal GetDefaultBlockSize(const CompSlice* slice)
    {
       AstInterface& fa = LoopTransformInterface::getAstInterface();
       LoopTransformOptions* opt = LoopTransformOptions::GetInstance();
       if (!opt->DoDynamicTuning()) {
            return opt->GetDefaultBlockSize();
       }
       else {
           int dt = opt->GetDynamicTuningIndex();
           AstInterface::AstNodeList l;
           l.push_back(fa.CreateConstInt(dt).get_ptr());

           CompSlice::ConstLoopIterator iter = slice->GetConstLoopIterator();
           LoopTreeNode *loop = iter.Current();
           SymbolicBound b = loop->GetLoopInfo()->GetBound();
           SymbolicVal size = b.ub - b.lb + 1;

           l.push_back(fa.CreateConstInt(1).get_ptr());
           l.push_back(size.CodeGen(fa).get_ptr());
           std::string funname = "getTuningValue";
           AstNodePtr init = fa.CreateFunctionCall(funname, l.begin(), l.end());
           return SymbolicVar(fa.NewVar(fa.GetType("int"), "",true,true,AST_NULL, init),AST_NULL);
       }
    }

const CompSlice* LoopNoBlocking::
SetBlocking(CompSliceLocalityRegistry *anal,
                           const CompSliceDepGraphNode::FullNestInfo& nestInfo)
   {
      const CompSliceNest* n = nestInfo.GetNest();
      int loopnum = n->NumberOfEntries();
      if (loopnum == 0) return 0;
       blocksize.clear();
       for (int i = 0; i < loopnum; ++i)
          blocksize.push_back(1);
       return n->Entry(loopnum-1);
   }

const CompSlice* OuterLoopReuseBlocking ::
SetBlocking( CompSliceLocalityRegistry *anal,
                           const CompSliceDepGraphNode::FullNestInfo& nestInfo)
   {
      const CompSliceNest& n = *nestInfo.GetNest();
      blocksize.clear();
      size_t num = n.NumberOfEntries();
      int reuseLevel = SliceNestReuseLevel(anal, n);
      for (int i = 0; i < reuseLevel; ++i)
            blocksize.push_back(1);
      size_t index;
      for ( index = reuseLevel; index < num-spill; ++index)  {
          blocksize.push_back(GetDefaultBlockSize(n[index]));
      }
      for (; index < num; ++index)
            blocksize.push_back(1);
       return n[num-1];
   }

const CompSlice* InnerLoopReuseBlocking ::
SetBlocking( CompSliceLocalityRegistry *anal,
                           const CompSliceDepGraphNode::FullNestInfo& nestInfo)
   {
      const CompSliceNest& n = *nestInfo.GetNest();
      blocksize.clear();
      unsigned num = n.NumberOfEntries();
      int reuseLevel = SliceNestReuseLevel(anal, n);
      blocksize.resize(reuseLevel + 1, 1);
      for ( size_t index = reuseLevel+1; index < num; ++index)
         blocksize.push_back(GetDefaultBlockSize(n[index]));
      return n[num-1];
}

const CompSlice* AllLoopReuseBlocking ::
SetBlocking( CompSliceLocalityRegistry *anal,
                           const CompSliceDepGraphNode::FullNestInfo& nestInfo)
   {
      const CompSliceNest& n = *nestInfo.GetNest();
      blocksize.clear();
      unsigned num = n.NumberOfEntries();
      if (num == 1) {
          blocksize.push_back(1);
          return n[0];
      }
      int reuseLevel = SliceNestReuseLevel(anal, n);
      for (int i = 0; i < reuseLevel; ++i)
            blocksize.push_back(1);
      for ( size_t index = reuseLevel; index < num; ++index)
           blocksize.push_back(GetDefaultBlockSize(n[index]));
      return n[num-1];
   }


int LoopBlocking:: SetIndex( int num)
     {
        if (block_index > 1) {
          for ( ; num >=0; --num) {
            if (BlockSize( num) != 1)
              break;
          }
          if (num < 0) {
            block_index = 1;
            num = blocksize.size()-1;
          }
        }
        if (block_index <= 1) {
           for ( ; num >=0; num --) {
             if (BlockSize(num) == 1)
                break;
           }
        }
        return num;
      }

extern bool DebugLoop();
LoopTreeNode* LoopBlocking::
apply( const CompSliceDepGraphNode::FullNestInfo& nestInfo,
       LoopTreeDepComp& comp, DependenceHoisting &op, LoopTreeNode *top)
{

  if (nestInfo.GetNest()->NumberOfEntries() <= 0)
          return top;

  bool debugloop = DebugLoop();

  if (debugloop) {
        std::cerr << "\n Apply Loop blocking: from\n";
        top->DumpTree();
  }
  LoopTreeNode* head= ApplyBlocking(nestInfo,comp,op,top);
  if (debugloop) {
      std::cerr << "\n transforming into\n";
      if (head == 0)
          top->DumpTree();
      else
         head->DumpTree();
  }
  return top;
}


LoopTreeNode* LoopBlocking::
ApplyBlocking( const CompSliceDepGraphNode::FullNestInfo& nestInfo,
              LoopTreeDepComp& comp, DependenceHoisting &op, LoopTreeNode *&top)
{
  const CompSliceNest& slices = *nestInfo.GetNest();
  if (DebugLoop()) {
     std::cerr << "\n Blocking slices: " << slices.toString() << "\n";
  }
  LoopTreeNode *head = 0;
  AstInterface& fa = LoopTransformInterface::getAstInterface();
  for (int j = FirstIndex(); j >= 0; j = NextIndex(j))  {
     top = op.Transform( comp, slices[j], top);
     SymbolicVal b = BlockSize(j);
     if (DebugLoop()) {
        std::cerr << "\n after slice " << j << " : \n";
        //top->DumpTree();
        comp.DumpTree();
        comp.DumpDep();
        std::cerr << "\n blocking size for this loop is " << b.toString() << "\n";
     }

     if (!(b == 1)) {
         LoopTreeNode *n = LoopTreeBlockLoop()( top, SymbolicVar(fa.NewVar(fa.GetType("int")), AST_NULL), b);
         if (DebugLoop()) {
            std::cerr << "\n after tiling loop with size " << b.toString() << " : \n";
            //top->DumpTree();
            comp.DumpTree();
            comp.DumpDep();
         }
         if (head == 0)
             head = n;
         else {
           while (n->FirstChild() != head)
              LoopTreeSwapNodePos()( n->Parent(), n);
         }
       }
   }
  return head;
}

const CompSliceDepGraphNode::NestInfo*
DoNonPerfectBlocking(const CompSliceDepGraphNode::FullNestInfo& nestInfo)
{
  const CompSliceDepGraphNode::NestInfo* innerNest = 0;
  const CompSliceDepGraphNode::NestInfo* nonperfect = nestInfo.InnerNest();
  for ( const CompSliceDepGraphNode::NestInfo* p = nonperfect; p != 0;
         p = p->InnerNest()) {
     const CompSliceNest* cur = p->GetNest();
     if (cur != 0) {
        if (innerNest == 0) innerNest = p;
        else { innerNest = 0; break; /*QY: only allow one nest */ }
     }
  }
  return innerNest;
}

const CompSlice* ParameterizeBlocking::
SetBlocking(CompSliceLocalityRegistry *anal,
                           const CompSliceDepGraphNode::FullNestInfo& nestInfo)
{
  const CompSlice* res = AllLoopReuseBlocking::SetBlocking(anal,nestInfo);
  const CompSliceNest* curNest = nestInfo.GetNest();
  unsigned size = curNest->NumberOfEntries();
  if (size <= 1 || !curNest->Entry(size-1)->SliceCommonLoop(curNest->Entry(0)))
  { /*QY: current loop nest is perfectly nested*/
     const CompSliceDepGraphNode::NestInfo* nonperfect = DoNonPerfectBlocking(nestInfo);
     if (nonperfect)
     { /*QY: extra loops inside; (not considered for blocking normally). */
        const CompSliceNest* innerNest = nonperfect->GetNest();
        assert(innerNest!=0); /*QY: this is why nonperfect is returned */
        int reuseLevel = SliceNestReuseLevel(anal, *innerNest);
        int j = 0, size = innerNest->NumberOfEntries();
        for (; j < reuseLevel; ++j) blocksize.push_back(1);
        for (; j < size; ++j) blocksize.push_back(GetDefaultBlockSize(innerNest->Entry(j)));
        res = innerNest->Entry(size-1);
     }
  }
  return res;
}

LoopTreeNode* ParameterizeBlocking::
ApplyBlocking( const CompSliceDepGraphNode::FullNestInfo& nestInfo,
              LoopTreeDepComp& comp, DependenceHoisting &op,
                                      LoopTreeNode *&top)
{
  const CompSliceNest* pslices = nestInfo.GetNest();
  assert(pslices != 0);
  const CompSliceNest& slices = *pslices;
  //AstInterface& fa = LoopTransformInterface::getAstInterface();
  int size = slices.NumberOfEntries();
  assert (size > 0);

  AutoTuningInterface* tuning = LoopTransformInterface::getAutoTuningInterface();
  assert(tuning != 0);

  for (int j=size-1;j >= 0; --j)  //QY: arrange the desired loop nesting order
     top = op.Transform( comp, slices[j], top);

  /*QY: check for non-perfectness which can be solved via loop distribution */
  const CompSliceDepGraphNode::NestInfo* inner = DoNonPerfectBlocking(nestInfo);
  if (inner == 0 && size == 1) { /*QY: all loops are perfectly nested*/ return top;}

  const CompSlice* slice_innermost = slices[size-1], *slice_top=slices[0];
  CompSlice::ConstLoopIterator p_inner
                     = slice_innermost->GetConstLoopIterator();
  LoopTreeNode* loop_innermost=*p_inner;
  if (size > 1)
    while (slice_top->QuerySliceLoop(loop_innermost))
     { ++p_inner; assert(!p_inner.ReachEnd()); loop_innermost = *p_inner; }

  /*QY: this is for triangular non-perfect nests where a single loop is shared by multiple slices; loops cannot be distributed in spite of non-perfectness */
  std::vector<FuseLoopInfo> non_perfects;
  if (size > 1) {
    for (unsigned i = 1; i < static_cast<unsigned>(size); ++i) {
       const CompSlice* slice_inner=slices[i];
       const CompSlice* slice_pivot = slices[i-1];
       CompSlice::ConstLoopIterator p_pivot=slice_pivot->GetConstLoopIterator();
       if  (slice_inner->SliceCommonLoop(slice_pivot)) {
         /*QY: outer loops are not perfectly nested*/
         FuseLoopInfo loops_cur(p_pivot.Current());
         for (LoopTreeTraverseSelectLoop p_inner(top);
         /*for (CompSlice::ConstLoopIterator p_inner=slice_inner->GetConstLoopIterator(); QY: the ordering of loops are not enforced in CompSlice*/
              !p_inner.ReachEnd(); ++p_inner) {
             LoopTreeNode* cur = p_inner.Current();
             if (!slice_inner->QuerySliceLoop(cur) || slice_pivot->QuerySliceLoop(cur)) continue;
             CompSlice::SliceLoopInfo curinfo = slice_inner->QuerySliceLoopInfo(cur);
             loops_cur.loops.push_back(FuseLoopInfo::Entry(cur,curinfo.minalign-p_pivot.CurrentInfo().minalign));
         }
         assert(loops_cur.loops.size() > 0);
         non_perfects.push_back(loops_cur);
       }
     }
  }
#ifdef DEBUG
  std::cerr << "Number of non-perfect entries: " << non_perfects.size() << "\n";
#endif


  if (inner == 0) { /*QY: no inner loops that can be blocked together*/
     if (non_perfects.size() > 0)
         tuning->BlockLoops(top, loop_innermost, this, &non_perfects);
     else tuning->BlockLoops(top, loop_innermost, this);
  }
  else {
      LoopTreeNode* innerTop = LoopTreeTransform().InsertHandle(loop_innermost,1);
      /*QY: need to call GenXformRoot for each innerNest before inner*/
      for (const CompSliceDepGraphNode::NestInfo* p = nestInfo.InnerNest();
           p != 0 ;  p = p->InnerNest()) {
         LoopTreeNode* curTop = p->GenXformRoot(innerTop);
         assert(curTop != 0);
         if (p == inner) { innerTop = curTop; break; }
      }
      const CompSliceNest* innerNest=inner->GetNest();
      assert(innerNest!=0);
      for (int j = innerNest->NumberOfEntries()-1; j >= 0; --j)
      {
         innerTop = op.Transform( comp, innerNest->Entry(j),innerTop);
      }
      /*QY: inner loops that are not involved in outer slice fusion*/
      LoopTreeNode* inner2=0;
      for (int i = 0; i < innerNest->NumberOfEntries(); ++i) {
         CompSlice::ConstLoopIterator p_inner2 = innerNest->Entry(i)->GetConstLoopIterator();
         inner2 = p_inner2.Current();
         FuseLoopInfo cur;
         for ( ; !p_inner2.ReachEnd(); ++p_inner2)
          {
             cur.loops.push_back(FuseLoopInfo::Entry(p_inner2.Current(),p_inner2.CurrentInfo().minalign));
          }
          non_perfects.push_back(cur);
      }
      tuning->BlockLoops(top, inner2, this, &non_perfects);
  }
  return top;
}

