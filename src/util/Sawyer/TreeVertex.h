// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_TreeVertex_H
#define Sawyer_TreeVertex_H

#include <Sawyer/Assert.h>
#include <Sawyer/Exception.h>

#include <boost/lexical_cast.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Sawyer {
namespace Tree {

enum class TraversalEvent {
    ENTER,                                          /**< Pre-order visitation. */
    LEAVE                                           /**< Post-order visitation. */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base vertex type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base vertex for use by users. */
template<class B>
class Vertex: public std::enable_shared_from_this<Vertex<B>> {
public:
    using UserBase = B;
    using UserBasePtr = std::shared_ptr<UserBase>;
    using TraversalEvent = Sawyer::Tree::TraversalEvent;

protected:
    /** Information about a child. */
    struct ChildDescriptor {
        size_t i;                                       /**< Index of the child counted across all inherited child edges. */
        std::string name;                               /**< Property name of the child. */
        UserBasePtr value;                              /**< Child pointer value. */
    };

public:
    template<class T> class Edge;

    /** Base class for errors and exceptions for this vertex type. */
    class Exception: public Sawyer::Exception::RuntimeError {
    public:
        /** Vertex that caused the error. */
        UserBasePtr vertex;

        /** Construct a new error with the specified message and the causing vertex. */
        Exception(const std::string &mesg, const UserBasePtr &vertex)
            : Sawyer::Exception::RuntimeError(mesg), vertex(vertex) {}

        ~Exception() {}
    };

    /** Error when attaching a vertex to a tree and the vertex is already attached somewhere else.
     *
     *  If the operation were allowed to continue without throwing an exception, the data structure would no longer be a tree. */
    class InsertionError: public Exception {
    public:
        /** Construct a new error with the vertex that caused the error. */
        explicit InsertionError(const UserBasePtr &vertex)
            : Exception("vertex is already attached to a tree", vertex) {}
    };

    /** Error when attaching a vertex to a tree would cause a cycle.
     *
     *  If the operation were allowed to continue without throwing an exception, the data structure would no longer be a tree. */
    class CycleError: public Exception {
    public:
        /** Construct a new error with the vertex that caused the error. */
        explicit CycleError(const UserBasePtr &vertex)
            : Exception("insertion of vertex would cause a cycle in the tree", vertex) {}
    };


    /** Points from a child to a parent in the tree.
     *
     *  This is the pointer type that points from a child vertex to its parent vertex if the child has a parent. Its value is
     *  adjusted automatically when the child vertex is inserted or erased as a child of some other vertex. The term "edge" is used
     *  instead of "pointer" because the relationship between child and parent is bidirectional.
     *
     *  A reverse edge is always a data member of a vertex and never instantiated in other circumstances. Thus users don't normally
     *  instantiate these directly, but they do interact with them through the @c parent data member that exists for all vertex
     *  types. */
    class ReverseEdge {
    private:
        Vertex &child_;                                 // required child vertex owning this edge

        // The parent pointer is a raw pointer because it is safe to do so, and because we need to know the pointer before the parent is
        // fully constructed.
        //
        // It is safe (never dangling) because the pointer can only be changed by a forward edge, which is always a member of a vertex,
        // and the parent pointer is only set to point to that vertex. When the parent is deleted the edge is deleted and its destructor
        // changes the parent pointer back to null.
        //
        // The parent pointer is needed during construction of the parent when the parent has some edge data members that are being
        // initialized to point to non-null children. This happens during the parent's construction, before the parent has any
        // shared or weak pointers.
        UserBase *parent_ = nullptr;                    // optional parent to which this edge points

    public:
        // No default constructor and not copyable.
        ReverseEdge() = delete;
        explicit ReverseEdge(const ReverseEdge&) = delete;
        ReverseEdge& operator=(const ReverseEdge&) = delete;

    public:
        ~ReverseEdge();                                 // internal use only
        explicit ReverseEdge(Vertex &child);            // internal use only

    public:
        /** Return the parent if there is one, else null.
         *
         * @{ */
        UserBasePtr operator()() const;
        UserBasePtr operator->() const;
        /** @} */

