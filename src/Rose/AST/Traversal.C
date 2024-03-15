#include <sage3basic.h>                                 // HUGE HEADER needed by AstPrePostProcessing
#include <Rose/AST/Traversal.h>

namespace Rose {
namespace AST {
namespace Traversal {

namespace detail {

// This class must be in a .C file because AstPrePostProcessing depends on some very large header files that we don't want
// to include into each compilation unit that uses the visitor.
class WrapVisitorBase: public AstPrePostProcessing {
    const VisitorBase &visitorBase;
public:
    WrapVisitorBase(const VisitorBase &visitorBase)
        : visitorBase(visitorBase) {}

    virtual void preOrderVisit(SgNode *node) {
        visitorBase(node, Order::PRE);
    }

    virtual void postOrderVisit(SgNode *node) {
        visitorBase(node, Order::POST);
    }
};

void
forwardHelper(SgNode *ast, const VisitorBase &visitorBase) {
    if (ast)
        WrapVisitorBase(visitorBase).traverse(ast);
}

void
reverseHelper(SgNode *ast, const VisitorBase &visitorBase) {
    std::vector<SgNode*> nodes;
    for (SgNode *node = ast; node; node = node->get_parent()) {
        visitorBase(node, Order::PRE);
        nodes.push_back(node);
    }
    for (/*void*/; !nodes.empty(); nodes.pop_back())
        visitorBase(nodes.back(), Order::POST);
}

void
findReverseHelper(SgNode *ast, FinderBase &finder) {
    for (SgNode *node = ast; node; node = node->get_parent()) {
        if (finder(node)) {
            finder.found = node;
            return;
        }
    }
}

} // namespace

} // namespace
} // namespace
} // namespace
