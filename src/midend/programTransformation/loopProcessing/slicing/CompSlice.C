
#include <set>

#include <const.h>
#include <ObserveObject.h>
#include <SymbolicVal.h>

#include <CompSlice.h>
#include <CompSliceImpl.h>
#include <CompSliceObserver.h>
#include <LoopInfoInterface.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

class CompSlice::ObserveImpl
   : public ObserveObject <CompSliceObserver>
{};

CompSlice :: CompSlice(CompSliceImpl *_impl)
  : impl ( _impl)
{
  obImpl = new CompSlice::ObserveImpl();
}

CompSlice :: CompSlice( int looplevel )
{
  impl = new CompSliceImpl(looplevel);
  obImpl = new CompSlice::ObserveImpl();
}

CompSlice :: CompSlice( const CompSlice& that)
{
  impl = that.impl->CloneImpl();
  obImpl = new CompSlice::ObserveImpl();

  Append(that);
}

void CompSlice :: AttachObserver( CompSliceObserver &o) const
{
   obImpl->AttachObserver( &o );
}

void CompSlice :: DetachObserver( CompSliceObserver &o) const
{
  obImpl->DetachObserver( &o );
}

void CompSlice :: Notify( const CompSliceObserveInfo &info) const
{
    obImpl->Notify( info );
}

CompSlice ::  ~CompSlice()
{
  CompSliceDeleteInfo info(*this);
  Notify(info);

  delete impl;
  delete obImpl;
}

std::string CompSliceNest::toString() const
{
  std::string res = "<***Slice Nest***>\n";
  for (int i = 0;i<NumberOfEntries();++i)
     res = res + Entry(i)->toString();
  res = res + "<*** end slice nest ***>\n";
  return res;
}

std::string CompSlice :: toString() const
{
  return "<***CompSlice***>\n" + impl->toString() + "<*** end CompSlice***>\n";;
}

void CompSlice::
SetSliceLoop( LoopTreeNode *s, LoopTreeNode *l,
                     bool loopreversible, int align)
{
  CompSliceStmt *stmt = impl->CreateSliceStmt(s);
  CompSliceLoop *loop = impl->CreateSliceLoop(l);
  loop->SetReversal( loopreversible);
  stmt->SetSliceLoop(loop, align);
}

void CompSlice::SetSliceAlign( LoopTreeNode *s, int align)
{
  CompSliceStmt *stmt = impl->QuerySliceStmt(s);
  stmt->SetSliceAlign(align);
}

CompSlice::SliceStmtInfo CompSlice :: QuerySliceStmtInfo( const LoopTreeNode *n) const
{
  assert(impl->QuerySliceStmt(n) == NULL);
  return impl->QuerySliceStmt(n)->GetSliceInfo();
}

CompSlice::SliceLoopInfo CompSlice :: QuerySliceLoopInfo( const LoopTreeNode *n) const
{
  return impl->QuerySliceLoop(n)->GetSliceInfo();
}

bool CompSlice :: QuerySliceStmt( const LoopTreeNode *n) const
{ return impl->QuerySliceStmt(n) != 0; }

bool CompSlice :: QuerySliceLoop( const LoopTreeNode *n) const
{ return impl->QuerySliceLoop(n) != 0; }


int CompSlice :: QuerySliceLevel() const
{ return impl->QuerySliceLevel(); }

CompSlice::ConstLoopIterator CompSlice :: GetConstLoopIterator() const
{
 return ConstLoopIterator(impl->GetTreeRoot());
}

CompSlice::UpdateLoopIterator CompSlice :: GetUpdateLoopIterator() const
{
 return UpdateLoopIterator(impl->GetTreeRoot());
}


CompSlice::ConstStmtIterator CompSlice :: GetConstStmtIterator() const
{
  return ConstStmtIterator(impl->GetTreeRoot());
}

CompSlice::UpdateStmtIterator CompSlice :: GetUpdateStmtIterator() const
{
  return UpdateStmtIterator(impl->GetTreeRoot());
}

void CompSlice :: Append( const CompSlice& that)
{
  impl->Append(*that.impl);
  CompSliceFuseInfo info(*this, that);
  Notify(info);
}