        /** Compare the parent pointer to another pointer.
         *
         * @{ */
        bool operator==(const UserBasePtr&) const;
        bool operator!=(const UserBasePtr&) const;
        bool operator==(const ReverseEdge&) const;
        bool operator!=(const ReverseEdge&) const;
        template<class T> bool operator==(const Edge<T>&) const;
        template<class T> bool operator!=(const Edge<T>&) const;
        /** @} */

        /** True if parent is not null. */
        explicit operator bool() const {
            return parent_ != nullptr;
        }

    private:
        // Used internally through ReverseEdgeAccess when a Edge<T> adjusts the ReverseEdge
        template<class T> friend class Edge;
        void reset();
        void set(UserBase &parent);
    };

public:
    /** A parent-to-child edge in a tree.
     *
     *  A parent-to-child edge is a pointer-like object that points from an parent vertex to a child vertex or nullptr. It is also
     *  responsible for adjusting the child's parent pointer. The term "edge" is used instead of "pointer" because the relationship
     *  between the parent and child is bidirectional. The full term is "forward edge", but since this is the only kind of edge that
     *  users will work with, we've shortened the name to just "edge". A "reverse edge" is the pointer from child to parent.
     *
     *  An forward edge is always a data member of a vertex and never instantiated in other circumstances. Thus users don't normally
     *  instanticate these directly, but they do interact with them to obtain pointers to children from a parent.
     *
     *  This type is used to define a data member in the parent that points to a child. For instance, the following binary
     *  expression vertex has left-hand-side and right-hand-side children that are part of the tree. If @c lhs and @c rhs were not
     *  intended to be part of the tree data structure, then their types would be pointers (@c ExpressionPtr) instead of edges.
     *
     * @code
     *  class BinaryExpression: public Expression {
     *  public:
     *      using Ptr = std::shared_ptr<BinaryExpression>;
     *
     *  public:
     *      Edge<Expression> lhs;
     *      Edge<Expression> rhs;
     *
     *  protected:
     *      BinaryExpression()
     *          : lhs(*this), rhs(*this) {}
     *
     *      static Ptr instance() {
     *          return Ptr(new BinaryExpression);
     *      }
     *  };
     * @endcode */
    template<class T>
    class Edge {
    public:
        /** Type of child being pointed to. */
        using Child = T;

        /** Type of pointer to the child. */
        using ChildPtr = std::shared_ptr<T>;

    private:
        UserBase &parent_;                              // required parent owning this child edge
        ChildPtr child_;                                // optional child to which this edge points

    public:
        // No default constructor and not copyable.
        Edge() = delete;
        Edge(const Edge&) = delete;
        Edge& operator=(const Edge&) = delete;

    public:
        ~Edge();

        /** Construct a child edge that belongs to the specified parent.
         *
         *  When constructing a class containing a data member of this type (i.e., a tree edge that points to a child of this
         *  vertex), the data member must be initialized by passing @c *this as the argument.  See the example in this class
         *  documentation.
         *
         *  An optional second argument initializes the child pointer for the edge. The initialization is the same as if the child
         *  had been assigned with @c operator= later. I.e., the child must not already have a parent.
         *
         * @{ */
        explicit Edge(UserBase &parent);
        Edge(UserBase &parent, const ChildPtr &child);
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
        bool operator==(const UserBasePtr&) const;
        bool operator!=(const UserBasePtr&) const;
        bool operator==(const ReverseEdge&) const;
        bool operator!=(const ReverseEdge&) const;
        template<class U> bool operator==(const Edge<U>&) const;
        template<class U> bool operator!=(const Edge<U>&) const;
        /** @} */

        /** Assign a pointer to a child.
         *
         *  If this edge points to an old child then that child is removed and its parent is reset. If the specified new child is
         *  non-null, then it is inserted and its parent pointer set to the parent of this edge.
         *
         *  However, if the new child already has a non-null parent, then no changes are made and an @ref InsertionError is thrown
         *  with the error's vertex pointing to the intended child. Otherwise, if the new child is non-null and is the parent or any
         *  more distant ancestor of this edge's vertex, then a @ref CycleError is thrown. Cycle errors are only thrown if debugging
         *  is enabled (i.e., the CPP macro @c NDEBUG is undefined).
         *
         *  Attempting to assign one child edge object to another is a compile-time error (its operator= is not declared) because
         *  every non-null child edge points to a child whose parent is non-null, which would trigger an @ref
         *  InsertionError. Therefore only null child edges could be assigned. But since only null child edges can be assigned, its
         *  more concise and clear to assign the null pointer directly.
         *
         * @{ */
        Edge& operator=(const ChildPtr &child);
        Edge& operator=(const ReverseEdge&);
        /** @} */

