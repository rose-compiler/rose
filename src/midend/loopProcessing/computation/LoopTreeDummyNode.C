
#include <general.h>

#include <sstream>
#include <vector>
#include <SymbolicBound.h>
#include <LoopTransformInterface.h>
#include <FunctionObject.h>
#include <LoopTreeTransform.h>
#include <LoopTreeDummyNode.h>
#include <ProcessAstTree.h>
#include <StmtInfoCollect.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

LoopTreeRestrLoopRange::  LoopTreeRestrLoopRange(  LoopTreeNode *l, int align,
                          SymbolicVal _lb, SymbolicVal _ub )
    : LoopTreeNode(), b(_lb,_ub), loop(l, align, this) 
{
  AttachObserver(*this);
}

LoopTreeRestrLoopRange:: ~LoopTreeRestrLoopRange()
{
  DetachObserver(*this);
}

void LoopTreeRestrLoopRange :: UpdateSwapNode( const SwapNodeInfo &info)
   { 
       VarInfo l = info.GetVarInfo();
       if (l.IsTop())
         return;
       if (info.GetDirection() < 0)  {
           ResetLoopAlign(); 
           b.ReplaceVars(l);
       }
   }

string LoopTreeRestrLoopRange :: ToString() const
   {
      stringstream out(stringstream::out);
      out << "restrict loop ivar ";
      out << loop.GetAncesLoop()->GetLoopInfo()->GetVar().ToString();
      out << " + " << loop.GetLoopAlign() << " within range:\n ( " << b.ToString();
      return out.str();
   }

AstNodePtr LoopTreeRestrLoopRange :: 
CodeGen( LoopTransformInterface &la, const AstNodePtr& c) const
   {
     AstInterface& fa = la;
      SymbolicVar ivar = loop.GetAncesLoop()->GetLoopInfo()->GetVar();
      int align = loop.GetLoopAlign();
      SymbolicVal lval = align? b.lb-align : b.lb;
      AstNodePtr cond1;
      if (b.lb == b.ub) {
         cond1 = fa.CreateRelEQ( ivar.CodeGen(fa), lval.CodeGen(fa));
      }
      else {
         SymbolicVal uval = align? b.ub-align : b.ub;
         cond1 = lval.IsNIL()? 0: fa.CreateRelGE( ivar.CodeGen(fa), lval.CodeGen(fa));
         AstNodePtr cond2 = uval.IsNIL()? 0:
                   fa.CreateRelLE( ivar.CodeGen(fa), uval.CodeGen(fa));
         if (cond1 != 0 && cond2 != 0)
              cond1 = fa.CreateLogicalAND( cond1, cond2);
         else if (cond1 == 0)
              cond1 = cond2;
      }
      return fa.CreateIf( cond1, c );
   }

LoopTreeNode* ApproachAncesLoop( LoopTreeNode* start, SelectObject<LoopTreeNode*>& sel)
{
   LoopTreeNode* l = 0;
      for (l = start->Parent(); l != 0 && l->ChildCount() == 1; 
           l = l->Parent()) { 
         if (sel(l))
            return l;
         if (l->IncreaseLoopLevel()) {
            while (l->Parent() != start)
               LoopTreeSwapNodePos()(start->Parent(), start);
            l = start;
         }
      }
      return 0; 
}

