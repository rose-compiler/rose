#ifndef Tree_H
#define Tree_H

#include <Sawyer/Assert.h>

#include <boost/lexical_cast.hpp>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class TreeNode;
using TreeNodePtr = std::shared_ptr<TreeNode>;
class TreeParentEdge;
template<class T> class TreeEdge;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Error types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for errors related to the AST. */
class TreeError: public std::runtime_error {
public:
    /** Node that caused the error. */
    TreeNodePtr node;

    /** Construct a new error with the specified message and causing node. */
    TreeError(const std::string &mesg, const TreeNodePtr&);
};

/** Error when attaching a node to a tree and the node is already attached somewhere else.
 *
 *  If the operation were allowed to continue without throwing an exception, the AST would no longer be a tree. */
class TreeAttachmentError: public TreeError {
public:
    /** Construct a new error with the node that caused the error. */
    explicit TreeAttachmentError(const TreeNodePtr&);
};

/** Error when attaching a node to a tree would cause a cycle.
 *
 *  If the operation were allowed to continue without throwing an exception, the AST would no longer be a tree. */
class TreeCycleError: public TreeError {
public:
    /** Construct a new error with the node that caused the error. */
    explicit TreeCycleError(const TreeNodePtr&);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TreeParentEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Internal. The only purpose of this class is so that the TreeEdge has permission to change the parent pointer in an
// TreeParentEdge.
class TreeParentEdgeAccess {
protected:
    void resetParent(TreeParentEdge&);
    void setParent(TreeParentEdge&, TreeNode&);
};

/** Points from a child to a parent in the AST.
 *
 *  This is the pointer type that points from a node to its parent. Its value is adjusted automatically when the containing
 *  node is attached or detached as a child of some other node. The term "edge" is used instead of "pointer" because the
 *  relationship between child and parent is bidirectional.
 *
 *  A parent edge is always a data member of a node and never instantiated in other circumstances. Thus users don't normally
 *  instantiate these directly, but they do interact with them through the @c parent data member that exists for all node types. */
class TreeParentEdge {
    TreeNode &child_;                                   // required child node owning this edge

    // The parent pointer is a raw pointer because it is safe to do so, and because we need to know the pointer before the parent is
    // fully constructed.
    //
    // It is safe (never dangling) because the pointer can only be changed by an TreeEdge, the TreeEdge is always a member of a
    // TreeNode, and the parent pointer is only set to point to that node. When the parent is deleted the TreeEdge is deleted and
    // its destructor changes the parent pointer back to null.
    //
    // The parent pointer is needed during construction of the parent when the parent has some TreeEdge data members that are being
    // initialized to point to non-null children. This happens during the parent's construction, before the parent has any shared or
    // weak pointers.
    TreeNode *parent_ = nullptr;                        // optional parent to which this edge points

public:
    // No default constructor and not copyable.
    TreeParentEdge() = delete;
    explicit TreeParentEdge(const TreeParentEdge&) = delete;
    TreeParentEdge& operator=(const TreeParentEdge&) = delete;

public:
    ~TreeParentEdge();                                  // internal use only
    explicit TreeParentEdge(TreeNode &child);           // internal use only

public:
    /** Return the parent if there is one, else null.
     *
     * @{ */
    TreeNodePtr operator()() const;
    TreeNodePtr operator->() const;
    /** @} */

    /** Compare the parent pointer to another pointer.
     *
     * @{ */
    bool operator==(const TreeNodePtr&) const;
    bool operator!=(const TreeNodePtr&) const;
    bool operator==(const TreeParentEdge&) const;
    bool operator!=(const TreeParentEdge&) const;
    template<class T> bool operator==(const TreeEdge<T>&) const;
    template<class T> bool operator!=(const TreeEdge<T>&) const;
    /** @} */

