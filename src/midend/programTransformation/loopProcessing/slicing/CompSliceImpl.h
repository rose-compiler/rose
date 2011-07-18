#include <stdio.h>
#include <stdlib.h>

#include <const.h>
#include <LoopTreeShadow.h>

#include <CompSlice.h>

class CompSliceLoop : public LoopTreeShadowNode, public LoopTreeObserver
{
  bool reversible;
 protected:
  virtual void UpdateDeleteNode( const LoopTreeNode *n)
  { LoopTreeShadowNode::ReplaceRepr(0);
    delete this; }
  virtual LoopTreeShadowNode* CloneNode(LoopTreeNode *n) const
    { return new CompSliceLoop( n, *this); }
  virtual void UpdateDistNode( const DistNodeInfo &info );
  virtual void UpdateMergeLoop( const MergeLoopInfo &info);

  void ReplaceRepr( LoopTreeNode *n)
   {  GetRepr()->DetachObserver(*this);
      n->AttachObserver(*this);
      LoopTreeShadowNode::ReplaceRepr(n);
   }
 public:
  CompSliceLoop( LoopTreeNode *l, CompSliceImpl* impl,
                bool r = true);
  CompSliceLoop( LoopTreeNode *n, const CompSliceLoop& that)
     : LoopTreeShadowNode(n, that) 
     { reversible = that.reversible; n->AttachObserver(*this); }
  ~CompSliceLoop() { if (GetRepr() != 0)
                        GetRepr()->DetachObserver(*this); }

  void SetReversal( bool r)
     { if (reversible) reversible = r; }

  CompSlice::SliceLoopInfo GetSliceInfo() const;
  bool LoopReversible() const { return reversible; }
  LoopTreeNode* GetSliceLoop() const { return GetRepr(); }
  void Dump() const { std::cerr << toString(); }
  virtual std::string toString() const
    { 
      std::string res =  "slice loop: " + GetRepr()->toString();
      if ( LoopReversible())
         res = res + " loop reversible ";
      return res;
    }
  virtual std::string GetClassName() const { return "CompSliceLoop"; }
};

class CompSliceStmt : public LoopTreeShadowNode, public LoopTreeObserver
{
  int align;

 protected:
  virtual LoopTreeShadowNode* CloneNode( LoopTreeNode *n) const
    { return new CompSliceStmt( n, *this); }
  virtual void UpdateSplitStmt( const SplitStmtInfo &info )
    {
       LoopTreeNode* that = CloneNode(info.GetSplitStmt());
       that->Link(this, AsPrevSibling);
       SplitStmtInfo info1( this, that,
                            info.GetLoop1(), info.GetLoop2(),info.GetRel());
       Notify(info1);
    }
  virtual void UpdateDeleteNode( const LoopTreeNode *n)
    { delete this; }
 public:
  CompSliceStmt( LoopTreeNode *s,  CompSliceImpl *tc, int align = 0);
  CompSliceStmt( LoopTreeNode *n, const CompSliceStmt &that)
     : LoopTreeShadowNode(n, that) 
     { align = that.align; n->AttachObserver(*this); }
  ~CompSliceStmt() { GetRepr()->DetachObserver(*this); }

  void SetSliceAlign( int a) { align = a; }
  void IncreaseSliceAlign( int a) { align += a; }
  LoopTreeNode* GetSliceStmt() const { return GetRepr(); }
  CompSlice::SliceStmtInfo GetSliceInfo() const
    { CompSliceLoop *l = static_cast <CompSliceLoop*> (Parent());
       return CompSlice::SliceStmtInfo(l->GetSliceLoop(), 
                                       l->LoopReversible(), align); 
     }
  int GetSliceAlign() const {return align; }
  void SetSliceLoop( CompSliceLoop *loop, int a)
     { Unlink();
       Link(loop, AsLastChild);
       align = a;
     }
  void Dump() const { std::cerr << toString() << std::endl; }
  virtual std::string toString() const
    { 
       char buf[20];
       sprintf( buf, "%d", align);
       return  "stmt: " + GetRepr()->toString() + "\nslicing alignment: " + std::string(buf)+"\n";
    }
  virtual std::string GetClassName() const { return "CompSliceStmt"; }
};

inline CompSlice::SliceLoopInfo CompSliceLoop:: GetSliceInfo() const
   {
     int mina = POS_INFTY,maxa = NEG_INFTY;
     for (CompSliceStmt* s = static_cast<CompSliceStmt*>(FirstChild());
          s != 0; s = static_cast<CompSliceStmt*>(s->NextSibling())) {
        if (mina > s->GetSliceAlign())
           mina = s->GetSliceAlign();
        if (maxa < s->GetSliceAlign())
           maxa = s->GetSliceAlign();
     }
     return CompSlice::SliceLoopInfo(reversible, ChildCount(), mina, maxa);
   }


class CompSliceImpl : public LoopTreeShadowCreate
{
 protected:
  virtual LoopTreeShadowNode* CreateShadowNode( LoopTreeNode *n)
   {
     LoopTreeShadowNode *result = 0;
     if (IsSimpleStmt(n))
        result = CreateSliceStmtNode( n);
     else if (n->IncreaseLoopLevel() != 0) 
        result = CreateSliceLoopNode( n );
     return result;
   }
   virtual CompSliceLoop* CreateSliceLoopNode( LoopTreeNode *n, CompSliceLoop* that = 0)
   {  bool r = (that == 0)? true : that->LoopReversible();
         return new CompSliceLoop( n, this, r); }
   virtual CompSliceStmt* CreateSliceStmtNode( LoopTreeNode *n, CompSliceStmt* that = 0)
   {  return new CompSliceStmt( n, this ); }

