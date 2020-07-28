#include <CompSlice.h>
#include <CompSliceObserver.h>
#include <GraphUpdate.h>
#include <PtrMap.h>
#include <assert.h>

template <class SelfInfo, class RelInfo, class Analysis> 
class CompSliceRegistry 
  : public VirtualGraphCreateTemplate<MultiGraphElemTemplate<SelfInfo>,
                                      MultiGraphElemTemplate<RelInfo> >,
    public CompSliceObserver
{
 public:
  typedef MultiGraphElemTemplate<SelfInfo> SliceNode;
  typedef MultiGraphElemTemplate<RelInfo> SliceEdge;
  typedef CompSliceRegistry<SelfInfo,RelInfo,Analysis> Registry;

 private:
   PtrMapWrap <CompSlice, SliceNode> nodeMap;
   Analysis& anal;

 protected:
   virtual void UpdateFuseSlice( const CompSliceFuseInfo &info)
    {
      const CompSlice *slice1 = &info.GetSlice(), *slice2 = &info.GetSlice2();
      SliceNode *n1=QuerySliceNode(slice1), *n2=QuerySliceNode(slice2);
      if (n2 != 0) {
        RelInfo rel = RelInfo(anal, slice1,slice2);
        GraphCrossEdgeIterator<CompSliceRegistry> cross(this,n1,n2);
        if (!cross.ReachEnd())
           rel = cross.Current()->GetInfo(); 
        n1->GetInfo().FuseSelfInfo(n2->GetInfo(), rel);
        MoveNodeEdge(this, n2, n1);
        DeleteNode(slice2);
      }
      else   
        n1->GetInfo().FuseSelfInfo(SelfInfo(anal,slice2), 
                                   RelInfo(anal,slice1,slice2));
     }
   virtual void UpdateDelete( const CompSlice &slice)
    { SliceNode* n = QuerySliceNode(&slice);
      nodeMap.RemoveMapping(const_cast<CompSlice*>(&slice));
      VirtualGraphCreateTemplate<SliceNode,SliceEdge>::DeleteNode(n);
    }
 public:
  CompSliceRegistry( Analysis& _anal, BaseGraphCreate *b = 0) 
      : VirtualGraphCreateTemplate<SliceNode,SliceEdge>(b), anal(_anal) {}
  ~CompSliceRegistry() 
   {
      for (typename PtrMapWrap<CompSlice, SliceNode>::Iterator p = nodeMap.GetIterator();
           !p.ReachEnd(); ++p) {
         const CompSlice *tmp = *p;
         assert(tmp != NULL);
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
        node = new SliceNode(this, SelfInfo(anal, slice));
        nodeMap.InsertMapping(const_cast<CompSlice*>(slice),node);
        this->AddNode(node);
        const_cast<CompSlice*>(slice)->AttachObserver(*this); 
      }
      return node;
    }
   SliceEdge* CreateEdge(const CompSlice* slice1, const CompSlice* slice2)
    {  
     SliceNode *node1 = CreateNode(slice1), *node2 = CreateNode(slice2);
     GraphCrossEdgeIterator<CompSliceRegistry> cross(this,node1,node2);
     SliceEdge *edge = 0;
     if (cross.ReachEnd()) {
       edge = new SliceEdge(this, RelInfo(anal, slice1,slice2));
       this->AddEdge(node1,node2,edge);
     }
     else
       edge = cross.Current();
     return edge;
    }
   void MoveEdgeEndPoint( SliceEdge *e, GraphAccess::EdgeDirection dir, SliceNode *n)
    { 
      SliceNode *n1 = this->GetEdgeEndPoint(e,  GraphAccess::EdgeOut), 
                *n2 = this->GetEdgeEndPoint(e, GraphAccess::EdgeIn);
      SliceEdge *edge = (dir == GraphAccess::EdgeOut)? 
             GraphCrossEdgeIterator<CompSliceRegistry>(this,n, n2).Current() : 
             GraphCrossEdgeIterator<CompSliceRegistry>(this,n1,n).Current();
      edge->GetInfo().FuseRelInfo(e->GetInfo());
      DeleteEdge(e);
    } 

   void DeleteEdge(SliceEdge *e) 
    { VirtualGraphCreateTemplate<SliceNode,SliceEdge>::DeleteEdge(e); }

   void DeleteNode( const CompSlice *slice) 
    { SliceNode* n = QuerySliceNode(slice); 
      nodeMap.RemoveMapping(const_cast<CompSlice*>(slice));
      const_cast<CompSlice*>(slice)->DetachObserver(*this);
      VirtualGraphCreateTemplate<SliceNode,SliceEdge>::DeleteNode(n); 
    }
};

