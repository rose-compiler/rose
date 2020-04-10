
#include "sage3basic.h"
#include "fixupTraversal.h"

using namespace std;

namespace Rose {
namespace AST {

template <typename Traversal>
struct FixupReplacer : public SimpleReferenceToPointerHandler {
  Traversal & ft;
  SgNode * node;

  FixupReplacer(Traversal & ft, SgNode* node): ft(ft), node(node) {}

  virtual void operator()(SgNode* & key, const SgName& debugStringName, bool /* traverse */) {
    if (key != NULL) {
      std::map<SgNode*, SgNode*>::const_iterator replacementMap_it = ft.replacementMap.find(key);
      if (replacementMap_it != ft.replacementMap.end() && key != replacementMap_it->second) {
        SgNode * r = replacementMap_it->second;
        key = r;
      }
    }
  }
};

struct FixupTraversal : public ROSE_VisitTraversal {
  const std::map<SgNode*, SgNode*> & replacementMap;

  FixupTraversal(
    const std::map<SgNode*, SgNode*> & inputReplacementMap
  ) :
    replacementMap(inputReplacementMap)
  {}

  void visit(SgNode* node) {
    FixupReplacer<FixupTraversal> r(*this, node);
    node->processDataMemberReferenceToPointers(&r);
  }
};

void fixupTraversal(
  const std::map<SgNode*, SgNode*> & replacementMap
) {
  TimingPerformance timer ("Reset the AST to share IR nodes:");

  FixupTraversal traversal(replacementMap);
  traversal.traverseMemoryPool();
}

struct FixupSubtreeTraversal : public SgSimpleProcessing {
  const std::map<SgNode*, SgNode*> & replacementMap;

  FixupSubtreeTraversal(
    const std::map<SgNode*, SgNode*> & inputReplacementMap
  ) :
    replacementMap(inputReplacementMap)
  {}

  void visit( SgNode* node) {
    FixupReplacer<FixupSubtreeTraversal> r(*this, node);
    node->processDataMemberReferenceToPointers(&r);
  }
};

void fixupSubtreeTraversal(
  SgNode* subtree,
  const std::map<SgNode*, SgNode*> & replacementMap
) {
  TimingPerformance timer ("Reset the AST to share IR nodes:");

  FixupSubtreeTraversal traversal(replacementMap);
  traversal.traverse(subtree,preorder);
}

}
}

