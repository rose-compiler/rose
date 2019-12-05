#include <sstream>

#include <SinglyLinkedList.h>
#include <PtrSet.h>
#include <map>

#include <LoopTree.h>
#include <LoopTreeTransform.h>
#include <LoopTreeObserver.h>
#include <LoopTreeDummyNode.h>
#include <StmtDepAnal.h>

extern bool DebugLoopDist();

/*QY: modify $prep$ to save the value of $v$ to variable named name_i; */
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
         const LoopTreeNode *orig = e.GetObserveNode();
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
Distribute( LoopTreeNode *n, SelectLoopTreeNode sel, int pos, ObserveTransform &ob, 
            LoopTreeNode** loc)
{
  if (sel(n)) {  
    if (DebugLoopDist()) 
      std::cerr << "loop dist selected node: " << n->toString() << "\n";
    return n; 
  }
  else {
    if (DebugLoopDist()) 
      std::cerr << "loop dist checking  node: " << n->toString() << "\n";
  }
  if (n->ChildCount() == 0) { return 0; }

  LoopTreeNode *tmp = 0, *tmp1 = 0,*n1 = 0;
  LoopTreeNode *cur = n->FirstChild(); 
  while (cur != 0) {
    tmp1 = cur;
    cur = cur->NextSibling();
    tmp = Distribute( tmp1, sel, pos, ob);
    if (tmp != tmp1) 
       break;
  }
  if (cur == 0 && tmp == tmp1) return n; 

  if (tmp1 == n->FirstChild() && tmp == 0) {
      if (loc != 0) *loc = tmp1;
      while (cur != 0) {
          tmp1 = cur;
          cur = cur->NextSibling();
          tmp = Distribute( tmp1, sel, pos, ob);
          if (tmp != 0) 
             break;
          else if (loc != 0) *loc = tmp1;
      }
      if (cur == 0 && tmp == 0) { if (loc != 0) *loc = 0; return 0; }
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
     tmp = Distribute( tmp1, sel, pos, ob);
     if (tmp != 0) {
        UnlinkNode(tmp);
        tmp->Link(n1, LoopTreeNode::AsLastChild);
     }
  }

  DistNodeInfo *info = new DistNodeInfo( n, n1, pos);
  ob.AddTransformInfo( info );

  return n1;
}

LoopTreeNode*  LoopTreeDistributeNode :: 
DistributeBefore(LoopTreeNode* parent, LoopTreeNode* loc)
{
  assert(parent != 0);
  LoopTreeNode* n = parent->Clone();
  n->Link( parent, LoopTreeNode::AsPrevSibling);
  LoopTreeNode *cur = parent->FirstChild(); 
  assert(cur != 0);
  for ( ;cur != 0 && cur != loc; ) {
      LoopTreeNode* tmp = cur;
      cur = cur->NextSibling();
      UnlinkNode(tmp);
      tmp->Link(n, LoopTreeNode::AsLastChild);
  }
  UnlinkNode(loc);
  loc->Link(n, LoopTreeNode::AsLastChild);
  if (n != 0) {
     DistNodeInfo info( parent, n, -1);
     Notify(info);
  }
  return n;
}

LoopTreeNode* LoopTreeDistributeNode ::
operator () ( LoopTreeNode *n, SelectLoopTreeNode stmts, Location config)
{
   ObserveTransform ob;
   LoopTreeNode* loc = 0, *n1=0;
   switch(config) {
     case BEFORE:
        {
         if (DebugLoopDist()) 
            std::cerr << "distributing loop before " << n->toString() << "\n";
        n1 = Distribute( n, stmts, -1, ob, &loc);
        if (n1 != n && n1 != 0) 
           n1->Link( n, LoopTreeNode::AsPrevSibling); break;
        }
     case AFTER:
        {
         if (DebugLoopDist()) 
            std::cerr << "distributing loop after " << n->toString() << "\n";
        n1 = Distribute( n, stmts, 1, ob, &loc);
        if (n1 != n && n1 != 0) 
            n1->Link( n, LoopTreeNode::AsNextSibling); break;
        }
     default: assert(0);
     }
     ob.Notify();
   return n1;
}

