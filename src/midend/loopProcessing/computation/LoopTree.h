
#ifndef LOOPTREE_H
#define LOOPTREE_H

#include <stdlib.h>
#include <string>

#include <SymbolicVal.h>
#include <TreeImpl.h>
#include <IteratorTmpl.h>
#include <IteratorCompound.h>
#include <FunctionObject.h>
#include <SinglyLinkedList.h>
#include <LoopTreeObserver.h>

class LoopTransformInterface;
                                                                                   
class LoopTreeNode;
class LoopTreeLoopNode;
class LoopTreeStmtNode;
class LoopTreeDummyNode;

class LoopTreeCreate;
class LoopTreeObserveImpl;
class LoopTreeObserveInfo;

class LoopInfo  : public VarInfo
{
  SymbolicVal step;
//Boolean reverse;
  int reverse;
 public:
  LoopInfo( SymbolicVar ivar,SymbolicVal lb, SymbolicVal ub, 
                    SymbolicVal s);
  LoopInfo( LoopTransformInterface &fa, const AstNodePtr& l);

  std::string ToString() const 
       { return VarInfo::ToString() + ":" + step.ToString();}

  bool ReverseEnum() const { return reverse; }
  const SymbolicVal& GetStep() const { return step; }
  SymbolicVal& GetStep() { return step; }

  SymbolicVal GetLoopLB() const 
         { return ReverseEnum()? GetBound().ub : GetBound().lb; }
  SymbolicVal GetLoopUB() const 
         { return ReverseEnum()? GetBound().lb : GetBound().ub;}
};

class LoopTreeNode  : public TreeNodeImpl<LoopTreeNode>
{
  LoopTreeObserveImpl *impl;
 protected:
  LoopTreeNode();
  LoopTreeNode(const LoopTreeNode& that);
  void UpdateDelete();
  virtual ~LoopTreeNode(); 

  void RemoveSelf( LoopTreeNode *repl = 0);
  void RemoveTree();
 public:
  virtual AstNodePtr CodeGen( LoopTransformInterface &fa, const AstNodePtr& c) const { return c; }
  virtual int LoopLevel() const
       { return Parent()->LoopLevel() + Parent()->IncreaseLoopLevel(); }
  virtual int IncreaseLoopLevel() const { return 0; }

  virtual std::string GetClassName() const = 0;

  virtual VarInfo GetVarInfo() const { return VarInfo(); }
  virtual AstNodePtr GetOrigStmt()  const { return 0; }

  virtual const LoopInfo* GetLoopInfo() const { return 0; }
  virtual LoopInfo* GetLoopInfo() { return 0; }

//virtual Boolean SelfRemove() { return false; }
  virtual int SelfRemove() { return false; }
  virtual LoopTreeNode* Clone() const = 0;

  void Dump() const { std::cerr << ToString() << std::endl; }
  virtual std::string ToString() const { return ""; }
  virtual AstNodePtr CodeGen( LoopTransformInterface &fa) const;

  void AttachObserver( LoopTreeObserver &o) const;
  void DetachObserver( LoopTreeObserver &o) const;
  void Notify( const LoopTreeObserveInfo &info) const;
  unsigned NumberOfObservers() const;
  void DumpTree() const { std::cerr << TreeToString() << std::endl; }
  std::string TreeToString() const;

//Boolean IsPerfectLoopNest() const;
  int IsPerfectLoopNest() const;
//Boolean ContainLoop() const;
  int ContainLoop() const;
 friend class LoopTreeTransform;
};

class LoopTreeStmtNode : public LoopTreeNode
{
  AstNodePtr start;

   ~LoopTreeStmtNode();
   LoopTreeStmtNode(  const AstNodePtr& s) : start(s) {}
   LoopTreeStmtNode(  const LoopTreeStmtNode& that)
     : LoopTreeNode(that), start(that.start) {}
 public:
   std::string ToString() const;
   std::string GetClassName() const { return "LoopTreeStmtNode"; }
   AstNodePtr CodeGen( LoopTransformInterface &fa, const AstNodePtr& c) const;
   AstNodePtr GetOrigStmt() const { return start; }
   LoopTreeNode* Clone() const { return new LoopTreeStmtNode( *this ); }
 friend class LoopTreeCreate;
};

class LoopTreeLoopNode : public LoopTreeNode, public LoopTreeObserver
{
  AstNodePtr orig;
  LoopInfo info;
//virtual Boolean SelfRemove();
  virtual int SelfRemove();
 protected:
  ~LoopTreeLoopNode();
  LoopTreeLoopNode( LoopTransformInterface &fa, const AstNodePtr& l); 
  LoopTreeLoopNode( const LoopTreeLoopNode& that) ;
  LoopTreeLoopNode( SymbolicVar _ivar,SymbolicVal _lb, SymbolicVal _ub, 
                  SymbolicVal _step);
  AstNodePtr CodeGen( LoopTransformInterface &fa, const AstNodePtr& c) const;
  void UpdateSwapNode( const SwapNodeInfo& info);
 public:
  std::string ToString() const;
  std::string GetClassName() const { return "LoopTreeLoopNode"; }
  int IncreaseLoopLevel() const { return 1; }
  LoopTreeNode* Clone() const
     { return new LoopTreeLoopNode( *this ); }
  VarInfo GetVarInfo() const { return info; }
  const LoopInfo* GetLoopInfo() const { return &info; }
  LoopInfo* GetLoopInfo() { return &info; }

 friend class LoopTreeCreate;
};

class LoopTreeRoot : public LoopTreeNode
{
  int level;
  ~LoopTreeRoot() {}
  LoopTreeRoot( int _level = 0) 
    : LoopTreeNode(), level(_level) {}