        /** True if child is not null. */
        explicit operator bool() const {
            return child_ != nullptr;
        }

    private:
        void checkChildInsertion(const ChildPtr &child) const;
    };
public:
    /** Pointer to the parent in the tree.
     *
     *  A vertex's parent pointer is adjusted automatically when the vertex is inserted or removed as a child of another vertex. An
     *  invariant of this design is that whenever vertex A is a child of vertex B, then vertex B is a parent of vertex A. */
    ReverseEdge parent;

protected:
    virtual void destructorHelper() {}
    Vertex();

public:
    /** Returns a shared pointer to this vertex. */
    UserBasePtr pointer();

    /** Traverse in reverse direction from children to parents.
     *
     *  The visitor is called for each vertex from the current vertex whose type is @p T until the root of the tree is reached
     *  unless the visitor indicates that the traversal should end. It does so by returning a value that is true in a Boolean
     *  context, and this value becomes the return value for the entire traversal.
     *
     *  Example: Find the closest ancestor whose type is Foo or is derived from Foo.
     *
     * @code
     *  auto foundFirst = tree->traverseReverse<Foo>([](const Foo::Ptr &foo, TraversalEvent event) {
     *      return TraversalEvent::ENTER == event ? foo : nullptr;
     *  });
     * @endcode
     *
     *  Example: Find the most distant ancestor whose type is Foo or is derived from Foo. The only change from the previous
     *  example is that we test as we leave the vertices. I.e., as the depth-first traversal is returning.
     *
     * @code
     *  auto foundLast = tree->traverseReverse<Foo>([](const Foo::Ptr &foo, TraversalEvent event) {
     *      return TraversalEvent::LEAVE == event ? foo : nullptr;
     *  });
     * @endcode
     *
     *  If you want to exclude the current vertex from the traversal, start searching at the parent instead, but be careful that
     *  the parent is not null or else its arrow operator will fail.
     *
     * @code
     *  auto foundFirstNotMe = tree->parent->traverseReverse<Foo>([](const Foo::Ptr &foo, TraversalEvent event) {
     *      return TraversalEvent::ENTER == event ? foo : nullptr;
     *  });
     * @endcode */
    template<class T, class Visitor>
    auto traverseReverse(const Visitor &visitor) {
        auto vertex = std::dynamic_pointer_cast<T>(this->pointer());
        if (vertex) {
            if (auto result = visitor(vertex, TraversalEvent::ENTER))
                return result;
        }

        if (parent) {
            if (auto result = parent()->template traverseReverse<T>(visitor))
                return result;
        }

        if (vertex) {
            return visitor(vertex, TraversalEvent::LEAVE);
        } else {
            return decltype(visitor(vertex, TraversalEvent::ENTER))();
        }
    }

    /** Traverse in forward direction from parents to children.
     *
     *  Perform a depth-first traversal of the tree starting with this vertex. The visitor functor is called twice for each vertex,
     *  first in the forward direction from the parent, then in the reverse direction from the children. The functor takes two
     *  arguments: the vertex being visited and an enum indicating whether the visit is the first (@ref Traverse::ENTER) or the
     *  second (@ref Traverse::LEAVE) visitation. The traversal has the same return type as the functor. If the functor returns a
     *  value which evaluates to true in Boolean context, then the traversal immediately returns that value, otherwise it continues
     *  until the entire subtree is visited and returns a default-constructed value. */
    template<class T, class Visitor>
    auto traverse(const Visitor &visitor) {
        auto vertex = std::dynamic_pointer_cast<T>(this->pointer());
        if (vertex) {
            if (auto retval = visitor(vertex, TraversalEvent::ENTER))
                return retval;
        }
        for (size_t i = 0; true; ++i) {
            const ChildDescriptor child = findChild(i);
            if (child.i < i) {
                break;
            } else if (child.value) {
                if (auto retval = child.value->template traverse<T>(visitor))
                    return retval;
            }
        }
        if (vertex) {
            return visitor(vertex, TraversalEvent::LEAVE);
        } else {
            return decltype(visitor(vertex, TraversalEvent::ENTER))();
        }
    }