LoopTreeNode* LoopTreeSplitStmt ::
operator () (LoopTreeNode *stmt, LoopTreeNode *restr1, LoopTreeNode *restr2)
{
  LoopTreeNode *n = stmt->CloneTree();
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
       const LoopTreeNode *node = info.GetObserveNode();
       LoopTreeNode *that = info.GetOtherNode();
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

bool SelectArray:: insert_selinfo
   (LoopTreeGetVarBound &context, /*QY: context info. for symbolic eval*/
    int arrDim,                   /*QY: the array dimension being processed*/
    SymbolicVal& selincr,         /*QY: increment info. of current selection*/
    SymbolicVal& selsize,         /*QY: size info of current selection*/
    int minLevel, int maxLevel,   /*QY: accLoop levels of current selection */
    const SymbolicVal& startdiff) /*QY: difference in starting address to copy*/
{
   bool mod = true;
   std::list<ArrayDim>::iterator p = selinfo.begin(); 
   /*QY: does selinfo already has a compatible dimension? */
   while (p != selinfo.end()) {
       ArrayDim& cursel = *p;
       if (cursel.get_arrDim() < arrDim) /*QY:do nothing; keep going */; 
       else if (cursel.get_arrDim() > arrDim) { /*QY:insert a new dim here*/
           selinfo.insert(p, ArrayDim(arrDim, selincr,selsize,minLevel,maxLevel));
           break;
       }
       else if (cursel.get_incr() == selincr) 
       { /* QY:extend this dimension if necessary*/
          if (startdiff != 0 || !(cursel.get_size() >= selsize)) {
              cursel.size = cursel.size + startdiff;
              cursel.size = Max(cursel.size, selsize, &context); 
          }
          else mod = false;
          break;
       }
       else if (startdiff != 0) assert(false); /* QY: cannot handle this yet*/
       else {
         CompareRel rel = CompareVal(cursel.incr, selincr, &context);
         if (cursel.incr == 1 || (rel & REL_LE)) {
            /*QY: an additional selection for arrDim; insert here*/ 
            selinfo.insert(p, ArrayDim(arrDim, selincr,selsize,minLevel,maxLevel));
            break;
         }
         /*QY: else an additional selection for arrDim; insert later*/ 
       }
       ++p;
    }
   if (p == selinfo.end()) { 
       selinfo.push_back(ArrayDim(arrDim, selincr,selsize,minLevel,maxLevel));
   }
   return mod;
}

bool SelectArray::
select( LoopTreeNode* stmt, LoopTreeNode* cproot,
         AstInterface::AstNodeList& index) 
{
   AstInterface& fa = LoopTransformInterface::getAstInterface();
   bool mod = false;

//std::cerr << "cproot=" << cproot->toString() << "\n";
//std::cerr << "stmt=" << stmt->toString() << "\n";

   std::vector <SymbolicVar> ivars;
   std::vector <std::pair<SymbolicVar, SymbolicVal> > repl;
   std::vector <SymbolicBound> bounds;
   /*QY: ivars: index vars of surrounding loops
         bounds: constant bounds of loop ivars; 
         repl: ivars that need to be renamed with surrouding loop ivars */
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
   int loopnum = ivars.size();
   LoopTreeGetVarBound context(cproot);
   AstInterface::AstNodeList::const_iterator indexp = index.begin();
   for (int i = 0; indexp != index.end(); ++i, ++indexp) {
      /*QY: cur: the current array subscript in terms of loop ivars */
      SymbolicVal cur = SymbolicValGenerator::GetSymbolicVal(fa, *indexp);
      for (size_t k = 0; k < repl.size(); ++k) 
         cur = ReplaceVal(cur, repl[k].first, repl[k].second);

      typedef std::pair<SymbolicVal, std::vector<SymbolicVal> > CopyDim; 
      std::list <CopyDim> coeffs; 
      /*QY: curdim.second contains coefficients of each loop ivar in cur*/
      CopyDim curdim(1, std::vector<SymbolicVal>());
      SymbolicVal lb = 
        DecomposeAffineExpression(cur, ivars, curdim.second, ivars.size()); 
      for (size_t j = 0; j < ivars.size(); ++j) {
/*std::cerr << "j = " << j << "; ivar[j] = " << ivars[j] << "; curdim[j]=" << curdim.second[j] << "\n";
*/
           lb = lb + curdim.second[j] * bounds[j].lb;
      }
      coeffs.push_back( curdim); /* currently only a single dimension*/

      /* compute new address of array copy starting address*/
      SymbolicVal& curstart = selstart[i]; 
      SymbolicVal startdiff = 0;
      if (curstart.IsNIL())
         curstart = lb;
      else {
         SymbolicVal newstart = Min(lb, curstart, &context); 
         startdiff = curstart - newstart;
         curstart = newstart;
      }
      /* may need to split some dimensions into multiple ones*/
//std::cerr << "cur=" << cur.toString() << "\n";
      do {
        curdim = coeffs.front();
        coeffs.pop_front();
        SymbolicVal incr = curdim.first;
        for (size_t j1 = 0; j1 < ivars.size(); ++j1) {
           SymbolicVal cut = curdim.second[j1];
           if (cut != 0) {
             if (cut != 1) {
               curdim.second.push_back(0);  
               CopyDim split(cut*incr, std::vector<SymbolicVal>()); 
               if (SplitEquation(curdim.second, cut, bounds, context, split.second))
                   coeffs.push_back(split);
             }
           }
        }
        SymbolicVal selsize = lb - curstart+1;
        int minindex = loopnum, maxindex = -1;
        for (int j = 0; j < loopnum; ++j) {
            SymbolicVal cut = curdim.second[j];
//td::cerr << "j = " << j << "; cut = " << cut.toString() << "\n";
            if (cut != 0) {
               /* find loops that enumerate different elems of array*/
               selsize = selsize + (bounds[j].ub - bounds[j].lb) * cut;
               if (minindex > j) { minindex = j; }
               if (maxindex < j) { maxindex = j; }
            }
        }
        if (minindex > maxindex) { 
            /*QY: No loop inside cproot enumerate the array*/
           minindex = maxindex = loopnum; 
        }
        mod = insert_selinfo(context, i, incr, selsize, loopnum-maxindex-1,loopnum-minindex-1, startdiff); 
      } 
      while (coeffs.size()); 
   }
   return mod;
}

std::string SelectArray::toString() const
{
  std::string r = arrname + "(";
  std::list<ArrayDim>::const_iterator selp = selinfo.begin();
  for (size_t i = 0; i < selstart.size(); ++i) {
      r = r + selstart[i].toString();
      for ( ; selp != selinfo.end() && size_t((*selp).get_arrDim()) == i; ++selp) 
         r = r + ":" + (*selp).get_incr().toString() + ":" + (*selp).get_size().toString() + ":";
      r = r + "; ";
  }
  r = r + ")" + ":" + AstInterface::AstToString(prep); 
  return r;
}

CopyArrayConfig::
CopyArrayConfig( AstInterface& fa, const SelectArray& _sel, 
      CopyArrayOpt _opt, LoopTreeNode* root)
   : shift(0), sel(_sel), opt(_opt)
{ 
   if (root != 0)
        shift = root->GetLoopInfo();
   if (shift != 0) {
     LoopTreeGetVarConstBound boundop(root);
     std::vector<SymbolicVal> copyvec;
     for (size_t i = 0; i < sel.arr_dim(); ++i) 
        copyvec.push_back(0);
     for (SelectArray::iterator p = sel.begin(); !(p == sel.end()); ++p) {
        SelectArray::ArrayDim& cursel = *p;
        SymbolicVal& cur = copyvec[cursel.get_arrDim()];
        cur = cur + cursel.get_size() * cursel.get_incr();
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
   if (shift != 0) {
        bufshift = sel.buf_offset(fa, arrshift);
        assert(bufshift != 0);
   }

   if  (shift_buffer())
      opt = (CopyArrayOpt)(opt | SHIFT_COPY);
   else opt = (CopyArrayOpt)(opt & (~SHIFT_COPY));
   if (sel.need_allocate_buffer())
      opt = (CopyArrayOpt)(opt | ALLOC_COPY);
   else opt = (CopyArrayOpt)(opt & (~ALLOC_COPY));
   if (sel.need_delete_buffer())
      opt = (CopyArrayOpt)(opt | DELETE_COPY);
   else opt = (CopyArrayOpt)(opt & (~DELETE_COPY));
} 

void SelectArray:: set_bufname(AstInterface& fa)
{
    AstNodeType buftype = basetype;
    assert(bufsize.size() >= 1);
    SymbolicVal copysize = bufsize[bufsize.size()-1];
    if (!scalar_repl()) {
       AstInterface::AstNodeList bufsizes;
       bufsizes.push_back(copysize.CodeGen(fa).get_ptr());
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
void SelectArray:: set_bufsize(AstInterface& fa)
{
      int i = 0;
      for (SelectArray::iterator p = begin(); !(p == end()); ++p,++i) {
         SymbolicVal& cur = (*p).size;
         /*QY: modify prep to save the copy size to variable arrname_csz_i */
         save_value(fa, cur, arrname+"_csz", i, prep);
         bufsize.push_back(cur);
      }

      SymbolicVal cursize = 1;
      for (int j = bufsize.size()-1; j >= 0; --j) {  
         SymbolicVal tmp = bufsize[j];
         /*QY: modify prep to save the buf size to variable arrname_bsz_i */
         save_value(fa, cursize, arrname+"_bsz", j, prep);
         bufsize[j] = cursize;
         cursize = cursize * tmp;
      }
      bufsize.push_back(cursize);
}

bool SelectArray:: scalar_repl() const
{
   unsigned dim = sel_dim();
   int bufsizeval = -1;
   return (bufsize[dim].isConstInt(bufsizeval) && bufsizeval <= 5);
}
AstNodePtr SelectArray::
buf_codegen(AstInterface& fa, int offsetval) const
{
      if (offsetval == 0)
         return fa.CreateVarRef(bufname);
      else {
         std::stringstream out;
         out << bufname << "_" << offsetval;
         return fa.CreateVarRef(out.str());
      }
}

 
AstNodePtr SelectArray::
buf_codegen(AstInterface& fa, const SymbolicVal& bufoffset) const
{
   int offsetval=-1;
   if (!bufoffset.isConstInt(offsetval)) {
      AstNodePtr offsetAST = bufoffset.CodeGen(fa);
      return fa.CreateArrayAccess( fa.CreateVarRef(bufname), offsetAST);
   }
   return buf_codegen(fa, offsetval);
}

SymbolicVal SelectArray::
buf_offset( AstInterface& fa, std::vector<SymbolicVal>& arrindex) const
{
   SymbolicVal bufoffset = 0;
   SelectArray::const_iterator selp = begin();
   int j = 0;
   for (size_t i = 0; i < arrindex.size(); ++i) {
      SymbolicVal offset = arrindex[i];
      if (offset == 0)
         continue;
      for  ( ; !(selp == end()) && (size_t)(*selp).dim < i; ++selp,++j);
      for  ( ; !(selp == end()) && (size_t)(*selp).dim == i; ++selp,++j) {
         const ArrayDim& curdim = (*selp);
         SymbolicVal offset1 = offset / curdim.incr;
         SymbolicVal offset2;
         if (HasFraction(offset1, &offset2)) {
   if (offset2 >= curdim.size) { assert(0); }
            bufoffset = bufoffset + offset2 * bufsize[j];
            if (offset2 != 0)
               offset = offset - offset2 * (*selp).incr;
         } 
         else {
   if (offset1 >= curdim.size) { assert(0); }
            bufoffset = bufoffset + offset1 * bufsize[j];
            break;
         }
      }
   } 
   return bufoffset;
}

AstNodePtr SelectArray::
buf_codegen( AstInterface& fa, AstInterface::AstNodeList& arrindex) const
{
   std::vector <SymbolicVal> indexval;
   int i = 0;
   for (AstInterface::AstNodeList::const_iterator indexp = arrindex.begin();
          indexp != arrindex.end(); ++indexp, ++i) {
      SymbolicVal curindexval = SymbolicValGenerator::GetSymbolicVal(fa, *indexp);
      SymbolicVal offset = curindexval - sel_start(i);
      indexval.push_back(offset);
   } 
   SymbolicVal offset = buf_offset(fa, indexval);
   if (scalar_repl()) {
      int offsetval = -1;
      if (!offset.isConstInt(offsetval)) assert(0);
      return buf_codegen(fa, offsetval);
   }
   else
      return buf_codegen(fa, offset);
}

AstNodePtr SelectArray:: delete_codegen(AstInterface& fa) const
{
      int sizeval = -1;
      SymbolicVal sz = bufsize[bufsize.size()-1];
      if (!sz.isConstInt(sizeval)) 
         return fa.CreateDeleteArray(fa.CreateVarRef(bufname));
      return AST_NULL;
}

bool SelectArray:: need_allocate_buffer() const
{
  if (prep != 0)
      return true;
  return bufsize[bufsize.size()-1].GetValType() != VAL_CONST; 
}
bool SelectArray:: need_delete_buffer() const
{
  return bufsize[bufsize.size()-1].GetValType() != VAL_CONST; 
}

AstNodePtr SelectArray::allocate_codegen(AstInterface& fa) const
{
   // int sizeval = -1;
   SymbolicVal sz = bufsize[bufsize.size()-1];
   int bufsizeval = -1;
   if (!sz.isConstInt(bufsizeval))  {
     AstInterface::AstNodeList bufsizevec;
     bufsizevec.push_back(sz.CodeGen(fa).get_ptr());
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

std::string CopyArrayOpt2String(CopyArrayOpt  opt)
{
  std::string r;
  if (opt & ALLOC_COPY)
       r = r + "_alloc"; 
  if (opt & INIT_COPY)
       r = r + "_init"; 
  if (opt & SAVE_COPY)
       r = r + "_save"; 
  if (opt & SHIFT_COPY)
       r = r + "_shift"; 
  if (opt & DELETE_COPY )
       r = r + "_delete"; 
  return r;
}

std::string CopyArrayConfig::toString() const
{
  std::string r = sel.toString();
  if (shift != 0) {
      r = r + " : shift " + shift->toString();
  }
  r = r + CopyArrayOpt2String(opt); 
  return r;
}

class CopyCodeGen
{
 protected:
  AstInterface& fa;
  const SelectArray& sel;
  std::vector<SymbolicVal> arrindex;

  virtual void basecopy_codegen(AstNodePtr& res)=0;

public: 
   CopyCodeGen( const SelectArray& _sel, const std::vector<SymbolicVal>& _arrindex)
       : fa(LoopTransformInterface::getAstInterface()), sel(_sel), arrindex(_arrindex) 
   { assert(arrindex.size() > 0); }

  virtual void scalarcopy_codegen(SelectArray::const_iterator selp1, int curdim,
                      int copynum, const SymbolicVal& curincr, AstNodePtr& res)
     {
       SymbolicVal& curindex = arrindex[curdim];
       apply(selp1, res); 
       for (int k = 1; k < copynum; ++k) {
          curindex = curindex + curincr;
          apply(selp1, res);
       }
     }
  virtual void loopcopy_codegen(SelectArray::const_iterator selp1, int curdim,
            const SymbolicVar& ivar, const SymbolicVal& cursize, 
            const SymbolicVal& curincr, AstNodePtr& res)
    {
      SymbolicVal ub = (cursize-1)*curincr;  
      AstNodePtr body = fa.CreateBlock();
      AstNodePtr curcopy = fa.CreateLoop(ivar.CodeGen(fa), 
                                       fa.CreateConstInt(0), 
                                       ub.CodeGen(fa),
                                       curincr.CodeGen(fa), body, false);
      fa.BlockAppendStmt(res, curcopy);

      SymbolicVal& curindex = arrindex[curdim];
      curindex = curindex + ivar;
      apply(selp1, body);
    }
  void apply( SelectArray::const_iterator selp, AstNodePtr& res)
  {
    if (selp == sel.end()) { basecopy_codegen(res); return; }
    const SelectArray::ArrayDim& cursel = *selp;
    SelectArray::const_iterator selp1 = selp;
    ++selp1;

    SymbolicVal cursize = cursel.get_size(), curincr = cursel.get_incr();
    int copynum = 0;
    if (cursize.isConstInt(copynum)) {
       scalarcopy_codegen(selp1, cursel.get_arrDim(), copynum, curincr, res); 
    }
    else {
      if (sel.scalar_repl()) assert(false);
      std::string ivarname = fa.NewVar(fa.GetType("int"));
      SymbolicVar ivar(ivarname, fa.GetRoot()); 
      loopcopy_codegen(selp1, cursel.get_arrDim(), ivar, cursize,curincr,res);
    }
  }
  void operator() (AstNodePtr& res)
   { apply(sel.begin(), res); }
};

class LoopCopyInitCodeGen  : public CopyCodeGen
{
 protected:
  SymbolicVal bufoffset;
 protected:
  void base_impl(AstNodePtr& res, const AstNodePtr& init) 
  {
    fa.BlockAppendStmt( res, init);
    fa.BlockAppendStmt( res,
             fa.CreateAssignment( bufoffset.CodeGen(fa), 
                      fa.CreateBinaryOP(AstInterface::BOP_PLUS, bufoffset.CodeGen(fa),
                                                       fa.CreateConstInt(1))));
  }
  virtual void basecopy_codegen(AstNodePtr& res) 
  {
    AstNodePtr lhs = sel.buf_codegen(fa, bufoffset);
    AstNodePtr rhs = LoopTransformInterface::CreateArrayAccess(sel.arr_name(),arrindex);
    AstNodePtr init = fa.CreateAssignment(lhs,rhs);
    base_impl(res, init);
  }
  public: 
   LoopCopyInitCodeGen( const SelectArray& _sel, 
              const std::vector<SymbolicVal>& _arrindex)
       : CopyCodeGen(_sel, _arrindex)
   {
      std::string bufoffsetname = fa.NewVar(fa.GetType("int"));
      bufoffset = SymbolicVar(bufoffsetname, fa.GetRoot());
   }
   LoopCopyInitCodeGen( const SelectArray& _sel, const std::vector<SymbolicVal>& _arrindex,
               const SymbolicVal& _bufoffset)
       : CopyCodeGen( _sel, _arrindex), bufoffset(_bufoffset) {}
   void operator() (AstNodePtr& res)
   { 
      fa.BlockAppendStmt(res, fa.CreateAssignment(bufoffset.CodeGen(fa),
                                             fa.CreateConstInt(0)));
      apply(sel.begin(), res); 
   }
};

class LoopCopySaveCodeGen : public LoopCopyInitCodeGen
{
  virtual void basecopy_codegen(AstNodePtr& res) 
  {
    AstNodePtr lhs = sel.buf_codegen(fa, bufoffset);
    AstNodePtr rhs = LoopTransformInterface::CreateArrayAccess(sel.arr_name(),arrindex);
    AstNodePtr init = fa.CreateAssignment(rhs,lhs);
    base_impl(res, init);
  }
 public: LoopCopySaveCodeGen( const SelectArray& _sel, const std::vector<SymbolicVal>& _arrindex)
    : LoopCopyInitCodeGen( _sel, _arrindex) {}
};

class LoopCopyShiftCodeGen : public LoopCopyInitCodeGen
{
  SymbolicVal bufshift; 
  std::vector<SymbolicVal> arrshift;
  virtual void basecopy_codegen(AstNodePtr& res) 
  {
    AstNodePtr lhs = sel.buf_codegen(fa, bufoffset);
    AstNodePtr rhs = sel.buf_codegen(fa, bufoffset + bufshift);
    AstNodePtr init = fa.CreateAssignment(lhs,rhs);
    base_impl(res, init);
  }
  virtual void scalarcopy_codegen(SelectArray::const_iterator selp1, int curdim,
                      int copynum, const SymbolicVal& curincr, AstNodePtr& res)
   {
      SymbolicVal curshift = arrshift[curdim]; 
      int shiftnum = 0;
      if (curshift.isConstInt(shiftnum) && shiftnum != 0 && curshift > curincr) {
         LoopCopyInitCodeGen::scalarcopy_codegen(selp1, curdim, copynum-shiftnum, curincr,res);
         copynum = shiftnum;
      } 
      LoopCopyInitCodeGen initgen(sel, arrindex, bufoffset);
      initgen.scalarcopy_codegen(selp1,curdim, copynum, curincr, res); 
  }
  virtual void loopcopy_codegen(SelectArray::const_iterator selp1, int curdim,
            const SymbolicVar& ivar, const SymbolicVal& cursize, 
            const SymbolicVal& curincr, AstNodePtr& res)
  {
     SymbolicVal curshift = arrshift[curdim];;
     if (curshift != 0 && curshift > curincr) {
        LoopCopyInitCodeGen::loopcopy_codegen(selp1,curdim, ivar, cursize-curshift, curincr, res);  
     }
     else curshift = cursize;
     LoopCopyInitCodeGen initgen(sel, arrindex, bufoffset);
     initgen.loopcopy_codegen(selp1,curdim, ivar, curshift, curincr, res); 
  }

 public: LoopCopyShiftCodeGen( const SelectArray& _sel, const std::vector<SymbolicVal>& _arrindex,
               const SymbolicVal& _bufshift,
               const std::vector<SymbolicVal>& _arrshift)
    : LoopCopyInitCodeGen( _sel, _arrindex),
      bufshift(_bufshift), arrshift(_arrshift) {}
};


class ScalarCopyInitCodeGen : public CopyCodeGen
{
 protected:
  int size;

  virtual void basecopy_codegen(AstNodePtr& res) 
  {
    AstNodePtr lhs = sel.buf_codegen(fa, size++);
    AstNodePtr rhs = LoopTransformInterface::CreateArrayAccess(sel.arr_name(),arrindex);
    fa.BlockAppendStmt(res, fa.CreateAssignment(lhs,rhs));
  }
 public: ScalarCopyInitCodeGen( const SelectArray& _sel, const std::vector<SymbolicVal>& _arrindex)
       : CopyCodeGen(_sel,_arrindex), size(0) {}
};

class ScalarCopySaveCodeGen : public ScalarCopyInitCodeGen
{
  virtual void basecopy_codegen(AstNodePtr& res) 
  {
    AstNodePtr lhs = sel.buf_codegen(fa, size++);
    AstNodePtr rhs = LoopTransformInterface::CreateArrayAccess(sel.arr_name(),arrindex);
    fa.BlockAppendStmt(res, fa.CreateAssignment(rhs,lhs));
  }
 public: ScalarCopySaveCodeGen( const SelectArray& _sel, const std::vector<SymbolicVal>& _arrindex)
    : ScalarCopyInitCodeGen(_sel, _arrindex) {}
};

class ScalarCopyShiftCodeGen : public ScalarCopyInitCodeGen
{
  SymbolicVal bufshift;

  virtual void basecopy_codegen(AstNodePtr& res) 
  {
    AstNodePtr lhs = sel.buf_codegen(fa, size++);
    AstNodePtr rhs;
    SymbolicVal offset = bufshift + size - 1;
    if (offset < sel.buf_size()) rhs = sel.buf_codegen(fa, offset);
    else rhs = LoopTransformInterface::CreateArrayAccess(sel.arr_name(),arrindex);
    fa.BlockAppendStmt(res, fa.CreateAssignment(lhs,rhs));
  }
 public: ScalarCopyShiftCodeGen( const SelectArray& _sel, const std::vector<SymbolicVal>& _arrindex,
               const SymbolicVal& _bufshift)
    : ScalarCopyInitCodeGen( _sel, _arrindex), bufshift(_bufshift) {}
};

AstNodePtr SelectArray::
initcopy_codegen( LoopInfo* shift) const
{ 
   AstInterface& fa = LoopTransformInterface::getAstInterface();
   AstNodePtr r = fa.CreateBlock(); 

   std::vector<SymbolicVal> arrindex;
   if (shift != 0) {
      for (size_t i = 0; i < arr_dim(); ++i) 
      {
         SymbolicVal curindex = ReplaceVal(sel_start(i), shift->GetVar(),
                                        shift->GetBound().lb);
         arrindex.push_back(curindex);
      }
   }
   else {
      for (size_t i = 0; i < arr_dim(); ++i) 
      {
         SymbolicVal curindex = sel_start(i);
assert(!curindex.IsNIL());
         arrindex.push_back(curindex);
      }
   }

   if (scalar_repl()) ScalarCopyInitCodeGen(*this, arrindex)( r); 
   else LoopCopyInitCodeGen(*this, arrindex)( r); 
   return r;
}

AstNodePtr SelectArray::
savecopy_codegen( ) const
{ 
   AstInterface& fa = LoopTransformInterface::getAstInterface();
   AstNodePtr r = fa.CreateBlock(); 

   std::vector<SymbolicVal> arrindex;
   for (size_t i = 0; i < arr_dim(); ++i) 
         arrindex.push_back(sel_start(i));

   if (scalar_repl()) ScalarCopySaveCodeGen(*this, arrindex)( r); 
   else LoopCopySaveCodeGen(*this, arrindex)( r); 
   return r;
}

AstNodePtr SelectArray::
shiftcopy_codegen( LoopInfo& shift, const std::vector<SymbolicVal>& arrshift,
                   const SymbolicVal& bufshift) const
{ 
   AstInterface& fa = LoopTransformInterface::getAstInterface();
   AstNodePtr r = fa.CreateBlock(); 

   std::vector<SymbolicVal> arrindex;
   for (size_t i = 0; i < arr_dim(); ++i) 
       arrindex.push_back( ReplaceVal(sel_start(i), shift.GetVar(),
                                 shift.GetVar()+shift.GetStep()));
   if (scalar_repl()) 
          ScalarCopyShiftCodeGen(*this, arrindex,bufshift)( r); 
   else LoopCopyShiftCodeGen(*this, arrindex,bufshift,arrshift)( r); 
   AstNodePtr cond = fa.CreateBinaryOP(AstInterface::BOP_LT,shift.GetVar().CodeGen(fa), 
                                       shift.GetBound().ub.CodeGen(fa));
   r = fa.CreateIf(cond, r);
   return r;
}

AstNodePtr CopyArrayConfig:: CodeGen( const AstNodePtr& h) const
{
   AstInterface& fa = LoopTransformInterface::getAstInterface();
   AstNodePtr r = fa.CreateBlock();
 
   if ( opt & ALLOC_COPY) { 
       AstNodePtr alloc = sel.allocate_codegen(fa);
       if (alloc != 0)
          fa.BlockAppendStmt(r, alloc);
   }
   if (opt & INIT_COPY) {
       AstNodePtr copy = sel.initcopy_codegen(shift);
       fa.BlockAppendStmt(r, copy);
   }
   
   fa.BlockAppendStmt(r, h);
   if (opt & SAVE_COPY) {
       AstNodePtr copy = sel.savecopy_codegen();
       fa.BlockAppendStmt(r, copy);
   }
   if (opt & SHIFT_COPY) {
       assert(shift != 0);
       AstNodePtr copy = sel.shiftcopy_codegen(*shift,arrshift,bufshift); 
       fa.BlockAppendStmt(r, copy);
   }
   if ((opt & DELETE_COPY)) {
       AstNodePtr del = sel.delete_codegen(fa);
       if (del != 0)
         fa.BlockAppendStmt(r,del); 
   }
   return r;
}

extern bool DebugCopyConfig();
void LoopTreeCopyArrayToBuffer::
operator()( LoopTreeNode* repl, LoopTreeNode* init,
            LoopTreeNode* save, CopyArrayConfig& c)
{
  CopyArrayOpt opt = c.get_opt();
  if (DebugCopyConfig()) {
      if (init != 0) 
        std::cerr << "init cutting node: " << init->toString() << std::endl;
      if (save != 0) 
        std::cerr << "save cutting node: " << save->toString() << std::endl;
      std::cerr << "copy config " << c.toString() << " : " << std::endl;
  }
  if (init == save) {
     LoopTreeNode *r = new LoopTreeCopyArray(c);
     InsertNode(r,init,-1); 
     return;
  }

  CopyArrayOpt opt1 = (CopyArrayOpt)(opt & (ALLOC_COPY | INIT_COPY));
  CopyArrayOpt opt2 = (CopyArrayOpt)(opt & (SAVE_COPY | SHIFT_COPY | DELETE_COPY ));
  if (opt1 != 0) {
     c.set_opt(opt1);
     LoopTreeNode *r = new LoopTreeCopyArray(c);
     InsertNode(r,init,-1); 
  }

  if (opt2 != 0) {
     c.set_opt(opt2);
     LoopTreeNode *r = new LoopTreeCopyArray(c);
     InsertNode(r,save,-1); 
  }
}

LoopTreeNode* LoopTreeReplaceAst :: 
operator()(LoopTreeNode* h,
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
       LoopTreeDistributeNode()( loop, sel, LoopTreeDistributeNode::AFTER);
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

