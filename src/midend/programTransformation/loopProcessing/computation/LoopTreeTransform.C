
#include <sstream>

#include <SinglyLinkedList.h>
#include <PtrSet.h>
#include <map>

#include <LoopTree.h>
#include <LoopTreeTransform.h>
#include <LoopTreeObserver.h>
#include <LoopTreeDummyNode.h>
#include <StmtDepAnal.h>

void save_value( AstInterface& fa, SymbolicVal& v, const std::string& name, int i, AstNodePtr& prep)
{
  SymbolicValType t = v.GetValType();
  if (t == VAL_CONST || t == VAL_VAR)
      return;
  std::stringstream out;
  out << name << "_" << i;
  SymbolicVar var(fa.NewVar(fa.GetType("int"), out.str(),true), fa.GetRoot());
  
  AstNodePtr ast = fa.CreateAssignment(var.CodeGen(fa), v.CodeGen(fa));
  if (prep == 0) 
      prep = ast;
  else {
     AstNodePtr tmp = prep;
     prep =  fa.CreateBlock();
     fa.BlockAppendStmt(prep, tmp);
     fa.BlockAppendStmt(prep, ast);
  }
  v = var;
  return;
}

void Notify( const LoopTreeObserveInfo &e)
    {
         LoopTreeNode *orig = e.GetObserveNode();
         orig->Notify(e);
    }

void LoopTreeTransform::
InsertNode( LoopTreeNode *node, LoopTreeNode *pos, int opt)
{
  if (opt > 0) {
    UnlinkNode(node);
    ReplaceChildren( pos, node, 1 );
    node->Link(pos, LoopTreeNode::AsLastChild);
  }
  else {
    node->Link(pos, LoopTreeNode::AsPrevSibling);
    UnlinkNode(pos);
    pos->Link(node, LoopTreeNode::AsLastChild);
  }
}

LoopTreeNode* LoopTreeTransform:: InsertHandle(LoopTreeNode *pos, int opt)
{
  LoopTreeNode *result = new LoopTreeDummyNode();
  InsertNode(result, pos, opt);
  return result;
}

LoopTreeNode* LoopTreeTransform :: 
InsertLoop( LoopTreeNode* loop, LoopTreeNode *pos, int opt)
{
  InsertNode( loop, pos, opt);
  int level = loop->LoopLevel();
  LoopTreeTraverseSelectStmt iter(pos);
  for (LoopTreeNode *s; (s = iter.Current()); iter.Advance())  {
     InsertStmtLoopInfo info1(s, level);
     Notify(  info1 );
  }
  InsertLoopInfo info( loop);
  Notify(  info );
  return loop;
}

class ObserveTransform
{
  SinglyLinkedListWrap<LoopTreeObserveInfo*> infoList;
 public:
  void AddTransformInfo( LoopTreeObserveInfo *info)
    { infoList.AppendLast( info ); }
  void Notify()
    {
       for (SinglyLinkedEntryWrap<LoopTreeObserveInfo*>* info;
            (info = infoList.First()); ) {
         ::Notify( *info->GetEntry());
         delete info->GetEntry();
         infoList.PopFirst();
      }
    }
};

LoopTreeNode*  LoopTreeDistributeNode ::
Distribute( LoopTreeNode *n, SelectLoopTreeNode sel, ObserveTransform &ob)
{
  if (sel(n))
      return n;
  if (n->ChildCount() == 0)
      return 0;

  LoopTreeNode *tmp = 0, *tmp1 = 0,*n1 = 0;
  LoopTreeNode *cur = n->FirstChild(); 
  while (cur != 0) {
    tmp1 = cur;
    cur = cur->NextSibling();
    tmp = Distribute( tmp1, sel, ob);
    if (tmp != tmp1) 
       break;
  }
  if (cur == 0 && tmp == tmp1)
    return n;

  if (tmp1 == n->FirstChild() && tmp == 0) {
      while (cur != 0) {
          tmp1 = cur;
          cur = cur->NextSibling();
          tmp = Distribute( tmp1, sel, ob);
          if (tmp != 0) 
             break;
      }
      if (cur == 0 && tmp == 0)
        return 0;
      n1 = n->Clone();
  }
  else {
    n1 = n->Clone();
    for ( LoopTreeNode *cur1 = n->FirstChild(); cur1 != tmp1; ) {
      LoopTreeNode* t = cur1;
      cur1 = cur1->NextSibling();
      UnlinkNode(t);
      t->Link( n1, LoopTreeNode::AsLastChild);
    }
  }
  if (tmp != 0) {
    UnlinkNode(tmp);
    tmp->Link( n1, LoopTreeNode::AsLastChild);
  }
  while (cur != 0) {
     tmp1 = cur;
     cur = cur->NextSibling();
     tmp = Distribute( tmp1, sel, ob);
     if (tmp != 0) {
        UnlinkNode(tmp);
        tmp->Link(n1, LoopTreeNode::AsLastChild);
     }
  }

  DistNodeInfo *info = new DistNodeInfo( n, n1);
  ob.AddTransformInfo( info );

  return n1;
}

LoopTreeNode* LoopTreeDistributeNode ::
operator () ( LoopTreeNode *n, SelectLoopTreeNode stmts, bool before)
{
   ObserveTransform ob;
   LoopTreeNode *n1 = Distribute( n, stmts, ob);
   if (n1 != n && n1 != 0) {
     if (before)
        n1->Link( n, LoopTreeNode::AsPrevSibling);
     else
        n1->Link( n, LoopTreeNode::AsNextSibling);
     ob.Notify();
   }
   return n1;
}