    /** Traversal that finds the closest ancestor of type T or derived from T. */
    template<class T>
    std::shared_ptr<T> findFirstAncestor() {
        return traverseReverse<T>([](const std::shared_ptr<T> &vertex, TraversalEvent event) {
            return TraversalEvent::ENTER == event ? vertex : nullptr;
        });
    };

    /** Traversal that finds the farthest ancestor of type T or derived from T. */
    template<class T>
    std::shared_ptr<T> findLastAncestor() {
        return traverseReverse<T>([](const std::shared_ptr<T> &vertex, TraversalEvent event) {
            return TraversalEvent::LEAVE == event ? vertex : nullptr;
        });
    };

    /** Traversal that finds all the descendants of a particular type.
     *
     *  Note that this is probably not the way you want to do this because it's expensive to create the list of all matching
     *  pointers. Instead, you probably want to call @ref traverse and handle each matching vertex inside the functor. */
    template<class T>
    std::vector<std::shared_ptr<T>> findDescendants() {
        std::vector<std::shared_ptr<T>> retval;
        traverse<T>([&retval](const std::shared_ptr<T> &vertex, TraversalEvent event) {
            if (TraversalEvent::ENTER == event)
                retval.push_back(vertex);
        });
        return retval;
    }

    /** Returns the property name for a child.
     *
     *  Returns the property name for the child at index @p i. If @p i is out of range, then an empty string is returned. */
    std::string childName(size_t i);

    /** Returns the pointer for a child.
     *
     *  Returns the pointer for the child at index @p i. If @p i is out of range, then a null pointer is returned, which is
     *  indistinguishable from the case when a valid index is specified but that child is a null pointer. */
    UserBasePtr child(size_t i);

