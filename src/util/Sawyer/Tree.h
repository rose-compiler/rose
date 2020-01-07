// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Tree_H
#define Sawyer_Tree_H

#include <Sawyer/Assert.h>
#include <Sawyer/Optional.h>

#include <memory>
#include <stdexcept>
#include <vector>

#if 1 // DEBUGGING [Robb Matzke 2019-02-18]
#include <iostream>
#endif

namespace Sawyer {

/** Tree data structure.
 *
 *  This name space contains the building blocks for relating heap-allocated objects (called nodes, base type @ref Node) in a
 *  tree-like way. The children of a node are referenced either by name or iteratively. Each node of the tree has a single
 *  parent pointer which is adjusted automatically to ensure consistency. A node can also point to another node without
 *  creating a managed parent-child edge; such node pointers are not followed during traversals.
 *
 *  The primary purpose of these classes is to ensure consistency in the tree data structure:
 *
 *  @li Pointers from children to parents are maintained automatically.
 *
 *  @li Exceptions are thrown if the user attempts to link nodes together in a way that doesn't form a tree.
 *
 *  @li The infrastructure is exception safe.
 *
 *  @li Nodes are reference counted via std::shared_ptr.
 *
 *  Although this implementation is intended to be efficient in time and space, the primary goal is safety. When the two goals
 *  are in conflict, safety takes precedence.
 *
 *  The basic usage is that the user defines some node types that inherit from @ref Tree::Node. If any of those types have
 *  parent-child tree edges (parent-to-child pointers that are followed during traversals), then those edges are declared
 *  as data members and initialized during construction:
 *
 * @code
 *  class MyNode: public Tree::Node {
 *  public:
 *      // Define a tree edge called "first" that points to a user-defined type
 *      // "First" (defined elsewhere) that derives from Tree::Node.
 *      Tree::ChildEdge<First> first;
 *
 *      // Define a tree edge called "second" that points to another "MyNode" node.
 *      Tree::ChildEdge<MyNode> second;
 *
 *      // Define a final edge called "third" that points to any kind of tree node.
 *      Tree::ChildEdge<Tree::Node> third;
 *
 *      // Initialize the nodes during construction. The first will point to a
 *      // specified node, and the other two will be initialized to null.
 *      explicit MyNode(const std::shared_ptr<First> &first)
 *          : first(this, first), second(this), third(this) {}
 *  }
 * @endcode
 *
 *  The data members can be the left hand side of assignment operators, in which case the parent pointers of the affected nodes
 *  are updated automatically.
 *
 * @code
 *  void test() {
 *      auto a = std::make_shared<First>();
 *      auto b = std::make_shared<First>();
 *
 *      auto root = std::make_shared<MyNode>(a);
 *      assert(root->first == a);
 *      assert(a->parent == root);     // set automatically by c'tor defined above
 *
 *      root->first = b;
 *      assert(root->first == b);
 *      assert(b->parent == root);     // set automatically
 *      assert(a->parent == nullptr);  // cleared automatically
 *  }
 * @endcode
 *
 *  See also, @ref Sawyer::Container::Graph "Graph", which can store non-class values such as 'int', is optimized for performance,
 *  and can easily handle cycles, self-edges, and parallel edges.
 *
 *  NOTICE: The @ref Tree API is experimental and still under development. Currently, each child pointer occupies twice the
 *  space as a raw pointer. Dereferencing a child pointer takes constant time but includes one dynamic cast.  Besides the child
 *  data member edges just described, each @ref Node object also has a @c parent data member that occupies as much space as a
 *  raw pointer and can be dereferenced in constant time, and a @c children vector of pointers with one element per @ref
 *  ChildEdge data member and one additional element. */
namespace Tree {

class Node;
class Children;

/** Short name for node pointers.
 *
 *  A shared-ownership pointer for nodes.
 *
 * @{ */
using NodePtr = std::shared_ptr<Node>;
using ConstNodePtr = std::shared_ptr<const Node>;
/** @} */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                   ____            _                 _   _
//                  |  _ \  ___  ___| | __ _ _ __ __ _| |_(_) ___  _ __  ___
//                  | | | |/ _ \/ __| |/ _` | '__/ _` | __| |/ _ \| '_ \/ __|
//                  | |_| |  __/ (__| | (_| | | | (_| | |_| | (_) | | | \__ |
//                  |____/ \___|\___|_|\__,_|_|  \__,_|\__|_|\___/|_| |_|___/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Traversal event bit flags. */
enum TraversalEvent {
    ENTER = 0x1,                                        /**< Traversal has just entered the node under consideration. */
    LEAVE = 0x2                                         /**< Traversal has just left the node under consideration. */
};

/** Traversal actions. */
enum TraversalAction {
    CONTINUE,                                           /**< Continue with the traversal. */
    SKIP_CHILDREN,                                      /**< For enter events, do not traverse into the node's children. */
    ABORT                                               /**< Abort the traversal immediately. */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exception declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Exceptions for tree-related operations. */
class Exception: public std::runtime_error {
public:
    Exception(const std::string &mesg)
        : std::runtime_error(mesg) {}
};

/** Exception if tree consistency would be violated.
 *
 *  If the user attempts to modify the connectivity of a tree in a way that would cause it to become inconsistent, then this
 *  exception is thrown. Examples of inconsistency are:
 *
 *  @li Attempting to insert a node in such a way that it would have two different parents.
 *
 *  @li Attempting to insert a node at a position that would cause it to be a sibling of itself.
 *
 *  @li Attempting to insert a node in such a way that it would become its own parent.
 *
 *  @li Attempting to insert a node in such a way as to cause a cycle in the connectivity. */
class ConsistencyException: public Exception {
public:
    NodePtr child;                        /**< Child node that was being modified. */

