#include <CompSlice.h>
#include <CompSliceObserver.h>
#include <GraphUpdate.h>
#include <PtrMap.h>

template <class SelfInfo, class InitSelfInfo, class SelfInfoToString,
          class RelInfo, class InitRelInfo, class RelInfoToString,
          class CreateInitInfo>
class CompSliceRegistry 
  : public IDGraphCreateTemplate<GraphNodeTemplate<SelfInfo,InitSelfInfo,SelfInfoToString>,
                                 GraphEdgeTemplate<RelInfo, InitRelInfo,RelInfoToString> >,
    public CompSliceObserver
{
 public:
  typedef GraphNodeTemplate<SelfInfo,InitSelfInfo,SelfInfoToString> SliceNode;
  typedef GraphEdgeTemplate<RelInfo, InitRelInfo,RelInfoToString> SliceEdge;
  typedef CompSliceRegistry<SelfInfo,InitSelfInfo,SelfInfoToString,
                            RelInfo,InitRelInfo,RelInfoToString,
                            CreateInitInfo> Registry;
  typedef typename IDGraphCreateTemplate<GraphNodeTemplate<SelfInfo,InitSelfInfo,SelfInfoToString>,
                             GraphEdgeTemplate<RelInfo, InitRelInfo,RelInfoToString> >::EdgeDirection
          EdgeDirection;
 private:
   PtrMapWrap <CompSlice, SliceNode> nodeMap;
   CreateInitInfo Create;

 protected:
   virtual void UpdateFuseSlice( const CompSliceFuseInfo &info)
    {
      const CompSlice *slice1 = &info.GetSlice(), *slice2 = &info.GetSlice2();
      SliceNode *n1=QuerySliceNode(slice1), *n2=QuerySliceNode(slice2);
      if (n2 != 0) {
        SliceEdge* edge = GraphGetCrossEdge<CompSliceRegistry>()(this,n1,n2);
        RelInfo rel = edge? edge->GetInfo() 
                          : RelInfo(Create(slice1,slice2));
        n1->GetInfo().FuseSelfInfo(n2->GetInfo(), rel);
        MoveEachNodeEdge
           <CompSliceRegistry<SelfInfo,InitSelfInfo,SelfInfoToString,
                              RelInfo,InitRelInfo,RelInfoToString,CreateInitInfo> >
               () (this, n2, n1);
        DeleteNode(slice2);
      }
      else   
        n1->GetInfo().FuseSelfInfo(Create(slice2), Create(slice1,slice2));
     }
   virtual void UpdateDelete( const CompSlice &slice)
    { SliceNode* n = QuerySliceNode(&slice);
      nodeMap.RemoveMapping(const_cast<CompSlice*>(&slice));
      IDGraphCreateTemplate<SliceNode,SliceEdge>::DeleteNode(n);
    }
 public:
  CompSliceRegistry( CreateInitInfo _Create, BaseGraphCreate *b = 0) 
      : IDGraphCreateTemplate<SliceNode,SliceEdge>(b), Create(_Create) {}
  ~CompSliceRegistry() 
   {
      for (typename PtrMapWrap<CompSlice, SliceNode>::Iterator p = nodeMap.GetIterator();
           !p.ReachEnd(); ++p) {
         const CompSlice *tmp = *p;
         tmp->DetachObserver(*this);
      }
   }

   SliceNode* QuerySliceNode( const CompSlice* slice)
   { return nodeMap.Map(const_cast<CompSlice*>(slice)); }

   SliceEdge* QuerySliceEdge(const CompSlice *slice1, const CompSlice *slice2)
    { SliceNode *node1 = nodeMap.Map(const_cast<CompSlice*>(slice1)), 
                *node2 = nodeMap.Map(const_cast<CompSlice*>(slice2));
      return (node1 && node2)? GetEdge(node1,node2) : 0;
    }
   SliceNode* CreateNode( const CompSlice *slice )
    {
      SliceNode* node = nodeMap.Map(const_cast<CompSlice*>(slice));
      if (node == 0) {
        node = new SliceNode(this, Create(slice));
        nodeMap.InsertMapping(const_cast<CompSlice*>(slice),node);
        CreateBaseNode(node);
        const_cast<CompSlice*>(slice)->AttachObserver(*this); 
      }
      return node;
    }
   SliceEdge* CreateEdge(const CompSlice* slice1, const CompSlice* slice2)
    {  
     SliceNode *node1 = CreateNode(slice1), *node2 = CreateNode(slice2);
     SliceEdge *edge = GraphGetCrossEdge<CompSliceRegistry>()(this,node1,node2);
     if (edge == 0) {
       edge = new SliceEdge(this, Create(slice1,slice2));
       CreateBaseEdge(node1,node2,edge);
     }
     return edge;
    }
   void MoveEdgeEndPoint( SliceEdge *e, EdgeDirection dir, SliceNode *n)
    { 
      SliceNode *n1 = GetEdgeEndPoint(e, GraphAccess::EdgeOut), 
                *n2 = GetEdgeEndPoint(e,GraphAccess::EdgeIn);
      SliceEdge *edge = (dir == GraphAccess::EdgeOut)? 
             GraphGetCrossEdge<CompSliceRegistry>()(this,n, n2) : 
             GraphGetCrossEdge<CompSliceRegistry>()(this,n1,n);
      edge->GetInfo().FuseRelInfo(e->GetInfo());
      DeleteEdge(e);
    } 

   void DeleteEdge(SliceEdge *e) 
    { IDGraphCreateTemplate<SliceNode,SliceEdge>::DeleteEdge(e); }

   void DeleteNode( const CompSlice *slice) 
    { SliceNode* n = QuerySliceNode(slice); 
      nodeMap.RemoveMapping(const_cast<CompSlice*>(slice));
      const_cast<CompSlice*>(slice)->DetachObserver(*this);
      IDGraphCreateTemplate<SliceNode,SliceEdge>::DeleteNode(n); 
    }
};

