#ifndef ROSE_AST_Traversal_H
#define ROSE_AST_Traversal_H

#include <RoseFirst.h>
#include <vector>

// A version of AST traversal that doesn't require huge header files to be included into the compilation unit that's calling the
// traversal.

class SgNode;

namespace Rose {
namespace AST {

/** Abstract syntax tree traversals. */
namespace Traversal {

/** Order that visitor is called for node w.r.t. children. */
enum class Order {                                      // bit flags
    PRE    = 0x01,                                      /**< Visitor is called before visiting the node's children. */
    POST   = 0x02                                       /**< Visitor is called after visiting the node's children. */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail {

// Base visitor class. The idea is that the base class is abstract so it can be implemented in a single .C file. It is implemented
// in terms of ROSE's `AstPrePostProcessing` which requires definitions for every Sg node. It's fine to #include all those
// definitions into that one .C file were we need them, but we want to avoid having to #include all those definitions into every
// .C file that needs to do an AST traversal.
//
// Classes derived from `VisitorBase` are defined by templates, but they've been designed so as to not require any class definitions
// except for `T` (and T's base classes).
class VisitorBase {
public:
    virtual ~VisitorBase() {}
    virtual void operator()(SgNode*, Order) const = 0;
};

// Calls functor before and after (pre and post) with the node pointer and an indication of when it is being called.
template<class T, class Functor>
class VisitorOrdered: public VisitorBase {
    Functor &f;
public:
    explicit VisitorOrdered(Functor &f)
        : f(f) {}

    void operator()(SgNode *node, const Order order) const override {
        if (T *typedNode = dynamic_cast<T*>(node))
            f(typedNode, order);
    }
};

// Calls functor only before visiting related nodes. Functor is called with only the node pointer argument.
template<class T, class Functor>
class VisitorPre: public VisitorBase {
    Functor &f;
public:
    explicit VisitorPre(Functor &f)
        : f(f) {}

    void operator()(SgNode *node, const Order order) const override {
        if (Order::PRE == order) {
            if (T *typedNode = dynamic_cast<T*>(node))
                f(typedNode);
        }
    }
};

// Calls functor only after visiting related nodes. Functor is called with only the node pointer argument.
template<class T, class Functor>
class VisitorPost: public VisitorBase {
    Functor &f;
public:
    explicit VisitorPost(Functor &f)
        : f(f) {}

    void operator()(SgNode *node, const Order order) const override {
        if (Order::POST == order) {
            if (T *typedNode = dynamic_cast<T*>(node))
                f(typedNode);
        }
    }
};

// Base class for finding a particular node. This works similarly to the `VisitorBase` in that this abstract base class is
// what's passed across compilation units so that only the one .C file that implements the traversal needs to include the whole
// world.
//
// The derived classes, which are class templates, only need the definition for their `T` type (and it's base classes), and do not
// need the definitions for all AST node classes.
class FinderBase {
public:
    SgNode *found = nullptr;

    virtual ~FinderBase() {}
    virtual bool operator()(SgNode *node) = 0;
};

// Calls the specified predicate for each visited node of the correct dynamic type.
template<class T, class Functor>
class Finder: public FinderBase {
    Functor &f;
public:
    explicit Finder(Functor &f)
        : f(f) {}

    bool operator()(SgNode *node) override {
        if (T *typedNode = dynamic_cast<T*>(node)) {
            return f(typedNode);
        } else {
            return false;
        }
    }
};

// Traverse an AST in a forward direction, calling the visitor for a node before and then after visiting the node's children.
void forwardHelper(SgNode *ast, const VisitorBase&);

// Traverse an AST in the reverse direction, calling the visitor for a node before and then after visiting the node's parents.
void reverseHelper(SgNode *ast, const VisitorBase&);

// Traverse an AST in reverse to find the first occurrence of a particular node, and saves that node in the `FinderBase`.
void findReverseHelper(SgNode *ast, FinderBase&);

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Traversals
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Traverse an AST in a forward direction.
 *
 * This function traverses the specified @p ast by following the child edges of the tree. It calls specified functor at each node
 * along the way that can be dynamically cast to a @p T pointer, and again when backtracking. In other words, when called for node
 * @c n, this traversal invokes the functor with the arguments @c n and @ref Order::PRE, then it recursively visits each child of @c
 * n, then it invokes the visitor a second time with arguments @c n and @ref Order::POST. The node argument of the functor is of
 * type `T*`.
 *
 * Example: Here's an example comparing the original `AstPrePostProcessing` to this newer function:
 *
 * @code
 *  // old code from prePostTraversal.C tutorial
 *  #include <sage3basic.h> // recursively about 600,000 lines of ROSE code plus various STL headers.
 *
 *  // Define the visitor
 *  class PreAndPostOrderTraversal: public AstPrePostProcessing {
 *  public:
 *      void preOrderVisit(SgNode *node) override {
 *          if (isSgForStatement(node))
 *              std::cout <<"entering `for` loop...\n";
 *      }
 *
 *      void postOrderVisit(SgNode *node) override {
 *          if (isSgForStatement(node))
 *              std::cout <<"leaving `for` loop...\n";
 *      }
 *  };
 *
 *  void debugForLoop(SgNode *node) {
 *      if (node) {
 *          PreAndPostOrderTraversal exampleTraversal;
 *          exampleTraversal.traverse(node);
 *      }
 *  }
 * @endcode
 *
 * Here's the corresponding code for the newer style traversals:
 *
 * @code
 *  // New code replacing prePostTraversal.C tutorial
 *  #include <Rose/AST/Traversal.h>  // recursively includes a few hundred lines of code
 *  void debugForLoop(SgNode *node) {
 *      Rose::AST::Traversal::forward<SgForStatement>(node, [](SgForStatement*, Rose::AST::Traversal::Order order) {
 *          std::cout <<(Rose::AST::Traversal::Order::PRE == order ? "entering" : "leaving") <<" for loop...\n";
 *      });
 *  }
 * @endcode */
template<class T, class Functor>
void forward(SgNode *ast, Functor functor) {
    detail::forwardHelper(ast, detail::VisitorOrdered<T, Functor>(functor));
}

/** Traverse an AST in a forward direction with pre-visit only.
 *
 *  This function is similar to the plain @ref forward traversal but instead of calling the functor both before and after visiting
 *  a node's children, this traversal calls the functor only before visiting the children. The functor is called with one argument:
 *  a non-null pointer to the node being visited, and this argument is of type `T*`. */
template<class T, class Functor>
void forwardPre(SgNode *ast, Functor functor) {
    detail::forwardHelper(ast, detail::VisitorPre<T, Functor>(functor));
}

/** Traverse an AST in a forward direction with post-visit only.
 *
 *  This function is similar to the plain @ref forward traversal but instead of calling the functor both before and after visiting
 *  a node's children, this traversal calls the functor only after visiting the children. The functor is called with one argument:
 *  a non-null pointer to the node being visited, and this argument is of type `T*`. */
template<class T, class Functor>
void forwardPost(SgNode *ast, Functor functor) {
    detail::forwardHelper(ast, detail::VisitorPost<T, Functor>(functor));
}

/** Traverse an AST in the reverse direction.
 *
 *  Traverses the specified AST following the child-to-parent edges. The functor is called for each node before and after its parent
 *  is visited. The functor is called with two arguments, the node pointer and an indication of when the functor is being
 *  called. The second argument being @ref Order::PRE before the node's parent is visited, and @ref Order::POST after the parent is
 *  visited. */
template<class T, class Functor>
void reverse(SgNode *ast, Functor functor) {
    detail::reverseHelper(ast, detail::VisitorOrdered<T, Functor>(functor));
}

/** Traverse an AST in the reverse with pre-visit only.
 *
 *  This function is similar to the plain @ref reverse traversal, but only calls the functor before visiting the node's parents.
 *  The functor takes only one argument: a pointer to the node being visited. */
template<class T, class Functor>
void reversePre(SgNode *ast, Functor functor) {
    detail::reverseHelper(ast, detail::VisitorPre<T, Functor>(functor));
}

/** Traverse an AST in the reverse with post-visit only.
 *
 *  This function is similar to the plain @ref reverse traversal, but only calls the functor after visiting the node's parents.
 *  The functor takes only one argument: a pointer to the node being visited. */
template<class T, class Functor>
void reversePost(SgNode *ast, Functor functor) {
    detail::reverseHelper(ast, detail::VisitorPost<T, Functor>(functor));
}

/** Reverse traversal that finds the first node satisfying the predicate.
 *
 *  This is a reverse traversal starting at the specified node and following child-to-parent edges to find the first node having
 *  dynamic type `T` and for which the user-specified predicate returns true. The predicate takes one argument: a pointer to a
 *  node. The return value might be the same node as the argument.  If the argument is null or if no satisfying node is found, then
 *  this function returns null. */
template<class T, class Functor>
T*
findParentSatisying(SgNode *ast, Functor functor) {
    detail::Finder<T, Functor> finder(functor);
    detail::findReverseHelper(ast, finder /*in,out*/);
    return dynamic_cast<T*>(finder.found);
}

/** Find the closest parent of specified type.
 *
 *  Traverses the specified AST and returns a pointer to the closest parent of the specified type. The return value might be the
 *  argument itself. If the argument is null or if no such node type can be found, then this function returns null. */
template<class T>
T*
findParentTyped(SgNode *ast) {
    auto yes = [](SgNode*) { return true; };
    detail::Finder<T, decltype(yes)> finder(yes);
    detail::findReverseHelper(ast, finder /*in,out*/);
    return dynamic_cast<T*>(finder.found);
}

/** Find all descendants of specified type.
 *
 *  Traverses the specified AST in depth-first pre-order and builds a vector that points to all the nodes of the tree that are of
 *  the specified type. All returned pointers are non-null. The specified root of the AST might be one of the pointers returned. */
template<class T>
std::vector<T*>
findDescendantsTyped(SgNode *ast) {
    std::vector<T*> retval;
    forwardPre<T>(ast, [&retval](T *node) {
        retval.push_back(node);
    });
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif /* ROSE_AST_Traversal_H */