    ConsistencyException(const NodePtr &child,
                         const std::string &mesg = "attempt to attach child that already has a different parent")
        : Exception(mesg), child(child) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ChildEdge declaration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** An edge from a parent to a child.
 *
 *  An edge is the inter-node link between a parent node and a child node. The @ref ChildEdge type is only allowed for
 *  data members of a @ref Node and is how the node defines which data members participate as edges in the tree.
 *
 *  To create a node and define that it points to two child nodes, one must declare the two child pointers using the @ref
 *  ChildEdge type, and then initialize the parent end of the edges during construction, as follows:
 *
 * @code
 *  class Parent: public Tree::Node {
 *  public:
 *      Tree::ChildEdge<ChildType1> first;  // ChildType1 is userdefined, derived from Tree::Node
 *      Tree::ChildEdge<ChildType2> second; // ditto for ChildType2
 *
 *      Parent()
 *          : first(this), second(this) {}
 *  };
 * @endcode
 *
 *  It is also possible to give non-null values to the child ends of the edges during construction:
 *
 * @code
 *  Parent::Parent(const std::shared_ptr<ChildType1> &c1)
 *      : first(this, c1), second(this, std::make_shared<ChildType2>()) {}
 * @endcode
 *
 *  The @ref ChildEdge members are used as if they were pointers:
 *
 * @code
 *  auto parent = std::make_shared<Parent>();
 *  assert(parent->first == nullptr);
 *
 *  auto child = std::make_shared<ChildType1>();
 *  parent->first = child;
 *  assert(parent->first == child);
 *  assert(child->parent == parent);
 * @endcode */
template<class T>
class ChildEdge final {
private:
    Node *container_;                                   // non-null ptr to node that's the source of this edge
    const size_t idx_;                                  // index of the child edge from the parent's perspective

public:
    /** Points to no child. */
    explicit ChildEdge(Node *container);

    /** Constructor that points to a child. */
    ChildEdge(Node *container, const std::shared_ptr<T> &child);

    ChildEdge(const ChildEdge&) = delete;               // just for now

    /** Point to a child node. */
    ChildEdge& operator=(const std::shared_ptr<T> &child) {
        assign(child);
        return *this;
    }

    /** Cause this edge to point to no child. */
    void reset() {
        assign(nullptr);
    }

    /** Obtain shared pointer. */
    std::shared_ptr<T> operator->() const {
        return shared();
    }

    /** Obtain pointed-to node. */
    T& operator*() const {
        ASSERT_not_null(shared());
        return *shared();
    }
    
    /** Conversion to bool. */
    explicit operator bool() const {
        return shared() != nullptr;
    }

    /** Pointer to the child. */
    std::shared_ptr<T> shared() const;

    /** Implicit conversion to shared pointer. */
    operator std::shared_ptr<T>() const {
        return shared();
    }

private:
    // Assign a new child to this edge.
    void assign(const NodePtr &child);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ParentEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Edge pointing from child to parent.
 *
 *  This is a special pointer type that allows a child node to point to a parent node. Each tree @ref Node has a @p parent
 *  pointer of this type. The value of the pointer is updated automatically when the node is inserted into or removed from a
 *  tree. */
class ParentEdge final {
private:
    Node* parent_;

public:
    ParentEdge()
        : parent_(nullptr) {}

    /** Obtain shared pointer. */
    NodePtr operator->() const {
        return shared();
    }

    /** Obtain pointed-to node. */
    Node& operator*() const {
        ASSERT_not_null(parent_);
        return *shared();
    }

    /** Return the parent as a shared-ownership pointer. */
    NodePtr shared() const;

    /** Conversion to bool. */
    explicit operator bool() const {
        return parent_ != nullptr;
    }

#if 0 // [Robb Matzke 2019-02-18]: Node is not declared yet?
    /** Implicit conversion to shared pointer. */
    operator std::shared_ptr<Node> const {
        return shared();
    }
#endif
    
    /** Relation.
     *
     * @{ */
    bool operator==(const ParentEdge &other) const { return parent_ == other.parent_; }
    bool operator!=(const ParentEdge &other) const { return parent_ != other.parent_; }
    bool operator< (const ParentEdge &other) const { return parent_ <  other.parent_; }
    bool operator<=(const ParentEdge &other) const { return parent_ <= other.parent_; }
    bool operator> (const ParentEdge &other) const { return parent_ >  other.parent_; }
    bool operator>=(const ParentEdge &other) const { return parent_ >= other.parent_; }
    /** @} */

private:
    friend class Children;

    // Set the parent
    void set(Node *parent) {
        parent_ = parent;
    }

