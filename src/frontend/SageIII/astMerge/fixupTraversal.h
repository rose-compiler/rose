#ifndef __FIXUP_TRAVERSAL_H__
#define __FIXUP_TRAVERSAL_H__

#include <map>

class SgNode;

namespace Rose {
namespace AST {

void fixupTraversal(std::map<SgNode *, SgNode *> const &);
void fixupSubtreeTraversal(SgNode *, std::map<SgNode *, SgNode *> const &);

}
}

#endif /* __FIXUP_TRAVERSAL_H__ */
