
#include <general.h>
#include <CompSliceDepGraph.h>
#include <SinglyLinkedList.h>
#include <PtrMap.h>
#include <TypedFusion.h>
#include <GraphScope.h>
#include <FusionAnal.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

class SliceGraphTypedFusionImpl : public TypedFusionOperator
{
  const LoopNestFusion& fuse;
  CompSliceLocalityRegistry *reg;
  CompSliceDepGraphCreate& ct;
 protected:
  typedef SinglyLinkedListWrap <CompSliceDepGraphNode*> FuseVecType1;
  typedef SinglyLinkedListWrap < FuseVecType1* >  FuseVecType2;
  FuseVecType2 fuseVec;
  PtrMapWrap <CompSliceDepGraphNode, FuseVecType1 > fuseMap;

  int GetNodeType( GraphNode *gn)
  {
   CompSliceDepGraphNode *n = static_cast<CompSliceDepGraphNode*>(gn);
   return n->GetInfo().NumberOfEntries() > 0;
  }

  void MarkFuseNodes( GraphNode *gn1, GraphNode *gn2)
  {
    CompSliceDepGraphNode *n1 = static_cast<CompSliceDepGraphNode*>(gn1), 
                          *n2 = static_cast<CompSliceDepGraphNode*>(gn2);
    FuseVecType1 *fuseInfo = fuseMap.Map(n1);
    if (fuseInfo == 0) {
      fuseInfo = new SinglyLinkedListWrap <CompSliceDepGraphNode*>;
      fuseInfo->AppendLast(n1);
      fuseVec.AppendLast( fuseInfo );
      fuseMap.InsertMapping(n1, fuseInfo);
    }
    fuseInfo->AppendLast(n2);
  }    

  Boolean PreventFusion( GraphNode *gn1, GraphNode *gn2, GraphEdge *e)
  {
    CompSliceDepGraphNode *n1 = static_cast<CompSliceDepGraphNode*>(gn1),
                          *n2 = static_cast<CompSliceDepGraphNode*>(gn2);
    DepInfoEdge *edge = static_cast<DepInfoEdge*>(e);
    return !fuse.Fusible( reg, n1->GetInfo(), n2->GetInfo(), edge->GetInfo());
  }
 public:
  SliceGraphTypedFusionImpl( CompSliceLocalityRegistry *r, CompSliceDepGraphCreate &c, 
                            const LoopNestFusion &f) 
     : fuse(f), reg(r), ct(c) {}
  void FuseNodes()
  {
    for ( FuseVecType2::Iterator p2(fuseVec); !p2.ReachEnd(); p2.Advance()) {
      FuseVecType1 *fuseInfo = p2.Current();
      for (FuseVecType1::Iterator pp(*fuseInfo); !pp.ReachEnd(); ++pp) {
         if ( (*pp) == 0)
             continue; 
         CompSliceDepGraphNode *n1 = pp.Current(), **n1p = 0;
         CompSliceNest& g1 = n1->GetInfo();

         FuseVecType1::Iterator p1 = pp;
         for ( p1.Advance(); !p1.ReachEnd(); p1.Advance()) {
            CompSliceDepGraphNode *n2 = p1.Current();
            if (n2 == 0)
                continue;
            CompSliceNest& g2 = n2->GetInfo();
            DepInfoEdge *edge = GraphGetCrossEdge<CompSliceDepGraph>()(&ct,n1, n2);
            DepInfo info = (edge != 0)? edge->GetInfo() :
                                  DepInfoGenerator::GetTopDepInfo();
            if (fuse.Fusible( reg, g1, g2, info)) {
                fuse.Fuse( reg, g1, g2, info);
                *p1 = 0;
                n1p = &p1.Current();
            }
            else if (edge != 0)
                 break;
         }
         if (n1p != 0) {
             *n1p = n1;
             (*pp) = 0;
         }
      }
      delete fuseInfo;
    }
    fuseVec.DeleteAll();
  }
};

void SliceNestTypedFusion( CompSliceLocalityRegistry *reg, CompSliceDepGraphCreate &t, 
                           const LoopNestFusion& fuse )
{
  SliceGraphTypedFusionImpl impl(reg, t,fuse);
  TypedFusion()(&t, impl, (int)true);
  impl.FuseNodes();
}

class SliceGraphReverseTypedFusionImpl : public SliceGraphTypedFusionImpl
{
 public:
  void FuseNodes()
  {
    for ( FuseVecType2::Iterator p2(fuseVec); !p2.ReachEnd(); p2.Advance()) {
      FuseVecType1 *fuseInfo = p2.Current();
      fuseInfo->Reverse(); 
    }
    fuseVec.Reverse();
    SliceGraphTypedFusionImpl :: FuseNodes();
  } 

  SliceGraphReverseTypedFusionImpl( CompSliceLocalityRegistry *r,CompSliceDepGraphCreate &_ct, 
                                   const LoopNestFusion& f) 
   : SliceGraphTypedFusionImpl(r, _ct,f) {}

};

void SliceNestReverseTypedFusion( CompSliceLocalityRegistry *reg, CompSliceDepGraphCreate &t, 
                                  const LoopNestFusion& fuse)
{
  SliceGraphReverseTypedFusionImpl impl(reg, t, fuse);
  GraphReverseEdgeImpl graph(&t);
  TypedFusion()(&graph, impl, (int)true);
  impl.FuseNodes();
}
