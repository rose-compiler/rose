#include <sage3basic.h>
#include <Rose/AST/Utils.h>

#include <ostream>
#include <string>
#include <vector>

namespace Rose {
namespace AST {
namespace Utils {

bool
checkParentPointers(SgNode *root, std::ostream &out) {
    struct Test: AstPrePostProcessing {
        std::vector<SgNode*> stack;
        bool isConsistent = true;
        std::ostream &out;

        Test(std::ostream &out)
            : out(out) {}

        void failure() {
            isConsistent = false;
            out <<"child/parent consistency failure in AST:\n";
            for (size_t i = 0; i < stack.size(); ++i) {
                ASSERT_not_null(stack[i]);
                SgNode *node = stack[i];
                SgNode *expectedParent = i > 0 ? stack[i-1] : nullptr;
                SgNode *actualParent = node->get_parent();
                out <<"  #" <<i <<": node (" <<node->class_name() <<"*)" <<node <<" parent = ";
                if (actualParent) {
                    out <<"(" <<actualParent->class_name() <<"*)" <<actualParent;
                } else {
                    out <<"nullptr";
                }
                if (expectedParent != actualParent) {
                    if (expectedParent) {
                        out <<", should be (" <<expectedParent->class_name() <<"*)" <<expectedParent <<" #" <<(i-1);
                    } else {
                        out <<", should be nullptr";
                    }
                }
                out <<"\n";
            }
        }

        void preOrderVisit(SgNode *node) override {
            if (!stack.empty() && node->get_parent() != stack.back()) {
                stack.push_back(node);
                failure();
            } else {
                stack.push_back(node);
            }
        }

        void postOrderVisit(SgNode *node) override {
            ASSERT_always_forbid(stack.empty());
            ASSERT_always_require(node == stack.back());
            stack.pop_back();
        }
    } t(out);
    t.traverse(root);
    ASSERT_require(t.stack.empty());
    return t.isConsistent;
}

void
repairParentPointers(SgNode *root) {
    struct: AstPrePostProcessing {
        std::vector<SgNode*> stack;

        void preOrderVisit(SgNode *node) override {
            if (!stack.empty())
                node->set_parent(stack.back());
            stack.push_back(node);
        }

        void postOrderVisit(SgNode *node) override {
            ASSERT_forbid(stack.empty());
            ASSERT_require(stack.back() == node);
            stack.pop_back();
        }
    } t;
    t.traverse(root);
    ASSERT_require(t.stack.empty());
}

} // namespace
} // namespace
} // namespace
