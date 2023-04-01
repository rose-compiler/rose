// Make sure the assertions for the tests are enabled. We could have also used the ASSERT_always_* forms, but that's quite a bit
// more to read.
#undef NDEBUG

#include <Rosebud/Ast.h>

using namespace Rosebud;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// How to declare nodes (the hard way)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------------------------------
// Every node inherits ultimately from Ast::Node. This can either be direct or indirect through base classes.
class Rational: public Ast::Node {
public:
    // Nodes can have pretty much any data members they want.
    unsigned numerator;
    unsigned denominator;
    bool negative;

    // Nodes are always allocated in heap, not stack or static. To help with this, we follow a simple principle for constructors:
    //
    //   1. All C++ constructors for the class are "protected".
    //   2. Every node type that can be instantiated has a static `instance` method.
    //
    // The protected C++ constructors prevent users from accidentally instantiating nodes on the stack or statically. The `instance`
    // methods, which can be overloaded, take the place of the C++ constructors from the user's perspective. They allocate the node
    // on the heap and return a shared-ownership pointer.
    //
    // Note that since the `instance` methods are what users use, we often end up with fewer, or less complicated, C++ constructors
    // because we don't need all kinds of combinations. Another benefite of `instance` is that one `instance` method can call
    // another, which is not generally possible for C++ constructors.
protected:
    Rational()
        : numerator(0), denominator(0), negative(1) {}
    Rational(unsigned numerator, unsigned denominator, bool negative)
        : numerator(numerator), denominator(denominator), negative(negative) {}
public:
    static std::shared_ptr<Rational> instance() {
        return std::shared_ptr<Rational>(new Rational);
    }
    static std::shared_ptr<Rational> instance(unsigned numerator, unsigned denominator, bool negative = false) {
        return std::shared_ptr<Rational>(new Rational(numerator, denominator, negative));
    }
};

//------------------------------------------------------------------------------------------------------------------------------
// Here's another node without so much commentary. Usually all the public stuff has Doxygen documentation, and the implementations
// are moved to *.C files instead of inline.
class Type: public Ast::Node {
public:
    std::string name;

protected:
    Type() = delete;
    explicit Type(const std::string &name)
        : name(name) {}
public:
    static std::shared_ptr<Type> instance(const std::string &name) {
        return std::shared_ptr<Type>(new Type(name));
    }
};

//------------------------------------------------------------------------------------------------------------------------------
// Here's a non-terminal node. I.e., a node that can appear in the interior of the tree instead of only at the leaves.
class BinaryOperator: public Ast::Node {
public:
    // One node can point to another. In this case, the edge from this node to the other is NOT considered part of the tree.  You
    // can use any kind of pointer you want, even raw pointers. For instance, here we decided that a binary operator must always
    // share ownership of the type, so we used a shared_ptr. On the other hand, the `printedType` is not owned by this node and is
    // allowed to be deleted even when this node points to it, so we used weak_ptr.
    std::shared_ptr<Type> type;                         // the real type of this expression
    std::weak_ptr<Type> printedType;                    // the optional type print if available

    // On the other hand, sometimes we want the pointed-to node to be part of the tree. We do that with Ast::ChildEdge.  By the way,
    // these are called "edges" instead of "pointers" because they're bi-directional as we'll see shortly.
    Ast::ChildEdge<Rational> lhs;                        // left hand side of operation
    Ast::ChildEdge<Rational> rhs;                        // right hand size of operation

    // The usual protected C++ initializers follow, but this time we MUST initialize the Ast::ChildEdge members because they don't
    // have default constructors. Their first argument is always `*this`, a reference to the object which owns them. Their optional
    // second argument is used to initialize the pointee. We'll talk more about this later.
protected:
    BinaryOperator()
        : lhs(*this), rhs(*this) {}

    BinaryOperator(const std::shared_ptr<Rational> &lhs, const std::shared_ptr<Rational> &rhs,
                   const std::shared_ptr<Type> &type, const std::shared_ptr<Type> &printedType)
        : type(type), printedType(printedType),
          lhs(*this, lhs), rhs(*this, rhs) {}

public:
    static std::shared_ptr<BinaryOperator> instance() {
        return std::shared_ptr<BinaryOperator>(new BinaryOperator);
    }

    // All those `std::shared_ptr<T>` are getting quite verbose, right? Having a simple policy of defining a `Ptr` alias for
    // every one of them alleviates the verbosity, but we're keeping them explicit for this set of examples.
    static std::shared_ptr<BinaryOperator>
    instance(const std::shared_ptr<Rational> &lhs, const std::shared_ptr<Rational> &rhs,
             const std::shared_ptr<Type> &type, const std::shared_ptr<Type> &printedType = nullptr) {
        return std::shared_ptr<BinaryOperator>(new BinaryOperator(lhs, rhs, type, printedType));
    }
};

//------------------------------------------------------------------------------------------------------------------------------
// Here's another non-terminal node that's derived from one of our other nodes. This one has no new data members. We still need
// to protect the C++ constructors and provide `instance` methods.
class Multiply: public BinaryOperator {
protected:
    Multiply() {}

