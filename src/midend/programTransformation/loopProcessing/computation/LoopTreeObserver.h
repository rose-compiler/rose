#ifndef OBSERVE_LOOP_TREE
#define OBSERVE_LOOP_TREE

#include <DepRel.h>
#include <ObserveObject.h>
#include <SymbolicBound.h>

class LoopTreeNode;
class BlockLoopInfo ;
class MergeLoopInfo ;
class DistNodeInfo ;
class SwapNodeInfo ;
class InsertLoopInfo;

class MergeStmtLoopInfo ;
class SwapStmtLoopInfo;
class DeleteStmtLoopInfo;
class InsertStmtLoopInfo;
class SplitStmtInfo;
class SplitStmtInfo2;
class LoopTreeCodeGenInfo;

class LoopTreeObserver
{
   LoopTreeObserver *next;
  public:
   LoopTreeObserver( LoopTreeObserver *n = 0) { next = n; }
   virtual ~LoopTreeObserver() {}
   virtual void UpdateBlockLoop(const BlockLoopInfo &info)
               { if (next != 0) next->UpdateBlockLoop( info ); }
   virtual void UpdateMergeLoop(const MergeLoopInfo &info)
               { if (next != 0) next->UpdateMergeLoop( info ); }
   virtual void UpdateDistNode(const DistNodeInfo &info)
               { if (next != 0) next->UpdateDistNode( info ); }
   virtual void UpdateSwapNode( const SwapNodeInfo &info)
               { if (next != 0) next->UpdateSwapNode( info ); }
   virtual void UpdateCodeGen(const LoopTreeCodeGenInfo& info)
               { if (next != 0) next->UpdateCodeGen( info ); }
   virtual void UpdateSplitStmt( const SplitStmtInfo &info)
               { if (next != 0) next->UpdateSplitStmt( info ); }
   virtual void UpdateSplitStmt2( const SplitStmtInfo2 &info)
               { if (next != 0) next->UpdateSplitStmt2( info ); }
   virtual void UpdateMergeStmtLoop( const MergeStmtLoopInfo &info)
               { if (next != 0) next->UpdateMergeStmtLoop( info ); }
   virtual void UpdateInsertLoop( const InsertLoopInfo &info)
              { if (next != 0) next->UpdateInsertLoop(info); }
   virtual void UpdateInsertStmtLoop( const InsertStmtLoopInfo &info)
              { if (next != 0) next->UpdateInsertStmtLoop(info); }
   virtual void UpdateDeleteStmtLoop( const DeleteStmtLoopInfo &info)
              { if (next != 0) next->UpdateDeleteStmtLoop(info); }
   virtual void UpdateSwapStmtLoop( const SwapStmtLoopInfo &info)
              { if (next != 0) next->UpdateSwapStmtLoop(info); }

   virtual void UpdateDeleteNode( const LoopTreeNode *n )
               { if (next != 0) next->UpdateDeleteNode( n ); }
   virtual void write(std::ostream& out) const {}
};

class LoopTreeObserveInfo  : public ObserveInfo<LoopTreeObserver>
{
  const LoopTreeNode *orig;
 protected:
  LoopTreeObserveInfo( const LoopTreeNode *n ) { orig = n; }
 public:
  virtual ~LoopTreeObserveInfo() {}

  const LoopTreeNode * GetObserveNode() const { return orig; }
};

class LoopTreeCodeGenInfo : public LoopTreeObserveInfo
{
  AstNodePtr res;
 public:
   LoopTreeCodeGenInfo( const LoopTreeNode *n, const AstNodePtr& _res) 
      : LoopTreeObserveInfo(n), res(_res) {}
   virtual ~LoopTreeCodeGenInfo() {}
   virtual void UpdateObserver( LoopTreeObserver &o) const
       { o.UpdateCodeGen( *this ); }
   AstNodePtr GetAST() const { return res; }
};

class DeleteNodeInfo : public LoopTreeObserveInfo
{
 public:
   DeleteNodeInfo( LoopTreeNode *n) : LoopTreeObserveInfo(n) {}
   virtual ~DeleteNodeInfo() {}
   virtual void UpdateObserver( LoopTreeObserver &o) const
       { o.UpdateDeleteNode( GetObserveNode() ); }
};

class DistNodeInfo : public LoopTreeObserveInfo
{
  protected:
    LoopTreeNode *newNode;
  public:
    DistNodeInfo( LoopTreeNode *o, LoopTreeNode *n)
       : LoopTreeObserveInfo(o)  {  newNode = n;  }
    virtual ~DistNodeInfo() {}
    LoopTreeNode * GetNewNode() const { return newNode; }

    virtual void UpdateObserver(LoopTreeObserver &o) const
               { o.UpdateDistNode( *this ); }
};

class BlockLoopInfo : public LoopTreeObserveInfo
{
  protected:
    SymbolicVal blocksize;
  public:
    BlockLoopInfo( LoopTreeNode *o, const SymbolicVal& bsize)
       : LoopTreeObserveInfo( o), blocksize(bsize) {}
    virtual ~BlockLoopInfo() {}
    SymbolicVal GetBlockSize() const { return blocksize; }

    virtual void UpdateObserver(LoopTreeObserver &o) const
            { o.UpdateBlockLoop( *this ); }
};

