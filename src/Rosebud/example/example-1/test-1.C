#include <generated/TestAddExpr.h>
#include <generated/TestBinExpr.h>
#include <generated/TestInt.h>
#include <generated/TestType.h>

#include <Sawyer/Assert.h>
#include <boost/format.hpp>

int main() {
    // Build a small expression tree that adds two integers
    auto intType = TestType::instance("int");
    auto i42 = TestInt::instance(intType, 42);
    auto i911 = TestInt::instance(intType, 911);
    auto add1 = TestAddExpr::instance(intType, i42, i911);

    // Check pointers
    ASSERT_require(add1->lhs() == i42);
    ASSERT_require(add1->rhs() == i911);
    ASSERT_require(i42->parent() == add1);
    ASSERT_require(i911->parent() == add1);
    ASSERT_require(i42->type() == intType);
    ASSERT_require(i911->type() == intType);
    ASSERT_require(add1->type() == intType);
    ASSERT_require(intType->parent() == nullptr);

    // Count all the nodes in the expression tree
    size_t total = 0;
    add1->traverse([&total](const Rose::Tree::BasePtr&, Rose::Tree::Base::Traversal event) {
        if (Rose::Tree::Base::Traversal::ENTER == event)
            ++total;
        return false;
    });
    ASSERT_require2(3 == total, (boost::format("total=%d") % total).str());

    // Remove a node and see what happens
    add1->lhs() = nullptr;
    ASSERT_require(add1->lhs() == nullptr);
    ASSERT_require(i42->parent() == nullptr);

    // Impossible to make the tree not-a-tree
    try {
        add1->lhs() = i911;
        ASSERT_not_reachable("no longer a tree");
    } catch (const Rose::Tree::InsertionError&) {
    }

    // Replace a node
    add1->rhs() = i42;
    ASSERT_require(add1->rhs() == i42);
    ASSERT_require(i42->parent() == add1);
    ASSERT_require(i911->parent() == nullptr);

    // This shouldn't compile
    i42->parent() = add1;
}
