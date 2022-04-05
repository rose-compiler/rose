
#include "sage3basic.h"
#include "Rose/AST/utils.h"

using namespace std;

namespace Rose { namespace AST { namespace Utils {

template <typename HandlerT, typename TraveralT>
struct EdgeTraversal : public TraveralT {
  using Self = EdgeTraversal<HandlerT, TraveralT>;
  using Handler = HandlerT;

  Handler handler;

  template <typename... Args>
  EdgeTraversal(Args... args) : handler(args...) {}

  void visit(SgNode* node) {
    node->processDataMemberReferenceToPointers(&handler);
  }
};

template <typename HandlerT> using EdgeMempoolTraversal = EdgeTraversal<HandlerT, ROSE_VisitTraversal>;
template <typename HandlerT> using EdgeTreeTraversal = EdgeTraversal<HandlerT, SgSimpleProcessing>;

struct EdgeReplacer : public SimpleReferenceToPointerHandler {
  const std::map<SgNode*, SgNode*> & rmap;

  EdgeReplacer(const std::map<SgNode*, SgNode*> & rmap_): rmap(rmap_) {}

  virtual void operator()(SgNode* & key, const SgName& debugStringName, bool /* traverse */) {
    if (key != NULL) {
      auto it = rmap.find(key);
      if (it != rmap.end() && key != it->second) {
        key = it->second;
      }
    }
  }
};

void edgePointerReplacement(const std::map<SgNode*, SgNode*> & rmap) {
  EdgeMempoolTraversal<EdgeReplacer> traversal(rmap);
  traversal.traverseMemoryPool();
}

void edgePointerReplacement(SgNode* subtree, const std::map<SgNode*, SgNode*> & rmap) {
  EdgeTreeTraversal<EdgeReplacer> traversal(rmap);
  traversal.traverse(subtree, preorder);
}

} } }