LoopTreeNode*  LoopTreeDistributeNode :: operator () (LoopTreeNodeIterator nodes)
{
  LoopTreeNode* p = nodes.Current()->Parent();
  LoopTreeNode* n = p->Clone();
  n->Link( p, LoopTreeNode::AsPrevSibling);
  for ( ; !nodes.ReachEnd(); ++nodes) {
    LoopTreeNode *tmp = nodes.Current();
    UnlinkNode(tmp);
    tmp->Link(n, LoopTreeNode::AsLastChild);
  }
  DistNodeInfo info( p, n);
  Notify(info);
  return n;
}

LoopTreeNode* LoopTreeSplitStmt ::
operator () (LoopTreeNode *stmt, LoopTreeNode *restr1, LoopTreeNode *restr2)
{
  LoopTreeNode *n = stmt->Clone();
  LoopTreeNode *tn = restr1;
  tn->Link( stmt, LoopTreeNode::AsPrevSibling);
  n->Link( tn, LoopTreeNode::AsLastChild);

  LoopTreeNode *tn1 = restr2;
  tn1->Link( stmt, LoopTreeNode::AsPrevSibling);
  UnlinkNode(stmt);
  stmt->Link( tn1, LoopTreeNode::AsLastChild);
  return n;
}


LoopTreeNode* CreateSplitRestr( LoopTreeNode *l1, LoopTreeNode *l2, DepRel r)
{
  int min = r.GetMinAlign(), max = r.GetMaxAlign();
  DepDirType t = r.GetDirType();
  if (t == DEPDIR_EQ && min == max) 
       return new LoopTreeRelateLoopIvar(l1, l2, min);
  else if (t == DEPDIR_GE)
       return new LoopTreeRestrLoopRange(l1, -min, l2->GetLoopInfo()->GetVar(),
                                         SymbolicVal());
  else if (t == DEPDIR_LE)
       return new LoopTreeRestrLoopRange(l1, -max, SymbolicVal(),
                                         l2->GetLoopInfo()->GetVar());
  else if (t == DEPDIR_EQ && min != max) {
       SymbolicVal ivar = l2->GetLoopInfo()->GetVar();
       return new LoopTreeRestrLoopRange(l1, 0, ivar+min, ivar+max);
  }
  else
       assert(false);
  return 0;
}

LoopTreeNode* LoopTreeSplitStmt ::
operator () (LoopTreeNode *stmt, LoopTreeNode *l1, LoopTreeNode *l2, DepRel rel)
{
  LoopTreeNode* n = operator()(stmt, CreateSplitRestr( l1, l2, rel), CreateSplitRestr( l1, l2, -rel));
  SplitStmtInfo info(stmt, n, l1->LoopLevel(),l2->LoopLevel(), rel);
  Notify(info);
  return n;
}

LoopTreeNode* LoopTreeSplitStmt ::
operator() ( LoopTreeNode *stmt, LoopTreeNode* loop, const SymbolicVal& split)
{
  LoopTreeNode* n = operator()(stmt, new LoopTreeRestrLoopRange(loop, 0, SymbolicVal(),split),
                    new LoopTreeRestrLoopRange(loop, 0, split+1, SymbolicVal()));
  SplitStmtInfo2 info(stmt, n, loop->LoopLevel(), split);
  Notify(info);
  return n;
}

void LoopTreeMergeLoop :: 
operator () (  LoopTreeNode *ances, LoopTreeNode *desc, int align)
{
  MergeLoopInfo info( desc, ances, align);
  Notify(info);
  int level = desc->LoopLevel();
  LoopTreeTraverseSelectStmt iter(desc);
  for (LoopTreeNode *s; (s = iter.Current()); iter.Advance()) {
     DeleteStmtLoopInfo info1(s, level);
     Notify(info1);
  }

  SymbolicVar ivar1 = ances->GetLoopInfo()->GetVar();
  LoopInfo *info2 = desc->GetLoopInfo();

  VarInfo tmp(ivar1, info2->GetBound());
  SymbolicBound b = tmp.GetVarRestr(ivar1);
  assert( !b.lb.IsNIL() || !b.ub.IsNIL());
  LoopTreeRestrLoopRange *tn = 
                new LoopTreeRestrLoopRange( ances, align, b.lb, b.ub);
  InsertNode( tn, desc, -1);
  if ( align != 0 || ivar1 != info2->GetVar()) {
    SymbolicVar ivar2 = info2->GetVar();
    LoopTreeReplLoopVar *tn1 = new LoopTreeReplLoopVar( ivar1, align, ivar2);
    InsertNode( tn1, desc, -1);
  }
  RemoveNode(desc);
}  

void  LoopTreeMergeStmtLoop ::
operator ()( LoopTreeNode *ancs, LoopTreeNode *desc,
                LoopTreeNode *stmt, int align)
{
  MergeStmtLoopInfo info( stmt, ancs->LoopLevel(), desc->LoopLevel(), align);
  Notify(  info );

  LoopTreeRelateLoopIvar *n = new LoopTreeRelateLoopIvar( ancs, desc, align);
  InsertNode( n, stmt, -1);
} 