 public:
  CompSliceImpl( int looplevel ) : LoopTreeShadowCreate(looplevel) {}
  ~CompSliceImpl() {}
  virtual CompSliceImpl* CloneImpl() const 
    { return new CompSliceImpl( QuerySliceLevel() ); }

  void Dump() const { GetTreeRoot()->DumpTree(); }
  std::string toString() const { return GetTreeRoot()->TreeToString(); }
  CompSliceStmt * QuerySliceStmt( const LoopTreeNode *n) const
    { return (IsSimpleStmt(n))?
                static_cast <CompSliceStmt*>( QueryShadowNode(n)):0;
    }
  CompSliceLoop* QuerySliceLoop( const LoopTreeNode *n) const
    { return n->IncreaseLoopLevel()?
                static_cast<CompSliceLoop*>(QueryShadowNode(n)):0; 
    }

  CompSliceLoop *CreateSliceLoop( LoopTreeNode *l, CompSliceLoop* that = 0)
    {
      CompSliceLoop *n = QuerySliceLoop(l);
      if (n == 0) {
        n = CreateSliceLoopNode(l, that);
        n->Link( GetTreeRoot(), LoopTreeNode::AsLastChild);
      }
      return n;
    }
  CompSliceStmt *CreateSliceStmt( LoopTreeNode *s, CompSliceStmt* that = 0)
    {
      CompSliceStmt *ns = QuerySliceStmt(s);
      if (ns == 0) {
         ns = CreateSliceStmtNode(s, that);
         ns->Link( GetTreeRoot(), LoopTreeNode::AsLastChild);
      }
      return ns;
    }
  virtual void Append( const CompSliceImpl& that)
   {
     LoopTreeTraverseSelectLoop loopIter(that.GetTreeRoot());
     for (LoopTreeNode *n; (n=loopIter.Current()); loopIter++) {
       CompSliceLoop *l = static_cast<CompSliceLoop*>(n);
       CompSliceLoop *myl = CreateSliceLoop(l->GetSliceLoop(), l);
       LoopTreeTraverseSelectStmt stmtIter(l);
       for (LoopTreeNode *n1; (n1 = stmtIter.Current()); stmtIter++)  {
          CompSliceStmt *s = static_cast <CompSliceStmt*>(n1);
          CompSliceStmt *mys = CreateSliceStmt(s->GetSliceStmt(), s);
          mys->SetSliceLoop( myl, s->GetSliceAlign() );
       }
     }
   }

  int QuerySliceLevel() const { return GetTreeRoot()->LoopLevel(); }
};

inline CompSliceLoop:: 
CompSliceLoop( LoopTreeNode *l, CompSliceImpl *_slice, bool r)
    : LoopTreeShadowNode( l, _slice), reversible(r)
 { l->AttachObserver(*this); }

inline CompSliceStmt::
CompSliceStmt( LoopTreeNode *s, CompSliceImpl *_slice, int a)
  : LoopTreeShadowNode(s, _slice), align(a)
{ s->AttachObserver(*this); }

inline void CompSliceLoop:: UpdateDistNode( const DistNodeInfo &info )
{
 LoopTreeNode * n = info.GetNewNode();
 const LoopTreeNode *orig = info.GetObserveNode();

 CompSliceLoop *that = 0;
 for (LoopTreeNode *s = FirstChild(); s != 0; ) {
    CompSliceStmt *ss = static_cast <CompSliceStmt*>(s);
    s = s->NextSibling();
    LoopTreeNode *c1 = ss->GetSliceStmt();
    while( c1 && c1 != orig && c1 != n)
       c1 = c1->Parent();
    assert( c1 != 0);
    if (c1 == n) {
       if (that == 0) {
           that = static_cast<CompSliceLoop*>( CloneNode(n) );
           that->Link( this, AsPrevSibling);
       }
       ss->SetSliceLoop(that, ss->GetSliceAlign());
    }
 }
 if (that != 0) {
    DistNodeInfo info1(this, that);
    Notify(info1);
 }
 if (ChildCount() == 0)
    delete this;
}

inline void CompSliceLoop:: UpdateMergeLoop( const MergeLoopInfo &info)
{
  int a = info.GetMergeAlign();
  LoopTreeNode *l = info.GetNewLoop();
  CompSliceLoop *nl = static_cast<CompSliceLoop*>(QueryShadowNode(l));
  if (nl != 0) {
     for (LoopTreeNode *n = FirstChild(); n != 0; ) {
       CompSliceStmt *cn = static_cast<CompSliceStmt*>(n);
       n = n->NextSibling();
       int a1 = cn->GetSliceAlign() - a;
       cn->SetSliceLoop( nl, a1);
     }
     Unlink();
     delete this;
  }
  else {
    ReplaceRepr(l);
    if (a != 0) {
      for (LoopTreeNode *n = FirstChild(); n != 0; n = n->NextSibling()) {
         CompSliceStmt *cn = static_cast<CompSliceStmt*>(n);
         int a1 = cn->GetSliceAlign() - a;
         cn->SetSliceAlign(a1);
      }
    }
  }
}


