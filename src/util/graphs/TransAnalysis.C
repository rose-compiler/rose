#include <general.h>

#include <iostream>
#include <stdlib.h>

#include <vector>

#include <SCCAnalysis.h>
#include <TransAnalysis.h>

#include <DGBaseGraphImpl.h>
#include <GraphScope.h>

#include <assert.h>

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

class ShadowGraphNode : public GraphNode
{
  GraphNode *orig;
 public:
  ShadowGraphNode(GraphCreate* c, GraphNode *o) : GraphNode(c) 
        {orig = o; }
  GraphNode* GetOrig() const { return orig; } 
};

class CloneShadowGraphCreate : public ShadowGraphCreate 
{
  PtrSetWrap <GraphNode> shadows;
 public:
  CloneShadowGraphCreate( BaseGraphCreate *_impl) : ShadowGraphCreate(_impl){}

  BaseGraphCreate::Node* CloneBaseNode( GraphNode *orig)
    { ShadowGraphNode *n = new ShadowGraphNode(this, orig);
      shadows.Add(n);
      return CreateBaseNode(n); }
  BaseGraphCreate::Node* GetOrigBaseNode( GraphNode *orig)
   { return GetBaseNode(orig); }
  Boolean IsShadowNode( const GraphNode *node) const
    { return shadows.IsMember(const_cast<GraphNode*>(node)); }
  Boolean IsShadowBaseNode( const BaseGraphCreate::Node *base) const
   { GraphNode *n = ShadowGraphCreate::GetIDNode(base);
      return IsShadowNode(n);
   }
  GraphNode *GetOrigNode(const BaseGraphCreate::Node *base) const
    { GraphNode *n = ShadowGraphCreate::GetIDNode(base);
      if (IsShadowNode(n))
        return static_cast<ShadowGraphNode*>(n)->GetOrig();
      return n;
    }
};

template <class T>
class ShadowBaseInfoGraph : public TransInfoGraph<T>
{
  CloneShadowGraphCreate *cloneCreate;
  TransInfoGraph<T> *recOp;
 public:
  ShadowBaseInfoGraph( CloneShadowGraphCreate *g, TransInfoGraph<T>* op)
     : cloneCreate(g), recOp(op) {}
  Boolean TransInfoComputed(const GraphNode *src, const GraphNode *snk)
      { assert( cloneCreate->IsBaseNode(src) && cloneCreate->IsBaseNode(snk));
        const BaseGraphCreate::Node* b1 = static_cast<const BaseGraphCreate::Node*>(src),
                           * b2 = static_cast<const BaseGraphCreate::Node*>(snk);
       return recOp->TransInfoComputed( cloneCreate->GetOrigNode(b1),
                                        cloneCreate->GetOrigNode(b2)); 
      }
  T GetTransInfo( const GraphNode *src, const GraphNode *snk)
      { assert( cloneCreate->IsBaseNode(src) && cloneCreate->IsBaseNode(snk));
        const BaseGraphCreate::Node* b1 = static_cast<const BaseGraphCreate::Node*>(src),
                           * b2 = static_cast<const BaseGraphCreate::Node*>(snk);
        return recOp->GetTransInfo( cloneCreate->GetOrigNode(b1),
                                        cloneCreate->GetOrigNode(b2));
      }
  void SetTransInfo( GraphNode *src, GraphNode *snk, T e) 
     { assert( cloneCreate->IsBaseNode(src) && cloneCreate->IsBaseNode(snk));
       const BaseGraphCreate::Node* b1 = static_cast<const BaseGraphCreate::Node*>(src),
                           * b2 = static_cast<const BaseGraphCreate::Node*>(snk);
       if (! cloneCreate->IsShadowBaseNode(b1) && 
           ! cloneCreate->IsShadowBaseNode(b2)) {
          recOp->SetTransInfo( cloneCreate->GetOrigNode(b1),
                                        cloneCreate->GetOrigNode(b2), e);
       }
     }
};