Boolean LoopTreeRestrLoopRange :: RemoveSelf()
{
      if (ChildCount() == 0) {
        RemoveTree();
        return true;     
      }
      ResetLoopAlign();
      LoopTreeGetVarBound f(this);

      SymbolicVar _ivar = GetVar();
      LoopInfo* restrinfo = GetRestrLoop()->GetLoopInfo();
      if (!b.lb.IsNIL() && !b.ub.IsNIL()) {
          CompareRel r1 = CompareVal( b.lb, b.ub, &f);
          if (r1 == REL_GT) {
             RemoveTree();
             return true;
          }
      }
      if (!b.lb.IsNIL()) {
          if (CompareVal(b.lb,restrinfo->GetBound().ub,&f) == REL_GT) {
             RemoveTree();
             return true;
          }
          CompareRel r1 = CompareVal( b.lb, restrinfo->GetBound().lb, &f);
          if (r1 == REL_LE || r1 == REL_LT || r1 == REL_EQ) {
             b.lb = SymbolicVal();
          }
          else 
             b.lb = Max(b.lb,  restrinfo->GetBound().lb,&f); 
      }
      if (!b.ub.IsNIL()) {
         if (CompareVal(restrinfo->GetBound().lb,b.ub, &f) == REL_GT) {
             RemoveTree();
             return true;
         }
         CompareRel r2 = CompareVal( restrinfo->GetBound().ub, b.ub, &f);
         if (r2 == REL_LE || r2 == REL_LT || r2 == REL_EQ) {
            b.ub =  SymbolicVal();
         }
         else 
             b.ub = Min(b.ub,  restrinfo->GetBound().ub,&f); 
      }

      if (b.lb.IsNIL() && b.ub.IsNIL()) {
         LoopTreeNode::RemoveSelf();
         return true;
      }
      if (!b.ub.IsNIL() && !b.lb.IsNIL() && b.ub <= b.lb) {
         LoopTreeNode *l1 = GetRestrLoop();
         for (LoopTreeNode *l = Parent(); l != l1;  l = l->Parent()) { 
            LoopInfo *info = l->GetLoopInfo(); 
            if (info == 0)
               continue;
            int align;
            if ((info->GetVar() - b.lb).ToInt(align)) {
                 LoopTreeMergeStmtLoop()( l1, l, this, align);
                 LoopTreeNode::RemoveSelf();
                 return true;
            }
         } 
      }
      return false;
}

class SelectRestrLoopAnces : public SelectObject<LoopTreeNode*>
{
  LoopTreeRestrLoopRange *node;
 public:
  SelectRestrLoopAnces( LoopTreeRestrLoopRange *n) : node(n) {}
  Boolean operator ()( LoopTreeNode * const& n)  const
   { if (n == node->GetRestrLoop())
        return true;
     if (n->GetClassName() != node->GetClassName())
        return false;
     LoopTreeRestrLoopRange *that = static_cast<LoopTreeRestrLoopRange*>(n);
     return that->GetRestrLoop() == node->GetRestrLoop();
   } 
};

Boolean LoopTreeRestrLoopRange :: SelfRemove()
    {
      ResetLoopAlign();
      if (RemoveSelf())
          return true;

      SymbolicVar ivar = GetVar();
      SelectRestrLoopAnces sel(this);
      LoopTreeNode* l = ApproachAncesLoop( this, sel);
      if ( l != 0) {
         LoopTreeGetVarConstBound boundop(Parent(), l);
         b.ReplaceVars(boundop);
         LoopTreeGetVarBound boundInfo(l->Parent());
         if (l == GetRestrLoop()) 
             l->GetLoopInfo()->GetBound().Intersect( b, &boundInfo);
         else  
             static_cast<LoopTreeRestrLoopRange*>(l)->b.Intersect(b,&boundInfo);
         LoopTreeNode::RemoveSelf();
         return true;
      }
      return MergeSibling(-1) ;
    }

LoopTreeNode* FindSibling( LoopTreeNode*& t, int opt)
{
   for ( LoopTreeNode* l = t->Parent();
           l && l->ChildCount() == 1;
           t = l, l = l->Parent()) {}
   return (opt < 0)? t->PrevSibling() : t->NextSibling();
}

