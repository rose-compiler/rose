#include <LoopTreeShadow.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

LoopTreeShadowNode ::
LoopTreeShadowNode( LoopTreeNode *n, LoopTreeShadowCreate *tc)
{
  repr = n;
  map = &tc->nodeMap;
  assert(map->Map(n) == 0);
  map->InsertMapping(n, this);
  LoopTreeNode::Unlink();
}

LoopTreeNode* LoopTreeShadowSelect::
Build( LoopTreeNode *orig, LoopTreeNode *p)
{
  if (select_stmt(orig)) {
     return CreateShadowNode(orig);
  }
  else {
     LoopTreeNode *shadow = orig->IncreaseLoopLevel()? 0 : p;
     LoopTreeNode* r1 = 0, *r2 = 0, *result = r1;
     LoopTreeNode *n = orig->FirstChild();
     for ( ; n != 0; n = n->NextSibling()) {
        if ( (r1 = Build( n, shadow)) != 0)
            break;
     }
     if (r1 == 0)
         return 0;
     if (shadow == 0 && orig->IncreaseLoopLevel()) {
        shadow = CreateShadowNode(orig);
        if (p != 0)
           shadow->Link( p, LoopTreeNode::AsLastChild);
        r1->Link( shadow, LoopTreeNode::AsLastChild);
        result = shadow;
     }
     if (n == 0) return result;
     for ( n = n->NextSibling() ;n != 0; n = n->NextSibling()) {
        if ( (r2 = Build(n, shadow)) != 0)
             break;
     }
     if (r2 == 0)
        return result;
     if (shadow == 0) {
        shadow = CreateShadowNode(orig);
        r1->Link( shadow, LoopTreeNode::AsLastChild);
        r2->Link( shadow, LoopTreeNode::AsLastChild);
        result = shadow;
     }
     if (n == 0)
        return result;
     for ( n = n->NextSibling() ;n != 0; n = n->NextSibling()) {
        Build(n, shadow);
     }
     return result;
  }
}

LoopTreeShadowCreate :: ~LoopTreeShadowCreate()
{
  LoopTreeNode *r = GetTreeRoot();
  LoopTreeNode *n = r->FirstChild();
  while (n != 0) {
    LoopTreeShadowNode *tmp = static_cast<LoopTreeShadowNode*>(n);
    n = n->NextSibling();
    delete tmp;
  }
}

LoopTreeShadowSelect::
LoopTreeShadowSelect( LoopTreeNode *orig)
     : LoopTreeShadowCreate(orig->LoopLevel())
{
   Build( orig, GetTreeRoot());
}