template <class T>
class ShadowBaseOP : public TransInfoOP<T>
{
  CloneShadowGraphCreate *cloneCreate; 
  TransInfoOP<T> *analOp;
 public:
   ShadowBaseOP( CloneShadowGraphCreate *g, TransInfoOP<T>* op)
     : cloneCreate(g), analOp(op) {}
   T  GetBottomInfo( const GraphNode *n1, const GraphNode *n2)
      { assert( cloneCreate->IsBaseNode(n1) && cloneCreate->IsBaseNode(n2));
        const BaseGraphCreate::Node* b1 = static_cast<const BaseGraphCreate::Node*>(n1),
                           * b2 = static_cast<const BaseGraphCreate::Node*>(n2);
        return analOp->GetBottomInfo( cloneCreate->GetOrigNode(b1),
                                      cloneCreate->GetOrigNode(b2)); }
   T  GetTopInfo(const GraphNode *n1, const GraphNode *n2)
      { assert( cloneCreate->IsBaseNode(n1) && cloneCreate->IsBaseNode(n2));
        const BaseGraphCreate::Node* b1 = static_cast<const BaseGraphCreate::Node*>(n1),
                           * b2 = static_cast<const BaseGraphCreate::Node*>(n2);
        return analOp->GetTopInfo( cloneCreate->GetOrigNode(b1),
                                   cloneCreate->GetOrigNode(b2)); }
   T  GetIDTransInfo( const GraphNode *n) 
      { assert( cloneCreate->IsBaseNode(n));
        const BaseGraphCreate::Node* b = static_cast<const BaseGraphCreate::Node*>(n);
        return analOp->GetIDTransInfo(cloneCreate->GetOrigNode(b)); }
   T  GetTransInfo( const GraphEdge *e) 
     { assert( cloneCreate->IsBaseEdge(e));
       const BaseGraphCreate::Edge* b = static_cast<const BaseGraphCreate::Edge*>(e);
       return analOp->GetTransInfo(cloneCreate->GetIDEdge(b)); }
   void  UnionWith(T& info1, T info2) { analOp->UnionWith(info1, info2);}
   T Composite(T info1, T info2) { return analOp->Composite(info1, info2); }
   T Closure(T info) { return analOp->Closure(info); }
};

template <class T>
class TransInfoCreate : public ShadowGraphCreate, public TransInfoGraph<T>
{
  TransInfoOP<T>* analOp;
 public:
  class Edge : public GraphEdge
  {
    T info;
   public:
    Edge( GraphCreate *c, const T &t)
     : GraphEdge(c), info(t) {}
    ~Edge() {}
    T& Info() { return info; }
  };

  TransInfoCreate( TransInfoOP<T>* op, BaseGraphCreate *_impl=0)
    : ShadowGraphCreate(_impl), analOp(op) {}
  ~TransInfoCreate() {}
  Boolean TransInfoComputed(const GraphNode *src, const GraphNode *snk)
   { return (ContainNode(src) && ContainNode(snk) && 
             GraphGetCrossEdge<GraphAccess>()(this,src,snk) != 0); }
  T GetTransInfo( const GraphNode *n1, const GraphNode *n2)
   {
     GraphEdge *e  = 0;
     if (!ContainNode(n1) || !ContainNode(n2) || 
         (e = GraphGetCrossEdge<GraphAccess>()(this,n1,n2)) == 0) 
        return analOp->GetTopInfo(n1,n2);
     return static_cast<Edge*>(e)->Info(); 
   }
  void SetTransInfo( GraphNode *n1, GraphNode *n2, T info)
    {
     if (!ContainNode(n1)) 
        CreateBaseNode(n1); 
     if (!ContainNode(n2))
        CreateBaseNode(n2);
     Edge *e = static_cast <Edge*>(GraphGetCrossEdge<GraphAccess>()(this,n1,n2));
     if (e == 0) {
         e = new Edge(this, info);
         CreateBaseEdge( n1, n2, e);
     }
     else
        e->Info() = info;
   }
};

template <class T>
class DAGOP 
{
    ShadowBaseOP<T>  baseOp;
    TransInfoCreate <T> infoCreate;
 public:
   DAGOP( CloneShadowGraphCreate *clone, TransInfoOP<T> *op)
       : baseOp(clone, op), infoCreate(&baseOp) {}