LoopTreeNode * LoopTreeBlockLoop ::
operator() ( LoopTreeNode *n, SymbolicVar bvar, SymbolicVal bsize)
{
  LoopInfo *info = n->GetLoopInfo();

  SymbolicVal step = info->GetStep() * bsize;
  LoopTreeBlockLoopNode *n1 = 
    new LoopTreeBlockLoopNode( bvar, info->GetBound().lb, 
                               info->GetBound().ub, step);
  InsertNode( n1, n, -1);
  LoopTreeGetVarBound bf( n1 );
  info->GetBound().Intersect( SymbolicBound(bvar, bvar + step - 1), &bf);

  BlockLoopInfo binfo( n, bsize);
  Notify(  binfo);
  return n1;
}

void LoopTreeTransform::
ReplaceChildren( LoopTreeNode *on, LoopTreeNode *nn, int pos)
{
  LoopTreeNode *p = nn->FirstChild();
  if (pos >= 0 || p == 0) {
    for ( LoopTreeNode *n1 = on->FirstChild(); n1 != 0; ) {
       LoopTreeNode *tmp = n1;
       n1 = n1->NextSibling();
       UnlinkNode(tmp);
       tmp->Link(nn, LoopTreeNode::AsLastChild);
    }
  }
  else {
    for ( LoopTreeNode *n1 = on->FirstChild(); n1 != 0; ) {
       LoopTreeNode *tmp = n1;
       n1 = n1->NextSibling();
       UnlinkNode(tmp);
       tmp->Link(p, LoopTreeNode::AsPrevSibling);
    }
  }
}

void  LoopTreeSwapNodePos ::
operator () (LoopTreeNode *parent, LoopTreeNode *child)
{
  UnlinkNode(child);
  child->Link( parent, LoopTreeNode::AsPrevSibling);
  InsertNode( parent, child, 1);

  SwapNodeInfo tinfo1(parent, child, 1,child->GetVarInfo()), 
               tinfo2(child, parent, -1,parent->GetVarInfo());
  Notify(tinfo1);
  Notify(tinfo2);
  if (parent->IncreaseLoopLevel() && child->IncreaseLoopLevel()) {
     int level = child->LoopLevel(), level1 = level+1;
     LoopTreeTraverseSelectStmt stmts(parent);
     for (LoopTreeNode *s; (s = stmts.Current()); stmts.Advance()) {
        SwapStmtLoopInfo info(s, level, level1);
        Notify(info);
     }
  }
}

void LoopTreeEmbedStmt :: 
operator () (LoopTreeNode *loop, LoopTreeNode* stmt, SymbolicVal selIter)
{
  LoopTreeNode* n = new LoopTreeRestrLoopRange(loop, 0, selIter, selIter);
  InsertNode(n, stmt, -1);
}

class OptimizeLoopTreeImpl : public LoopTreeObserver
{
  LoopTreeNode *cur;
  PtrSetWrap<LoopTreeNode> markSet;
  void ResetHoldTreeNode( LoopTreeNode *n1, bool detach = true)
      { 
        if (cur != 0 && detach) {
           cur->DetachObserver(*this);
        }
        cur = n1;  
        if ( cur != 0) {
           cur->AttachObserver(*this); 
           markSet.insert(cur);
        }
      }
  void UpdateDeleteNode( const LoopTreeNode *n )
      {
        assert (cur == n); 
        LoopTreeNode* n1 = n->PrevSibling();
        if (n1 != 0 && n1->PrevSibling() == 0) {
           while (n1->ChildCount() == 1)
              n1 = n1->FirstChild();
           ResetHoldTreeNode(n1, false);
        }
        else
             Advance(false);
        markSet.erase( const_cast<LoopTreeNode*>(n) );
      }
  void UpdateSwapNode( const SwapNodeInfo &info)
     {
       LoopTreeNode *node = info.GetObserveNode(), *that = info.GetOtherNode();
       assert (cur == node); 
       if (info.GetDirection() < 0) {
            if (!markSet.IsMember(that) ) {
               ResetHoldTreeNode(that);
            }
       }
     }
  LoopTreeNode* GetNext( LoopTreeNode *n) const
      { 
         if (n == 0)
            return 0;
         else 
           return LoopTreeTraverse::NextNode(n, LoopTreeTraverse::PostOrder); 
      }
  void Advance( bool detach = true)
    {
         LoopTreeNode *n = GetNext(cur);
         ResetHoldTreeNode( n, detach );
    }
 public:
  OptimizeLoopTreeImpl() : cur(0) {}
  void operator () (LoopTreeNode *r)
   {
     ResetHoldTreeNode(
        LoopTreeTraverse::FirstNode( r, LoopTreeTraverse::PostOrder));
     while (cur != 0) {
       if (cur == r) {
          ResetHoldTreeNode(0);
          break;
       }
       LoopTreeNode *tmp = cur ;
       if (!tmp->SelfRemove() && tmp == cur ) {
             Advance();
       }
     }
   }
};
void OptimizeLoopTree( LoopTreeNode *r)
 { OptimizeLoopTreeImpl()(r); }

