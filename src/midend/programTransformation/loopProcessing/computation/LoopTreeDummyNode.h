
#ifndef LOOP_TREE_DUMMY_NODE
#define LOOP_TREE_DUMMY_NODE

#include <LoopTreeHoldNode.h>
#include <LoopTree.h>
#include <SymbolicVal.h>
#include <LoopTransformInterface.h>

class LoopTreeDummyNode : public LoopTreeNode
{
  virtual ~LoopTreeDummyNode() {}
 public:
  LoopTreeDummyNode() {}
  std::string GetClassName() const { return "LoopTreeDummyNode"; }
  bool SelfRemove() { RemoveSelf(); return true; }
  LoopTreeNode* Clone() const {return new LoopTreeDummyNode(); }
};

class LoopTreeBlockLoopNode : public LoopTreeLoopNode
{
  ~LoopTreeBlockLoopNode()  {}

 public:
   LoopTreeBlockLoopNode( const SymbolicVar& ivar, const SymbolicVal& lb, 
                          const SymbolicVal& ub, const SymbolicVal& step)
     : LoopTreeLoopNode(ivar, lb, ub, step) {}

   std::string GetClassName() const { return "LoopTreeBlockLoopNode"; }
   int IncreaseLoopLevel() const { return 0; }
   LoopTreeNode* Clone() const
     { return new LoopTreeBlockLoopNode( *this); }
};

class LoopTreeRestrLoopRange 
   : public LoopTreeNode, public LoopTreeObserver
{
  SymbolicBound b;
  HoldAncesLoopObserver loop;
  ~LoopTreeRestrLoopRange(); 

  void ResetLoopAlign() 
      { if (loop.GetLoopAlign() != 0) {
             b = b -loop.GetLoopAlign(); 
             loop.SetLoopAlign(0);
        }
      }
  void UpdateSwapNode( const SwapNodeInfo &info);
  int IncreaseLoopLevel() const { return 0; }

 public:
  LoopTreeRestrLoopRange(  LoopTreeNode *l, int align, 
                          SymbolicVal _lb, SymbolicVal _ub ) ;

  LoopTreeNode* GetRestrLoop() const { return loop.GetAncesLoop(); }

  std::string GetClassName() const { return "LoopTreeRestrLoopRange"; }
  SymbolicVar GetVar() const 
      { return GetRestrLoop()->GetLoopInfo()->GetVar(); }
  VarInfo GetVarInfo() const 
      { 
        int align = loop.GetLoopAlign();
        return VarInfo(GetRestrLoop()->GetLoopInfo()->GetVar(), b - align);
      }
  virtual std::string toString() const;
  AstNodePtr CodeGen( const AstNodePtr& c) const;

  bool RemoveSelf();
  bool SelfRemove();
  bool MergeSibling( int opt) ;
  LoopTreeNode* Clone()  const
   { return new LoopTreeRestrLoopRange( loop.GetAncesLoop(),
                                        loop.GetLoopAlign(), b.lb, b.ub); }
};

class LoopTreeRelateLoopIvar : public LoopTreeNode
{
  HoldAncesLoopObserver loop1, loop2;
 public:
  LoopTreeRelateLoopIvar( LoopTreeNode* l1, LoopTreeNode *l2, int align)
    : LoopTreeNode(), loop1(l1, 0, this), loop2(l2,align,this) {} 
  virtual ~LoopTreeRelateLoopIvar() {} 

  LoopTreeNode* GetLoop1() const { return loop1.GetAncesLoop(); }
  LoopTreeNode* GetLoop2() const { return loop2.GetAncesLoop(); }
  SymbolicVar GetIvar1() const { return GetLoop1()->GetLoopInfo()->GetVar(); }
  SymbolicVar GetIvar2() const { return GetLoop2()->GetLoopInfo()->GetVar(); }
  int GetAlign() const { return loop2.GetLoopAlign() - loop1.GetLoopAlign();}
  std::string GetClassName() const { return "LoopTreeRelateLoopIvar"; }

  bool SelfRemove();
  bool MergeSibling( int opt);
  virtual std::string toString() const;
  AstNodePtr CodeGen( const AstNodePtr& c) const;
  LoopTreeNode* Clone() const
    { return new LoopTreeRelateLoopIvar(GetLoop1(), GetLoop2(), GetAlign()); }
};

class LoopTreeReplLoopVar : public LoopTreeNode,
                            protected LoopTreeObserver
{
  SymbolicVar oldvar;
  SymbolicVal newval;
  int align;
 public:
  LoopTreeReplLoopVar( const SymbolicVal& nval, int a, 
                       const SymbolicVar& oldv) 
    : LoopTreeNode(), oldvar(oldv), newval(nval), align(a) 
     { AttachObserver(*this); }
  ~LoopTreeReplLoopVar() {}

  SymbolicVar  GetOldVar() const { return oldvar; }
  SymbolicVal  GetNewVal() const { return align? newval + align : newval; }
  VarInfo GetVarInfo() const 
      { SymbolicVal v = GetNewVal();
        return VarInfo(oldvar, SymbolicBound(v,v)); }

  std::string GetClassName() const { return "LoopTreeReplLoopVar"; }
  bool SelfRemove() ;
  virtual std::string toString() const;
  AstNodePtr CodeGen( const AstNodePtr& c) const; 
  LoopTreeNode* Clone()  const
      { return new LoopTreeReplLoopVar( newval, align, oldvar); }
};

class LoopTreeReplAst :  public LoopTreeNode
{
 public:
  LoopTreeReplAst(const AstNodePtr& o, const AstNodePtr& r )
    : LoopTreeNode(), orig(o),repl(r) {}
  ~LoopTreeReplAst() {}

  std::string GetClassName() const { return "LoopTreeReplAst"; }
  virtual std::string toString() const;
  AstNodePtr CodeGen( ) const; 
  LoopTreeNode* Clone()  const
      { return new LoopTreeReplAst(orig, repl); }
 private:
  AstNodePtr orig, repl;
};


class LoopTreeCopyArray : public LoopTreeNode,
                            protected LoopTreeObserver
{
 public:
  LoopTreeCopyArray( const CopyArrayConfig& c)
    : LoopTreeNode(), config(c) { AttachObserver(*this); }
  ~LoopTreeCopyArray() {}

  const CopyArrayConfig& get_config() const { return config; }
  CopyArrayConfig& get_config() { return config; }

  std::string GetClassName() const { return "LoopTreeCopyArray"; }
  virtual std::string toString() const;
  AstNodePtr CodeGen( const AstNodePtr& c) const;
  LoopTreeNode* Clone()  const
      { return new LoopTreeCopyArray( config); }
 private:
  CopyArrayConfig config;
  void UpdateSwapNode( const SwapNodeInfo &info) { assert(false); }
};

#endif