   void  ComputeTransInfoOnDAG( DAGBaseGraphImpl *impl, 
                                DAGBaseNodeImpl *node, 
                                GraphAccess::EdgeDirection dir)
   {
     impl->TopoSort( dir == GraphAccess::EdgeOut );
     for (DAGBaseGraphImpl::NodeIterator baseIter = impl->GetNodeIterator();
          !baseIter.ReachEnd(); ++baseIter) {
       DAGBaseNodeImpl *n1 = static_cast<DAGBaseNodeImpl*>(baseIter.Current());
       T result = GetTransInfo(n1, node, dir);
       for (DAGBaseGraphImpl::EdgeIterator edgeIter = impl->GetNodeEdgeIterator(n1,dir);
            ! edgeIter.ReachEnd(); ++edgeIter) {
         DAGBaseEdgeImpl *e = static_cast<DAGBaseEdgeImpl*>(edgeIter.Current());
         T r = baseOp.GetTransInfo(e) ;
         DAGBaseNodeImpl *n2 = impl->GetEdgeEndPoint(e, GraphAccess::Reverse(dir) );
         T r2 = GetTransInfo(n2, node, dir);
         r = (dir == GraphAccess::EdgeOut)? baseOp.Composite(r,r2):baseOp.Composite(r2,r);
         baseOp.UnionWith(result, r);
       }
       SetTransInfo(n1, node, result, dir);
     }
   }

   void SetTransInfo( DAGBaseNodeImpl *src, DAGBaseNodeImpl* snk,
                      const T e, GraphAccess::EdgeDirection dir = GraphAccess::EdgeOut)
    { if (dir==GraphAccess::EdgeOut)
         infoCreate.SetTransInfo(src, snk, e);
      else
         infoCreate.SetTransInfo(snk, src, e);
    }
   void ClearTransInfo( DAGBaseNodeImpl *n )
    { infoCreate.DeleteNode(n); }
   TransInfoGraph<T>* GetTransInfoGraph() { return &infoCreate; }
   T GetTransInfo( const DAGBaseNodeImpl *src, 
                   const DAGBaseNodeImpl* snk, GraphAccess::EdgeDirection dir=GraphAccess::EdgeOut)
    {  
      if (src == snk)
         return baseOp.GetIDTransInfo(src);
      return (dir==GraphAccess::EdgeOut)? infoCreate.GetTransInfo(src, snk)
                            : infoCreate.GetTransInfo(snk, src);
    }
};

template <class T>
class TransAnalSCCGraphNode : public GroupGraphNode
  {
   typedef struct TwinNodeStruct
   {
     DAGBaseNodeImpl *orig, *split;
     TwinNodeStruct() : orig(0), split(0) {}
     TwinNodeStruct( const TwinNodeStruct& that) : orig(that.orig), split(that.split) {}
     TwinNodeStruct& operator = (const TwinNodeStruct& that)
               { orig = that.orig; split = that.split; return *this; }
     TwinNodeStruct( DAGBaseNodeImpl *o, DAGBaseNodeImpl *s )
       { orig = o; split = s; }
   } TwinNode;


    std::vector <TwinNode> splitVec;
    CloneShadowGraphCreate *cloneCreate;
    DAGBaseGraphImpl *cloneBase;
    DAGOP <T> *dagOp;
    Boolean bottom;

    int NumberOfSplitNodes() { return splitVec.size(); }
    TwinNode* GetTwinNode(int i) { return &splitVec[i]; }
    void CreateTwinNode( DAGBaseNodeImpl *n, DAGBaseNodeImpl *n1)
         { splitVec.push_back( TwinNode(n,n1) ); }

   void Preprocess(TransInfoOP<T> *analOp, GraphAccess *orig, int limit);
   DAGBaseNodeImpl*
     ComputeInitTransInfo (GraphNode *n, GraphAccess::EdgeDirection dir,
                           TransInfoOP<T>* analOp, GraphAccess* orig, TransInfoGraph<T>* recOp);
   void CloseTransInfo( DAGBaseNodeImpl *n, GraphAccess::EdgeDirection dir, int num,
                        TransInfoOP<T>* analOp, TransInfoGraph<T>* recOp);
 public:
    TransAnalSCCGraphNode( GroupGraphCreate *g)
         : GroupGraphNode(g)
            { cloneBase = 0; cloneCreate = 0; dagOp = 0; bottom = false; }
    ~TransAnalSCCGraphNode()
      { if (cloneCreate != 0) {
           delete dagOp;
           delete cloneCreate;
        }
      }
    Boolean ComputeTransInfo( GraphNode *n, GraphAccess::EdgeDirection dir,
                              TransInfoOP<T>* analOp, GraphAccess *g, int limit,
                              TransInfoGraph<T>* recOp);
   Boolean ReachBottom() const { return  bottom; }
};