bool SelectArray::
select(LoopTransformInterface& la, LoopTreeNode* stmt, LoopTreeNode* cproot,
         AstInterface::AstNodeList& index) 
{
   AstInterface& fa = la;
   bool mod = false;

   std::vector <SymbolicVar> ivars;
   std::vector <std::pair<SymbolicVar, SymbolicVal> > repl;
   std::vector <SymbolicBound> bounds;
   LoopTreeGetVarConstBound boundop(stmt,cproot);
   for (LoopTreeNode* c = stmt; c != cproot; c = c->Parent()) {
      LoopInfo* info = c->GetLoopInfo();
      if (info != 0) {
         const SymbolicVar& ivar= info->GetVar();
         ivars.push_back(ivar);
         bounds.push_back( boundop.GetConstBound(ivar));
      }
      else {
         VarInfo vinfo = c->GetVarInfo();
         const SymbolicBound& b = vinfo.GetBound();
         if (b.lb == b.ub)
            repl.push_back(std::pair<SymbolicVar,SymbolicVal>(vinfo.GetVar(), b.lb));
      }
   }
   LoopTreeGetVarBound context(cproot);
   AstInterface::AstNodeList::const_iterator indexp = index.begin();
   for (int i = 0; indexp != index.end(); ++i, ++indexp) {
      SymbolicVal cur = SymbolicValGenerator::GetSymbolicVal(fa, *indexp);
      for (size_t k = 0; k < repl.size(); ++k) 
         cur = ReplaceVal(cur, repl[k].first, repl[k].second);
      typedef std::pair<SymbolicVal, std::vector<SymbolicVal> > CopyDim; 
      std::list <CopyDim> coeffs; 
      CopyDim curdim(1, std::vector<SymbolicVal>());
      SymbolicVal lb = 
        DecomposeAffineExpression(la, cur, ivars, curdim.second, ivars.size()); 
      for (size_t j = 0; j < ivars.size(); ++j) {
           lb = lb + curdim.second[j] * bounds[j].lb;
      }
      coeffs.push_back( curdim);
      SymbolicVal& curstart = selstart[i]; 
      SymbolicVal startdiff = 0;
      if (curstart.IsNIL())
         curstart = lb;
      else {
         SymbolicVal newstart = Min(lb, curstart, &context); 
         startdiff = curstart - newstart;
         curstart = newstart;
      }
      while (coeffs.size()) {
        curdim = coeffs.front();
        coeffs.pop_front();
        SymbolicVal incr = curdim.first;
        for (size_t j1 = 0; j1 < ivars.size(); ++j1) {
           SymbolicVal cut = curdim.second[j1];
           if (cut != 0) {
             if (cut != 1) {
               curdim.second.push_back(0);  
               CopyDim split(cut*incr, std::vector<SymbolicVal>()); 
               if (SplitEquation(la, curdim.second, cut, bounds, context, split.second))
                   coeffs.push_back(split);
             }
           }
        }
        SymbolicVal selsize = lb - curstart+1;
        for (size_t j = 0; j < ivars.size(); ++j) {
            SymbolicVal cut = curdim.second[j];
            if (cut != 0) {
               selsize = selsize + (bounds[j].ub - bounds[j].lb) * cut;
             
            }
        }
        if (selsize <= 1 && startdiff == 0)
           continue;
        std::list<ArrayDim>::iterator p = selinfo.begin(); 
        for ( ; p != selinfo.end(); ++p) {
            ArrayDim& cursel = *p;
            if (cursel.dim < i) 
                 continue;
            else if (cursel.dim > i) {
                mod = true;
                selinfo.insert(p, ArrayDim(i, incr,selsize));
                break;
            }
            else {
              if (cursel.incr == incr) {
                if (startdiff != 0 || !(cursel.size >= selsize)) {
                   mod = true;
                   cursel.size = cursel.size + startdiff;
                   cursel.size = Max(cursel.size, selsize, &context); 
                }
                break;
              }
              else if (startdiff != 0)
                 assert(false);
              else if (CompareVal(incr * selsize, cursel.incr, 
                                &context) & REL_LE) 
                 continue;
              else if (CompareVal(cursel.size*cursel.incr, incr,
                                &context) & REL_LE) {
                mod = true;
                selinfo.insert(p, ArrayDim(i, incr,selsize));
                break;
              }
              else {
                mod= true;
                selsize = Max(cursel.size * cursel.incr, incr * selsize, 
                              &context); 
                std::list<ArrayDim>::iterator p1 = p; 
                for ( ; p1 != selinfo.end(); p = p1++) {
                  ArrayDim& sel1 = *p1;
                  assert(sel1.dim >= i);
                  if (sel1.dim > i) { 
                      (*p).size = selsize;
                      (*p).incr = 1;
                      break;
                  } 
                  else if (sel1.incr == 1) {
                      sel1.size = Max(sel1.size, selsize, &context);
                      selinfo.erase(p);
                      break;
                  }
                  else if (CompareVal(sel1.size * sel1.incr, incr,&context)
                               & REL_LE) {
                      sel1.size = Max(sel1.size, selsize / sel1.incr);
                      selinfo.erase(p);
                      break;
                  }
                  else {
                    selsize = Max(selsize, sel1.size * sel1.incr, &context);
                    selinfo.erase(p);
                  }
                }
                if (p1 == selinfo.end()) {
                   (*p).size = selsize;
                   (*p).incr = 1;
                } 
                break;
              }
           }
         }
         if (p == selinfo.end()) { 
               mod = true;
               selinfo.push_back(ArrayDim(i, incr,selsize));
         }
      } 
   }
   return mod;
}

