
#ifndef COMPSLICE
#define COMPSLICE

#include <LoopTree.h>
#include <LoopTreeObserver.h>
#include <LoopTreeDepComp.h>

class CompSliceObserver;
class CompSliceObserveInfo;
class CompSliceImpl;
class CompSlice 
{
  class ObserveImpl;
  CompSliceImpl *impl;
  ObserveImpl *obImpl;

  void SetSliceLoop( LoopTreeNode *s, LoopTreeNode *l,
                     bool loopreversible = false, int align = 0);
  void SetSliceAlign( LoopTreeNode *s, int align);
  void Notify( const CompSliceObserveInfo &info);
 protected:
  CompSlice(CompSliceImpl *_impl);
  CompSliceImpl* GetImpl() const { return impl; }
  
 public:
  CompSlice( int level);
  CompSlice( const CompSlice& that);
  ~CompSlice();

  void Dump() const { std::cerr << toString() << std::endl;}
  std::string toString() const;
  int QuerySliceLevel() const;

  struct SliceStmtInfo {
     LoopTreeNode *loop;
     bool reversible;
     int align;
     SliceStmtInfo() : loop(0), reversible(false), align(0) {}
     SliceStmtInfo(LoopTreeNode *l, bool r, int a) 
       : loop(l), reversible(r), align(a) {}
     operator bool()  { return loop != 0; }
  };
  struct SliceLoopInfo {
    bool reversible;
    unsigned stmtcount;
    int minalign, maxalign;
    SliceLoopInfo() : reversible(false) { stmtcount=minalign=maxalign = 0;}
    SliceLoopInfo( bool r, unsigned c, int mina, int maxa)
      : reversible(r), stmtcount(c),minalign(mina),maxalign(maxa) {}
    operator bool() { return !stmtcount; }
  };
  SliceStmtInfo QuerySliceStmtInfo( const LoopTreeNode *s) const;
  SliceLoopInfo QuerySliceLoopInfo( const LoopTreeNode *l) const;
  bool QuerySliceStmt( const LoopTreeNode *s) const;
  bool QuerySliceLoop( const LoopTreeNode *l) const;

  class ConstStmtIterator : public LoopTreeTraverseSelectStmt
  { public:
     ConstStmtIterator( LoopTreeNode *r)
      : LoopTreeTraverseSelectStmt( r) {}
     ConstStmtIterator( const ConstStmtIterator& that)
       : LoopTreeTraverseSelectStmt(that) {}
     LoopTreeNode* Current() const;
     SliceStmtInfo CurrentInfo() const;
     friend class CompSlice;
   };
   class ConstLoopIterator : public LoopTreeTraverseSelectLoop
   { public:
      ConstLoopIterator( LoopTreeNode *root);
      ConstLoopIterator( const CompSlice::ConstLoopIterator& that)
          : LoopTreeTraverseSelectLoop(that) {}
      LoopTreeNode* Current() const;
      SliceLoopInfo CurrentInfo() const;
      bool CurrentLoopReversible() const ;
      ConstStmtIterator GetConstStmtIterator() const;
    };
  ConstLoopIterator GetConstLoopIterator() const;
  ConstStmtIterator GetConstStmtIterator() const;