template <class T>
class TransAnalSCCOperator : public SCCGroupGraphOperator
{
 public:
  TransAnalSCCOperator(GroupGraphCreate *g) : SCCGroupGraphOperator(g) {}
  void CreateSCC()
      { SetCurSCC( new TransAnalSCCGraphNode<T>( GetGroupGraphCreate() ) ); }
  TransAnalSCCGraphNode<T>* GetOrigSCCNode( const GraphNode* n)
     { return (TransAnalSCCGraphNode<T>*)(GetSCCNode(const_cast<GraphNode*>(n))); }


};

template <class T>
class TransAnalSCCCreate  : public SCCGraphCreate
{
  GraphAccess *orig;
  TransInfoOP<T> *analOp;
  TransAnalSCCOperator<T> *sccOp;
  int limit;
 public:
   TransAnalSCCCreate ( GraphAccess *g, TransInfoOP<T> *op, int split)
    : SCCGraphCreate(g, sccOp = new TransAnalSCCOperator<T>(this) ), 
      orig(g), analOp(op), limit(split) {}
   virtual ~TransAnalSCCCreate () { delete sccOp; }

  TransAnalSCCGraphNode<T>* GetOrigSCCNode( const GraphNode* n)
     { return sccOp->GetOrigSCCNode(n); }

  Boolean ComputeTransInfo( GraphNode *n1, GraphNode *n2,
                            TransInfoGraph<T> *recOp);
  friend class TransAnalSCCGraphNode <T> ;
};

template <class T>
Boolean TransAnalSCCGraphNode<T> ::
ComputeTransInfo( GraphNode *n, GraphAccess::EdgeDirection dir, TransInfoOP<T>* analOp, 
                  GraphAccess *orig, int limit, TransInfoGraph<T>* recOp)
     {
      if (cloneCreate == 0)
        Preprocess(analOp, orig, limit);
      if (!bottom || !ContainNode(n) ) {
         DAGBaseNodeImpl* base = static_cast<DAGBaseNodeImpl*>
                                        (cloneCreate->GetOrigBaseNode(n));
         if (base == 0) {
            if ((base = ComputeInitTransInfo(n, dir, analOp, orig, recOp)) == 0)
              return false;
         }
         dagOp->ComputeTransInfoOnDAG( cloneBase, base, dir);
         ShadowBaseInfoGraph<T> baseRec(cloneCreate, recOp);
         CloseTransInfo(base, dir, NumberOfSplitNodes(), analOp, &baseRec);
         if (!ContainNode(n)) {
            dagOp->ClearTransInfo(base);
            cloneCreate->DeleteNode(n);
         }
      }
      return true;
     }