class MergeLoopInfo : public LoopTreeObserveInfo
{
  protected:
    LoopTreeNode *newLoop;
    int align;
  public:
    MergeLoopInfo( LoopTreeNode *o, LoopTreeNode *n, 
                  int a)
       : LoopTreeObserveInfo( o) {  newLoop = n; align = a; }
    virtual ~MergeLoopInfo() {}
    LoopTreeNode * GetNewLoop() const { return newLoop; }
    int GetMergeAlign() const { return align; }

    virtual void UpdateObserver(LoopTreeObserver &o) const
              { o.UpdateMergeLoop( *this ); }
};

class MergeStmtLoopInfo : public LoopTreeObserveInfo
{
    int align, loop1, loop2;
  public:
    MergeStmtLoopInfo( LoopTreeNode *stmt, int l1, int l2, int _align)
      : LoopTreeObserveInfo( stmt ) { loop1 = l1; loop2 = l2; align = _align; }
    virtual ~MergeStmtLoopInfo() {}

    int GetLoop1() const { return loop1; }
    int GetLoop2() const { return loop2; }
    int GetMergeAlign() const { return align; }

    virtual void UpdateObserver( LoopTreeObserver &o) const
                { o.UpdateMergeStmtLoop( *this ); }
};

class InsertStmtLoopInfo : public LoopTreeObserveInfo
{
    int loop;
  public:
    InsertStmtLoopInfo( LoopTreeNode *s, int level)
      : LoopTreeObserveInfo( s ), loop(level) {}
    virtual ~InsertStmtLoopInfo() {}

    int GetLoop() const { return loop; }
    virtual void UpdateObserver( LoopTreeObserver &o) const
                { o.UpdateInsertStmtLoop( *this ); }
};
class InsertLoopInfo : public LoopTreeObserveInfo
{
  public:
    InsertLoopInfo( LoopTreeNode *l)
      : LoopTreeObserveInfo( l ) {}
    virtual ~InsertLoopInfo() {}

    virtual void UpdateObserver( LoopTreeObserver &o) const
                { o.UpdateInsertLoop( *this ); }
};

class DeleteStmtLoopInfo : public LoopTreeObserveInfo
{
    int loop;
  public:
    DeleteStmtLoopInfo( LoopTreeNode *s, int l)
      : LoopTreeObserveInfo( s ), loop(l)  {}
    virtual ~DeleteStmtLoopInfo() {}
    int GetLoop() const { return loop; }
    virtual void UpdateObserver( LoopTreeObserver &o) const
                { o.UpdateDeleteStmtLoop( *this ); }
};

class SwapStmtLoopInfo : public LoopTreeObserveInfo
{
    int index1, index2;
  public:
    SwapStmtLoopInfo( LoopTreeNode *s, int i1, int i2)
      : LoopTreeObserveInfo( s ),index1(i1),index2(i2) {}
    virtual ~SwapStmtLoopInfo() {}

    int GetLoop1() const { return index1; }
    int GetLoop2() const { return index2; }

    virtual void UpdateObserver( LoopTreeObserver &o) const
                { o.UpdateSwapStmtLoop( *this ); }
};


class SwapNodeInfo : public LoopTreeObserveInfo
{
     int direction;
     LoopTreeNode *othernode;
     VarInfo varinfo;
  public:
     SwapNodeInfo( LoopTreeNode *o, LoopTreeNode *other, int dir, 
                   const VarInfo& v)
       : LoopTreeObserveInfo( o ), direction(dir), othernode(other),
         varinfo(v)  {}
     virtual ~SwapNodeInfo() {}
     int GetDirection() const { return direction; }
     LoopTreeNode* GetOtherNode() const { return othernode; }
     VarInfo GetVarInfo() const { return varinfo; }
     virtual void UpdateObserver( LoopTreeObserver &o) const
                 { o.UpdateSwapNode( *this ); }
};
 
class SplitStmtInfo : public LoopTreeObserveInfo
{
  LoopTreeNode* splitstmt;
  int loop1,  loop2;
  DepRel rel;
 public:
  SplitStmtInfo( LoopTreeNode *stmt, LoopTreeNode *n,  
                 int l1, int l2, const DepRel& r)
     : LoopTreeObserveInfo(stmt),splitstmt(n),loop1(l1),loop2(l2),rel(r) {}
  virtual ~SplitStmtInfo() {}
 
  LoopTreeNode * GetSplitStmt() const { return splitstmt; }
  int GetLoop1() const { return loop1; }
  int GetLoop2() const { return loop2; }
  DepRel GetRel() const { return rel; }
  virtual void UpdateObserver( LoopTreeObserver &o) const
                 { o.UpdateSplitStmt( *this ); }
};

class SplitStmtInfo2 : public LoopTreeObserveInfo
{
  LoopTreeNode* splitstmt;
  int loop;
  SymbolicVal le;
 public:
  SplitStmtInfo2( LoopTreeNode *stmt, LoopTreeNode* n, int l, const SymbolicVal& mid)
     : LoopTreeObserveInfo(stmt),splitstmt(n),loop(l),le(mid) {}
  virtual ~SplitStmtInfo2() {}
 
  LoopTreeNode * GetSplitStmt() const { return splitstmt; }
  int GetLoop() const { return loop; }
  SymbolicVal GetLE() const { return le; }
  virtual void UpdateObserver( LoopTreeObserver &o) const
                 { o.UpdateSplitStmt2( *this ); }
};

#endif
