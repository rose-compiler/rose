// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Test for Rose::Tree
#include <Sawyer/Tree.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A few tree vertex types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This part is typical of a BasicTypes.h header
class Expression;
using ExpressionPtr = std::shared_ptr<Expression>;
using ExpressionList = Sawyer::Tree::List<Expression, Expression>;
using ExpressionListPtr = std::shared_ptr<ExpressionList>;
class BinaryExpression;
using BinaryExpressionPtr = std::shared_ptr<BinaryExpression>;
class Recursive;
using RecursivePtr = std::shared_ptr<Recursive>;

// This part is typical of a header, although the implementations are usually elsewhere
class Expression: public Sawyer::Tree::Vertex<Expression> {
public:
    using Ptr = ExpressionPtr;
protected:
    Expression() {}
};

class BinaryExpression: public Expression {
public:
    using Ptr = BinaryExpressionPtr;
    Edge<Expression> lhs;
    Edge<Expression> rhs;
    Edge<ExpressionList> list;

protected:
    BinaryExpression()
        : lhs(*this), rhs(*this), list(*this, ExpressionList::instance()) {}

public:
    static Ptr instance() {
        return Ptr(new BinaryExpression);
    }
};

class Recursive: public Sawyer::Tree::Vertex<Recursive> {
public:
    using Ptr = RecursivePtr;
    Edge<Recursive> child;

protected:
    Recursive()
        : child(*this) {}

public:
    static Ptr instance() {
        return Ptr(new Recursive);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// vertex pointers are initialized to null
static void test01() {
    BinaryExpressionPtr e;
    ASSERT_always_require(e == nullptr);
}

// instantion can use `auto` and the type is mentioned on the rhs
static void test02() {
    auto e = BinaryExpression::instance();
    ASSERT_always_require(e != nullptr);
}

// child edges are initialized to null
static void test03() {
    auto e = BinaryExpression::instance();
    ASSERT_always_require(e->lhs == nullptr);
    ASSERT_always_require(e->rhs == nullptr);
}

// initialized child edges are initialized
static void test04() {
    auto e = BinaryExpression::instance();
    ASSERT_always_require(e != nullptr);
}

// you can get a pointer from a vertex object
static void test05() {
    auto e = BinaryExpression::instance();
    auto f = [](BinaryExpression &vertex) {
        return vertex.pointer();
    }(*e);
    ASSERT_always_require(e == f);
}

// parent pointers are initialized to null
static void test06() {
    auto e = BinaryExpression::instance();
    ASSERT_always_require(e->parent == nullptr);
}

// inserting a child changes its parent pointer
static void test07() {
    auto parent = BinaryExpression::instance();
    auto child = BinaryExpression::instance();

    parent->lhs = child;
    ASSERT_always_require(parent->lhs == child);
    ASSERT_always_require(child->parent == parent);
}

// erasing a child resets its parent pointer
static void test08() {
    auto parent = BinaryExpression::instance();
    auto child = BinaryExpression::instance();
    parent->lhs = child;

    parent->lhs = nullptr;
    ASSERT_always_require(parent->lhs == nullptr);
    ASSERT_always_require(child->parent == nullptr);
}

// inserting a different child changes both children's parent pointers
static void test09() {
    auto parent = BinaryExpression::instance();
    auto child = BinaryExpression::instance();
    parent->lhs = child;

    auto child2 = BinaryExpression::instance();
    parent->lhs = child2;
    ASSERT_always_require(parent->lhs == child2);
    ASSERT_always_require(child2->parent == parent);
    ASSERT_always_require(child->parent == nullptr);
}

// reassigning a child is a no-op
static void test10() {
    auto parent = BinaryExpression::instance();
    auto child = BinaryExpression::instance();
    parent->lhs = child;

    parent->lhs = child;
    ASSERT_always_require(parent->lhs == child);
    ASSERT_always_require(child->parent == parent);
}

// inserting a child in two different places is an error with no side effect
static void test11() {
    auto parent = BinaryExpression::instance();
    auto child = BinaryExpression::instance();
    parent->lhs = child;

    try {
        parent->rhs = child;
        ASSERT_always_not_reachable("data structure is no longer a tree");
    } catch (const Sawyer::Tree::Vertex<Expression>::InsertionError &error) {
        ASSERT_always_require(error.vertex == child);
        ASSERT_always_require(parent->lhs == child);
        ASSERT_always_require(child->parent == parent);
        ASSERT_always_require(parent->rhs == nullptr);
    }
}

// inserting a child into two different trees is an error with no side effect
static void test12() {
    auto parent = BinaryExpression::instance();
    auto child = BinaryExpression::instance();
    parent->lhs = child;

    auto parent2 = BinaryExpression::instance();
    try {
        parent2->lhs = child;
        ASSERT_always_not_reachable("data structure is no longer a tree");
    } catch (const Sawyer::Tree::Vertex<Expression>::InsertionError &error) {
        ASSERT_always_require(error.vertex == child);
        ASSERT_always_require(parent->lhs == child);
        ASSERT_always_require(child->parent == parent);
        ASSERT_always_require(parent2->lhs == nullptr);
    }
}

// inserting a child as its own parent is an error with no side effect
static void test13() {
    auto r = Recursive::instance();

#ifdef NDEBUG
    // Allowed in production configurations since it is not a constant time check
    r->child = r;
#else
    try {
        r->child = r;
        ASSERT_always_not_reachable("data structure is no longer a tree");
    } catch (const Sawyer::Tree::Vertex<Recursive>::CycleError &error) {
        ASSERT_always_require(error.vertex == r);
        ASSERT_always_require(r->child == nullptr);
        ASSERT_always_require(r->parent == nullptr);
    }
#endif
}

// inserting a child as its own descendant is an error with no side effect
static void test14() {
    auto r1 = Recursive::instance();
    auto r2 = Recursive::instance();
    auto r3 = Recursive::instance();
    r1->child = r2;
    r2->child = r3;

#ifdef NDEBUG
    // Allowed in production configurations since it is not a constant time check
    r3->child = r1;
#else
    try {
        r3->child = r1;
        ASSERT_always_not_reachable("data structure is no longer a tree");
    } catch (const Sawyer::Tree::Vertex<Recursive>::CycleError &error) {
        ASSERT_always_require(error.vertex == r1);
        ASSERT_always_require(r1->child == r2);
        ASSERT_always_require(r2->child == r3);
        ASSERT_always_require(r3->child == nullptr);
        ASSERT_always_require(r3->parent == r2);
        ASSERT_always_require(r2->parent == r1);
        ASSERT_always_require(r1->parent == nullptr);
    }
#endif
}

// lists are initialized to be empty
static void test15() {
    auto s = ExpressionList::instance();
    ASSERT_always_require(s->empty());
    ASSERT_always_require(s->size() == 0);
}

// null pointers can be pushed and popped
static void test16() {
    auto s = ExpressionList::instance();

    s->push_back(nullptr);
    ASSERT_always_require(!s->empty());
    ASSERT_always_require(s->size() == 1);

    s->push_back(nullptr);
    ASSERT_always_require(s->size() == 2);
    ASSERT_always_require(s->at(0) == nullptr);

    s->pop_back();
    ASSERT_always_require(s->size() == 1);
    ASSERT_always_require(s->at(0) == nullptr);

    s->pop_back();
    ASSERT_always_require(s->size() == 0);
    ASSERT_always_require(s->empty());
}

// pushing a child changes its parent pointer
static void test17() {
    auto parent = ExpressionList::instance();
    auto child = BinaryExpression::instance();

    parent->push_back(child);
    ASSERT_always_require(parent->size() == 1);
    ASSERT_always_require(parent->at(0) == child);
    ASSERT_always_require(child->parent == parent);
}

// popping a child clears its parent pointer
static void test18() {
    auto parent = ExpressionList::instance();
    auto child = BinaryExpression::instance();
    parent->push_back(child);

    parent->pop_back();
    ASSERT_always_require(child->parent == nullptr);
}

// assigning a child to a list changes its parent pointer
static void test19() {
    auto parent = ExpressionList::instance();
    parent->push_back(nullptr);

    auto child = BinaryExpression::instance();
    parent->at(0) = child;
    ASSERT_always_require(parent->at(0) == child);
    ASSERT_always_require(child->parent == parent);
}

// overwriting a child in a list changes its parent pointer
static void test20() {
    auto parent = ExpressionList::instance();
    auto child = BinaryExpression::instance();
    parent->push_back(child);

    auto child2 = BinaryExpression::instance();
    parent->at(0) = child2;
    ASSERT_always_require(parent->at(0) == child2);
    ASSERT_always_require(child2->parent == parent);
    ASSERT_always_require(child->parent == nullptr);
}

// reassigning a child to a list is a no-op
static void test21() {
    auto parent = ExpressionList::instance();
    auto child = BinaryExpression::instance();
    parent->push_back(child);

    parent->at(0) = child;
    ASSERT_always_require(parent->at(0) == child);
    ASSERT_always_require(child->parent == parent);
}

// inserting a child twice is an error with no side effect
static void test22() {
    auto parent = ExpressionList::instance();
    auto child = BinaryExpression::instance();
    parent->push_back(child);

    try {
        parent->push_back(child);
        ASSERT_always_not_reachable("data structure is no longer a tree");
    } catch (const Sawyer::Tree::Vertex<Expression>::InsertionError &error) {
        ASSERT_always_require(error.vertex == child);
        ASSERT_always_require(parent->size() == 1);
        ASSERT_always_require(parent->at(0) == child);
        ASSERT_always_require(child->parent == parent);
    }
}

// assigning a child to a second element is an error with no side effect
static void test23() {
    auto parent = ExpressionList::instance();
    auto child = BinaryExpression::instance();
    parent->push_back(child);
    parent->push_back(nullptr);

    try {
        parent->at(1) = child;
        ASSERT_always_not_reachable("data structure is no longer a tree");
    } catch (const Sawyer::Tree::Vertex<Expression>::InsertionError &error) {
        ASSERT_always_require(error.vertex == child);
        ASSERT_always_require(parent->size() == 2);
        ASSERT_always_require(parent->at(0) == child);
        ASSERT_always_require(parent->at(1) == nullptr);
        ASSERT_always_require(child->parent == parent);
    }
}

// replacing a child with null resets its parent pointer
static void test24() {
    auto parent = ExpressionList::instance();
    auto child = BinaryExpression::instance();
    parent->push_back(child);

    parent->at(0) = nullptr;
    ASSERT_always_require(parent->at(0) == nullptr);
    ASSERT_always_require(child->parent == nullptr);
}

// the array operator works
static void test25() {
    auto parent = ExpressionList::instance();
    auto c1 = BinaryExpression::instance();
    auto c2 = BinaryExpression::instance();
    parent->push_back(c1);
    parent->push_back(nullptr);
    parent->push_back(c2);

    ASSERT_always_require((*parent)[0] == c1);
    ASSERT_always_require((*parent)[1] == nullptr);
    ASSERT_always_require((*parent)[2] == c2);

    (*parent)[2] = nullptr;
    ASSERT_always_require((*parent)[2] == nullptr);
    ASSERT_always_require(c2->parent == nullptr);
}

// list iteration works
static void test26() {
    auto parent = ExpressionList::instance();
    auto c1 = BinaryExpression::instance();
    auto c2 = BinaryExpression::instance();
    parent->push_back(c1);
    parent->push_back(nullptr);
    parent->push_back(c2);

    auto iter = parent->begin();
    ASSERT_always_require(iter != parent->end());
    ASSERT_always_require(*iter == c1);
    ASSERT_always_require(iter[0] == c1);
    ASSERT_always_require(iter[1] == nullptr);
    ASSERT_always_require(iter[2] == c2);

    auto iter2 = ++iter;
    ASSERT_always_require(iter2 == iter);
    ASSERT_always_require(!(iter2 < iter));
    ASSERT_always_require(iter2 - iter == 0);
    ASSERT_always_require(iter2 + 0 == iter);
    ASSERT_always_require(iter != parent->end());
    ASSERT_always_require(*iter == nullptr);

    const auto iter3 = iter++;
    ASSERT_always_require(iter3 != iter);
    ASSERT_always_require(iter3 < iter);
    ASSERT_always_require(iter3 - iter == 1);
    ASSERT_always_require(iter3 + 1 == iter);
    ASSERT_always_require(iter != parent->end());
    ASSERT_always_require(*iter == c2);
    ASSERT_always_require(iter3[1] == c2);

    ++iter;
    ASSERT_always_require(iter == parent->end());

    --iter;
    ASSERT_always_require(*iter == c2);

    iter -= 2;
    ASSERT_always_require(*iter == c1);

    iter += 3;
    ASSERT_always_require(iter == parent->end());
}

int main() {
    test01();
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();
    test16();
    test17();
    test18();
    test19();
    test20();
    test21();
    test22();
    test23();
    test24();
    test25();
    test26();
}
