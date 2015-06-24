#include <rose.h>
#include <InsnSemanticsExpr.h>

using namespace rose;
using namespace rose::BinaryAnalysis::InsnSemanticsExpr;

static void
test_add_simplifications() {
    TreeNodePtr reg = LeafNode::create_variable(32, "esp_0");
    TreeNodePtr nreg = InternalNode::create(32, OP_NEGATE, reg);
    TreeNodePtr number = LeafNode::create_integer(32, 0xfffffffc);
    TreeNodePtr t1 = InternalNode::create(32, OP_ADD, reg, nreg, number);
    std::cout <<"(add esp_0 (negate esp_0) 0xfffffffc) = " <<*t1 <<"\n";
}

int
main() {
    test_add_simplifications();
}