  class UpdateStmtIterator : public ConstStmtIterator, public LoopTreeObserver
   {  void Attach();
      void Detach();
      void UpdateSplitStmt( const SplitStmtInfo &info);
      void UpdateDeleteNode( const LoopTreeNode *n );
    public:
      UpdateStmtIterator( LoopTreeNode *root)
         : ConstStmtIterator( root) { Attach(); }
      UpdateStmtIterator( const UpdateStmtIterator& that) 
        : ConstStmtIterator( that) {  Attach(); }
      ~UpdateStmtIterator() { Detach(); }
      void Reset()
        { Detach(); ConstStmtIterator::Reset(); Attach(); }
      void Advance()
        { Detach(); ConstStmtIterator::Advance(); Attach(); }
      void operator++()  { Advance(); }
      void operator++(int) { Advance(); }
   };
   class UpdateLoopIterator : public LoopTreeObserver, public ConstLoopIterator
   {  void Attach();
      void Detach();
      void UpdateDistNode(const DistNodeInfo &info);
      void UpdateDeleteNode( const LoopTreeNode *n );
    public:
      UpdateLoopIterator( LoopTreeNode *root)
         : ConstLoopIterator(root) { Attach(); }
      UpdateLoopIterator( const CompSlice::UpdateLoopIterator& that)
         : ConstLoopIterator(that) { Attach(); }
      ~UpdateLoopIterator() {Detach(); }
      void Reset() 
        { Detach(); ConstLoopIterator::Reset(); Attach(); }
      void Advance()
        { Detach(); ConstLoopIterator::Advance(); Attach(); }
      void operator++()  { Advance(); }
      void operator++(int) { Advance(); }
      UpdateStmtIterator GetUpdateStmtIterator() const;
  };
  UpdateLoopIterator GetUpdateLoopIterator() const;
  UpdateStmtIterator GetUpdateStmtIterator() const;

  void AttachObserver( CompSliceObserver &o) const; 
  void DetachObserver( CompSliceObserver &o) const;

  bool SliceCommonLoop( CompSlice *slice2) const;
  bool SliceCommonStmt( CompSlice *slice2) const;
  bool SliceCodeSegment( LoopTreeNode *root) const;
  bool SliceLoopReversible() const;

  void Append( const CompSlice& that);
  void IncreaseAlign( int align);
 friend class DependenceHoisting;
};

SymbolicBound SliceLoopRange(const CompSlice *slice, LoopTreeNode *root);
struct LoopStepInfo
{ SymbolicVal step; bool reversible;
  LoopStepInfo(SymbolicVal s, bool r) : step(s), reversible(r){}
};
LoopStepInfo SliceLoopStep(const CompSlice *slice);
SymbolicVar SliceLoopIvar( AstInterface &fa, const CompSlice *slice);

class CompSliceNest;
class DependenceHoisting 
{
  virtual CompSlice* CreateCompSlice( int level ) { return new CompSlice(level); }
 public:
  void Analyze( LoopTreeDepComp& comp, CompSliceNest &result);
  void Analyze( LoopTreeDepComp& comp, LoopTreeTransDepGraphCreate* g, 
                   CompSliceNest& result);
  virtual LoopTreeNode* Transform( LoopTransformInterface &_fa, LoopTreeDepComp& c,
                                   const CompSlice *slice, LoopTreeNode *root);
  virtual ~DependenceHoisting() {}
};

class CompSliceNestObserver;
class CompSliceNestObserveInfo;
class CompSliceNest
{
  CompSlice** sliceVec;
  unsigned maxsize, size;
  class ObserveImpl;
  ObserveImpl *impl;

  void Append( CompSlice *slice) 
       { assert(size < maxsize); 
         sliceVec[size]=slice; ++size; }
  void Reset( unsigned _maxsize);
  void Notify( const CompSliceNestObserveInfo &info);
 public:
  CompSliceNest();
  ~CompSliceNest();
  int NumberOfEntries() const { return size; }
  const CompSlice* Entry(int index) const { return sliceVec[index]; }
  const CompSlice* operator [] (int index) const { return sliceVec[index]; }
  void Dump() const  { std::cerr << toString() << std::endl; }
  std::string toString() const ;

  void AttachObserver( CompSliceNestObserver &o) const ;
  void DetachObserver( CompSliceNestObserver &o) const;

  void SwapEntry( int index1, int index2);
  void DeleteEntry( int index);
  void DuplicateEntry( int desc,int src);
  void AlignEntry(int index, int align);
  void AppendNest( const CompSliceNest& that);
 friend class DependenceHoisting;
};

#endif