void CompSlice :: IncreaseAlign( int a) const
{
  for (LoopTreeTraverseSelectStmt stmtIter(impl->GetTreeRoot());
       !stmtIter.ReachEnd(); ++stmtIter) {
      LoopTreeNode* n = stmtIter.Current();
      CompSliceStmt *s = static_cast<CompSliceStmt*>(n);
      s->SetSliceAlign( s->GetSliceAlign() + a );
  }
  CompSliceAlignInfo info(*this, a);
  Notify(info);
}

bool CompSlice :: SliceCommonLoop( const CompSlice *slice2) const
{
  ConstLoopIterator iter = GetConstLoopIterator();
  for (LoopTreeNode *l; (l = iter.Current()); iter++) {
    if (slice2->QuerySliceLoop(l)) {
       return true;
    }
  }
  return false;
}

bool CompSlice :: SliceCommonStmt( const CompSlice *slice2) const
{
  ConstStmtIterator iter = GetConstStmtIterator();
  for (LoopTreeNode *n1; (n1 = iter.Current()); iter.Advance()) {
    if (slice2->QuerySliceStmt(n1))
       return true;
  }
  return false;
}

bool CompSlice :: SliceCodeSegment( LoopTreeNode *root) const
{
  LoopTreeTraverseSelectStmt iter( root);
  for (LoopTreeNode *s; (s = iter.Current()); iter.Advance()) {
    if (!QuerySliceStmt(s))
       return false;
  }
  return true;
}

bool CompSlice :: SliceLoopReversible() const
{
  for (ConstLoopIterator iter = GetConstLoopIterator();
        iter.Current() != 0; iter.Advance()) {
    if (! iter.CurrentLoopReversible())
       return false;
  }
  return true;
}

class CompSliceNest::ObserveImpl
   : public ObserveObject <CompSliceNestObserver>
{};

void CompSliceNest :: AttachObserver( CompSliceNestObserver &o) const
{
   impl->AttachObserver( &o );
}

void CompSliceNest :: DetachObserver( CompSliceNestObserver &o) const
{
  impl->DetachObserver( &o );
}

void CompSliceNest :: Notify( const CompSliceNestObserveInfo& info)
{
    impl->Notify( info );
}

CompSliceNest :: CompSliceNest(unsigned _maxsize)
   : sliceVec(0), maxsize(0),size(0)
{
  impl = new ObserveImpl();
  if (_maxsize > 0)
     Reset(_maxsize);
}

CompSliceNest ::  ~CompSliceNest()
{
  CompSliceNestDeleteInfo info(*this);
  Notify(info);

  delete impl;
  for (size_t i = 0; i < size; ++i) {
    const CompSlice* tmp =  sliceVec[i];
    delete tmp;
  }
  delete [] sliceVec;
}

void CompSliceNest:: Reset( unsigned _maxsize)
{
  assert(sliceVec == 0);
  maxsize = _maxsize;
  size = 0;
  sliceVec = new CompSlice*[maxsize];
}

void CompSliceNest :: SwapEntry( int index1, int index2)
       { CompSlice *tmp = sliceVec[index1];
         sliceVec[index1] = sliceVec[index2];
         sliceVec[index2] = tmp;
         CompSliceNestSwapInfo info(*this, index1, index2);
         Notify(info);
       }


void CompSliceNest :: DeleteEntry( int index, bool saveAsInner)
{
  CompSliceNestDeleteEntryInfo info(*this, index, saveAsInner);
  Notify(info);
  if (!saveAsInner)
     delete sliceVec[index];
  for (size_t i = index; i + 1 < size; ++i)
    sliceVec[i] = sliceVec[i+1];
  --size;
}

void CompSliceNest :: AlignEntry( int index, int align)
{
  sliceVec[index]->IncreaseAlign(align);
  CompSliceNestAlignEntryInfo info(*this, index, align);
  Notify(info);
}

void CompSliceNest::AppendNest( const CompSliceNest& that)
{
  int num = NumberOfEntries();
  assert( num == that.NumberOfEntries());
  for (int i = 0; i < num; ++i) {
    sliceVec[i]->Append(*that.Entry(i));
  }
  CompSliceNestFusionInfo info( *this, that);
  Notify(info);
}

