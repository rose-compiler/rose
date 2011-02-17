
#ifndef LOOPTREE_H
#define LOOPTREE_H

#include <stdlib.h>
#include <string>

#include <SymbolicVal.h>
#include <TreeImpl.h>
#include <IteratorTmpl.h>
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
  bool reverse;
 public:
  LoopInfo( SymbolicVar ivar,SymbolicVal lb, SymbolicVal ub, 
                    SymbolicVal s);
  LoopInfo( LoopTransformInterface &fa, const AstNodePtr& l);

  std::string toString() const 
       { return VarInfo::toString() + ":" + step.toString();}

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
  //According to Qing, GetOrigStmt() will return non-null for non-loop nodes
  virtual AstNodePtr GetOrigStmt()  const { return AST_NULL; }
  // For loop node, I add  GetOrigStmt2() to return the corresponding AstNodePtr
  virtual AstNodePtr GetOrigStmt2()  const { return AST_NULL; }

  virtual const LoopInfo* GetLoopInfo() const { return 0; }
  virtual LoopInfo* GetLoopInfo() { return 0; }

  virtual bool SelfRemove() { return false; }
  virtual LoopTreeNode* Clone() const = 0;

  void Dump() const { std::cerr << toString() << std::endl; }
  virtual std::string toString() const { return ""; }
  virtual AstNodePtr CodeGen( LoopTransformInterface &fa) const;

  void AttachObserver( LoopTreeObserver &o) const;
  void DetachObserver( LoopTreeObserver &o) const;
  void Notify( const LoopTreeObserveInfo &info) const;
  unsigned NumberOfObservers() const;
  void DumpTree() const { std::cerr << TreeToString() << std::endl; }
  std::string TreeToString() const;

  bool IsPerfectLoopNest() const;
  bool ContainLoop() const;
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
   std::string toString() const;
   std::string GetClassName() const { return "LoopTreeStmtNode"; }

   virtual AstNodePtr CodeGen( LoopTransformInterface &fa, const AstNodePtr& c) const;
   AstNodePtr GetOrigStmt() const { return start; }
   LoopTreeNode* Clone() const { return new LoopTreeStmtNode( *this ); }
 friend class LoopTreeCreate;
};

class LoopTreeLoopNode : public LoopTreeNode, public LoopTreeObserver
{
  AstNodePtr orig;
  LoopInfo info;
  virtual bool SelfRemove();
 protected:
  ~LoopTreeLoopNode();
  LoopTreeLoopNode( LoopTransformInterface &fa, const AstNodePtr& l); 
  LoopTreeLoopNode( const LoopTreeLoopNode& that) ;
  LoopTreeLoopNode( SymbolicVar _ivar,SymbolicVal _lb, SymbolicVal _ub, 
                  SymbolicVal _step);
  virtual AstNodePtr CodeGen( LoopTransformInterface &fa, const AstNodePtr& c) const;
  void UpdateSwapNode( const SwapNodeInfo& info);
 public:
  std::string toString() const;
  std::string GetClassName() const { return "LoopTreeLoopNode"; }
  int IncreaseLoopLevel() const { return 1; }
  //Liao, May 16, 2008 
  // GetOrigStmt() is designed to return NULL for read loop nodes according to Qing
  // So I use a new member function here as a workaround. 
  // Qing said there is a way to directly call dependence analysis on AST node
  // But I cannot find it so I stick to this extension 
  AstNodePtr GetOrigStmt2() const { return orig; }
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
  std::string toString() const;
  LoopTreeNode* Clone() const { return 0; }
  std::string GetClassName() const { return "LoopTreeRoot"; }
  virtual int LoopLevel() const { return level; }

 friend class LoopTreeCreate;
};

typedef IteratorImpl<LoopTreeNode*> LoopTreeNodeIteratorImpl;
typedef IteratorWrap<LoopTreeNode*, LoopTreeNodeIteratorImpl> 
LoopTreeNodeIterator;

class LoopTreeCreate
{
  LoopTreeRoot *root;
  LoopTreeObserveImpl *impl;
 public:
  virtual LoopTreeNode* CreateStmtNode(AstNodePtr s)
     { LoopTreeNode *r =  new LoopTreeStmtNode(s); AttachObserver(r); return r;}
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
  virtual ~LoopTreeCreate();

  LoopTreeNode* GetTreeRoot() const { return root; }
  int LoopLevel() const { return root->LoopLevel(); }
  void Dump() const { root->DumpTree(); }
  virtual AstNodePtr CodeGen( LoopTransformInterface& fa) { return root->CodeGen(fa); }

  void AttachObserver( LoopTreeObserver &o) const;
  void DetachObserver( LoopTreeObserver &o) const;
  void AttachObserver( LoopTreeNode* n) const;
};

class LoopTreeInterface {
 public:
  LoopTreeNode* GetParent( LoopTreeNode* n) const { return n->Parent(); }
  LoopTreeNode* getNULL() const { return 0; }
  
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
  bool ReachEnd() const {  return cur == 0; }
};

class LoopTreeTraverseSelectStmt 
  : public IteratorImplTemplate<LoopTreeNode*, LoopTreeTraverse>
{ 
   void SetIterator() {
       for ( ; !impl.ReachEnd(); impl.Advance()) { 
            LoopTreeNode* n = impl.Current();
            if (n->GetOrigStmt() != AST_NULL && !n->IncreaseLoopLevel())
               break;
        }
   }
 public: 
  LoopTreeTraverseSelectStmt( LoopTreeNode *r, 
                 LoopTreeTraverse::TraversalOpt o= LoopTreeTraverse::PreOrder)
   :IteratorImplTemplate<LoopTreeNode*,LoopTreeTraverse>(LoopTreeTraverse(r,o)) 
    { SetIterator(); }
  void Advance() { impl.Advance(); SetIterator(); }
  void Reset() { impl.Reset(); SetIterator(); }
  LoopTreeNode* Current() const { return impl.Current(); }
  LoopTreeNode*& Current() { return impl.Current(); }
  IteratorImpl<LoopTreeNode*>* Clone() const
   { return new LoopTreeTraverseSelectStmt(*this); }
};

class LoopTreeTraverseSelectLoop 
  : public IteratorImplTemplate<LoopTreeNode*, LoopTreeTraverse>
{ 
  void SetIterator() {
         for ( ; !impl.ReachEnd(); impl.Advance()) { 
            if (impl.Current()->IncreaseLoopLevel())
                break;
         }
   }
 public: 
  LoopTreeTraverseSelectLoop( LoopTreeNode *r, 
                 LoopTreeTraverse::TraversalOpt o= LoopTreeTraverse::PreOrder)
   :IteratorImplTemplate<LoopTreeNode*,LoopTreeTraverse>(LoopTreeTraverse(r,o)) 
    { SetIterator(); }
  void Advance() { impl.Advance(); SetIterator(); }
  void Reset() { impl.Reset(); SetIterator(); }
  LoopTreeNode* Current() const { return impl.Current(); }
  LoopTreeNode*& Current() { return impl.Current(); }
  IteratorImpl<LoopTreeNode*>* Clone() const
   { return new LoopTreeTraverseSelectLoop(*this); }
};

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