template <class T>
void TransAnalSCCGraphNode<T>::Preprocess( TransInfoOP<T> *analOp, GraphAccess *orig, int limit ) 
{
  if (cloneCreate != 0) return;
  cloneBase = new DAGBaseGraphImpl();
  cloneCreate = new CloneShadowGraphCreate(cloneBase); 
  dagOp = new DAGOP<T>(cloneCreate, analOp);
  typedef GraphSelectEndSet<GraphAccess,GroupNodeSelect,GroupNodeSelect> ScopeSelect;
  typedef GraphSelect<GraphAccess, ScopeSelect> ScopeImpl;
  ScopeImpl sccImpl(orig,  ScopeSelect(orig, GroupNodeSelect(this), GroupNodeSelect(this)));
  GraphScopeTemplate<GraphNode,GraphEdge,ScopeImpl> scc( sccImpl);
  cloneCreate->CloneGraph( &scc);
  cloneBase->TopoSort();
  for (DAGBaseGraphImpl::NodeIterator nodeIter = 
                cloneBase->GetNodeIterator();
       !nodeIter.ReachEnd(); ++nodeIter) {
    DAGBaseNodeImpl *n = static_cast<DAGBaseNodeImpl*>(nodeIter.Current());
    PtrSetWrap <DAGBaseEdgeImpl> backEdges;
    for (DAGBaseGraphImpl::EdgeIterator edgeIter = 
             cloneBase->GetNodeEdgeIterator(n, GraphAccess::EdgeIn);
         !edgeIter.ReachEnd(); ++edgeIter) {
      DAGBaseEdgeImpl *e = static_cast<DAGBaseEdgeImpl*>(edgeIter.Current()); 
      if (e->IsBackEdge())
          backEdges.Add(e);
    }
    if (backEdges.NumberOfEntries() > 0) {
      GraphNode * orig = cloneCreate->GetOrigNode(n);
      BaseGraphCreate::Node* nn = cloneCreate->CloneBaseNode(orig);
      DAGBaseNodeImpl * dagTwin = static_cast <DAGBaseNodeImpl*>(nn);
      CreateTwinNode(n, dagTwin);
      for (PtrSetWrap<DAGBaseEdgeImpl>::Iterator edgeIter1 = 
                                backEdges.GetIterator();
           !edgeIter1.ReachEnd(); ++edgeIter1) {
         DAGBaseEdgeImpl *e = edgeIter1.Current();
         cloneBase->MoveEdgeEndPoint(e, dagTwin, GraphAccess::EdgeIn);
      }
    }
  }
  int num = NumberOfSplitNodes();
  bottom = (num > limit); 
  if (! bottom) {
     TransInfoGraph<T> *tg = dagOp->GetTransInfoGraph();
     for (int index = 0; index < num; index ++ ) {
        TwinNode *twin = GetTwinNode(index);
        dagOp->ComputeTransInfoOnDAG(cloneBase, twin->split, GraphAccess::EdgeOut);
        dagOp->ComputeTransInfoOnDAG(cloneBase, twin->orig, GraphAccess::EdgeIn);
        CloseTransInfo(twin->split, GraphAccess::EdgeOut, index, analOp, tg);
        CloseTransInfo(twin->orig, GraphAccess::EdgeIn, index, analOp, tg);
        T r = dagOp->GetTransInfo(twin->orig, twin->split);
        r = analOp->Closure(r);
        dagOp->SetTransInfo( twin->orig, twin->split, r);
     }
  }
}

template <class T>
DAGBaseNodeImpl* TransAnalSCCGraphNode<T>::
ComputeInitTransInfo (GraphNode *n, GraphAccess::EdgeDirection dir, 
                       TransInfoOP<T>* analOp, GraphAccess *g, TransInfoGraph<T> *recOp)
{
  DAGBaseNodeImpl* base = 0;
  
  for ( GraphAccess::NodeIterator nodeIter = g->GetNodeIterator();
        !nodeIter.ReachEnd(); ++nodeIter) {
      GraphNode *n1 = nodeIter.Current();
      T result = 
        (dir==GraphAccess::EdgeOut)?analOp->GetTopInfo(n1,n):analOp->GetTopInfo(n,n1);
      for (GraphAccess::EdgeIterator edgeIter = 
                   g->GetNodeEdgeIterator( n1, dir);
           !edgeIter.ReachEnd(); ++edgeIter) {
        GraphEdge *e = edgeIter.Current();
        GraphNode *n2 = g->GetEdgeEndPoint(e, GraphAccess::Reverse(dir) );
        if (! ContainNode(n2)) {
           T r = analOp->GetTransInfo(e) ;
           if (n2 != n) {
              T r2 = recOp->GetTransInfo(n2, n);
              r = (dir == GraphAccess::EdgeOut)? 
                   analOp->Composite(r, recOp->GetTransInfo(n2,n)) 
                  :analOp->Composite(recOp->GetTransInfo(n,n2), r);
           }
           analOp->UnionWith(result, r);
        }
      }
      if (! result.IsTop()) {
        if (base == 0) 
            base = static_cast<DAGBaseNodeImpl*>
                           (cloneCreate->CloneGraphNode(n));
        DAGBaseNodeImpl* base1 = static_cast<DAGBaseNodeImpl*>
                                         (cloneCreate->GetOrigBaseNode(n1));
        dagOp->SetTransInfo(base1,base,result,dir);
     }
  }
  return base;
}

