
#include <stdlib.h>

#include <CompSlice.h>

#include <DepRel.h>
#include <LoopAnalysis.h>
#include <DepGraph.h>
#include <SymbolicVal.h>
#include <DepCompTransform.h>
#include <LoopTreeTransform.h>
#include <LoopInfoInterface.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

struct SliceInfo
{
  LoopTreeNode* loop, *stmt;
  int looplevel;
  bool reversible;
  LoopAlignInfo alignInfo;

  SliceInfo(LoopTreeNode *s = 0, LoopTreeNode *l=0, int level=-1,
                bool r=false, int mina=1, int maxa = -1)
    : loop(l), stmt(s), looplevel(level), reversible(r), alignInfo(mina,maxa) {}
  void SetLoop(LoopTreeNode *l, int level) { loop=l; looplevel=level; }
  operator bool() { return alignInfo; }
  void write(std::ostream& out) const
   { out << "slice stmt: \n";
     stmt->write(out);
     out << "slice loop: \n";
     loop->write(out);
     out << "alignment: " << alignInfo.mina << "->" << alignInfo.maxa << "\n";
   }
};

class TransSlicingAnal
{
  SliceInfo* sliceLoops;
  LoopAlignInfo* fuseInfo;
  int size, maxsize;
 public:
  void Reset( int m = 0, int s = 0)
    {
       assert(m >= s);
       if (m != maxsize) {
          if (sliceLoops != 0)
             delete [] sliceLoops;
          if (fuseInfo != 0)
             delete [] fuseInfo;
       }
       maxsize = m;
       if (s > 0 && size == 0) {
         sliceLoops = new SliceInfo[maxsize];
         fuseInfo = new LoopAlignInfo[ maxsize * maxsize / 2 - 1];
      }
      size = s;
    }
  int FuseInfoOffset( int i1, int i2) const
   {
      return i1 * maxsize / 2 + i2 -1;
   }

  TransSlicingAnal() : sliceLoops(0), fuseInfo(0), size(0), maxsize(0) {}
  ~TransSlicingAnal() { Reset(); }
  TransSlicingAnal& operator = (const TransSlicingAnal& that)
   {
      Reset( that.maxsize, that.size);
      for (int i = 0; i < size; ++i) {
         sliceLoops[i] = that.sliceLoops[i];
          for (int j = i+1; j < size; ++j) {
              int offset = FuseInfoOffset(i,j);
              fuseInfo[offset] = that.fuseInfo[offset];
          }
      }
      return *this;
   }
  TransSlicingAnal(const TransSlicingAnal& that)
   {
      Reset( that.maxsize, that.size);
      for (int i = 0; i < size; ++i) {
         sliceLoops[i] = that.sliceLoops[i];
          for (int j = i+1; j < size; ++j) {
              int offset = FuseInfoOffset(i,j);
              fuseInfo[offset] = that.fuseInfo[offset];
          }
      }
   }

  bool LoopSlicible( LoopTreeDepComp& comp, LoopTreeTransDepGraphCreate *tg,
                        SliceInfo& curloop, LoopAlignInfo* _fuseInfo)
   {
    typedef TransLoopSlicable<LoopTreeDepGraphNode> TransLoopSlicable;
    typedef TransLoopReversible<LoopTreeDepGraphNode> TransLoopReversible;
    typedef LoopAnalInfo<LoopTreeDepGraphNode> LoopAnalInfo;

    LoopAnalInfo anal1( comp.GetDepNode(curloop.stmt), curloop.looplevel);
    if ( ! TransLoopSlicable()( tg, anal1) )
       return false;
    curloop.reversible = TransLoopReversible()(tg,anal1);
    if (size == 0)
       curloop.alignInfo.Set(0,0);
    else {
      curloop.alignInfo.Set(NEG_INFTY, POS_INFTY);
      for (int i = 0; i < size; ++i) {
        SliceInfo& loop2 = sliceLoops[i];
        LoopAnalInfo anal2( comp.GetDepNode(loop2.stmt), loop2.looplevel);
        LoopAlignInfo fuse=
            (loop2.loop == curloop.loop)?
             LoopAlignInfo(0,0) :
             TransLoopFusible<LoopTreeDepGraphNode>()(tg, anal1,anal2);
        if (!fuse)
           return false;
        curloop.alignInfo &= loop2.alignInfo + fuse;
        _fuseInfo[i] = fuse;
      }
    }
    return true;
   }
   void CommitSliceInfo( const SliceInfo &curloop, LoopAlignInfo* _fuseInfo)
   {
      Reset(maxsize, size+1);
      sliceLoops[size-1] = curloop;
      for (int i = 0; i < size-1; ++i) {
        SliceInfo& loop2 = sliceLoops[i];
        loop2.alignInfo &= curloop.alignInfo - _fuseInfo[i];
        fuseInfo[FuseInfoOffset(i,size-1)] = _fuseInfo[i];
      }
   }
   unsigned NumberOfSliceStmts() const { return size; }
   LoopTreeNode* SliceStmt(int i) { return sliceLoops[i].stmt; }
   SliceInfo& SliceLoop(int i) { return sliceLoops[i]; }
   LoopTreeNode* LastSliceStmt() { return sliceLoops[size-1].stmt; }
   void write(std::ostream& out) const;
};