Boolean LoopTreeRestrLoopRange :: MergeSibling( int opt) 
   {
     LoopTreeNode *p1 = this;
     LoopTreeNode *p2 = FindSibling(p1, opt);
     for (LoopTreeNode *that = p2; 
          that != 0; 
          that = (that->ChildCount() == 1)? that->FirstChild() : 0) {
        if (that->GetClassName() != GetClassName())
           continue;
        LoopTreeRestrLoopRange *n = static_cast<LoopTreeRestrLoopRange*>(that);
        if ( n->GetRestrLoop() == GetRestrLoop()) {
          if ( (b.lb.IsNIL() || n->b.lb.IsNIL()) && 
               (b.ub.IsNIL() || n->b.ub.IsNIL()) )
               continue;
          if (p1 != this) {
             while (p1->Parent() != this)
                LoopTreeSwapNodePos()(Parent(), this);
          }
          if (p2 != that) {
             while (p2->Parent() != that)
                LoopTreeSwapNodePos()(that->Parent(), that);
          } 
          SymbolicVal lbr = b.lb, ubr = b.ub;
          if ( !lbr.IsNIL())
             lbr = n->b.lb.IsNIL()? n->b.lb : Min(lbr, n->b.lb);
          if (! ubr.IsNIL())
               ubr = n->b.ub.IsNIL()? n->b.ub : Max(ubr, n->b.ub);

          if (!lbr.IsNIL()) {
             if (!n->b.lb.IsNIL() && n->b.lb <= lbr)
                n->b.lb = SymbolicVal();
             if (!b.lb.IsNIL() && b.lb <= lbr)
                b.lb = SymbolicVal();
             if (!n->b.lb.IsNIL() && !b.lb.IsNIL()) 
               lbr = SymbolicVal();
          }

          if (!ubr.IsNIL()) {
             if (!n->b.ub.IsNIL() && n->b.ub >= ubr)
               n->b.ub = SymbolicVal();
             if (!b.ub.IsNIL() && b.ub >= ubr)
                b.ub = SymbolicVal();
             if (!n->b.ub.IsNIL() && !b.ub.IsNIL())
                ubr = SymbolicVal();
          }

          if (lbr.IsNIL() && ubr.IsNIL())
              continue;
	  LoopTreeRestrLoopRange *nr = new LoopTreeRestrLoopRange(GetRestrLoop(), 0, lbr, ubr);
	  nr->Link(this, AsPrevSibling);
          Unlink(); Link(nr, AsLastChild);
          n->Unlink();
          if (opt > 0)
             n->Link(nr, AsLastChild);
          else
             n->Link(nr, AsFirstChild);
          Boolean succ1 = RemoveSelf(), succ2 = n->RemoveSelf();
          if (!succ1 && succ2) 
             succ1 = MergeSibling(opt); 
          return succ1;
        }
      }
      return false;
   }

class SelectRelateLoopAnces : public SelectObject<LoopTreeNode*>
{
  LoopTreeRelateLoopIvar *node;
 public:
  SelectRelateLoopAnces( LoopTreeRelateLoopIvar *n) : node(n) {}
  Boolean operator ()( LoopTreeNode * const& n)  const
   {  return n == node->GetLoop2(); }
};

Boolean LoopTreeRelateLoopIvar :: SelfRemove()
    {
      if (ChildCount() == 0) {
        RemoveTree();
        return true;     
      }
      SelectRelateLoopAnces sel(this);
      LoopTreeNode* l = ApproachAncesLoop( this, sel);
      if (l != 0) {
        LoopTreeMergeLoop()( GetLoop1(), GetLoop2(), GetAlign());
        RemoveSelf();
        return true;
      }
      return MergeSibling(-1);
    }

Boolean LoopTreeRelateLoopIvar :: MergeSibling( int opt)
   {
     LoopTreeNode *p1 = this;
     LoopTreeNode *p2 = FindSibling(p1, opt);
     for (LoopTreeNode *that = p2; 
          that != 0 ;
          that = (that->ChildCount() == 1)? that->FirstChild() : 0) {
        if (that->GetClassName() != GetClassName())
           continue;
        LoopTreeRelateLoopIvar *n = static_cast<LoopTreeRelateLoopIvar*>(that);
        if ( n->GetLoop1() == GetLoop1() && n->GetLoop2() == GetLoop2()
            && GetAlign() == n->GetAlign()) {
           while (n->Parent()->ChildCount() == 1)
              LoopTreeSwapNodePos()(n->Parent(), n);
           while (Parent()->ChildCount() == 1)
              LoopTreeSwapNodePos()(Parent(), this);
           Unlink();
           Link(n, AsLastChild);
           RemoveSelf();
           return true;
        }
     }
     return false;
   }

string LoopTreeRelateLoopIvar :: ToString() const
   {
     stringstream out(stringstream::out);
     out << "relate loop ivar :";
     out << loop1.GetAncesLoop()->GetLoopInfo()->GetVar().ToString();
     out << loop1.GetAncesLoop()->LoopLevel();
     out << " + " << loop1.GetLoopAlign();
     out << " = ";
     out << loop2.GetAncesLoop()->GetLoopInfo()->GetVar().ToString();
     out << loop2.GetAncesLoop()->LoopLevel();
     out << " + " << loop2.GetLoopAlign();
     out << "\n";
     return out.str();
   }