std::string SelectArray::toString() const
{
  std::string r = "(";
  std::list<ArrayDim>::const_iterator selp = selinfo.begin();
  for (size_t i = 0; i < selstart.size(); ++i) {
      r = r + selstart[i].toString();
      for ( ; selp != selinfo.end() && size_t((*selp).dim) == i; ++selp) 
         r = r + ":" + (*selp).incr.toString() + ":" + (*selp).size.toString();
      r = r + "; ";
  }
  r = r + ")"; 
  return r;
}

CopyArrayConfig::
CopyArrayConfig( AstInterface& fa, const std::string& arr, const AstNodeType& base, 
                const SelectArray& _sel, LoopTreeNode* root)
   : shift(0), arrname(arr), basetype(base), sel(_sel)
{ 
   if (root != 0)
        shift = root->GetLoopInfo();
   if (shift != 0) {
     LoopTreeGetVarConstBound boundop(root);
     std::vector<SymbolicVal> copyvec;
     for (size_t i = 0; i < sel.arr_dim(); ++i) 
        copyvec.push_back(0);
     for (SelectArray::iterator p = sel.begin(); !(p == sel.end()); ++p) {
        SymbolicVal& cur = copyvec[p.cur_dim()];
        cur = cur + p.cur_size() * p.cur_incr();
     }
     bool succ = false;
     for (size_t j = 0; j < sel.arr_dim(); ++j) {
       SymbolicVal cur = sel.sel_start(j);
       SymbolicVal n = ReplaceVal(cur,shift->GetVar(),
                                  shift->GetVar()+shift->GetStep());
       SymbolicVal diff = n - cur;
       if (!(CompareVal(diff,copyvec[j], &boundop) & REL_LT)) {
          succ = false;
          break;
       }
       else if (diff != 0)
           succ = true;
       arrshift.push_back(diff);
     }
     if (!succ) {
        shift = 0;
        arrshift.clear();
     }
   }   

   set_bufsize(fa); 
   set_bufname(fa); 
   if (shift != 0) {
        bufshift = buf_offset(fa, arrshift);
        assert(bufshift != 0);
   }
} 

void CopyArrayConfig:: set_bufname(AstInterface& fa)
{
    AstNodeType buftype = basetype;
    SymbolicVal copysize = bufsize[bufsize.size()-1];
    if (!scalar_repl()) {
       AstInterface::AstNodeList bufsizes;
       bufsizes.push_back(copysize.CodeGen(fa));
       buftype = fa.GetArrayType(buftype, bufsizes);
       bufname = fa.NewVar(buftype, arrname + "_buf", true);
    } 
    else {
       int bufsizeval;
       if (!copysize.isConstInt(bufsizeval))
            assert(false);
       bufname = fa.NewVar(buftype, arrname + "_buf", true);
       for (int i = 1; i < bufsizeval; ++i) {
          std::stringstream out;
          out << bufname << "_" << i;
          fa.NewVar(buftype, out.str(), false); 
       }
    }
}
void CopyArrayConfig:: set_bufsize(AstInterface& fa)
{
      // unsigned dim = sel.sel_dim();
      int i = 0;
      for (SelectArray::iterator p = sel.begin(); !(p == sel.end()); ++p,++i) {
         SymbolicVal& cur = p.cur_size();
         save_value(fa, cur, arrname+"_csz", i, prep);
         bufsize.push_back(cur);
      }

      SymbolicVal cursize = 1;
      for (int j = bufsize.size()-1; j >= 0; --j) {  
         SymbolicVal tmp = bufsize[j];
         save_value(fa, cursize, arrname+"_bsz", j, prep);
         bufsize[j] = cursize;
         cursize = cursize * tmp;
      }
      bufsize.push_back(cursize);
}

bool CopyArrayConfig:: scalar_repl() const
{
   unsigned dim = sel.sel_dim();
   int bufsizeval = -1;
   return (bufsize[dim].isConstInt(bufsizeval) && bufsizeval <= 5);
}
 
AstNodePtr CopyArrayConfig::
buf_codegen(AstInterface& fa, const SymbolicVal& bufoffset) const
{
   if (scalar_repl()) {
      int offsetval = -1;
      if (!bufoffset.isConstInt(offsetval))
          assert(false);
      if (offsetval == 0)
         return fa.CreateVarRef(bufname);
      else {
         std::stringstream out;
         out << bufname << "_" << offsetval;
         return fa.CreateVarRef(out.str());
      }
   }
   else {
      AstInterface::AstNodeList bufindex;
      bufindex.push_back( bufoffset.CodeGen(fa));
      return fa.CreateArrayAccess( fa.CreateVarRef(bufname), bufindex);
   }
}

SymbolicVal CopyArrayConfig::
buf_offset( AstInterface& fa, std::vector<SymbolicVal>& arrindex) const
{
   SymbolicVal bufoffset = 0;
   SelectArray::const_iterator selp = sel.begin();
   int j = 0;
   for (size_t i = 0; i < arrindex.size(); ++i) {
      SymbolicVal offset = arrindex[i];
      if (offset == 0)
         continue;
      for  ( ; !(selp == sel.end()) && (size_t)selp.cur_dim() < i; ++selp,++j);
      for  ( ; !(selp == sel.end()) && (size_t)selp.cur_dim() == i; ++selp,++j) {
         SymbolicVal offset1 = offset / selp.cur_incr();
         SymbolicVal offset2;
         if (HasFraction(offset1, &offset2)) {
            bufoffset = bufoffset + offset2 * bufsize[j];
            if (offset2 != 0)
               offset = offset - offset2 * selp.cur_incr();
         } 
         else {
            bufoffset = bufoffset + offset2 * bufsize[j];
            break;
         }
      }
   } 
   return bufoffset;
}

