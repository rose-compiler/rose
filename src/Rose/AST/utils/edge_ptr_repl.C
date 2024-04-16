
#include "sage3basic.h"
#include "Rose/AST/Utility.h"

namespace Rose { namespace AST { namespace Utility {

template <typename HandlerT, typename TraveralT>
struct EdgeTraversal : public TraveralT {
  using Self = EdgeTraversal<HandlerT, TraveralT>;
  using Handler = HandlerT;

  Handler handler;

  template <typename... Args>
  EdgeTraversal(Args&&... args) : handler(std::forward<Args>(args)...) {}

  virtual ~EdgeTraversal() {}

  void visit(SgNode* node) {
    node->processDataMemberReferenceToPointers(&handler);
  }
};

template <typename HandlerT> using EdgeMempoolTraversal = EdgeTraversal<HandlerT, ROSE_VisitTraversal>;
template <typename HandlerT> using EdgeTreeTraversal = EdgeTraversal<HandlerT, SgSimpleProcessing>;

struct EdgeReplacer : public SimpleReferenceToPointerHandler {
  replacement_map_t const & rmap;

  EdgeReplacer(replacement_map_t const & rmap_): rmap(rmap_) {}

  virtual ~EdgeReplacer() {}

  virtual void operator()(SgNode* & key, const SgName& debugStringName, bool /* traverse */) {
    if (key != nullptr) {
      auto it = rmap.find(key);
      if (it != rmap.end() && key != it->second) {
        SgNode * r = it->second;
        key = r;
      }
    }
  }
};

void edgePointerReplacement(replacement_map_t const & rmap) {
  EdgeMempoolTraversal<EdgeReplacer> traversal(rmap);
  traversal.traverseMemoryPool();
}

void edgePointerReplacement(SgNode * subtree, replacement_map_t const & rmap) {
  EdgeTreeTraversal<EdgeReplacer> traversal(rmap);
  traversal.traverse(subtree, preorder);
}

} } }