AstNodePtr LoopTreeRelateLoopIvar :: 
CodeGen( LoopTransformInterface &la, const AstNodePtr& c) const
   { 
     AstInterface& fa = la;
     AstNodePtr cond = fa.CreateRelEQ( GetIvar1().CodeGen(fa),
                                              (GetIvar2()+GetAlign()).CodeGen(fa));
     AstNodePtr result = fa.CreateIf( cond, c);
     return result;
   }

Boolean LoopTreeReplLoopVar :: SelfRemove() 
    {
      if (ChildCount() == 0) {
        RemoveTree();
        return true;     
      }
      if ( GetOldVar() == GetNewVal()) {
         RemoveSelf();
         return true;
      }
      if (ChildCount()==1) {
        for (LoopTreeNode *n1 = FirstChild(); n1->ChildCount() == 1; 
             n1 = n1->FirstChild()) {
           if (n1->GetClassName() != GetClassName())
                continue;
           LoopTreeReplLoopVar *tmp =  static_cast <LoopTreeReplLoopVar*>(n1);
           if (tmp->GetNewVal() == GetOldVar()) {
              tmp->newval  = newval;
              tmp->align = 0;
              return false;
/*
              while (FirstChild() != tmp) 
                 LoopTreeSwapNodePos()( this, FirstChild() );
              tmp->newval = newval;
              tmp->align = 0;
              RemoveSelf();
              return true;
*/
           }
        }
      }
      return false;
    }

string LoopTreeReplLoopVar :: ToString() const
   {
     stringstream out(stringstream::out);
      out << "replace loop index variable " <<  oldvar.ToString();
      out << " with val:" << newval.ToString();
      out << " + " << align;
      out << "\n";
     return out.str();
   }

AstNodePtr LoopTreeReplLoopVar :: CodeGen( LoopTransformInterface &fa, const AstNodePtr& c) const
  {
    AstNodePtr result = c;
    if ( align != 0 || newval != oldvar ) {
      SymbolicVal val = GetNewVal();
      AstTreeReplaceVar op(oldvar, val); 
      op(fa, result);
    }
    return result;
  }

string LoopTreeCopyArray::ToString() const
{
   return config.ToString() + CopyArrayConfig::CopyOpt2String(opt);
}

string LoopTreeReplAst::ToString() const
{
  return "replace " + AstInterface::AstToString(orig) + " -> " + AstInterface::AstToString(repl);
}

AstNodePtr LoopTreeReplAst::CodeGen( LoopTransformInterface &fa) const
{
  AstInterface& ai = fa;
  ai.ReplaceAst(orig, repl);
  AstNodePtr r = LoopTreeNode::CodeGen(fa);
  ai.ReplaceAst(repl, orig);
  return r;
}
 
AstNodePtr LoopTreeCopyArray::
CodeGen( LoopTransformInterface &la, const AstNodePtr& h) const
{
   AstInterface& fa = la;
   AstNodePtr r = fa.CreateBasicBlock();
   fa.ReplaceAst(h,r);
 
   if ( opt & CopyArrayConfig::ALLOC_COPY) { 
       AstNodePtr alloc = config.allocate_codegen(fa);
       if (alloc != 0)
          fa.BasicBlockAppendStmt(r, alloc);
   }
   if (opt & CopyArrayConfig::INIT_COPY) {
       AstNodePtr copy = 
             config.copy_codegen(la, CopyArrayConfig::INIT_COPY); 
       fa.BasicBlockAppendStmt(r, copy);
   }
   
   fa.BasicBlockAppendStmt(r, h);
   if (opt & CopyArrayConfig::SAVE_COPY) {
       AstNodePtr copy = config.copy_codegen(la, CopyArrayConfig::SAVE_COPY); 
       fa.BasicBlockAppendStmt(r, copy);
   }
   if (opt & CopyArrayConfig::SHIFT_COPY) {
       AstNodePtr copy = config.copy_codegen(la, CopyArrayConfig::SHIFT_COPY); 
       fa.BasicBlockAppendStmt(r, copy);
   }
   if ((opt & CopyArrayConfig::DELETE_COPY)) {
       AstNodePtr del = config.delete_codegen(fa);
       if (del != 0)
         fa.BasicBlockAppendStmt(r,del); 
   }
   return r;
}


