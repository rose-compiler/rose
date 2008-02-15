#include <general.h>
#include <stdlib.h>

#include <LoopTree.h>
#include <LoopTreeObserver.h>
#include <LoopTreeBuild.h>
#include <ObserveObject.h>
#include <CommandOptions.h>
#include <LoopTransformInterface.h>
#include <SymbolicBound.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

Boolean LoopTreeLoopNode :: SelfRemove() 
{ 
  LoopTreeGetVarBound f(this);
  if (ChildCount() == 0 || 
      CompareVal(info.GetBound().ub, info.GetBound().lb,&f) == REL_LT) {
    RemoveTree();
    return true;    
  }
  return false; 
}

void LoopTreeLoopNode :: UpdateSwapNode( const SwapNodeInfo &swapInfo)
{
  VarInfo that = swapInfo.GetVarInfo();
  if (that.IsTop()) 
      return;
  if (swapInfo.GetDirection() < 0) {
     info.GetBound().ReplaceVars(that);
  }
  else {
    LoopTreeGetVarBound boundInfo(Parent());
    info.GetBound().Intersect(that.GetVarRestr(info.GetVar()), &boundInfo);
  }
}

class LoopTreeObserveImpl
   : public ObserveObject <LoopTreeObserver>
{
};

LoopTreeNode:: LoopTreeNode() 
  { impl = new LoopTreeObserveImpl(); }

LoopTreeNode:: LoopTreeNode( const LoopTreeNode& that) 
  { 
    impl = new LoopTreeObserveImpl(); 
/* do not copy observers?
    for (LoopTreeObserveImpl::Iterator p = that.impl->GetIterator(); 
         !p.ReachEnd(); ++p) {
        LoopTreeObserver* cur = *p;
        AttachObserver(*cur);
    }
*/
  }
void LoopTreeNode::UpdateDelete()
{
  if (impl != 0) {
    DeleteNodeInfo info(this);
    impl->Notify(info);
    if (Parent() != 0)
      Unlink();
    delete impl;
    impl = 0;
  }
}

LoopTreeNode::~LoopTreeNode()
{
  UpdateDelete();
}


LoopTreeStmtNode:: ~LoopTreeStmtNode() 
 { 
   UpdateDelete();
 }

LoopTreeCreate::LoopTreeCreate( int _level) 
 { 
   root = new LoopTreeRoot( _level); 
   impl = new LoopTreeObserveImpl(); 
 }
LoopTreeCreate :: ~LoopTreeCreate()
{
  delete root;
  delete impl;
}

void LoopTreeCreate :: AttachObserver( LoopTreeObserver &o) const
{
   impl->AttachObserver( &o );
   if (root != 0)
     for (LoopTreeTraverse p(root); !p.ReachEnd(); p.Advance()) 
       (p.Current())->AttachObserver(o);
}

void LoopTreeCreate :: DetachObserver( LoopTreeObserver &o) const
{
  impl->DetachObserver( &o );
  if (root != 0)
     for (LoopTreeTraverse p(root); !p.ReachEnd(); p.Advance()) 
       (p.Current())->DetachObserver(o);
}

void LoopTreeCreate::AttachObserver( LoopTreeNode* n) const
{
  for (LoopTreeObserveImpl::Iterator p = impl->GetIterator(); 
       !p.ReachEnd(); ++p) {
     LoopTreeObserver* cur = *p;
     n->AttachObserver(*cur);
  }
}

LoopTreeLoopNode::
LoopTreeLoopNode( SymbolicVar _ivar,SymbolicVal _lb, SymbolicVal _ub, 
                  SymbolicVal _step)
    : info(_ivar, _lb, _ub, _step) 
{
  AttachObserver(*this);
}

LoopTreeLoopNode::  
LoopTreeLoopNode( LoopTransformInterface &fa, const AstNodePtr& l)
    : LoopTreeNode(), orig(l), info(fa, l) 
{
  AttachObserver(*this);
}

LoopTreeLoopNode::LoopTreeLoopNode( const LoopTreeLoopNode& that) 
        : LoopTreeNode(that), orig(that.orig), info(that.info) 
{
  AttachObserver(*this);
}

LoopTreeLoopNode:: ~LoopTreeLoopNode() 
 { 
   UpdateDelete();
 }

void LoopTreeNode :: RemoveTree()
{
  for (LoopTreeNode *child = FirstChild(); child != 0; ) {
    LoopTreeNode *tmp = child;
    child = child->NextSibling();
    tmp->RemoveTree();
  }
  delete this;
}

void LoopTreeNode :: RemoveSelf( LoopTreeNode *p)
{
  for (LoopTreeNode *child = FirstChild(); child != 0; ) {
    LoopTreeNode *tmp = child;
    child = child->NextSibling();
    tmp->Unlink();
    if (p != 0)
      tmp->Link(p,AsLastChild);
    else
      tmp->Link(this, AsPrevSibling);
  }
  delete this;
}

void LoopTreeNode :: AttachObserver( LoopTreeObserver &o) const
{
   impl->AttachObserver( &o );
}

void LoopTreeNode :: DetachObserver( LoopTreeObserver &o) const
{
  impl->DetachObserver( &o );
}