    /** True if parent is not null. */
    explicit operator bool() const {
        return parent_ != nullptr;
    }

private:
    // Used internally through TreeParentEdgeAccess when a TreeEdge<T> adjusts the TreeParentEdge
    friend class TreeParentEdgeAccess;
    void reset();
    void set(TreeNode&);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TreeEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A parent-to-child edge in a tree.
 *
 *  A parent-to-child edge is a pointer-like object that points from an parent node to a child node or nullptr. It is also
 *  responsible for adjusting the child's parent pointer. The term "edge" is used instead of "pointer" because the relationship
 *  between the parent and child is bidirectional.
 *
 *  A child edge is always a data member of a node and never instantiated in other circumstances. Thus users don't normally
 *  instanticate these directly, but they do interact with them to obtain pointers to children from a parent.
 *
 *  This type is used to define a data member in the parent that points to a child. For instance, the following binary expression
 *  node has left-hand-side and right-hand-side children that are part of the tree.
 *
 * @code
 *  class BinaryExpression: public Expression {
 *  public:
 *      TreeEdge<Expression> lhs;
 *      TreeEdge<Expression> rhs;
 *
 *  protected:
 *      BinaryExpression()
 *          : lhs(*this), rhs(*this) {}
 *
 *      static std::shared_ptr<BinaryExpression> instance() {
 *          return std::shared_ptr<BinaryExpression>(new BinaryExpression);
 *      }
 *  };
 * @endcode */
template<class T>
class TreeEdge: protected TreeParentEdgeAccess {
public:
    /** Type of child being pointed to. */
    using Child = T;

    /** Type of pointer to the child. */
    using ChildPtr = std::shared_ptr<T>;

private:
    TreeNode &parent_;                                  // required parent owning this child edge
    ChildPtr child_;                                    // optional child to which this edge points

public:
    // No default constructor and not copyable.
    TreeEdge() = delete;
    TreeEdge(const TreeEdge&) = delete;
    TreeEdge& operator=(const TreeEdge&) = delete;

public:
    ~TreeEdge();

    /** Construct a child edge that belongs to the specified parent.
     *
     *  When constructing a class containing a data member of this type (i.e., a tree edge that points to a child of this node), the
     *  data member must be initialized by passing @c *this as the argument.  See the example in this class documentation.
     *
     *  An optional second argument initializes the child pointer for the edge. The initialization is the same as if the child
     *  had been assigned with @c operator= later. I.e., the child must not already have a parent.
     *
     * @{ */
    explicit TreeEdge(TreeNode &parent);
    TreeEdge(TreeNode &parent, const ChildPtr &child);
    /** @} */

    /** Return the child if there is one, else null.
     *
     * @{ */
    const ChildPtr& operator->() const;
    const ChildPtr& operator()() const;
    /** @} */

    /** Compare the child pointer to another pointer.
     *
     * @{ */
    bool operator==(const std::shared_ptr<TreeNode>&) const;
    bool operator!=(const std::shared_ptr<TreeNode>&) const;
    bool operator==(const TreeParentEdge&) const;
    bool operator!=(const TreeParentEdge&) const;
    template<class U> bool operator==(const TreeEdge<U>&) const;
    template<class U> bool operator!=(const TreeEdge<U>&) const;
    /** @} */

    /** Assign a pointer to a child.
     *
     *  If this edge points to an old child then that child is removed and its parent is reset. If the specified new child is
     *  non-null, then it is inserted and its parent pointer set to the parent of this edge.
     *
     *  However, if the new child already has a non-null parent, then no changes are made and a @ref TreeAttachmentError is thrown
     *  with the error's node point to the new child. Otherwise, if the new child is non-null and is the parent or any more distant
     *  ancestor of this edge's node, then a @ref TreeCycleError is thrown. Cycle errors are only thrown if debugging is enabled
     *  (i.e., the CPP macro @c NDEBUG is undefined).
     *
     *  Attempting to assign one child edge object to another is a compile-time error (its operator= is not declared) because every
     *  non-null child edge points to a child whose parent is non-null, which would trigger an @ref TreeAttachmentError. Therefore
     *  only null child edges could be assigned. But since only null child edges can be assigned, its more concise and clear to
     *  assign the null pointer directly.
     *
     * @{ */
    TreeEdge& operator=(const ChildPtr &child);
    TreeEdge& operator=(const TreeParentEdge&);
    /** @} */