AstNodePtr CopyArrayConfig::
buf_codegen( AstInterface& fa, AstInterface::AstNodeList& arrindex) const
{
   std::vector <SymbolicVal> indexval;
   int i = 0;
   for (AstInterface::AstNodeList::const_iterator indexp = arrindex.begin();
          indexp != arrindex.end(); ++indexp, ++i) {
      SymbolicVal curindexval = SymbolicValGenerator::GetSymbolicVal(fa, *indexp);
      SymbolicVal offset = curindexval - sel.sel_start(i);
      indexval.push_back(offset);
   } 
   SymbolicVal offset = buf_offset(fa, indexval);
   return buf_codegen(fa, offset);
}

AstNodePtr CopyArrayConfig:: delete_codegen(AstInterface& fa) const
{
      int sizeval = -1;
      SymbolicVal sz = bufsize[bufsize.size()-1];
      if (!sz.isConstInt(sizeval)) 
         return fa.CreateDeleteArray(fa.CreateVarRef(bufname));
      return AST_NULL;
}

bool CopyArrayConfig:: need_allocate_buffer() const
{
  if (prep != 0)
      return true;
  return bufsize[bufsize.size()-1].GetValType() != VAL_CONST; 
}
bool CopyArrayConfig:: need_delete_buffer() const
{
  return bufsize[bufsize.size()-1].GetValType() != VAL_CONST; 
}

AstNodePtr CopyArrayConfig::allocate_codegen(AstInterface& fa) const
{
   // int sizeval = -1;
   SymbolicVal sz = bufsize[bufsize.size()-1];
   int bufsizeval = -1;
   if (!sz.isConstInt(bufsizeval))  {
     AstInterface::AstNodeList bufsizevec;
     bufsizevec.push_back(sz.CodeGen(fa));
     AstNodePtr alloc = 
           fa.CreateAllocateArray(fa.CreateVarRef(bufname),basetype, bufsizevec);
     if (prep == 0)
          return alloc;
     else { 
         AstNodePtr r = fa.CreateBlock();
         fa.BlockAppendStmt(r, prep);
         fa.BlockAppendStmt(r, alloc);
         return r;
     }
   }
   else
     return prep; 
}

std::string CopyArrayConfig::toString() const
{
  std::string r = arrname + sel.toString() + ":" + AstToString(prep); 
  if (shift != 0) {
      r = r + " : shift " + shift->toString();
/*
      for (int i = 0; i < shift.size(); ++i)
         r = r + shiftbuf[i]->toString() + "-";
*/
  }
  return r;
}

void CopyArrayConfig::
copy_scalar_codegen(LoopTransformInterface& la, 
                    SelectArray::const_iterator selp, 
                    std::vector<SymbolicVal>& arrindex, CopyOpt opt,
                    AstNodePtr& r, int &size) const
{
  AstInterface& fa = la;
  if (selp == sel.end()) {
    AstNodePtr lhs = buf_codegen(fa, SymbolicVal(size++));
    AstNodePtr rhs;
    if (opt == SHIFT_COPY) {
        SymbolicVal offset = bufshift + size - 1;
        if (offset < bufsize[bufsize.size()-1]) 
            rhs = buf_codegen(fa, offset);
    }
    if (rhs == 0) {
      AstInterface::AstNodeList indexlist;
      for (std::vector<SymbolicVal>::const_iterator indexp = arrindex.begin();
            indexp != arrindex.end(); ++indexp) 
              indexlist.push_back((*indexp).CodeGen(fa));
      rhs = la.CreateArrayAccess(fa.CreateVarRef(arrname),indexlist); 
    }
    AstNodePtr init;
    if (opt == INIT_COPY || opt == SHIFT_COPY) 
        init = fa.CreateAssignment(lhs,rhs);
    else if (opt == SAVE_COPY)
         init = fa.CreateAssignment(rhs,lhs);
    else
         assert(false);
    fa.BlockAppendStmt(r, init);
    return;
  }
  SelectArray::const_iterator selp1 = selp;
  ++selp1;
  SymbolicVal curindex = arrindex[selp.cur_dim()];
  int copynum = 0;
  if (!selp.cur_size().isConstInt(copynum)) 
       assert(false);
  copy_scalar_codegen(la, selp1, arrindex, opt, r, size); 
  for (int k = 1; k < copynum; ++k) {
    curindex = curindex + selp.cur_incr();
    arrindex[selp.cur_dim()] = curindex;
    copy_scalar_codegen(la, selp1, arrindex, opt, r, size); 
  }
}