template <class T>
void TransAnalSCCGraphNode<T>::
CloseTransInfo( DAGBaseNodeImpl *n, GraphAccess::EdgeDirection dir, int num, 
                TransInfoOP<T>* analOp, TransInfoGraph<T>* recOp)
{
   for (DAGBaseGraphImpl::NodeIterator nodeIter =
                  cloneBase->GetNodeIterator();
        !nodeIter.ReachEnd(); ++nodeIter) {
      DAGBaseNodeImpl *n1 = static_cast<DAGBaseNodeImpl*>(nodeIter.Current());
      T result = dagOp->GetTransInfo(n1, n, dir);
      T origInfo = result;
      if (!bottom) {
        for (int index = 0; index < num ; ++index ) {
           TwinNode *twin = GetTwinNode(index);
           T close = dagOp->GetTransInfo( twin->orig, twin->split);
           T r1 = close, r2 = origInfo;
           if (dir == GraphAccess::EdgeOut) {
              if (n1 != twin->orig) {
                 r1 = dagOp->GetTransInfo(n1, twin->split );
                 analOp->UnionWith(r1, analOp->Composite(r1, close));
                 r2 = dagOp->GetTransInfo(twin->orig, n);
              }
              r1 = analOp->Composite(r1, r2);
           }
           else {
             if (n1 != twin->split) {
                r1 = dagOp->GetTransInfo(twin->orig, n1 );
                analOp->UnionWith( r1, analOp->Composite(close, r1));
                r2 = dagOp->GetTransInfo(n, twin->split );
             }
             r1 = analOp->Composite(r2, r1);
          }
          analOp->UnionWith(result, r1);
       }
     }
     else {
         T close = analOp->GetBottomInfo(n1, n1);
         result = (dir == GraphAccess::EdgeOut)? analOp->Composite(close, result )
                                               : analOp->Composite(result, close);
     }
     if (dir == GraphAccess::EdgeOut)
          recOp->SetTransInfo(n1, n, result);
     else
          recOp->SetTransInfo(n, n1, result);
   }
}

template <class T>
Boolean TransAnalSCCCreate <T>::
ComputeTransInfo( GraphNode *n1, GraphNode *n2, TransInfoGraph<T> *recOp)
    {
      Boolean succ = false;

      TransAnalSCCGraphNode<T> *sccNode1 = GetOrigSCCNode(n1);
      TransAnalSCCGraphNode<T> *sccNode2 = GetOrigSCCNode(n2);
      if (sccNode1 == sccNode2) { 
         succ = sccNode1->ComputeTransInfo( n2, GraphAccess::EdgeOut, analOp, orig, limit, recOp);
         if (sccNode1->ReachBottom()) 
             recOp->SetTransInfo( n1, n2, analOp->GetBottomInfo(n1, n2));
      }
      else {
        DAGBaseNodeImpl *base1 =
           static_cast<DAGBaseNodeImpl*>(GetBaseNode(sccNode1));
        DAGBaseNodeImpl *base2 =
           static_cast<DAGBaseNodeImpl*>(GetBaseNode(sccNode2));
        if (base1->TopoOrderIndex() > base2->TopoOrderIndex())
          return false;
        for ( NodeIterator iter = GetNodeIterator(); 
             !iter.ReachEnd(); ++iter) {
           TransAnalSCCGraphNode<T> *scc = static_cast<TransAnalSCCGraphNode<T>*>(iter.Current());
           succ = succ || (scc->ComputeTransInfo(n2, GraphAccess::EdgeOut, analOp, orig, limit, recOp));
           if (scc == sccNode1)
              break;
        }
      }
      return succ;
    }

template <class T>
GraphTransAnalysis<T>  :: 
GraphTransAnalysis(GraphAccess *g, TransInfoOP<T>  *_op, int splitlimit)
{
  impl = new TransAnalSCCCreate <T> (g, _op, splitlimit);
  impl->TopoSort(true);
}

template <class T>
GraphTransAnalysis<T>  :: ~GraphTransAnalysis()
{
  delete impl; 
}

template <class T>
Boolean GraphTransAnalysis<T> ::
ComputeTransInfo( TransInfoGraph<T>* tg, GraphNode *n1, GraphNode *n2)
{
  return impl->ComputeTransInfo(n1, n2, tg);
}