    // This time, we show how a non-tree child can be allocated on the fly.
    Multiply(const std::shared_ptr<Rational> &lhs, const std::shared_ptr<Rational> &rhs)
        : BinaryOperator(lhs, rhs, Type::instance("rational"), Type::instance("multiplied")) {}

public:
    static std::shared_ptr<Multiply>
    instance(const std::shared_ptr<Rational> &lhs, const std::shared_ptr<Rational> &rhs) {
        return std::shared_ptr<Multiply>(new Multiply(lhs, rhs));
    }
};

//------------------------------------------------------------------------------------------------------------------------------
// A special `ListNode` type acts like a vector of pointers to children all of the same type.
using ExpressionList = Ast::ListNode<BinaryOperator>;

//------------------------------------------------------------------------------------------------------------------------------
// Finally, so we can test cycles, a node that takes its own type as a child
class PossibleCycles: public Ast::Node {
public:
    Ast::ChildEdge<PossibleCycles> child;

protected:
    PossibleCycles()
        : child(*this) {}
    PossibleCycles(const std::shared_ptr<PossibleCycles> &child)
        : child(*this, child) {}

public:
    static std::shared_ptr<PossibleCycles> instance(const std::shared_ptr<PossibleCycles> &child = nullptr) {
        return std::shared_ptr<PossibleCycles>(new PossibleCycles(child));
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// node pointers are initialized to null
static void test01() {
    std::shared_ptr<Rational> v;
    ASSERT_require(v == nullptr);
}

// Instantiation can use `auto` and the type is still clear since it appears on the rhs of the `=`. Nodes are always pointers.
static void test02() {
    auto v = Rational::instance();
    ASSERT_not_null(v);
}

// You can get a new shared pointer from a reference. References are useful as function arguments to say that the function
// expects the node to exist and expects that its lifetime will outlast any call to the function.
static void test03() {
    auto v = Rational::instance();
    auto w = [](Rational &exists) {
        return exists.pointer();
    }(*v);

    ASSERT_require(v == w);
}

// Parent pointers are initialized to null
static void test04() {
    auto v = Rational::instance();
    ASSERT_require(v->parent == nullptr);
}

// Parent pointers cannot be assigned
static void test05() {
    auto v = Rational::instance();
    auto w = Rational::instance();

    // Error message is something like this:
    //   error: no match for 'operator=' (operand types are 'Rosebud::Ast::ParentEdge' and 'std::shared_ptr<Rational>')
    //   note: candidate: 'Rosebud::Ast::ParentEdge& Rosebud::Ast::ParentEdge::operator=(....)' (deleted)
#ifdef COMPILE_ERROR_05
    v->parent = w;                                      // this is a compile-time error
#endif
}

// Child pointers are initialized to null
static void test06() {
    auto b = BinaryOperator::instance();
    ASSERT_require(b->lhs == nullptr);
    ASSERT_require(b->rhs == nullptr);
}

// Setting a child pointer adjusts the child's parent pointer
static void test07() {
    auto b = BinaryOperator::instance();
    auto r = Rational::instance();

    b->lhs = r;
    ASSERT_require(r->parent == b);
}

// Removing a child from its parent clears the child's parent pointer
static void test08() {
    auto b = BinaryOperator::instance();
    auto r = Rational::instance();
    b->lhs = r;

    b->lhs = nullptr;
    ASSERT_require(r->parent == nullptr);
}

// Assigning the same child twice is allowed
static void test09() {
    auto b = BinaryOperator::instance();
    auto r = Rational::instance();
    b->lhs = r;

    b->lhs = r;
    ASSERT_require(r->parent == b);
}

// Assigning a new child adjusts both children's parent pointers
static void test10() {
    auto b = BinaryOperator::instance();
    auto r = Rational::instance();
    b->lhs = r;

    auto s = Rational::instance();
    b->lhs = s;
    ASSERT_require(r->parent == nullptr);
    ASSERT_require(s->parent == b);
}

// Assigning the same child to two different parts of a tree is an error since the result would not be a tree.
static void test11() {
    auto b = BinaryOperator::instance();
    auto r = Rational::instance();
    b->lhs = r;

    try {
        b->rhs = r;
        ASSERT_not_reachable("result is no longer a tree");
    } catch (const Ast::AttachmentError &e) {
        ASSERT_require(e.node == r);
        ASSERT_require(b->lhs == r);
        ASSERT_require(r->parent == b);
        ASSERT_require(b->rhs == nullptr);
    }
}

// Assigning a node as a child of itself is an error
static void test12() {
    auto p = PossibleCycles::instance();

    try {
        p->child = p;
        ASSERT_not_reachable("result is no longer a tree");
    } catch (const Ast::CycleError &e) {
        ASSERT_require(e.node == p);
        ASSERT_require(p->child == nullptr);
        ASSERT_require(p->parent == nullptr);
    }
}

// Attempting to assign a node as a child of itself does not change the node
static void test13() {
    auto p1 = PossibleCycles::instance();
    auto p2 = PossibleCycles::instance();
    p1->child = p2;
    ASSERT_require(p1->child == p2);
    ASSERT_require(p2->parent == p1);

    try {
        p2->child = p2;
        ASSERT_not_reachable("result is no longer a tree");
    } catch (const Ast::AttachmentError &e) {
        ASSERT_require(e.node == p2);
        ASSERT_require(p2->child == nullptr);
        ASSERT_require(p2->parent == p1);
    }
}

// Forming a cycle in the tree is a runtime error
static void test14() {
    auto p1 = PossibleCycles::instance();
    auto p2 = PossibleCycles::instance();
    auto p3 = PossibleCycles::instance();
    auto p4 = PossibleCycles::instance();

    p1->child = p2;
    p2->child = p3;
    p3->child = p4;

    try {
        p4->child = p1;
        ASSERT_not_reachable("result is no longer a tree");
    } catch (const Ast::CycleError &e) {
        ASSERT_require(e.node == p1);
        ASSERT_require(p1->child == p2);
        ASSERT_require(p1->parent == nullptr);
        ASSERT_require(p2->parent == p1);
    }
}

// A list node is initialized to be empty
static void test15() {
    auto e = ExpressionList::instance();
    ASSERT_not_null(e);
    ASSERT_require(e->empty());
    ASSERT_require(e->size() == 0);
    ASSERT_require(e->begin() == e->end());
}

// Pushing something onto a list changes its size
static void test16() {
    auto e = ExpressionList::instance();

    e->push_back(nullptr);
    ASSERT_require(!e->empty());
    ASSERT_require(e->size() == 1);
    ASSERT_require(e->begin() != e->end());
}

// Pushing a null pointer onto a list is allowed
static void test17() {
    auto e = ExpressionList::instance();

    e->push_back(nullptr);
    ASSERT_require(e->at(0) == nullptr);
    ASSERT_require((*e)[0] == nullptr);
    ASSERT_require(e->front() == nullptr);
    ASSERT_require(e->back() == nullptr);
}

// Pushing a child onto a list node adjusts the child's parent pointer
static void test18() {
    auto e = ExpressionList::instance();
    auto b = BinaryOperator::instance();

    e->push_back(b);
    ASSERT_require(e->size() == 1);
    ASSERT_require(e->front() == b);
    ASSERT_require(b->parent == e);
}

// Replacing a null with a child changes the child's parent pointer
static void test19() {
    auto e = ExpressionList::instance();
    e->push_back(nullptr);

    auto b = BinaryOperator::instance();
    e->at(0) = b;
    ASSERT_require(e->at(0) == b);
    ASSERT_require((*e)[0] == b);
    ASSERT_require(b->parent == e);
}

// Replacing a child with null resets the child's parent pointer
static void test20() {
    auto e = ExpressionList::instance();
    auto b = BinaryOperator::instance();
    e->push_back(b);

    e->at(0) = nullptr;
    ASSERT_require(e->at(0) == nullptr);
    ASSERT_require((*e)[0] == nullptr);
    ASSERT_require(b->parent == nullptr);
}

// Adding the same child twice is a runtime error
static void test21() {
    auto e = ExpressionList::instance();
    auto b = BinaryOperator::instance();
    e->push_back(b);

    try {
        e->push_back(b);
        ASSERT_not_reachable("result is no longer a tree");
    } catch (const Ast::AttachmentError &error) {
        ASSERT_require(error.node == b);
        ASSERT_require(e->size() == 1);
        ASSERT_require(e->at(0) == b);
        ASSERT_require(b->parent == e);
    }
}

// Popping a child resets the child's parent pointer
static void test22() {
    auto e = ExpressionList::instance();
    auto b = BinaryOperator::instance();
    e->push_back(b);

    e->pop_back();
    ASSERT_require(e->empty());
    ASSERT_require(b->parent == nullptr);
}

// It is possible to iterate over children
static void test23() {
    auto e = ExpressionList::instance();
    auto b0 = BinaryOperator::instance();
    auto b1 = BinaryOperator::instance();
    auto b2 = BinaryOperator::instance();
    e->push_back(b0);
    e->push_back(b1);
    e->push_back(b2);

    // iteration
    ASSERT_require(e->size() == 3);
    auto i = e->begin();
    ASSERT_require(i != e->end());
    ASSERT_require(*i == b0);
    ++i;
    ASSERT_require(i != e->end());
    ASSERT_require(*i == b1);
    ++i;
    ASSERT_require(i != e->end());
    ASSERT_require(*i == b2);
    ++i;
    ASSERT_require(i == e->end());

    // range for
    size_t j = 0;
    for (const auto &c: *e) {
        switch (j) {
            case 0:
                ASSERT_require(c == b0);
                break;
            case 1:
                ASSERT_require(c == b1);
                break;
            case 2:
                ASSERT_require(c == b2);
                break;
            default:
                ASSERT_not_reachable("too many");
        }
        ++j;
    }
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
}
