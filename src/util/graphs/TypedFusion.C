#include <SinglyLinkedList.h>
#include <PtrMap.h>

#include <TypedFusion.h>

#define BADFUSE NEG_INFTY+1

class FusionAnalInfo
{
 public:
  GraphAccessInterface::Node* node;
  int num, maxBadPrev, count, next, type;
  FusionAnalInfo(GraphAccessInterface::Node *n, int index, int _type)
    {
      num = index+1; node = n;
      maxBadPrev = 0; count = 0; next = 0;
      type = _type;
    }
  FusionAnalInfo() : node(0) { num = maxBadPrev = count = next = type = 0; }
  ~FusionAnalInfo() {}
};

void TypedFusion ::
operator() ( GraphAccessInterface *dg, TypedFusionOperator &fuseOp, int fusetype)
{
  int lastnum = 0;
  PtrMapWrap <GraphAccessInterface::Node, FusionAnalInfo> infoMap;
  SinglyLinkedListWrap<FusionAnalInfo*> workStack;

  int index = 0, fused = 0, lastfused = 0;
  unsigned size = CountIteratorSize(dg->GetNodeIterator());
  if (size <= 1)
      return;

  FusionAnalInfo** infoVec = new FusionAnalInfo*[size];

  for ( GraphAccessInterface::NodeIterator nodeIter= dg->GetNodeIterator();
        !nodeIter.ReachEnd(); ++nodeIter, ++index) {
    GraphAccessInterface::Node *n = *nodeIter;
    int type = fuseOp.GetNodeType(n);
    FusionAnalInfo *info = new FusionAnalInfo(n, index, type);
    infoMap.InsertMapping(n, info);
    for (GraphAccessInterface::EdgeIterator edgeIter = dg->GetNodeEdgeIterator(n, GraphAccess::EdgeIn);
          ! edgeIter.ReachEnd(); ++edgeIter) {
      info->count++;
    }
    infoVec[index] = info;
    if (info->count == 0) {
      workStack.AppendLast(info);
    }
  }
  
  while ( workStack.size() > 0) {
    FusionAnalInfo *info = workStack.First()->GetEntry();
    workStack.PopFirst();
    GraphAccessInterface::Node *node = info->node;
    int p = 0;
    if (info->type == fusetype) {
      if (info->maxBadPrev == 0)
        p = fused;
      else
        p = infoVec[info->maxBadPrev-1]->next;
    }
    if (p != 0) {
      FusionAnalInfo *info1 = infoVec[p - 1];
      info->num = info1->num;
      fuseOp.MarkFuseNodes(info1->node, info->node);
      if (info1->maxBadPrev > info->maxBadPrev)
        info->maxBadPrev = info1->maxBadPrev;
    }
    else {
      lastnum = lastnum+1;
      if (info->num != lastnum) {
         FusionAnalInfo *info1 = infoVec[lastnum-1];
         infoVec[lastnum-1] = info;
         infoVec[info->num-1] = info1;
         info1->num = info->num;
         info->num = lastnum;
      }
      if (info->type == fusetype) {
        if (lastfused == 0) {
          fused = lastnum;
          lastfused = fused;
        }
        else {
          FusionAnalInfo *info1 = infoVec[lastfused-1];
          info1->next = lastnum;
          lastfused = lastnum;
        }
      }
    }
    for (GraphAccessInterface::EdgeIterator edgeIter = dg->GetNodeEdgeIterator( node, GraphAccess::EdgeOut);
          !edgeIter.ReachEnd(); ++edgeIter) {
      GraphAccessInterface::Edge *edge = edgeIter.Current();
      GraphAccessInterface::Node *m = dg->GetEdgeEndPoint(edge, GraphAccess::EdgeIn );
      FusionAnalInfo *info1 = infoMap.Map(m);
      info1->count--;
      if (info1->count == 0) {
        workStack.AppendLast(info1);
      }
      if (info->type != fusetype) {
        if (info1->maxBadPrev < info->maxBadPrev)
          info1->maxBadPrev = info->maxBadPrev;
      }
      else if (info1->type != fusetype) {
        if (info1->maxBadPrev < info->num)
          info1->maxBadPrev = info->num;
      }
      else {
        if ( ! fuseOp.PreventFusion( node, m, edge)) {
          if (info->maxBadPrev > info1->maxBadPrev)
            info1->maxBadPrev = info->maxBadPrev;
        }
        else if (info1->maxBadPrev < info->num)
          info1->maxBadPrev = info->num;
      }
    }
  }
  for (size_t i = 0; i < size; ++i)
    delete infoVec[i];
  delete [] infoVec;
}