    /** Returns the number of children.
     *
     *  This is the number of children for this class and the base class, recursively. Some children may be null pointers. */
    size_t nChildren(size_t i);

protected:
    /** Finds information about an indexed child.
     *
     *  The index, @p i, specifies the child about which information is returned. Children are numbered recursively in base classes
     *  followed by the current class. This function is re-implemented in every derived class that has children.
     *
     *  A @ref ChildDescriptor is returned for every query. If the index is out of range for the class, then the return value is for
     *  a child one past the end. I.e., the index is equal to the number of children, the name is empty, and the value is a null
     *  pointer. */
    virtual ChildDescriptor findChild(size_t i) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations for ReverseEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class B>
Vertex<B>::ReverseEdge::~ReverseEdge() {
    ASSERT_require(this->parent_ == nullptr);
}

template<class B>
Vertex<B>::ReverseEdge::ReverseEdge(Vertex &child)
    : child_(child) {}

template<class B>
typename Vertex<B>::UserBasePtr
Vertex<B>::ReverseEdge::operator()() const {
    if (parent_) {
        return parent_->pointer();
    } else {
        return {};
    }
}

template<class B>
typename Vertex<B>::UserBasePtr
Vertex<B>::ReverseEdge::operator->() const {
    ASSERT_not_null(parent_);
    return parent_->pointer();
}

template<class B>
bool
Vertex<B>::ReverseEdge::operator==(const UserBasePtr &ptr) const {
    return ptr.get() == parent_;
}

template<class B>
bool
Vertex<B>::ReverseEdge::operator!=(const UserBasePtr &ptr) const {
    return ptr.get() != parent_;
}

template<class B>
bool
Vertex<B>::ReverseEdge::operator==(const ReverseEdge &other) const {
    return parent_ == other.parent_;
}

template<class B>
bool
Vertex<B>::ReverseEdge::operator!=(const ReverseEdge &other) const {
    return parent_ != other.parent_;
}

template<class B>
template<class T>
bool
Vertex<B>::ReverseEdge::operator==(const Edge<T> &other) const {
    return parent_ == other();
}

template<class B>
template<class T>
bool
Vertex<B>::ReverseEdge::operator!=(const Edge<T> &other) const {
    return parent_ != other();
}

template<class B>
void
Vertex<B>::ReverseEdge::reset() {
    parent_ = nullptr;
}

template<class B>
void
Vertex<B>::ReverseEdge::set(UserBase &parent) {
    parent_ = &parent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations for Edge<T>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class B>
template<class T>
Vertex<B>::Edge<T>::~Edge() {
    if (child_)
        child_->parent.reset();
}

template<class B>
template<class T>
Vertex<B>::Edge<T>::Edge(UserBase &parent)
    : parent_(parent) {}

template<class B>
template<class T>
Vertex<B>::Edge<T>::Edge(UserBase &parent, const std::shared_ptr<T> &child)
    : parent_(parent), child_(child) {
    checkChildInsertion(child);
    if (child)
        child->parent.set(parent);
}

template<class B>
template<class T>
void
Vertex<B>::Edge<T>::checkChildInsertion(const std::shared_ptr<T> &child) const {
    if (child) {
        if (child->parent)
            throw InsertionError(child);
#ifndef NDEBUG
        for (const UserBase *p = &parent_; p; p = p->parent().get()) {
            if (p == child.get())
                throw CycleError(child);
        }
#endif
    }
}

template<class B>
template<class T>
const std::shared_ptr<T>&
Vertex<B>::Edge<T>::operator->() const {
    ASSERT_not_null(child_);
    return child_;
}

template<class B>
template<class T>
const std::shared_ptr<T>&
Vertex<B>::Edge<T>::operator()() const {
    return child_;
}

template<class B>
template<class T>
bool
Vertex<B>::Edge<T>::operator==(const UserBasePtr &ptr) const {
    return child_ == ptr;
}

template<class B>
template<class T>
bool
Vertex<B>::Edge<T>::operator!=(const UserBasePtr &ptr) const {
    return child_ != ptr;
}

template<class B>
template<class T>
bool
Vertex<B>::Edge<T>::operator==(const ReverseEdge &other) const {
    return child_ == other();
}

template<class B>
template<class T>
bool
Vertex<B>::Edge<T>::operator!=(const ReverseEdge &other) const {
    return child_.get() != other();
}

template<class B>
template<class T>
template<class U>
bool
Vertex<B>::Edge<T>::operator==(const Edge<U> &other) const {
    return child_.get() == other.get();
}

template<class B>
template<class T>
template<class U>
bool
Vertex<B>::Edge<T>::operator!=(const Edge<U> &other) const {
    return child_.get() != other.get();
}

template<class B>
template<class T>
Vertex<B>::Edge<T>&
Vertex<B>::Edge<T>::operator=(const std::shared_ptr<T> &child) {
    if (child != child_) {
        checkChildInsertion(child);

        // Unlink the child from the tree
        if (child_) {
            child_->parent.reset();                     // child-to-parent edge
            child_.reset();                             // parent-to-child edge
        }

        // Link new child into the tree
        if (child) {
            child->parent.set(parent_);
            child_ = child;
        }
    }
    return *this;
}

template<class B>
template<class T>
Vertex<B>::Edge<T>&
Vertex<B>::Edge<T>::operator=(const ReverseEdge &parent) {
    return (*this) = parent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations for Vertex<B>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class B>
Vertex<B>::Vertex()
    : parent(*this) {}

template<class B>
typename Vertex<B>::UserBasePtr
Vertex<B>::pointer() {
    auto retval = std::dynamic_pointer_cast<UserBase>(this->shared_from_this());
    ASSERT_not_null(retval);
    return retval;
}

template<class B>
std::string
Vertex<B>::childName(size_t i) {
    return findChild(i).name;
}

template<class B>
typename Vertex<B>::UserBasePtr
Vertex<B>::child(size_t i) {
    return findChild(i).value;
}

template<class B>
size_t
Vertex<B>::nChildren(size_t i) {
    return findChild(std::numeric_limits<size_t>::max()).i;
}

template<class B>
typename Vertex<B>::ChildDescriptor
Vertex<B>::findChild(size_t i) const {
    return ChildDescriptor{0, "", nullptr};
}

} // namespace
} // namespace
#endif
