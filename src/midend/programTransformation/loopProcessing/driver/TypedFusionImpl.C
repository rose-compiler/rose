
#include <CompSliceDepGraph.h>
#include <SinglyLinkedList.h>
#include <PtrMap.h>
#include <TypedFusion.h>
#include <GraphScope.h>
#include <FusionAnal.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

//#define DEBUG 1

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

  int GetNodeType( GraphAccessInterface::Node *gn)
  {
   CompSliceDepGraphNode *n = static_cast<CompSliceDepGraphNode*>(gn);
   return n->GetInfo().GetNest() != 0;
  }

  void MarkFuseNodes( GraphAccessInterface::Node *gn1, GraphAccessInterface::Node *gn2)
  {
#ifdef DEBUG
std::cerr << "SliceGraphTypedFusionImpl:MarkFuseNodes\n";
#endif
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
#ifdef DEBUG
std::cerr << "n1 = " << n1->toString() << "\n";
std::cerr << "n2 = " << n2->toString() << "\n";
std::cerr << "end SliceGraphTypedFusionImpl:MarkFuseNodes\n";
#endif
  }

  bool PreventFusion( GraphAccessInterface::Node *gn1, GraphAccessInterface::Node *gn2, GraphAccessInterface::Edge *e)
  {
    CompSliceDepGraphNode *n1 = static_cast<CompSliceDepGraphNode*>(gn1),
                          *n2 = static_cast<CompSliceDepGraphNode*>(gn2);
    DepInfoEdge *edge = static_cast<DepInfoEdge*>(e);
    CompSliceNest* nest1 = n1->GetInfo().GetNest();
    CompSliceNest* nest2 = n2->GetInfo().GetNest();
    if (nest1 == 0 || nest2 == 0) return true;
    return !fuse.Fusible( reg, *nest1, *nest2, edge->GetInfo());
  }
 public:
  SliceGraphTypedFusionImpl( CompSliceLocalityRegistry *r, CompSliceDepGraphCreate &c,
                            const LoopNestFusion &f)
     : fuse(f), reg(r), ct(c) {}
  virtual ~SliceGraphTypedFusionImpl() {}
  void FuseNodes()
  {
#ifdef DEBUG
std::cerr << "SliceGraphTypedFusionImpl:FuseNodes\n";
#endif
    for ( FuseVecType2::Iterator p2(fuseVec); !p2.ReachEnd(); p2.Advance()) {
      FuseVecType1 *fuseInfo = p2.Current();
      for (FuseVecType1::Iterator pp(*fuseInfo); !pp.ReachEnd(); ++pp) {
         if ( (*pp) == 0)
             continue;
         CompSliceDepGraphNode *n1 = pp.Current(), **n1p = 0;
         CompSliceNest* g1 = n1->GetInfo().GetNest();
         if (g1 == 0) continue;

         FuseVecType1::Iterator p1 = pp;
         for ( p1.Advance(); !p1.ReachEnd(); p1.Advance()) {
            CompSliceDepGraphNode *n2 = p1.Current();
            if (n2 == 0) continue;
            CompSliceNest* g2 = n2->GetInfo().GetNest();
            if (g2 == 0) continue;
            GraphCrossEdgeIterator<CompSliceDepGraph> crossIter(&ct,n1, n2);
            DepInfoEdge *edge = 0;
            DepInfo info = DepInfoGenerator::GetTopDepInfo();
            if (!crossIter.ReachEnd()) {
                edge = crossIter.Current();
                info =  edge->GetInfo();
            }
            if (fuse.Fusible( reg, *g1, *g2, info)) {
                fuse.Fuse( reg, *g1, *g2, info);
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

void SliceNestTypedFusion( CompSliceLocalityRegistry *reg,
                           CompSliceDepGraphCreate &t,
                           const LoopNestFusion& fuse )
{
#ifdef DEBUG
std::cerr << "SliceNestTypedFusion\n";
#endif
  SliceGraphTypedFusionImpl impl(reg, t,fuse);
  GraphAccessWrapTemplate<GraphAccessInterface::Node,
                          GraphAccessInterface::Edge,
                          CompSliceDepGraphCreate> access(&t);

  TypedFusion()(&access, impl, (int)true);
  impl.FuseNodes();
}

class SliceGraphReverseTypedFusionImpl : public SliceGraphTypedFusionImpl
{
 public:
  void FuseNodes()
  {
#ifdef DEBUG
std::cerr << "SliceGraphReverseTypedFusionImpl:FuseNodes\n";
#endif
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
#ifdef DEBUG
std::cerr << "SliceNestReverseTypedFusion\n";
#endif
  SliceGraphReverseTypedFusionImpl impl(reg, t, fuse);
  GraphReverseEdge<CompSliceDepGraphCreate> graph(&t);
  GraphAccessWrapTemplate<GraphAccessInterface::Node,
                          GraphAccessInterface::Edge,
                        GraphReverseEdge<CompSliceDepGraphCreate> > acc(&graph);
  TypedFusion()(&acc, impl, (int)true);
  impl.FuseNodes();
}