void LoopTreeNode :: Notify( const LoopTreeObserveInfo &info) const
{
  if (impl)
    impl->Notify( info );
}

unsigned LoopTreeNode:: NumberOfObservers() const
{
  if (impl != 0)
    return impl->NumberOfObservers();
  return 0;
}

AstNodePtr LoopTreeNode :: CodeGen( LoopTransformInterface &la) const
{
  cerr << "LoopTreeNode::CodeGen start" << endl;
  AstInterface& fa = la;
  AstNodePtr result = 0;
  if (ChildCount() == 1) {
     result = FirstChild()->CodeGen(la);
  }
  else if (ChildCount() > 0) {
     std::vector<AstNodePtr> children;
     for (LoopTreeNode *child = FirstChild(); child != 0; 
          child = child->NextSibling()) {
         children.push_back(child->CodeGen(la));
     }
     result = fa.CreateBasicBlock(); // This needs to not exist (because it has a NULL parent) when CodeGen is called for the children
     for (size_t i = 0; i < children.size(); ++i) {
         fa.BasicBlockAppendStmt(result, children[i]);
     }
  }
  cerr << "LoopTreeNode::CodeGen before two-argument call " << result << endl;
  AstNodePtr result2 = CodeGen(la, result); 
  cerr << "LoopTreeNode::CodeGen end " << result << " -> " << result2 << endl;
  return result2;
}

string LoopTreeStmtNode :: ToString() const
{
  return AstInterface::AstToString( start );
}

AstNodePtr LoopTreeStmtNode :: 
CodeGen( LoopTransformInterface &la, const AstNodePtr& c) const
{ 
   AstInterface& fa = la;
   return fa.CopyAstTree( start ); 
}

LoopInfo :: LoopInfo( LoopTransformInterface &fa, const AstNodePtr& ctrl)
{
  bool succ = fa.IsFortranLoop(ctrl, &GetVar(), 
                                         &GetBound().lb, &GetBound().ub, &step);
  if (!succ)
     assert(false);
  reverse = (step <= 0);
  if (reverse) {
     SymbolicBound b = GetBound();
     GetBound().ub = b.lb;
     GetBound().lb = b.ub;
  }
}

LoopInfo:: LoopInfo( SymbolicVar ivar,SymbolicVal lb, SymbolicVal ub, 
                    SymbolicVal s)
    : VarInfo(ivar, lb, ub), step(s) 
{ 
  reverse = (step <= 0); 
  if (reverse) {
     SymbolicBound b = GetBound();
     GetBound().ub = b.lb;
     GetBound().lb = b.ub;
  }
}

string LoopTreeLoopNode :: ToString() const
{ return info.ToString(); }

AstNodePtr LoopTreeLoopNode :: 
CodeGen( LoopTransformInterface &la, const AstNodePtr& c) const
{
  AstInterface& fa = la;
  if (info.GetLoopUB() == info.GetLoopLB()) {       
      AstNodePtr varGenerated = info.GetVar().CodeGen(fa);
      AstNodePtr lbGenerated = info.GetLoopLB().CodeGen(fa);
      AstNodePtr r =  fa.CreateBasicBlock();
      fa.BasicBlockAppendStmt(r, 
            fa.CreateAssignment(varGenerated, lbGenerated));
      fa.BasicBlockAppendStmt(r, c);
      return r;
  }   
  else
      return fa.CreateLoop( info.GetVar().CodeGen(fa), 
                            info.GetLoopLB().CodeGen(fa),
                            info.GetLoopUB().CodeGen(fa), 
                            info.GetStep().CodeGen(fa), c);
}

string LoopTreeNode :: TreeToString() const
{
  string res = ToString();
  if (ChildCount() > 0) {
     for (LoopTreeNode *n = FirstChild(); n != 0; n = n->NextSibling()) {
        res = res + "\n" + n->TreeToString();
     }
     res = res + "end-sub-tree\n";
  }
  return res;
}

string LoopTreeRoot :: ToString() const
{
  char buf[20];
  sprintf( buf, "%d", level);
  return "LOOP_TREE_ROOT at level" + string(buf);
}


Boolean LoopTreeNode :: ContainLoop() const
{
  LoopTreeNode *r = const_cast<LoopTreeNode*>(this);
  return ! LoopTreeTraverseSelectLoop(r).ReachEnd();
}

Boolean LoopTreeNode :: IsPerfectLoopNest() const
{
  int level = 0;
  const LoopTreeNode *l=this; 
  for ( ; l->ChildCount() == 1; l = l->FirstChild()) {
     level += l->IncreaseLoopLevel();
  }

  for (LoopTreeNode *s = l->FirstChild(); s != 0 ; s = s->NextSibling()) {
    if (s->GetOrigStmt() == 0)
        return false;
  }
  return true;
}


LoopTreeNode* LoopTreeCreate:: CloneTree( LoopTreeNode *n)
{
  LoopTreeNode *n1 = n->Clone();
  for (LoopTreeNode *child = n->FirstChild(); child != 0;
       child = child->NextSibling()) {
       CloneTree(child)->Link( n1, LoopTreeNode::AsLastChild);
  }
  return n1;
}