void TransSlicingAnal:: write(std::ostream& out) const
{
      for (int i = 0; i < size; ++i) {
         sliceLoops[i].write(out);
      }
}

void GetLoopTreeSize( LoopTreeNode* root, int& stmtnum, int& stmtlevel)
{
  stmtnum = 0;
  stmtlevel = 0;
  for ( LoopTreeTraverseSelectStmt stmtIter(root);
        !stmtIter.ReachEnd(); ++stmtIter) {
      ++stmtnum;
      int i = stmtIter.Current()->LoopLevel();
      if (stmtlevel < i)
           stmtlevel = i;
  }
}

void DependenceHoisting ::
Analyze( LoopTreeDepComp &comp, LoopTreeTransDepGraphCreate *tg,
         CompSliceNest& result)
{
  LoopTreeInterface interface;
  LoopTreeNode *root = comp.GetLoopTreeRoot();
  int rootlevel = root->LoopLevel();
  int size, slicesize;
  GetLoopTreeSize(root, slicesize, size);
  size -= rootlevel;
  size *= slicesize;

  result.Reset(size);
  TransSlicingAnal* tmpSlices = new TransSlicingAnal[size];
  LoopAlignInfo* buf1 = new LoopAlignInfo[slicesize], *buf2 = new LoopAlignInfo[slicesize];
  for (int i = 0; i < size; ++i)
    tmpSlices[i].Reset(slicesize);

  /* QY: 6/2009: create a tmporary slice for each individual slicable loop;
     i.e. for each loop that can be placed at the outermost position */
  LoopTreeTraverseSelectStmt stmtIter(root);
  LoopTreeNode *stmt = stmtIter.Current();
  if (stmt == 0) return;
  size = 0;
  int index = stmt->LoopLevel()-1;
  for (LoopTreeNode *loop = GetEnclosingLoop(stmt, interface);
       index >= rootlevel; loop = GetEnclosingLoop(loop, interface)) {
    SliceInfo curloop(stmt, loop, index--);
    TransSlicingAnal anal;
    if (anal.LoopSlicible( comp, tg, curloop, buf1)) {
       tmpSlices[size++].CommitSliceInfo(curloop, buf1);
    }
  }

  /*QY:6/2009: try to expand each tmpSlice to include all statements */
  for (int sliceindex = 0; sliceindex < size; ++sliceindex) {
    TransSlicingAnal &anal = tmpSlices[sliceindex];
    stmtIter.Current() = anal.LastSliceStmt();
    /* QY:for each stmt, find a surrounding slicing loop to go with
       the loops already in the current slice (stored in anal) */
    for ( stmtIter++; (stmt= stmtIter.Current()); stmtIter++) {
      SliceInfo curloop(stmt);
      index = stmt->LoopLevel()-1;
      LoopTreeNode *loop = GetEnclosingLoop(stmt, interface);
      for ( ; index >= rootlevel; loop = GetEnclosingLoop(loop, interface)) {
        curloop.SetLoop(loop, index--);
        if (anal.LoopSlicible( comp, tg, curloop, buf1))
           break;
      }
      if (loop == 0) /* QY: no slicable loop has been found for stmt */
         { break; }
      else {
         for (loop = GetEnclosingLoop(loop, interface);
              index >= rootlevel; loop = GetEnclosingLoop(loop, interface)) {
           SliceInfo curloop1(stmt, loop,index--);
           if (anal.LoopSlicible( comp, tg, curloop1, buf2) ) {
              tmpSlices[size++] = anal;
              tmpSlices[size-1].CommitSliceInfo( curloop1, buf2);
           }
         }
         /* commit the current loop/stmt as part of the current slice */
         anal.CommitSliceInfo(curloop, buf1);
      }
    }
    if (int(anal.NumberOfSliceStmts()) == slicesize) {
       /* QY: a slice is found that includes all statements */
      CompSlice *slice = CreateCompSlice(rootlevel);
      for (int i = 0; i < slicesize; ++i) {
        SliceInfo& info = anal.SliceLoop(i);
        slice->SetSliceLoop(info.stmt, info.loop, info.reversible,
                            info.alignInfo.mina);
      }
      result.Append( slice );
    }
  }
  delete[] tmpSlices;
  delete[] buf1;
  delete[] buf2;
}