    /** True if child is not null. */
    explicit operator bool() const {
        return child_ != nullptr;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TreeNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for tree data structures. */
class TreeNode: public std::enable_shared_from_this<TreeNode> {
public:
    /** Shared-ownership pointer to a @ref TreeNode. */
    using Ptr = TreeNodePtr;

    /** Traversal direction. */
    enum class Traversal {
        ENTER,                                          /**< Pre-order visitation. */
        LEAVE                                           /**< Post-order visitation. */
    };

protected:
    /** Information about a child. */
    struct TreeChildDescriptor {
        size_t i;                                       /**< Index of the child counted across all inherited child edges. */
        std::string name;                               /**< Property name of the child. */
        Ptr value;                                      /**< Child pointer value. */
    };

public:
    /** Pointer to the parent in the tree.
     *
     *  A node's parent pointer is adjusted automatically when the node is inserted or removed as a child of another node. An
     *  invariant of this design is that whenever node A is a child of node B, then node B is a parent of node A. */
    TreeParentEdge parent;

public:
    virtual ~TreeNode() {}
protected:
    virtual void destructorHelper() {}

protected:
    TreeNode();

public:
    /** Returns a shared pointer to this node. */
    Ptr pointer();

    /** Traverse upward following parent pointers.
     *
     *  The visitor is called for each node from the current node until the root of the tree is reached unless the visitor indicates
     *  that the traversal should end. It does so by returning a value that is true in a Boolean context, and this value becomes the
     *  return value for the entire traversal. */
    template<class Visitor>
    auto visitParents(const Visitor &visitor) {
        for (auto node = pointer(); node; node = node->parent()) {
            if (auto result = visitor(node))
                return result;
        }
        return decltype(visitor(TreeNodePtr()))();
    }

    /** Traverse downward in the tree.
     *
     *  Perform a depth-first traversal of the tree starting with this vertex. The visitor functor is called twice for each vertex,
     *  first on the way down and then on the way back up. The functor takes two arguments: the node being visited and an enum
     *  indicating whether the visit is on the way down (@ref Traverse::ENTER) or the way up (@ref Traverse::LEAVE). The traversal
     *  has the same return type as the functor. If the functor returns a value which evaluates to true in Boolean context, then the
     *  traversal immediately returns that value, otherwise it continues until the entire subtree is visited and returns a
     *  default-constructed value. */
    template<class Visitor>
    auto visitChildren(const Visitor &visitor) {
        if (auto retval = visitor(this->pointer(), Traversal::ENTER))
            return retval;
        for (size_t i = 0; true; ++i) {
            const TreeChildDescriptor child = findChild(i);
            if (child.i < i) {
                break;
            } else if (child.value) {
                if (auto retval = child.value->visitChildren(visitor))
                    return retval;
            }
        }
        return visitor(this->pointer(), Traversal::LEAVE);
    }

    /** Traversal that finds an ancestor of a particular type. */
    template<class T>
    std::shared_ptr<T> findAncestor() {
        return visitParents([](const TreeNodePtr &node) -> std::shared_ptr<T> {
                return std::dynamic_pointer_cast<T>(node);
            });
    };

    /** Traversal that finds all the descendants of a particular type.
     *
     *  Note that this is probably not the way you want to do this because it's expensive to create the list of all matching
     *  pointers. Instead, you probably want to call @ref visitChildren and handle the nodes inside the functor. */
    template<class T>
    std::vector<std::shared_ptr<T>> findDescendants() {
        std::vector<std::shared_ptr<T>> retval;
        visitChildren([&retval](const TreeNodePtr &node, Traversal event) {
            if (Traversal::ENTER == event) {
                if (auto child = std::dynamic_pointer_cast<T>(node))
                    retval.push_back(child);
            }
        });
        return retval;
    }

    /** Returns the property name for a child.
     *
     *  Returns the property name for the specified child index. If @p i is out of range, then an empty string is returned. */
    std::string childName(size_t i) {
        return findChild(i).name;
    }

    /** Returns the pointer for a child.
     *
     *  Returns the pointer for the specified child index. If @p i is out of range, then a null pointer is returned, which is
     *  indistinguishable from the case when a valid index is specified but that child is a null pointer. */
    Ptr child(size_t i) {
        return findChild(i).value;
    }

    /** Returns the number of children.
     *
     *  This is the number of children for this class and the base class, recursively. Some children may be null pointers. */
    size_t nChildren(size_t i) {
        return findChild(std::numeric_limits<size_t>::max()).i;
    }

protected:
    /** Finds information about an indexed child.
     *
     *  The index, @p i, specifies the child about which information is returned. Children are numbered recursively in base classes
     *  followed by the current class. This function is re-implemented in every derived class that has children.
     *
     *  A @ref TreeChildDescriptor is returned for every query. If the index is out of range for the class, then the return value is
     *  for a child one past the end. I.e., the index is equal to the number of children, the name is empty, and the value is a null
     *  pointer. */
    virtual TreeChildDescriptor findChild(size_t i) const {
        return TreeChildDescriptor{0, "", nullptr};
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TreeListNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Node that points to an ordered sequence of indexable children.
 *
 *  This node acts like an @c std::vector except that inserting and erasing children also adjusts the child's parent pointer. */
template<class T>
class TreeListNode: public TreeNode {
private:
    using EdgeVector = std::vector<std::unique_ptr<TreeEdge<T>>>;

public:
    /** Shared-ownership pointer to nodes of this type. */
    using Ptr = std::shared_ptr<TreeListNode>;

    /** Type of child. */
    using Child = T;

    /** Type of child pointer. */
    using ChildPtr = std::shared_ptr<T>;

    // These are for compatibility with std::vector
    using value_type = TreeEdge<T>;                               /**< Type of values stored in this class. */
    using size_type = typename EdgeVector::size_type;             /**< Size type. */
    using difference_type = typename EdgeVector::difference_type; /**< Distance between elements. */
    using reference = value_type&;                                /**< Reference to value. */
    using const_reference = const value_type&;                    /**< Reference to cons value. */
    using pointer = value_type*;                                  /**< Pointer to value. */
    using const_pointer = const value_type*;                      /**< Pointer to const value. */

private:
    EdgeVector elmts_;

public:
    /** Random access iterator to non-const edges.
     *
     *  Iterators are invalidated in the same situations as for @c std::vector. */
    class iterator {
        friend class TreeListNode;
        typename EdgeVector::iterator base_;
        iterator() = delete;
        iterator(typename EdgeVector::iterator base)
            : base_(base) {}

    public:
        /** Cause iterator to point to the next edge.
         *
         * @{ */
        iterator& operator++() {
            ++base_;
            return *this;
        }
        iterator operator++(int) {
            auto temp = *this;
            ++base_;
            return temp;
        }
        /** @} */

        /** Cause iterator to point to previous edge.
         *
         * @{ */
        iterator& operator--() {
            --base_;
            return *this;
        }
        iterator operator--(int) {
            auto temp = *this;
            --base_;
            return temp;
        }
        /** @} */

        /** Advance iterator in forward (or backward if negative) direction by @p n edges.
         *
         * @{ */
        iterator& operator+=(difference_type n) {
            base_ += n;
            return *this;
        }
        iterator operator+(difference_type n) const {
            iterator retval = *this;
            retval += n;
            return retval;
        }
        /** @} */

        /** Advance iterator in backward (or forward if negative) direction by @p n edges.
         *
         * @{ */
        iterator& operator-=(difference_type n) {
            base_ -= n;
            return *this;
        }
        iterator operator-(difference_type n) const {
            iterator retval = *this;
            retval -= n;
            return retval;
        }
        /** @} */

        /** Distance between two iterators. */
        difference_type operator-(const iterator &other) const {
            return other.base_ - base_;
        }

        /** Return an edge relative to the current one.
         *
         *  Returns the edge that's @p n edges after (or before if negative) the current edge. */
        TreeEdge<T>& operator[](difference_type n) {
            ASSERT_not_null(base_[n]);
            return *base_[n];
        }

        /** Return a reference to the current edge. */
        TreeEdge<T>& operator*() {
            ASSERT_not_null(*base_);
            return **base_;
        }

        /** Return a pointer to the current edge. */
        TreeEdge<T>* operator->() {
            ASSERT_not_null(*base_);
            return &**base_;
        }

        /** Make this iterator point to the same element as the @ other iterator. */
        iterator& operator=(const iterator &other) {
            base_ = other.base_;
            return *this;
        }

        /** Compare two iterators.
         *
         * @{ */
        bool operator==(const iterator &other) const {
            return base_ == other.base_;
        }
        bool operator!=(const iterator &other) const {
            return base_ != other.base_;
        }
        bool operator<(const iterator &other) const {
            return base_ < other.base_;
        }
        bool operator<=(const iterator &other) const {
            return base_ <= other.base_;
        }
        bool operator>(const iterator &other) const {
            return base_ > other.base_;
        }
        bool operator>=(const iterator &other) const {
            return base_ >= other.base_;
        }
        /** @} */
    };

protected:
    TreeListNode() {}

public:
    /** Allocating constructor.
     *
     *  Constructs a new node that has no children. */
    static std::shared_ptr<TreeListNode> instance() {
        return std::shared_ptr<TreeListNode>(new TreeListNode);
    }

    /** Test whether vector is empty.
     *
     *  Returns true if this node contains no child edges, null or otherwise. */
    bool empty() const {
        return elmts_.empty();
    }

    /** Number of child edges.
     *
     *  Returns the number of children edges, null or otherwise. */
    size_t size() const {
        return elmts_.size();
    }

    /** Reserve space so the child edge vector can grow without being reallocated. */
    void reserve(size_t n) {
        elmts_.reserve(n);
    }

    /** Reserved capacity. */
    size_t capacity() const {
        return elmts_.capacity();
    }

    /** Insert a child pointer at the end of this node.
     *
     *  If the new element is non-null, then it must satisfy all the requirements for inserting a node as a child of another
     *  node, and its parent pointer will be adjusted automatically. */
    TreeEdge<T>& push_back(const std::shared_ptr<T>& elmt) {
        elmts_.push_back(std::make_unique<TreeEdge<T>>(*this, elmt));
        return *elmts_.back();
    }

    /** Erase a child edge from the end of this node.
     *
     *  If the edge being erased points to a child, then that child's parent pointer is reset. */
    TreeNodePtr pop_back() {
        ASSERT_forbid(elmts_.empty());
        TreeNodePtr retval = (*elmts_.back())();
        elmts_.pop_back();
        return retval;
    }

    /** Return a reference to the nth edge.
     *
     * @{ */
    TreeEdge<T>& operator[](size_t n) {
        return *elmts_.at(n);
    }
    TreeEdge<T>& at(size_t i) {
        return *elmts_.at(i);
    }
    /** @} */

    /** Return an iterator pointing to the first edge. */
    iterator begin() {
        return iterator(elmts_.begin());
    }

    /** Return an iterator pointing to one past the last edge. */
    iterator end() {
        return iterator(elmts_.end());
    }

    /** Return a reference to the first edge. */
    TreeEdge<T>& front() {
        ASSERT_forbid(elmts_.empty());
        return *elmts_.front();
    }

    /** Return a reference to the last edge. */
    TreeEdge<T>& back() {
        ASSERT_forbid(elmts_.empty());
        return *elmts_.back();
    }

protected:
    virtual TreeChildDescriptor findChild(size_t i) const override {
        if (i < elmts_.size()) {
            return TreeChildDescriptor{i, boost::lexical_cast<std::string>(i), (*elmts_[i])()};
        } else {
            return TreeChildDescriptor{elmts_.size(), "", nullptr};
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Template implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
bool
TreeParentEdge::operator==(const TreeEdge<T> &other) const {
    return parent_ == other();
}

template<class T>
bool
TreeParentEdge::operator!=(const TreeEdge<T> &other) const {
    return parent_ != other();
}

template<class T>
TreeEdge<T>::~TreeEdge() {
    if (child_)
        resetParent(child_->parent);
}

template<class T>
TreeEdge<T>::TreeEdge(TreeNode &parent)
    : parent_(parent) {}

template<class T>
TreeEdge<T>::TreeEdge(TreeNode &parent, const std::shared_ptr<T> &child)
    : parent_(parent), child_(child) {
    if (child) {
        if (child->parent)
            throw TreeAttachmentError(child);
        setParent(child->parent, parent);
    }
}

template<class T>
const std::shared_ptr<T>&
TreeEdge<T>::operator->() const {
    ASSERT_not_null(child_);
    return child_;
}

template<class T>
const std::shared_ptr<T>&
TreeEdge<T>::operator()() const {
    return child_;
}

template<class T>
bool
TreeEdge<T>::operator==(const std::shared_ptr<TreeNode> &ptr) const {
    return child_ == ptr;
}

template<class T>
bool
TreeEdge<T>::operator!=(const std::shared_ptr<TreeNode> &ptr) const {
    return child_ != ptr;
}

template<class T>
bool
TreeEdge<T>::operator==(const TreeParentEdge &other) const {
    return child_ == other();
}

template<class T>
bool
TreeEdge<T>::operator!=(const TreeParentEdge &other) const {
    return child_.get() != other();
}

template<class T>
template<class U>
bool
TreeEdge<T>::operator==(const TreeEdge<U> &other) const {
    return child_.get() == other.get();
}

template<class T>
template<class U>
bool
TreeEdge<T>::operator!=(const TreeEdge<U> &other) const {
    return child_.get() != other.get();
}

template<class T>
TreeEdge<T>&
TreeEdge<T>::operator=(const std::shared_ptr<T> &child) {
    if (child != child_) {
        // Check for errors
        if (child) {
            if (child->parent)
                throw TreeAttachmentError(child);
#ifndef NDEBUG
            parent_.traverseUpward([&child](const TreeNodePtr &node) {
                if (child == node) {
                    throw TreeCycleError(child);
                } else {
                    return false;
                }
            });
#endif
        }

        // Unlink the child from the tree
        if (child_) {
            resetParent(child_->parent);
            child_.reset();                             // parent-to-child edge
        }

        // Link new child into the tree
        if (child) {
            setParent(child->parent, parent_);
            child_ = child;
        }
    }
    return *this;
}

template<class T>
TreeEdge<T>&
TreeEdge<T>::operator=(const TreeParentEdge &parent) {
    return (*this) = parent();
}

#endif