    // Clear the parent
    void reset() {
        parent_ = nullptr;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Children declaration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Vector of parent-to-child pointers.
 *
 *  Each @ref Node has a @c children data member that similar to a <tt>const std::vector<NodePtr></tt> and which points to all
 *  the child nodes. This list has one element per @ref ChildEdge data member regardless of whether that @ref ChildEdge has a
 *  null or non-null value.  Assigning a new node to a @ref ChildEdge pointer will automatically update the corresponding
 *  element of this list. */
class Children final {
private:
    Node *container_;
    std::vector<NodePtr > children_;

public:
    Children(Node *container);
    Children(const Children&) = delete;
    Children& operator=(const Children&) = delete;

    //----------------------------------------
    //           Read-only API
    //----------------------------------------
public:
    /** Number of nodes in vector. */
    size_t size() const {
        return children_.size();
    }

    /** Maximum potential size. */
    size_t max_size() const {
        return children_.max_size();
    }

    /** Size of allocated storage. */
    size_t capacity() const {
        return children_.capacity();
    }

    /** Empty predicate. */
    bool empty() const {
        return children_.empty();
    }

    /** Request change in capacity. */
    void reserve(size_t n) {
        children_.reserve(n);
    }

    /** Request container to reduce capacity. */
    void shrink_to_fit() {
        children_.shrink_to_fit();
    }

    /** Child pointer at index, checked. */
    const NodePtr at(size_t idx) const {
        ASSERT_require(idx < children_.size());
        return children_.at(idx);
    }

    /** Child pointer at index, unchecked. */
    const NodePtr operator[](size_t idx) const {
        ASSERT_require(idx < children_.size());
        return children_[idx];
    }

    /** First child pointer. */
    const NodePtr front() const {
        ASSERT_forbid(empty());
        return children_.front();
    }

    /** Last child pointer. */
    const NodePtr back() const {
        ASSERT_forbid(empty());
        return children_.back();
    }

    /** The actual underlying vector of child pointers. */
    const std::vector<NodePtr >& elmts() const {
        return children_;
    }

    /** Relations.
     *
     * @{ */
    bool operator==(const Children &other) const { return children_ == other.children_; }
    bool operator!=(const Children &other) const { return children_ != other.children_; }
    bool operator< (const Children &other) const { return children_ <  other.children_; }
    bool operator<=(const Children &other) const { return children_ <= other.children_; }
    bool operator> (const Children &other) const { return children_ >  other.children_; }
    bool operator>=(const Children &other) const { return children_ >= other.children_; }
    /** @} */

    //----------------------------------------
    //              Internal stuff
    //----------------------------------------
private:
    template<class T> friend class ListNode;
    template<class T> friend class ChildEdge;

    // Cause the indicated parent-child edge to point to a new child. The old value, if any will be removed from the tree and
    // its parent pointer reset. The new child will be added to the tree at the specified parent-to-child edge and its parent
    // pointer adjusted to point to the node that now owns it. As with direct assignment of pointers to the parent-to-child
    // edge data members, it is illegal to attach a node to a tree in such a way that the structure would no longer be a tree,
    // and in such cases no changes are made an an exception is thrown.
    void setAt(size_t idx, const NodePtr &newChild);
    void setAt(size_t idx, nullptr_t) {
        setAt(idx, NodePtr());
    }

    // Check that the newChild is able to be inserted replacing the oldChild. If not, throw exception. Either or both of the
    // children may be null pointers, but 'parent' must be non-null.
    void checkInsertionConsistency(const NodePtr &newChild, const NodePtr &oldChild, Node *parent);

    // Insert a new child edge at the specified position in the list. Consistency checks are performed and the child's parent
    // pointer is adjusted.
    void insertAt(size_t idx, const NodePtr &child);

    // Remove one of the child edges. If the edge pointed to a non-null child node, then that child's parent pointer is reset.
    void eraseAt(size_t idx);

    // Remove all children.
    void clear();

    // Add a new parent-child-edge and return its index
    size_t appendEdge(const NodePtr &child);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Node declaration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for @ref Tree nodes.
 *
 *  All nodes of a tree inherit from this type. The main features that this class provides are:
 *
 *  @li When declaring a tree node, the parent-child edges are declared using the @ref ChildEdge type as described in that
 *  type's documentation.
 *
 *  @li Every node has a @c children data member that enumerates the nodes pointed to by the @ref ChildEdge data members.
 *  This list of all children is updated automatically as assignments are made to the @ref ChildEdge data members.
 *
 *  @li Every node has a @c parent data member that points to the parent node in the tree, or null if this node is the
 *  root of a tree.  The parent pointers are updated automatically as nodes are inserted into and removed from the tree.
 *
 *  @li Node memory is managed using @c std::shared_ptr, and node memory is reclaimed automatically when there are no more
 *  references to the node, either as parent-child edges or other shared pointers outside the tree's control. The pointers
 *  from child to parent within the tree data structure itself are weak and do not count as references to the parent node.
 *
 *  @li a number of traversals are defined, the two most general being @ref traverse and @ref traverseParents. The other
 *  traversals are more specialized and are built upon the first two. */
class Node: public std::enable_shared_from_this<Node> {
private:
    enum TraversalDirection { TRAVERSE_UPWARD, TRAVERSE_DOWNWARD };

public:
    ParentEdge parent;                                  /**< Pointer to the parent node, if any. */
    Children children;                                  /**< Vector of pointers to children. */

public:
    /** Construct an empty node. */
    Node(): children(this) {}

    /** Nodes are polymorphic. */
    virtual ~Node() {}

    // Nodes are not copyable since doing so would cause the children (which are not copied) to have two parents. Instead, we
    // will provide mechanisms for copying nodes without their children (shallow copy) or recursively copying an entire tree
    // (deep copy).
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    /** Traverse the tree starting at this node and following child pointers.
     *
     *  The @p functor takes two arguments: the tree node under consideration, and a TraversalEvent that indicates whether the
     *  traversal is entering or leaving the node.  The functor must return a @ref TraversalAction to describe what should
     *  happen next.
     *
     *  If the functor called by the node enter event returns @ref SKIP_CHILDREN, then none of the children are traversed and
     *  the next call to the functor is for leaving that same node.
     *
     *  If any call to the functor returns ABORT, then the traversal is immediately aborted.
     *
     *  If any functor returns ABORT, then the @ref traverse function also returns ABORT. Otherwise the @ref traverse function
     *  returns CONTINUE.
     *
     * @{ */
    template<class Functor>
    TraversalAction traverse(Functor functor) {
        return traverseImpl<Functor>(TRAVERSE_DOWNWARD, functor);
    }
    template<class Functor>
    TraversalAction traverse(Functor functor) const {
        return traverseImpl<Functor>(TRAVERSE_DOWNWARD, functor);
    }
    /** @} */

    /** Traverse the tree restricted by type.
     *
     *  Traverses the entire tree, but calls the functor only for nodes of the specified type (or subtype).
     *
     * @{ */
    template<class T, class Functor>
    TraversalAction traverseType(Functor functor);
    template<class T, class Functor>
    TraversalAction traverseType(Functor functor) const;
    /** @} */

    /** Traverse the tree by following parent pointers.
     *
     *  Other than following pointers from children to parents, this traversal is identical to the downward @ref traverse
     *  method.
     *
     * @{ */
    template<class Functor>
    TraversalAction traverseParents(Functor functor) {
        return traverseImpl<Functor>(TRAVERSE_UPWARD, functor);
    }
    template<class Functor>
    TraversalAction traverseParents(Functor functor) const {
        return traverseImpl<Functor>(TRAVERSE_UPWARD, functor);
    }
    /** @} */

    /** Traverse an tree to find the first node satisfying the predicate.
     *
     * @{ */
    template<class Predicate>
    NodePtr find(Predicate predicate) {
        return findImpl<Node, Predicate>(TRAVERSE_DOWNWARD, predicate);
    }
    template<class Predicate>
    NodePtr find(Predicate predicate) const {
        return findImpl<Node, Predicate>(TRAVERSE_DOWNWARD, predicate);
    }
    /** @} */

    /** Find first child that's the specified type.
     *
     * @{ */
    template<class T>
    std::shared_ptr<T> findType() {
        return findImpl<T>(TRAVERSE_DOWNWARD, [](const std::shared_ptr<T>&) { return true; });
    }
    template<class T>
    std::shared_ptr<T> findType() const {
        return findImpl<T>(TRAVERSE_DOWNWARD, [](const std::shared_ptr<T>&) { return true; });
    }
    /** @} */

    /** Find first child of specified type satisfying the predicate.
     *
     * @{ */
    template<class T, class Predicate>
    std::shared_ptr<T> findType(Predicate predicate) {
        return findImpl<T, Predicate>(TRAVERSE_DOWNWARD, predicate);
    }
    template<class T, class Predicate>
    std::shared_ptr<T> findType(Predicate predicate) const {
        return findImpl<T, Predicate>(TRAVERSE_DOWNWARD, predicate);
    }
    /** @} */

    /** Find closest ancestor that satifies the predicate.
     *
     * @{ */
    template<class Predicate>
    NodePtr findParent(Predicate predicate) {
        return findImpl<Node, Predicate>(TRAVERSE_UPWARD, predicate);
    }
    template<class Predicate>
    NodePtr findParent(Predicate predicate) const {
        return findImpl<Node, Predicate>(TRAVERSE_UPWARD, predicate);
    }
    /** @} */

    /** Find closest ancestor of specified type.
     *
     * @{ */
    template<class T>
    std::shared_ptr<T> findParentType() {
        return findImpl<T>(TRAVERSE_UPWARD, [](const std::shared_ptr<T>&) { return true; });
    }
    template<class T>
    std::shared_ptr<T> findParentType() const {
        return findImpl<T>(TRAVERSE_UPWARD, [](const std::shared_ptr<T>&) { return true; });
    }
    /** @} */

    /** Find closest ancestor of specified type that satisfies the predicate.
     *
     * @{ */
    template<class T, class Predicate>
    std::shared_ptr<T> findParentType(Predicate predicate) {
        return findImpl<T, Predicate>(TRAVERSE_UPWARD, predicate);
    }
    template<class T, class Predicate>
    std::shared_ptr<T> findParentType(Predicate predicate) const {
        return findImpl<T, Predicate>(TRAVERSE_UPWARD, predicate);
    }
    /** @} */

private:
    // implementation for all the traversals
    template<class Functor>
    TraversalAction traverseImpl(TraversalDirection, Functor);
    template<class Functor>
    TraversalAction traverseImpl(TraversalDirection, Functor) const;

    // implementation for traversals whose purpose is to find something
    template<class T, class Predicate>
    std::shared_ptr<T> findImpl(TraversalDirection, Predicate);
    template<class T, class Predicate>
    std::shared_ptr<T> findImpl(TraversalDirection, Predicate) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ListNode declaration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A node containing only a list of children.
 *
 *  This class is used for nodes whose sole purpose is to hold a list of child nodes. Rather than having any dedicated data
 *  members, it accesses the @c children member directly in order to store the ordered list of child pointers. New classes
 *  cannot be derived from this class since doing so would enable the derived class to have additional ChildPtr data members
 *  that would interfere with the @c children list.
 *
 *  Although the @c children data member provides a read-only API for accessing the children, we also need to provde an API
 *  that can modify that list.  The entire @c children API is available also from this node directly so that the reading and
 *  writing APIs can be invoked consistently on this object.
 *
 *  A parent node that points to a node containing a list as well as nodes that are not lists is declared as follows:
 *
 * @code
 *  class ChildType1;    // some user-defined type derived from Tree::Node
 *  class ChildType2;    // ditto
 *  class ChildType3;    // ditto
 *
 *  class Parent: public Tree::Node {
 *  public:
 *      Tree::ChildEdge<ChildType1> first;                // a.k.a., children[0]
 *      Tree::ChildEdge<Tree::ListNode<ChildType2> list;  // a.k.a., children[1]
 *      Tree::ChildEdge<ChildType3> last;                 // a.k.a., children[2] regardless of list's size
 *
 *      Parent()
 *          : first(this), list(this), last(this) {}
 *  }
 * @endcode
 *
 *  A common practice when creating a @ref ListNode is to allocate then node when the parent is constructed:
 *
 * @code
 *  Parent::Parent()
 *      : first(this), list(this, std::make_shared<Tree::ListNode<ChildType2> >()), last(this) {}
 * @endcode
 *
 *  If you follow the recommendation of always allocating @ref ListNode data members, then the 10th child (index 9) of the
 *  parent node's list can be accessed without worrying about whether <tt>parent->list</tt> is a null pointer:
 *
 * @code
 *  std::shared_ptr<Parent> parent = ...;
 *  std::shared_ptr<ChildType2> item = parent->list->at(9);
 * @endcode
 *
 *  Since a @ref ListNode is a type of @ref Node, it has a @c children data member of type @ref Children. All the functions
 *  defined for @ref Children are also defined in @ref ListNode itself, plus @ref ListNode has a number of additional member
 *  functions for inserting and removing children--something that's not possible with other @ref Node types.
 *
 *  The @ref ListNode type is final because if users could derive subclasses from it, then those subclasses could add
 *  @ref ChildEdge data members that would interfere with the child node counting. */
template<class T>
class ListNode final: public Node {
public:
    //----------------------------------------
    // Read-only API delegated to 'children'
    //----------------------------------------

    /** Number of children. */
    size_t size() const {
        return children.size();
    }

    /** Maximum size. */
    size_t max_size() const {
        return children.max_size();
    }

    /** Capacity. */
    size_t capacity() const {
        return children.capacity();
    }

    /** Empty predicate. */
    bool empty() const {
        return children.empty();
    }

    /** Reserve space for more children. */
    void reserve(size_t n) {
        children.reserve(n);
    }

    /** Shrink reservation. */
    void shrink_to_fit() {
        children.shrink_to_fit();
    }

    /** Child at specified index. */
    const std::shared_ptr<T> at(size_t i) const {
        return std::dynamic_pointer_cast<T>(children.at(i));
    }

    /** Child at specified index. */
    const std::shared_ptr<T> operator[](size_t i) const {
        return std::dynamic_pointer_cast<T>(children[i]);
    }

    /** First child, if any. */
    const std::shared_ptr<T> front() const {
        return std::dynamic_pointer_cast<T>(children.front());
    }

    /** Last child, if any. */
    const std::shared_ptr<T> back() const {
        return std::dynamic_pointer_cast<T>(children.back());
    }

    /** Vector of all children. */
    std::vector<std::shared_ptr<T> > elmts() const;

    /** Find the index for the specified node.
     *
     *  Finds the index for the first child at or after @p startAt and returns its index. Returns nothing if the specified
     *  node is not found. */
    Optional<size_t> index(const std::shared_ptr<T> &node, size_t startAt = 0) const;

    //----------------------------------------
    //              Modifying API
    //----------------------------------------

    /** Remove all children. */
    void clear() {
        children.clear();
    }
    
    /** Append a child pointer. */
    void push_back(const std::shared_ptr<T> &newChild) {
        children.insertAt(children.size(), newChild);
    }

    /** Make child edge point to a different child.
     *
     * @{ */
    void setAt(size_t i, const std::shared_ptr<T> &child) {
        children.setAt(i, child);
    }
    void setAt(size_t i, nullptr_t) {
        children.setAt(i, nullptr);
    }
    /** @} */


    /** Insert the node at the specified index.
     *
     *  The node must not already have a parent. The index must be greater than or equal to zero and less than or equal to the
     *  current number of nodes. Upon return, the node that was inserted will be found at index @p i. */
    void insertAt(size_t i, const std::shared_ptr<T> &newChild) {
        children.insertAt(i, newChild);
    }

    /** Erase node at specified index.
     *
     *  If the index is out of range then nothing happens. */
    void eraseAt(size_t i) {
        children.eraseAt(i);
    }

    /** Erase the first occurrence of the specified child.
     *
     *  Erases the first occurrence of the specified child at or after the starting index.
     *
     *  If a child was erased, then return the index of the erased child. */
    Optional<size_t> erase(const std::shared_ptr<T> &toErase, size_t startAt = 0);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                        ____      _       _   _
//                       |  _ \ ___| | __ _| |_(_) ___  _ __  ___
//                       | |_) / _ \ |/ _` | __| |/ _ \| '_ \/ __|
//                       |  _ <  __/ | (_| | |_| | (_) | | | \__ |
//                       |_| \_\___|_|\__,_|\__|_|\___/|_| |_|___/
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ChildEdge<T> and ChildEdge<U>
template<class T, class U> bool operator==(const ChildEdge<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs.shared() == rhs.shared(); }
template<class T, class U> bool operator!=(const ChildEdge<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs.shared() != rhs.shared(); }
template<class T, class U> bool operator< (const ChildEdge<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs.shared() <  rhs.shared(); }
template<class T, class U> bool operator<=(const ChildEdge<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs.shared() <= rhs.shared(); }
template<class T, class U> bool operator> (const ChildEdge<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs.shared() >  rhs.shared(); }
template<class T, class U> bool operator>=(const ChildEdge<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs.shared() >= rhs.shared(); }

// ChildEdge<T> and nullptr
template<class T> bool operator==(const ChildEdge<T> &lhs, nullptr_t) noexcept { return lhs.shared() == nullptr; }
template<class T> bool operator!=(const ChildEdge<T> &lhs, nullptr_t) noexcept { return lhs.shared() != nullptr; }
template<class T> bool operator< (const ChildEdge<T> &lhs, nullptr_t) noexcept { return lhs.shared() <  nullptr; }
template<class T> bool operator<=(const ChildEdge<T> &lhs, nullptr_t) noexcept { return lhs.shared() <= nullptr; }
template<class T> bool operator> (const ChildEdge<T> &lhs, nullptr_t) noexcept { return lhs.shared() >  nullptr; }
template<class T> bool operator>=(const ChildEdge<T> &lhs, nullptr_t) noexcept { return lhs.shared() >= nullptr; }

// nullptr and ChildEdge<T>
template<class T> bool operator==(nullptr_t, const ChildEdge<T> &rhs) noexcept { return nullptr == rhs.shared(); }
template<class T> bool operator!=(nullptr_t, const ChildEdge<T> &rhs) noexcept { return nullptr != rhs.shared(); }
template<class T> bool operator< (nullptr_t, const ChildEdge<T> &rhs) noexcept { return nullptr <  rhs.shared(); }
template<class T> bool operator<=(nullptr_t, const ChildEdge<T> &rhs) noexcept { return nullptr <= rhs.shared(); }
template<class T> bool operator> (nullptr_t, const ChildEdge<T> &rhs) noexcept { return nullptr >  rhs.shared(); }
template<class T> bool operator>=(nullptr_t, const ChildEdge<T> &rhs) noexcept { return nullptr >= rhs.shared(); }

// ChildEdge<T> and std::shared_ptr<U>
template<class T, class U> bool operator==(const ChildEdge<T> &lhs, const std::shared_ptr<U> &rhs) noexcept { return lhs.shared() == rhs; }
template<class T, class U> bool operator!=(const ChildEdge<T> &lhs, const std::shared_ptr<U> &rhs) noexcept { return lhs.shared() != rhs; }
template<class T, class U> bool operator< (const ChildEdge<T> &lhs, const std::shared_ptr<U> &rhs) noexcept { return lhs.shared() <  rhs; }
template<class T, class U> bool operator<=(const ChildEdge<T> &lhs, const std::shared_ptr<U> &rhs) noexcept { return lhs.shared() <= rhs; }
template<class T, class U> bool operator> (const ChildEdge<T> &lhs, const std::shared_ptr<U> &rhs) noexcept { return lhs.shared() >  rhs; }
template<class T, class U> bool operator>=(const ChildEdge<T> &lhs, const std::shared_ptr<U> &rhs) noexcept { return lhs.shared() >= rhs; }

// std::shared_ptr<T> and ChildEdge<U>
template<class T, class U> bool operator==(const std::shared_ptr<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs == rhs.shared(); }
template<class T, class U> bool operator!=(const std::shared_ptr<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs != rhs.shared(); }
template<class T, class U> bool operator< (const std::shared_ptr<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs <  rhs.shared(); }
template<class T, class U> bool operator<=(const std::shared_ptr<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs <= rhs.shared(); }
template<class T, class U> bool operator> (const std::shared_ptr<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs >  rhs.shared(); }
template<class T, class U> bool operator>=(const std::shared_ptr<T> &lhs, const ChildEdge<U> &rhs) noexcept { return lhs >= rhs.shared(); }

// ParentEdge and nullptr
inline bool operator==(const ParentEdge &lhs, nullptr_t) noexcept { return lhs.shared() == nullptr; }
inline bool operator!=(const ParentEdge &lhs, nullptr_t) noexcept { return lhs.shared() != nullptr; }
inline bool operator< (const ParentEdge &lhs, nullptr_t) noexcept { return lhs.shared() <  nullptr; }
inline bool operator<=(const ParentEdge &lhs, nullptr_t) noexcept { return lhs.shared() <= nullptr; }
inline bool operator> (const ParentEdge &lhs, nullptr_t) noexcept { return lhs.shared() >  nullptr; }
inline bool operator>=(const ParentEdge &lhs, nullptr_t) noexcept { return lhs.shared() >= nullptr; }

// nullptr and ParentEdge
inline bool operator==(nullptr_t, const ParentEdge &rhs) noexcept { return nullptr == rhs.shared(); }
inline bool operator!=(nullptr_t, const ParentEdge &rhs) noexcept { return nullptr != rhs.shared(); }
inline bool operator< (nullptr_t, const ParentEdge &rhs) noexcept { return nullptr <  rhs.shared(); }
inline bool operator<=(nullptr_t, const ParentEdge &rhs) noexcept { return nullptr <= rhs.shared(); }
inline bool operator> (nullptr_t, const ParentEdge &rhs) noexcept { return nullptr >  rhs.shared(); }
inline bool operator>=(nullptr_t, const ParentEdge &rhs) noexcept { return nullptr >= rhs.shared(); }

// ParentEdge and std::shared_ptr<T>
template<class T> bool operator==(const ParentEdge &lhs, const std::shared_ptr<T> &rhs) noexcept { return lhs.shared() == rhs; }
template<class T> bool operator!=(const ParentEdge &lhs, const std::shared_ptr<T> &rhs) noexcept { return lhs.shared() != rhs; }
template<class T> bool operator< (const ParentEdge &lhs, const std::shared_ptr<T> &rhs) noexcept { return lhs.shared() <  rhs; }
template<class T> bool operator<=(const ParentEdge &lhs, const std::shared_ptr<T> &rhs) noexcept { return lhs.shared() <= rhs; }
template<class T> bool operator> (const ParentEdge &lhs, const std::shared_ptr<T> &rhs) noexcept { return lhs.shared() >  rhs; }
template<class T> bool operator>=(const ParentEdge &lhs, const std::shared_ptr<T> &rhs) noexcept { return lhs.shared() >= rhs; }

// std::shared_ptr<T> and ParentEdge
template<class T> bool operator==(const std::shared_ptr<T> &lhs, const ParentEdge &rhs) noexcept { return lhs == rhs.shared(); }
template<class T> bool operator!=(const std::shared_ptr<T> &lhs, const ParentEdge &rhs) noexcept { return lhs != rhs.shared(); }
template<class T> bool operator< (const std::shared_ptr<T> &lhs, const ParentEdge &rhs) noexcept { return lhs <  rhs.shared(); }
template<class T> bool operator<=(const std::shared_ptr<T> &lhs, const ParentEdge &rhs) noexcept { return lhs <= rhs.shared(); }
template<class T> bool operator> (const std::shared_ptr<T> &lhs, const ParentEdge &rhs) noexcept { return lhs >  rhs.shared(); }
template<class T> bool operator>=(const std::shared_ptr<T> &lhs, const ParentEdge &rhs) noexcept { return lhs >= rhs.shared(); }

// ParentEdge and ChildEdge<T>
template<class T> bool operator==(const ParentEdge &lhs, const ChildEdge<T> &rhs) noexcept { return lhs.shared() == rhs.shared(); }
template<class T> bool operator!=(const ParentEdge &lhs, const ChildEdge<T> &rhs) noexcept { return lhs.shared() != rhs.shared(); }
template<class T> bool operator< (const ParentEdge &lhs, const ChildEdge<T> &rhs) noexcept { return lhs.shared() <  rhs.shared(); }
template<class T> bool operator<=(const ParentEdge &lhs, const ChildEdge<T> &rhs) noexcept { return lhs.shared() <= rhs.shared(); }
template<class T> bool operator> (const ParentEdge &lhs, const ChildEdge<T> &rhs) noexcept { return lhs.shared() >  rhs.shared(); }
template<class T> bool operator>=(const ParentEdge &lhs, const ChildEdge<T> &rhs) noexcept { return lhs.shared() >= rhs.shared(); }

// ChildEdge<T> and ParentEdge
template<class T> bool operator==(const ChildEdge<T> &lhs, const ParentEdge &rhs) noexcept { return lhs.shared() == rhs.shared(); }
template<class T> bool operator!=(const ChildEdge<T> &lhs, const ParentEdge &rhs) noexcept { return lhs.shared() != rhs.shared(); }
template<class T> bool operator< (const ChildEdge<T> &lhs, const ParentEdge &rhs) noexcept { return lhs.shared() <  rhs.shared(); }
template<class T> bool operator<=(const ChildEdge<T> &lhs, const ParentEdge &rhs) noexcept { return lhs.shared() <= rhs.shared(); }
template<class T> bool operator> (const ChildEdge<T> &lhs, const ParentEdge &rhs) noexcept { return lhs.shared() >  rhs.shared(); }
template<class T> bool operator>=(const ChildEdge<T> &lhs, const ParentEdge &rhs) noexcept { return lhs.shared() >= rhs.shared(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                        ___                 _                           _        _   _
//                       |_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  ___
//                        | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \/ __|
//                        | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | \__ |
//                       |___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|___/
//                                     |_|
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ChildEdge implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
ChildEdge<T>::ChildEdge(Node *container)
    : container_(container), idx_(container->children.appendEdge(nullptr)) {
    ASSERT_not_null(container_);
}

template<class T>
ChildEdge<T>::ChildEdge(Node *container, const std::shared_ptr<T> &child)
    : container_(container), idx_(container->children.appendEdge(child)) {
    ASSERT_not_null(container_);
}

template<class T>
std::shared_ptr<T>
ChildEdge<T>::shared() const {
    return std::dynamic_pointer_cast<T>(container_->children[this->idx_]);
}

template<class T>
void
ChildEdge<T>::assign(const NodePtr &newChild) {
    container_->children.setAt(idx_, newChild);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ParentEdge implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NodePtr
ParentEdge::shared() const {
    return parent_ ? parent_->shared_from_this() : NodePtr();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Children implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Children::Children(Node *container)
    : container_(container) {
    ASSERT_not_null(container);
}

size_t
Children::appendEdge(const NodePtr &child) {
    size_t idx = children_.size();
    children_.push_back(nullptr);
    setAt(idx, child);
    return idx;
}

void
Children::checkInsertionConsistency(const NodePtr &newChild, const NodePtr &oldChild, Node *parent) {
    ASSERT_not_null(parent);

    if (newChild && newChild != oldChild) {
        if (newChild->parent != nullptr) {
            if (newChild->parent.shared().get() == parent) {
                throw ConsistencyException(newChild, "node is already a child of the parent");
            } else {
                throw ConsistencyException(newChild, "node is already attached to a tree");
            }
        }

        for (Node *ancestor = parent; ancestor; ancestor = ancestor->parent.parent_) {
            if (newChild.get() == ancestor)
                throw ConsistencyException(newChild, "node insertion would introduce a cycle");
        }
    }
}

void
Children::setAt(size_t idx, const NodePtr &newChild) {
    ASSERT_require(idx < children_.size());
    NodePtr oldChild = children_[idx];
    checkInsertionConsistency(newChild, oldChild, container_);
    if (oldChild)
        oldChild->parent.reset();
    children_[idx] = newChild;
    if (newChild)
        newChild->parent.set(container_);
}

void
Children::insertAt(size_t idx, const NodePtr &child) {
    ASSERT_require(idx <= children_.size());
    checkInsertionConsistency(child, nullptr, container_);
    children_.insert(children_.begin() + idx, child);
    if (child)
        child->parent.set(container_);
}

void
Children::eraseAt(size_t idx) {
    ASSERT_require(idx < children_.size());
    if (children_[idx])
        children_[idx]->parent.reset();
    children_.erase(children_.begin() + idx);
}

void
Children::clear() {
    while (!children_.empty()) {
        if (children_.back())
            children_.back()->parent.reset();
        children_.pop_back();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Node implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------
// traverseImpl
//----------------------------------------
template<class Functor>
TraversalAction
Node::traverseImpl(TraversalDirection direction, Functor functor) {
    switch (TraversalAction action = functor(this->shared_from_this(), ENTER)) {
        case CONTINUE:
            if (TRAVERSE_DOWNWARD == direction) {
                for (size_t i=0; i<children.size() && CONTINUE==action; ++i) {
                    if (children[i])
                        action = children[i]->traverseImpl<Functor>(direction, functor);
                }
            } else if (parent) {
                action = parent->traverseImpl<Functor>(direction, functor);
            }
            // fall through
        case SKIP_CHILDREN:
            if (ABORT != action && (action = functor(this->shared_from_this(), LEAVE)) == SKIP_CHILDREN)
                action = CONTINUE;
            // fall through
        default:
            return action;
    }
}

template<class Functor>
TraversalAction
Node::traverseImpl(TraversalDirection direction, Functor functor) const {
    switch (TraversalAction action = functor(this->shared_from_this(), ENTER)) {
        case CONTINUE:
            if (TRAVERSE_DOWNWARD == direction) {
                for (size_t i=0; i<children.size() && CONTINUE==action; ++i) {
                    if (children[i])
                        action = children[i]->traverseImpl<Functor>(direction, functor);
                }
            } else if (parent) {
                action = parent->traverseImpl<Functor>(direction, functor);
            }
            // fall through
        case SKIP_CHILDREN:
            if (ABORT != action && (action = functor(this->shared_from_this(), LEAVE)) == SKIP_CHILDREN)
                action = CONTINUE;
            // fall through
        default:
            return action;
    }
}

//----------------------------------------
// traverseType
//----------------------------------------

template<class T, class Functor>
struct TraverseTypeHelper {
    Functor functor;

    TraverseTypeHelper(Functor functor)
        : functor(functor) {}

    TraversalAction operator()(const NodePtr &node, TraversalEvent event) {
        if (std::shared_ptr<T> typed = std::dynamic_pointer_cast<T>(node)) {
            return functor(typed, event);
        } else {
            return CONTINUE;
        }
    }
};

template<class T, class Functor>
TraversalAction
Node::traverseType(Functor functor) {
    return traverseImpl(TRAVERSE_DOWNWARD, TraverseTypeHelper<T, Functor>(functor));
}

template<class T, class Functor>
TraversalAction
Node::traverseType(Functor functor) const {
    return traverseImpl(TRAVERSE_DOWNWARD, TraverseTypeHelper<T, Functor>(functor));
}

//----------------------------------------
// findImpl
//----------------------------------------
template<class T, class Predicate>
std::shared_ptr<T>
Node::findImpl(TraversalDirection direction, Predicate predicate) {
    std::shared_ptr<T> found;
    traverseImpl(direction,
                 [&predicate, &found](const NodePtr &node, TraversalEvent event) {
                     if (ENTER == event) {
                         std::shared_ptr<T> typedNode = std::dynamic_pointer_cast<T>(node);
                         if (typedNode && predicate(typedNode)) {
                             found = typedNode;
                             return ABORT;
                         }
                     }
                     return CONTINUE;
                 });
    return found;
}

template<class T, class Predicate>
std::shared_ptr<T>
Node::findImpl(TraversalDirection direction, Predicate predicate) const {
    std::shared_ptr<T> found;
    traverseImpl(direction,
                 [&predicate, &found](const ConstNodePtr &node, TraversalEvent event) {
                     if (ENTER == event) {
                         std::shared_ptr<const T> typedNode = std::dynamic_pointer_cast<const T>(node);
                         if (typedNode && predicate(typedNode)) {
                             found = typedNode;
                             return ABORT;
                         }
                     }
                     return CONTINUE;
                 });
    return found;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ListNode implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
std::vector<std::shared_ptr<T> >
ListNode<T>::elmts() const {
    std::vector<std::shared_ptr<T> > retval;
    retval.reserve(children.size());
    for (size_t i = 0; i < children.size(); ++i)
        retval.push_back(std::dynamic_pointer_cast<T>(children[i]));
    return retval;
}

template<class T>
Optional<size_t>
ListNode<T>::index(const std::shared_ptr<T> &node, size_t startAt) const {
    for (size_t i = startAt; i < children.size(); ++i) {
        if (children[i] == node)
            return i;
    }
    return Nothing();
}

template<class T>
Optional<size_t>
ListNode<T>::erase(const std::shared_ptr<T> &toErase, size_t startAt) {
    Optional<size_t> where = index(toErase, startAt);
    if (where)
        children.eraseAt(*where);
    return where;
}

} // namespace
} // namespace

#endif