void DependenceHoisting ::
Analyze( LoopTreeDepComp &comp, CompSliceNest& result)
{
  typedef PerfectLoopSlicable<DepInfoEdge,LoopTreeDepGraph> LoopSlicable;
  typedef PerfectLoopReversible<DepInfoEdge,LoopTreeDepGraph>
          LoopReversible;

  LoopTreeNode *root = comp.GetLoopTreeRoot();
  int rootlevel = root->LoopLevel(), index = rootlevel-1;
  int stmtnum, stmtlevel;
  GetLoopTreeSize(root, stmtnum, stmtlevel);
  if (!stmtnum)
    return;
  result.Reset(stmtlevel - rootlevel );

  LoopTreeDepGraph *dg = comp.GetDepGraph();
  for (LoopTreeNode *n = root;
       n; n = (n->ChildCount() == 1)? n->FirstChild() : 0) {
     if (!n->IncreaseLoopLevel())
        continue;
     index++;
     if (!LoopSlicable()(dg, index))
        continue;
     CompSlice *slice = CreateCompSlice( rootlevel );
     bool r = LoopReversible()(dg, index);
     LoopTreeTraverseSelectStmt stmtIter(n);
     for (LoopTreeNode *s; (s = stmtIter.Current()); stmtIter++)
       slice->SetSliceLoop( s, n, r, 0);
     result.Append(slice);
  }
}

LoopTreeNode* DependenceHoisting::
Transform ( LoopTreeDepComp &comp,
            const CompSlice *slice, LoopTreeNode *h1)
{
  bool alreadySliced = true;
  for (CompSlice::ConstLoopIterator iter = slice->GetConstLoopIterator();
        iter.Current() != 0; iter.Advance()) {
      if (iter.Current() != h1) { alreadySliced = false; break; }
  }
  if (alreadySliced) return h1;

  SymbolicVar ivar = SliceLoopIvar( LoopTransformInterface::getAstInterface(), slice);
  SymbolicVal step = SliceLoopStep( slice).step;
  SymbolicBound bound = SliceLoopRange( slice, h1);
  LoopTreeNode *h2 = (step > 0)? comp.GetLoopTreeCreate()->CreateLoopNode(ivar,bound.lb,bound.ub,step)
                               : comp.GetLoopTreeCreate()->CreateLoopNode(ivar,bound.ub,bound.lb,step);
  LoopTreeTransform().InsertLoop(h2, h1, -1);

  CompSlice::ConstStmtIterator stmtIter=slice->GetConstStmtIterator();
  for (LoopTreeNode *stmt; (stmt = stmtIter.Current()); stmtIter++) {
      CompSlice::SliceStmtInfo info = stmtIter.CurrentInfo();
      LoopTreeMergeStmtLoop()( h2, info.loop, stmt, info.align);
  }

  CompSlice::UpdateLoopIterator loopIter= slice->GetUpdateLoopIterator();
  LoopTreeNode* loop;
  while ((loop = loopIter.Current())) {
     loopIter++;
     DepCompDistributeLoop()(comp,loop);
  }
  for (loopIter.Reset(); (loop = loopIter.Current()); loopIter++) {
    if (loopIter.CurrentInfo().stmtcount < CountEnclosedStmts(loop)){
      CompSlice::SliceLoopInfo info = loopIter.CurrentInfo();
      LoopTreeTraverseSelectStmt inStmts(loop);
      for (LoopTreeNode *s; (s = inStmts.Current()); ) {
        inStmts.Advance();
        CompSlice::SliceStmtInfo info1(slice->QuerySliceStmtInfo(s));
        if (info1.loop != loop)  {
          DepRel r ( DEPDIR_LE, info1.align - info.minalign);
          DepRel r1 = comp.GetDomain(s).Entry(info1.loop->LoopLevel(), loop->LoopLevel());
          DepRel r2 = r & r1;
          if (r2 != r1 && !r2.IsTop())
              LoopTreeSplitStmt()( s, info1.loop, loop, r);
        }
      }
      DepCompDistributeLoop()(comp, loop);
      loop = loopIter.Current();
      while (loopIter.CurrentInfo().stmtcount < CountEnclosedStmts(loop)) {
        while (loop->ChildCount() == 1) {
          LoopTreeNode* child = loop->FirstChild();
          LoopTreeSwapNodePos()( loop, child);
          if (child->IncreaseLoopLevel())
            break;
        }
        DepCompDistributeLoop()( comp, loop );
      }
    }
  }
  OptimizeLoopTree(h2);
  return h2;
}