void CompSliceNest :: DuplicateEntry( int desc, int src)
{
  assert(size < maxsize);
  for (int i = size; i > desc; i--) {
     sliceVec[i] = sliceVec[i-1];
  }
  ++size;
  if (src > desc)
    ++src;
  sliceVec[desc] = new CompSlice(*sliceVec[src]);

  CompSliceNestDuplicateEntryInfo info(*this, desc,src);
  Notify(info);
}

SymbolicBound SliceLoopRange(const CompSlice *slice, LoopTreeNode *root)
{
  SymbolicBound result;
  LoopTreeGetVarBound bf(root->Parent());
  CompSlice::ConstStmtIterator stmtIter = slice->GetConstStmtIterator();
  for (LoopTreeNode* stmt; (stmt = stmtIter.Current()); stmtIter++) {
    CompSlice::SliceStmtInfo info = stmtIter.CurrentInfo();
    LoopInfo *l  = info.loop->GetLoopInfo();
    SymbolicBound b = LoopTreeGetVarConstBound(info.loop, root->Parent()).GetConstBound(l->GetVar());
    b.lb = b.lb - info.align; b.ub = b.ub - info.align;
    result.Union(b, &bf);
  }
  return result;
}

LoopStepInfo SliceLoopStep(const CompSlice *slice)
{
  CompSlice::ConstLoopIterator iter = slice->GetConstLoopIterator();
  LoopTreeNode *loop = iter.Current();
  SymbolicVal step = loop->GetLoopInfo()->GetStep();
  bool reversible = iter.CurrentLoopReversible();
  for ( iter.Advance(); (loop = iter.Current()); iter.Advance()) {
    SymbolicVal step1 = loop->GetLoopInfo()->GetStep();
    bool r = iter.CurrentLoopReversible();
    if (step !=step1) {
       assert(step== -step1 && (reversible || r));
       if (!r)
         step = step1;
    }
    if (!r)
       reversible = false;
  }
  return LoopStepInfo(step, reversible);
}

SymbolicVar SliceLoopIvar( AstInterface &fa, const CompSlice *slice)
{
  typedef std::set<std::string, std::less<std::string> > nameset;
  nameset sliceVars, usedVars;
  CompSlice::ConstLoopIterator loopIter = slice->GetConstLoopIterator();
  LoopTreeInterface interface;
  for (LoopTreeNode *loop; (loop = loopIter.Current()); loopIter.Advance()) {
    std::string name = loop->GetLoopInfo()->GetVar().GetVarName();
    sliceVars.insert( name);
    CompSlice::ConstStmtIterator stmtIter=loopIter.GetConstStmtIterator();
    for (LoopTreeNode *stmt; (stmt=stmtIter.Current()); stmtIter.Advance()) {
       for (LoopTreeNode *l = GetEnclosingLoop(stmt,interface);
            l != 0; l = GetEnclosingLoop(l, interface) ) {
          if (l == loop) continue;
          name = l->GetLoopInfo()->GetVar().GetVarName();
          usedVars.insert( (name) );
       }
    }
  }
  for (nameset::iterator p = sliceVars.begin(); p != sliceVars.end();
       ++p) {
    if (usedVars.find(*p) == usedVars.end())
      return SymbolicVar(*p, AST_NULL);;
  }
  return SymbolicVar(fa.NewVar(fa.GetType("int")), AST_NULL);
}

CompSlice::ConstLoopIterator::
ConstLoopIterator( LoopTreeNode *root)
 : LoopTreeTraverseSelectLoop(root, LoopTreeTraverse::ChildrenOnly)
{}

LoopTreeNode* CompSlice::ConstStmtIterator::Current() const
 { LoopTreeNode* cur = LoopTreeTraverseSelectStmt::Current();
   return (cur == 0)? 0 :
        static_cast<CompSliceStmt*>(cur)->GetSliceStmt();
 }