 public:
  std::string ToString() const;
  LoopTreeNode* Clone() const { return 0; }
  std::string GetClassName() const { return "LoopTreeRoot"; }
  virtual int LoopLevel() const { return level; }

 friend class LoopTreeCreate;
};

typedef IteratorImpl<LoopTreeNode*> LoopTreeNodeIteratorImpl;
typedef IteratorWrap<LoopTreeNode*, LoopTreeNodeIteratorImpl> LoopTreeNodeIterator;

class LoopTreeCreate
{
  LoopTreeRoot *root;
  LoopTreeObserveImpl *impl;
 public:
  virtual LoopTreeNode* CreateStmtNode(AstNodePtr s)
     { LoopTreeNode *r =  new LoopTreeStmtNode(s);
       AttachObserver(r);
       return r; }
  virtual LoopTreeNode* CreateLoopNode( LoopTransformInterface &fa, 
                                         const AstNodePtr& ctrl) { 
        LoopTreeNode* r=  new LoopTreeLoopNode(fa, ctrl); 
        AttachObserver(r); return r; 
      }
  virtual LoopTreeNode* CreateLoopNode(const SymbolicVar& ivar, 
                                        const SymbolicVal& lb,
                                           const SymbolicVal& ub, 
                                         const SymbolicVal& step)
      { LoopTreeNode* r= new LoopTreeLoopNode( ivar, lb, ub, step); 
        AttachObserver(r); return r; }
  virtual LoopTreeNode* CreateLoopNode(SymbolicVar ivar)
      { LoopTreeNode* r= new LoopTreeLoopNode( ivar,SymbolicVal(),SymbolicVal(),
                                     SymbolicVal()); 
        AttachObserver(r); return r; }

  LoopTreeNode* CloneTree( LoopTreeNode *n);

  LoopTreeCreate( int _level = 0);
  ~LoopTreeCreate();

  LoopTreeNode* GetTreeRoot() const { return root; }
  int LoopLevel() const { return root->LoopLevel(); }
  void Dump() const { root->DumpTree(); }
  AstNodePtr CodeGen( LoopTransformInterface& fa) { return root->CodeGen(fa); }

  void AttachObserver( LoopTreeObserver &o) const;
  void DetachObserver( LoopTreeObserver &o) const;
  void AttachObserver( LoopTreeNode* n) const;
};

class LoopTreeInterface {
 public:
  LoopTreeNode* GetParent( LoopTreeNode* n) const { return n->Parent(); }
  const LoopTreeNode* GetParent( const LoopTreeNode* n) const 
               { return n->Parent(); }

  VarInfo GetVarInfo( const LoopTreeNode* n) const 
              { return n->GetVarInfo(); }
  bool IsLoop( const LoopTreeNode* n) const { return n->IncreaseLoopLevel(); }
};

class LoopTreeTraverse : public TreeTraverse<LoopTreeNode>
{
  LoopTreeNode *root, *cur, *last;
  TraversalOpt opt;
 public:
  LoopTreeTraverse( LoopTreeNode *r, TraversalOpt o= PreOrder)
    : root(r), opt(o) { Reset(); }
  ~LoopTreeTraverse() {}
  LoopTreeNode * Current() const { return cur; }
  LoopTreeNode*& Current() { return cur; }
  void Reset() { cur = FirstNode(root, opt); last = LastNode(root,opt); }
  void Advance()
     { if (cur != 0 && cur != last)
	 cur = NextNode(cur, opt);
       else
         cur = 0;
     }
//Boolean ReachEnd() const {  return cur == 0; }
  int ReachEnd() const {  return cur == 0; }
};

typedef SelectObject<LoopTreeNode*> SelectLoopTreeNode;

class SelectStmt : public SelectLoopTreeNode
{
// public: Boolean operator()(LoopTreeNode* const& n)  const
  public: int operator()(LoopTreeNode* const& n)  const
           { return n->GetOrigStmt() != 0 && !n->IncreaseLoopLevel(); }
};
class SelectLoop : public SelectLoopTreeNode
{
//public: Boolean operator()(LoopTreeNode* const& n)  const
  public: int operator()(LoopTreeNode* const& n)  const
      { return n->IncreaseLoopLevel(); }
};
typedef SelectIterator<LoopTreeNode*,LoopTreeNode*&,LoopTreeTraverse,SelectStmt>
         LoopTreeTraverseSelectStmt;
typedef SelectIterator<LoopTreeNode*,LoopTreeNode*&,LoopTreeTraverse,SelectLoop>
        LoopTreeTraverseSelectLoop;
inline unsigned CountEnclosedStmts(LoopTreeNode *root) 
  { return CountIteratorSize(LoopTreeTraverseSelectStmt(root)); }

class LoopTreeGetVarBound 
 : public SymbolicBoundAnalysis<LoopTreeNode*, LoopTreeInterface>
{ public:
    LoopTreeGetVarBound ( LoopTreeNode* n, LoopTreeNode* _ances = 0) 
      : SymbolicBoundAnalysis<LoopTreeNode*, LoopTreeInterface>
        (LoopTreeInterface(), n, _ances) {}
};

class LoopTreeGetVarConstBound 
 : public SymbolicConstBoundAnalysis<LoopTreeNode*, LoopTreeInterface>
{ public:
    LoopTreeGetVarConstBound ( LoopTreeNode* n, LoopTreeNode* _ances = 0) 
      : SymbolicConstBoundAnalysis<LoopTreeNode*, LoopTreeInterface>
        (LoopTreeInterface(), n, _ances) {}
};
#endif