void CopyArrayConfig:: 
copy_loop_codegen(LoopTransformInterface& la, SelectArray::const_iterator selp, 
                  std::vector<SymbolicVal>& arrindex, CopyOpt opt,
                  AstNodePtr& r, const AstNodePtr& bufoffset) const
{
  AstInterface& fa = la;
  if (selp == sel.end()) {
    AstInterface::AstNodeList bufindex;
    bufindex.push_back(bufoffset);
    AstNodePtr lhs = la.CreateArrayAccess(fa.CreateVarRef(bufname),bufindex);

    AstInterface::AstNodeList indexlist;
    AstNodePtr rhs;
    if (opt == SHIFT_COPY) {
        bufindex.clear();
        bufindex.push_back(
            fa.CreateBinaryOP(AstInterface::BOP_PLUS, fa.CopyAstTree(bufoffset),
                                               bufshift.CodeGen(fa)));
        rhs = fa.CreateArrayAccess(fa.CreateVarRef(bufname), bufindex); 
    }
    else {
       for (std::vector<SymbolicVal>::const_iterator indexp = arrindex.begin();
            indexp != arrindex.end(); ++indexp) 
              indexlist.push_back((*indexp).CodeGen(fa));
       AstNodePtr arrnode = fa.CreateVarRef(arrname);
       rhs = la.CreateArrayAccess(arrnode, indexlist); 
     }
     AstNodePtr init;
     if (opt == INIT_COPY || opt == SHIFT_COPY) 
       init = fa.CreateAssignment(lhs,rhs);
     else if (opt == SAVE_COPY)
       init = fa.CreateAssignment(rhs,lhs);
     else
       assert(false);
     fa.BlockAppendStmt( r, init);
     fa.BlockAppendStmt( r,
             fa.CreateAssignment( fa.CopyAstTree(bufoffset), 
                      fa.CreateBinaryOP(AstInterface::BOP_PLUS,
                                           fa.CopyAstTree(bufoffset),
                                                       fa.CreateConstInt(1))));
     return;
  }
   int cpsize = 0, cpincr = 0;
   if (selp.cur_size().isConstInt(cpsize) && selp.cur_incr().isConstInt(cpincr))  {
      int dim = selp.cur_dim();
      SymbolicVal& curindex = arrindex[dim];
      ++selp;
      copy_loop_codegen( la, selp, arrindex, opt, r, bufoffset); 
      int i = 1;
      if (opt == SHIFT_COPY) {
         for ( ; i < cpsize-arrshift[dim]; ++i) {
            curindex = curindex + cpincr;
            copy_loop_codegen(la,selp,arrindex,opt,r,fa.CopyAstTree(bufoffset)); 
         }
         opt = INIT_COPY;
      }
      for ( ; i < cpsize; ++i) {
         curindex = curindex + cpincr;
         copy_loop_codegen(la,selp,arrindex,opt,r,fa.CopyAstTree(bufoffset)); 
      }
   }
   else {
      std::string ivarname = fa.NewVar(fa.GetType("int"));
      AstNodePtr ivar = fa.CreateVarRef(ivarname); 

      int dim = selp.cur_dim();
      SymbolicVal cursize = selp.cur_size(), curincr = selp.cur_incr();
      ++selp;
    
      SymbolicVal lb = 0;
      SymbolicVal ub = (cursize-1)*curincr;  
      SymbolicVal curshift;
      if (opt == SHIFT_COPY && (curshift = arrshift[dim]) != 0 
          && curshift > curincr) {
        SymbolicVal ub1 = ub - curshift;
        AstNodePtr tmpbody = fa.CreateBlock();
        AstNodePtr tmpcopy = fa.CreateLoop( ivar, fa.CreateConstInt(0),
                                            ub1.CodeGen(fa),
                                            curincr.CodeGen(fa), tmpbody, false);
        fa.BlockAppendStmt(r, tmpcopy);
        copy_loop_codegen( la, selp, arrindex, SHIFT_COPY, tmpbody, bufoffset); 
        SymbolicVal tmp = (ub1 + curincr)/ curincr;
        HasFraction(tmp, &lb);
        opt = INIT_COPY;
      }
      AstNodePtr body = fa.CreateBlock();
      AstNodePtr curcopy = fa.CreateLoop( ivar, lb.CodeGen(fa),ub.CodeGen(fa),
                                            curincr.CodeGen(fa), body, false);
      fa.BlockAppendStmt(r, curcopy);
      SymbolicVal& curindex = arrindex[dim];
      curindex = curindex + SymbolicVar(ivarname, fa.GetRoot());
      copy_loop_codegen( la, selp, arrindex, opt, body, bufoffset); 
   }
}

AstNodePtr CopyArrayConfig::
copy_codegen( LoopTransformInterface& la, CopyOpt opt) const
{ 
   AstInterface& fa = la;
   AstNodePtr r = fa.CreateBlock(); 

   std::vector<SymbolicVal> arrindex;
   if (opt == SHIFT_COPY) {
      assert(shift != 0);
      for (size_t i = 0; i < sel.arr_dim(); ++i) 
         arrindex.push_back( ReplaceVal(sel.sel_start(i), shift->GetVar(),
                                 shift->GetVar()+shift->GetStep()));
   }
   else if (shift != 0 && opt == INIT_COPY) {
      for (size_t i = 0; i < sel.arr_dim(); ++i) 
         arrindex.push_back( ReplaceVal(sel.sel_start(i), shift->GetVar(),
                                        shift->GetBound().lb));
   }
   else {
      for (size_t i = 0; i < sel.arr_dim(); ++i) 
         arrindex.push_back(sel.sel_start(i));
   }

   if (scalar_repl()) {
       int size = 0;
       copy_scalar_codegen(la, sel.begin(), arrindex, opt, r, size); 
   }
   else {
      std::string bufoffsetname = fa.NewVar(fa.GetType("int"));
      AstNodePtr bufoffset = fa.CreateVarRef(bufoffsetname);
      fa.BlockAppendStmt(r, fa.CreateAssignment(fa.CopyAstTree(bufoffset),
                                             fa.CreateConstInt(0)));
      copy_loop_codegen( la, sel.begin(), arrindex, opt, r, bufoffset); 
   }
   if (opt == SHIFT_COPY) {
      AstNodePtr cond = fa.CreateBinaryOP(AstInterface::BOP_LT,shift->GetVar().CodeGen(fa), 
                                       shift->GetBound().ub.CodeGen(fa));
      r = fa.CreateIf(cond, r);
   }
   return r;
}

