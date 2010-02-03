#include "sage3basic.h"


#include "AstFixParentTraversal.h"

//The inherited attribute contains a pointer to the parent. As we
//continue our traversal, we pass down the current pointer to be the
//parent pointer of the children
ParentAttribute AstFixParentTraversal::evaluateInheritedAttribute(SgNode * node, ParentAttribute p) {

  node->set_parent(p.get_parent());
  return ParentAttribute(node);

}

#if 0
//This operates in much the same way as AstFixParentTraversal
ParentAttribute AstCheckParentTraversal::evaluateInheritedAttribute(SgNode * node, ParentAttribute p) {

  ROSE_ASSERT(node->get_parent() == p.get_parent());
  return ParentAttribute(node);

}
#endif
