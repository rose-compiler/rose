#include <iostream>
#include <stdlib.h>

#include <vector>

#include <SCCAnalysis.h>
#include <TransAnalysis.h>

#include <DGBaseGraphImpl.h>
#include <GraphScope.h>

#include <assert.h>

class ShadowGraphNode  : MultiGraphElem
{
 public:
  ShadowGraphNode(MultiGraphCreate* c, GraphAccessInterface::Node *o) : MultiGraphElem(c) 
        {orig = o; }
  GraphAccessInterface::Node* GetOrig() const { return orig; } 
 private:
  GraphAccessInterface::Node *orig;
};

typedef VirtualGraphCreateTemplate<GraphAccessInterface::Node,
                                   GraphAccessInterface::Edge>  
ShadowGraphCreate;

class CloneShadowGraphCreate : public ShadowGraphCreate
{
  PtrSetWrap <const GraphAccessInterface::Node> shadows;
 public:
  CloneShadowGraphCreate( BaseGraphCreate *_impl) : ShadowGraphCreate(_impl){}

  BaseGraphNode* CloneBaseNode( GraphAccessInterface::Node *orig)
    { 
      ShadowGraphNode *n = new ShadowGraphNode(this, orig);
      shadows.insert(n);
      return AddNode(n); 
    }
  BaseGraphNode* GetOrigBaseNode( GraphAccessInterface::Node *orig)
   { return GetBaseNode(orig); }
  bool IsShadowNode( const GraphAccessInterface::Node *node) const
    { return shadows.IsMember(node); }
  bool IsShadowBaseNode( const BaseGraphNode *base) const
   { 
      GraphAccessInterface::Node *n = ShadowGraphCreate::GetVirtualNode(base);
      return IsShadowNode(n);
   }
  GraphAccessInterface::Node *GetOrigNode(const BaseGraphNode *base) const
    { 
      GraphAccessInterface::Node *n = ShadowGraphCreate::GetVirtualNode(base);
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
  bool TransInfoComputed(const GraphAccessInterface::Node *src, const GraphAccessInterface::Node *snk)
      { 
        const DAGBaseNodeImpl* b1 = static_cast<const DAGBaseNodeImpl*>(src),
                           * b2 = static_cast<const DAGBaseNodeImpl*>(snk);
       return recOp->TransInfoComputed( cloneCreate->GetOrigNode(b1),
                                        cloneCreate->GetOrigNode(b2)); 
      }
  T GetTransInfo( const GraphAccessInterface::Node *src, const GraphAccessInterface::Node *snk)
      { 
        const DAGBaseNodeImpl* b1 = static_cast<const DAGBaseNodeImpl*>(src),
                           * b2 = static_cast<const DAGBaseNodeImpl*>(snk);
        return recOp->GetTransInfo( cloneCreate->GetOrigNode(b1),
                                        cloneCreate->GetOrigNode(b2));
      }
  void SetTransInfo( GraphAccessInterface::Node *src, GraphAccessInterface::Node *snk, T e) 
     { 
                 const DAGBaseNodeImpl* b1 = static_cast<const DAGBaseNodeImpl*>(src),
                 * b2 = static_cast<const DAGBaseNodeImpl*>(snk);
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
   T  GetBottomInfo( const GraphAccessInterface::Node *n1, const GraphAccessInterface::Node *n2)
      { 
                  const DAGBaseNodeImpl* b1 = static_cast<const DAGBaseNodeImpl*>(n1),
                  * b2 = static_cast<const DAGBaseNodeImpl*>(n2);
        return analOp->GetBottomInfo( cloneCreate->GetOrigNode(b1),
                                      cloneCreate->GetOrigNode(b2)); }
   bool IsTop(T t) { return analOp->IsTop(t); }
   T  GetTopInfo(const GraphAccessInterface::Node *n1, const GraphAccessInterface::Node *n2)
      { 
          const DAGBaseNodeImpl* b1 = static_cast<const DAGBaseNodeImpl*>(n1),
          * b2 = static_cast<const DAGBaseNodeImpl*>(n2);
        return analOp->GetTopInfo( cloneCreate->GetOrigNode(b1),
                                   cloneCreate->GetOrigNode(b2)); }
   T  GetIDTransInfo( const GraphAccessInterface::Node *n) 
      { 
        const DAGBaseNodeImpl* b = static_cast<const DAGBaseNodeImpl*>(n);
        return analOp->GetIDTransInfo(cloneCreate->GetOrigNode(b)); }
   T  GetTransInfo( const GraphAccessInterface::Edge *e) 
     { 
       const DAGBaseEdgeImpl* b = static_cast<const DAGBaseEdgeImpl*>(e);
       return analOp->GetTransInfo(cloneCreate->GetVirtualEdge(b)); 
     }
   void  UnionWith(T& info1, T info2) { analOp->UnionWith(info1, info2);}
   T Composite(T info1, T info2) { return analOp->Composite(info1, info2); }
   T Closure(T info) { return analOp->Closure(info); }
};

template <class T>
class TransInfoCreate : public ShadowGraphCreate, public TransInfoGraph<T>
{
  TransInfoOP<T>* analOp;
 public:
  class EdgeImpl : public MultiGraphElem
  {
    T info;
   public:
    EdgeImpl( MultiGraphCreate *c, const T &t)
     : MultiGraphElem(c), info(t) {}
    ~EdgeImpl() {}
    T& Info() { return info; }
  };

  TransInfoCreate( TransInfoOP<T>* op, BaseGraphCreate *_impl=0)
    : ShadowGraphCreate(_impl), analOp(op) {}
  ~TransInfoCreate() {}
  bool TransInfoComputed(const GraphAccessInterface::Node *src, const GraphAccessInterface::Node *snk)
   { return ContainNode(src) && ContainNode(snk) && 
      !GraphCrossEdgeIterator<TransInfoCreate<T> >(this,src,snk).ReachEnd(); }
  T GetTransInfo( const GraphAccessInterface::Node *n1, const GraphAccessInterface::Node *n2)
   {
     if (!ContainNode(n1) || !ContainNode(n2))
        return analOp->GetTopInfo(n1,n2);
     GraphCrossEdgeIterator<TransInfoCreate<T> > cross(this,n1,n2);
     if (cross.ReachEnd())
        return analOp->GetTopInfo(n1,n2);
     EdgeImpl *e = static_cast<EdgeImpl*>(cross.Current());
     return e->Info(); 
   }
  void SetTransInfo( GraphAccessInterface::Node *n1, GraphAccessInterface::Node *n2, T info)
    {
     if (!ContainNode(n1)) 
        AddNode(n1); 
     if (!ContainNode(n2))
        AddNode(n2);
         GraphCrossEdgeIterator<TransInfoCreate<T> >  crossIter(this,n1,n2);
         EdgeImpl *e;
         if (crossIter.ReachEnd()) {
         e = new EdgeImpl(this, info);
         AddEdge( n1, n2, e);
     }
         else {
                 e= static_cast<EdgeImpl*>(crossIter.Current());
                 e->Info() = info;}
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
         DAGBaseNodeImpl *n2 = static_cast<DAGBaseNodeImpl*>
                 (impl->GetEdgeEndPoint(e, GraphAccess::Reverse(dir) ));
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
    bool bottom;

    int NumberOfSplitNodes() { return splitVec.size(); }
    TwinNode* GetTwinNode(int i) { return &splitVec[i]; }
    void CreateTwinNode( DAGBaseNodeImpl *n, DAGBaseNodeImpl *n1)
         { splitVec.push_back( TwinNode(n,n1) ); }

   void Preprocess(TransInfoOP<T> *analOp, GraphAccessInterface *orig, int limit);
   DAGBaseNodeImpl*
     ComputeInitTransInfo (GraphAccessInterface::Node *n, 
                           GraphAccess::EdgeDirection dir,
                           TransInfoOP<T>* analOp, GraphAccessInterface* orig, 
                           TransInfoGraph<T>* recOp);
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
    bool ComputeTransInfo( GraphAccessInterface::Node *n, GraphAccess::EdgeDirection dir,
                              TransInfoOP<T>* analOp, GraphAccessInterface *g, 
                              int limit,
                              TransInfoGraph<T>* recOp);
   bool ReachBottom() const { return  bottom; }
};

template <class T>
class TransAnalSCCOperator : public SCCGroupGraphOperator
{
 public:
  TransAnalSCCOperator(GroupGraphCreate *g) : SCCGroupGraphOperator(g) {}
  void CreateSCC()
      { SetCurSCC( new TransAnalSCCGraphNode<T>( GetGroupGraphCreate() ) ); }
  TransAnalSCCGraphNode<T>* GetOrigSCCNode( const GraphAccessInterface::Node* n)
     { return (TransAnalSCCGraphNode<T>*)(GetSCCNode(const_cast<GraphAccessInterface::Node*>(n))); }


};

template <class T>
class TransAnalSCCCreate  : public SCCGraphCreate
{
  GraphAccessInterface *orig;
  TransInfoOP<T> *analOp;
  TransAnalSCCOperator<T> *sccOp;
  int limit;
 public:
   TransAnalSCCCreate ( GraphAccessInterface *g, TransInfoOP<T> *op, int split)
    : SCCGraphCreate(g, sccOp = new TransAnalSCCOperator<T>(this) ), 
      orig(g), analOp(op), limit(split) {}
   virtual ~TransAnalSCCCreate () { delete sccOp; }

  TransAnalSCCGraphNode<T>* GetOrigSCCNode( const GraphAccessInterface::Node* n)
     { return sccOp->GetOrigSCCNode(n); }

  bool ComputeTransInfo( GraphAccessInterface::Node *n1, GraphAccessInterface::Node *n2,
                            TransInfoGraph<T> *recOp);
  friend class TransAnalSCCGraphNode <T> ;
};

template <class T>
bool TransAnalSCCGraphNode<T> ::
ComputeTransInfo( GraphAccessInterface::Node *n, GraphAccess::EdgeDirection dir, 
                  TransInfoOP<T>* analOp, 
                  GraphAccessInterface *orig, int limit, 
                  TransInfoGraph<T>* recOp)
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
void TransAnalSCCGraphNode<T>::Preprocess( TransInfoOP<T> *analOp, GraphAccessInterface *orig, int limit ) 
{
  if (cloneCreate != 0) return;
  cloneBase = new DAGBaseGraphImpl();
  cloneCreate = new CloneShadowGraphCreate(cloneBase); 
  dagOp = new DAGOP<T>(cloneCreate, analOp);
  GroupNodeSelect sccImpl(orig,  this);
  GraphAccessWrapTemplate<GraphAccessInterface::Node,GraphAccessInterface::Node,
                          GroupNodeSelect> scc( &sccImpl);
  cloneCreate->AddGraph( &scc);
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
          backEdges.insert(e);
    }
    if (backEdges.NumberOfEntries() > 0) {
      GraphAccessInterface::Node * orig = cloneCreate->GetOrigNode(n);
      BaseGraphCreate::Node* nn = cloneCreate->CloneBaseNode(orig);
      DAGBaseNodeImpl * dagTwin = static_cast <DAGBaseNodeImpl*>(nn);
      CreateTwinNode(n, dagTwin);
      for (PtrSetWrap<DAGBaseEdgeImpl>::const_iterator edgeIter1 = 
                                backEdges.begin();
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
ComputeInitTransInfo (GraphAccessInterface::Node *n, 
                      GraphAccess::EdgeDirection dir, 
                       TransInfoOP<T>* analOp, GraphAccessInterface *g, 
                       TransInfoGraph<T> *recOp)
{
  DAGBaseNodeImpl* base = 0;
  
  for ( const_iterator nodeIter = begin(); !(nodeIter == end()); ++nodeIter) {
      GraphAccessInterface::Node *n1 = nodeIter.Current();
      T result = 
        (dir==GraphAccess::EdgeOut)?analOp->GetTopInfo(n1,n):analOp->GetTopInfo(n,n1);
      for (GraphAccessInterface::EdgeIterator edgeIter = g->GetNodeEdgeIterator( n1, dir);!edgeIter.ReachEnd(); ++edgeIter) {
        GraphAccessInterface::Edge *e = edgeIter.Current();
        GraphAccessInterface::Node *n2 = g->GetEdgeEndPoint(e, GraphAccess::Reverse(dir) );
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
      if (!analOp->IsTop(result)) {
        if (base == 0) 
            base = static_cast<DAGBaseNodeImpl*>
                           (cloneCreate->AddNode(n));
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
bool TransAnalSCCCreate <T>::
ComputeTransInfo( GraphAccessInterface::Node *n1, GraphAccessInterface::Node *n2, 
                  TransInfoGraph<T> *recOp)
    {
      bool succ = false;

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
                NodeIterator iter ;
                for ( iter = GetNodeIterator(); 
                                        !iter.ReachEnd(); ++iter) {
                        TransAnalSCCGraphNode<T> *scc = static_cast<TransAnalSCCGraphNode<T>*>(iter.Current());
                        if (scc == sccNode2) break;
                }
        for ( ; !iter.ReachEnd(); ++iter) {
           TransAnalSCCGraphNode<T> *scc = static_cast<TransAnalSCCGraphNode<T>*>(iter.Current());
           if (scc->ComputeTransInfo(n2, GraphAccess::EdgeOut, analOp, orig, limit, recOp))
                           succ=true;
           if (scc == sccNode1)
              break;
        }
      }
      return succ;
    }

template <class T>
GraphTransAnalysis<T>  :: 
GraphTransAnalysis(GraphAccessInterface *g, TransInfoOP<T>  *_op, int splitlimit)
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
bool GraphTransAnalysis<T> ::
ComputeTransInfo( TransInfoGraph<T>* tg, GraphAccessInterface::Node *n1, GraphAccessInterface::Node *n2)
{
  return impl->ComputeTransInfo(n1, n2, tg);
}
