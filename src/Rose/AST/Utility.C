#include <Rose/AST/Utility.h>

#include <Rose/AST/Traversal.h>

#include <SgNode.h>

#include <Sawyer/Assert.h>

#include <ostream>
#include <string>
#include <vector>

namespace Rose {
namespace AST {
namespace Utility {

// Show an error message about the specified path in a subtree.
static void
showFailure(const std::vector<SgNode*> &path, std::ostream &out) {
    out <<"child/parent consistency failure in AST:\n";

    for (size_t position = path.size(); position > 0; --position) {
        // Current node: the child
        SgNode *child = path[position-1];
        ASSERT_not_null(child);
        out <<"  node #" <<(position - 1) <<" = (" <<child->class_name() <<"*)" <<child;

        // The child's parent according to the child
        SgNode *actualParent = child->get_parent();
        if (actualParent) {
            auto inPath = std::find(path.begin(), path.end(), actualParent);
            if (inPath != path.end()) {
                out <<", parent = node #" <<(inPath - path.begin()) <<"\n";
            } else {
                out <<", parent = (" <<actualParent->class_name() <<"*)" <<actualParent;
            }
        } else {
            out <<", parent = nullptr";
        }

        // The child's parent according to the parent-to-child pointers defining the tree.
        if (position >= 2) {
            SgNode *expectedParent = path[position-2];
            if (actualParent != expectedParent)
                out <<", expected parent is node #" <<(position-2);
        }
        out <<"\n";
    }
}

bool
checkParentPointers(SgNode *root, std::ostream &out) {
    bool isConsistent = true;
    std::vector<SgNode*> path;

    Traversal::forward<SgNode>(root, [&isConsistent, &path, &out](SgNode *node, const Traversal::Order order) {
        if (Traversal::Order::PRE == order) {
            if (!path.empty() && node->get_parent() != path.back()) {
                path.push_back(node);
                showFailure(path, out);
                isConsistent = false;
            } else {
                path.push_back(node);
            }
        } else {
            ASSERT_always_forbid(path.empty());
            ASSERT_always_require(node == path.back());
            path.pop_back();
        }
    });
    ASSERT_require(path.empty());

    return isConsistent;
}

void
repairParentPointers(SgNode *root) {
    std::vector<SgNode*> path;

    Traversal::forward<SgNode>(root, [&path](SgNode *node, const Traversal::Order order) {
        if (Traversal::Order::PRE == order) {
            if (!path.empty())
                node->set_parent(path.back());
            path.push_back(node);
        } else {
            ASSERT_forbid(path.empty());
            ASSERT_require(path.back() == node);
            path.pop_back();
        }
    });
    ASSERT_require(path.empty());
}

} // namespace
} // namespace
} // namespace