CompSlice::SliceStmtInfo CompSlice::ConstStmtIterator::CurrentInfo() const
    { LoopTreeNode* cur = LoopTreeTraverseSelectStmt::Current();
      return static_cast<CompSliceStmt*>(cur)->GetSliceInfo();
    }

LoopTreeNode* CompSlice::ConstLoopIterator::Current() const
    { LoopTreeNode* cur = LoopTreeTraverseSelectLoop::Current();
      return (cur==0)? 0 :
        static_cast<CompSliceLoop*>(cur)->GetSliceLoop();
     }
CompSlice::SliceLoopInfo CompSlice::ConstLoopIterator::CurrentInfo() const
    { LoopTreeNode* cur = LoopTreeTraverseSelectLoop::Current();
      CompSliceLoop* l = static_cast<CompSliceLoop*>(cur);
      return (l==0)? SliceLoopInfo() : l->GetSliceInfo();
    }
bool CompSlice::ConstLoopIterator::
CurrentLoopReversible() const
    { LoopTreeNode* cur = LoopTreeTraverseSelectLoop::Current();
      return static_cast<CompSliceLoop*>(cur)->LoopReversible(); }

void  CompSlice::UpdateStmtIterator::Attach()
   { LoopTreeNode *cur = LoopTreeTraverseSelectStmt::Current();
     if (cur != 0) cur->AttachObserver(*this);
    }
void CompSlice::UpdateStmtIterator::Detach()
{ LoopTreeNode *cur = LoopTreeTraverseSelectStmt::Current();
   if (cur != 0) cur->DetachObserver(*this);
}
void CompSlice::UpdateStmtIterator::
UpdateSplitStmt( const SplitStmtInfo &info)
  {
   const LoopTreeNode *o = info.GetObserveNode();
   LoopTreeNode *n = info.GetSplitStmt();
   if (n->NextSibling() == o) {
     o->DetachObserver(*this);
     LoopTreeTraverseSelectStmt::Current() = n;
     Attach();
   }
  }
void CompSlice::UpdateStmtIterator::
UpdateDeleteNode( const LoopTreeNode *n )
   { CompSlice::ConstStmtIterator::Advance(); Attach(); }

void  CompSlice::UpdateLoopIterator::Attach()
   { LoopTreeNode *cur = LoopTreeTraverseSelectLoop::Current();
     if (cur != 0) cur->AttachObserver(*this);
    }
void CompSlice::UpdateLoopIterator::Detach()
{ LoopTreeNode *cur = LoopTreeTraverseSelectLoop::Current();
   if (cur != 0) cur->DetachObserver(*this);
}
void CompSlice::UpdateLoopIterator::
UpdateDistNode(const DistNodeInfo &info)
   { const LoopTreeNode *o = info.GetObserveNode();
     LoopTreeNode *n = info.GetNewNode();
     if (n->NextSibling() == o) {
       o->DetachObserver(*this);
       LoopTreeTraverseSelectLoop::Current() = n;
       Attach();
     }
   }
void CompSlice::UpdateLoopIterator::
UpdateDeleteNode( const LoopTreeNode *n )
   { CompSlice::ConstLoopIterator::Advance(); Attach(); }

CompSlice::ConstStmtIterator CompSlice::ConstLoopIterator::
GetConstStmtIterator() const
{  LoopTreeNode *cur = LoopTreeTraverseSelectLoop::Current();
   return CompSlice::ConstStmtIterator(cur);
}

CompSlice::UpdateStmtIterator CompSlice::UpdateLoopIterator::
GetUpdateStmtIterator() const
{  LoopTreeNode *cur = LoopTreeTraverseSelectLoop::Current();
   return CompSlice::UpdateStmtIterator(cur);
}

/* QY: can be used find common root of two nodes under parent but currently not being used
class FindCommonInterface {
  LoopTreeNode* parent;
 public:
  FindCommonInterface(LoopTreeNode* p) : parent(p) {}
  LoopTreeNode* GetParent(LoopTreeNode* n)
     { LoopTreeNode* res = n->Parent();
       assert(res != 0);
       if (res == parent) return 0;
       return res;
     }
  bool IsLoop(LoopTreeNode* n) { return true; }
  LoopTreeNode* getNULL() { return 0; }
};
*/