std::string CopyArrayConfig:: CopyOpt2String( int opt)
{
  std::string r;
  if (opt & ALLOC_COPY)
       r = r + "_alloc"; 
  if (opt & INIT_COPY)
       r = r + " _init"; 
  if (opt & SAVE_COPY)
       r = r + "_save"; 
  if (opt & SHIFT_COPY)
       r = r + "_shift"; 
  if (opt & DELETE_COPY )
       r = r + "_delete"; 
  return r;
}

void LoopTreeCopyArrayToBuffer::
operator()( LoopTransformInterface& la, LoopTreeNode* init, LoopTreeNode* save,
            const CopyArrayConfig& c, int opt)
{
  int opt1 = (opt & (CopyArrayConfig::ALLOC_COPY | CopyArrayConfig::INIT_COPY));
  int opt2 = (opt & (CopyArrayConfig::SAVE_COPY | CopyArrayConfig::SHIFT_COPY | CopyArrayConfig::DELETE_COPY ));
  if (opt1 != 0) {
     LoopTreeNode *r = new LoopTreeCopyArray(c, opt1); 
     InsertNode(r,init,-1); 
  }
  if (opt2 != 0) {
     LoopTreeNode *r = new LoopTreeCopyArray(c, opt2); 
     InsertNode(r,save,-1); 
  }
}

LoopTreeNode* LoopTreeReplaceAst :: 
operator()(LoopTransformInterface& la, LoopTreeNode* h,
                 const AstNodePtr& orig, const AstNodePtr& repl)
{
  LoopTreeNode* r = new LoopTreeReplAst(orig, repl);
  InsertNode(r, h, -1);
  return r;
}

class ApplyLoopSplittingImpl
{
  typedef SymbolicVal SplitEntry;
  typedef std::list<SplitEntry> SplitList;

  struct LoopLevelCompare {
     bool operator()(const LoopTreeNode* n1, const LoopTreeNode* n2) const
      {
         return n1->LoopLevel() > n2->LoopLevel();
      }
  };
  typedef std::map <LoopTreeNode*, SplitList, LoopLevelCompare> SplitMap;

  void SplitLoop( LoopTreeNode* loop, 
                  SplitList::const_iterator cur, SplitList::const_iterator end)
  {
       if (cur == end) {
          return;
       }
       PtrSetWrap <LoopTreeNode> stmtlist;
       for (LoopTreeTraverseSelectStmt stmts(loop);
            !stmts.ReachEnd(); stmts.Advance()) {
          stmtlist.insert(stmts.Current());
       }  
       SymbolicVal mid = (*cur);
       for (PtrSetWrap<LoopTreeNode>::const_iterator listp = stmtlist.begin();
           !listp.ReachEnd(); listp.Advance()) {
         LoopTreeNode* curstmt = listp.Current();
         LoopTreeSplitStmt()(curstmt, loop, mid);
       }
       SelectPtrSet<LoopTreeNode> sel(stmtlist);
       LoopTreeDistributeNode()( loop, sel, false);
       LoopTreeNode* loop1 = loop->NextSibling();
       OptimizeLoopTree(loop);       
       OptimizeLoopTree(loop1); 
       ++cur;
       if (!loop1->SelfRemove())
          SplitLoop(loop1, cur, end);
       if (!loop->SelfRemove()) 
           SplitLoop( loop, cur,end); 
  } 
 public:
  void operator()(LoopTreeNode *r)
  {
   SplitMap splitmap;
  
   for (LoopTreeTraverse p(r); !p.ReachEnd(); p.Advance()) { 
    LoopTreeNode* n = p.Current();
    if (n->GetClassName() == "LoopTreeRestrLoopRange") {
        LoopTreeRestrLoopRange *cur = dynamic_cast<LoopTreeRestrLoopRange*>(n);
        LoopTreeNode* loop = cur->GetRestrLoop();
        SplitList& splitlist = splitmap[loop];
        VarInfo curinfo = cur->GetVarInfo();
        SymbolicVal lb = curinfo.GetBound().lb, ub = curinfo.GetBound().ub;
        SymbolicVal mid = lb.IsNIL()? ub : lb-1;
        if (!lb.IsNIL())  
           splitlist.push_back(lb-1);
        if (!ub.IsNIL())
           splitlist.push_back(ub);
    }
   } 
   for (SplitMap::const_iterator splitp = splitmap.begin();
       splitp != splitmap.end(); ++splitp) {
     const std::pair<LoopTreeNode* const, SplitList>& cur = *splitp;
     SplitLoop( cur.first, cur.second.begin(),cur.second.end());
   }
  }
};

void ApplyLoopSplitting(LoopTreeNode *r)
{
  ApplyLoopSplittingImpl()(r);
}

