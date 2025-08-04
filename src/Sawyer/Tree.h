// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_TreeVertex_H
#define Sawyer_TreeVertex_H

#include <Sawyer/Assert.h>
#include <Sawyer/Exception.h>

#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/signals2/signal.hpp>
#include <memory>
#include <string>
#include <vector>

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#endif

#ifdef SAWYER_HAVE_CEREAL
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#endif

namespace Sawyer {

/** Tree data structure.
 *
 *  The classes in this namespace are for creating a tree data structure having vertices that are polymorphic types, using smart
 *  pointers to manage vertex memory, having upward "parent" pointers that are managed automatically, and having downward and upward
 *  traversals. */
namespace Tree {

/** Traversal event.
 *
 *  The traversal event is used to indicate whether a vertex visitor is being invoked in pre-order or post-order during a
 *  depth-first traversal. */
enum class TraversalEvent {
    ENTER,                                          /**< Pre-order visitation. */
    LEAVE                                           /**< Post-order visitation. */
};

/** Base class for tree vertices.
 *
 *  A tree vertex is a type with zero or more edges that point to child vertices. The set of all vertices recursively reachable from
 *  any particular vertex along these edges forms a tree data structure.
 *
 *  The user might want to have multiple independent tree types whose vertices aren't derived from any common base class. This is
 *  accomplished by the user's base class inheriting from this class template, whose argument is the user's base class.
 *
 *  All vertex instantiations are reference counted and ponted to by @c std::shared_ptr. Some good practices are:
 *
 *  @li In a separate header file intended to be lightweight, often named "BasicTypes.h", create a forward declaration for the
 *  class.
 *
 *  @li Declare the C++ constructors with protected access and replace them with public, static member functions named @c instance
 *  that allocate a new object in the heap and return a pointer to it. This prevents users from accidentally instantiating objects
 *  on the stack or in the data segment.
 *
 *  @li Create a nested type named @c Ptr inside the class. This should be an alias for the @c std::shared_ptr type. If you do this
 *  consistently, then you can always use "Ptr" instead of the much longer "std::shared_ptr<MyClassName>". Since one often needs
 *  this type outside this header file, adding an alias to the aforementioned "BasicTypes.h" is prudent, in which case the one in
 *  BasicTypes.h should be like "MyClassNamePtr" (i.e., ending with "Ptr") and the alias inside the class would be "using Ptr =
 *  MyClassNamePtr".
 *
 *  As mentioned, a vertex may have zero or more edges that point to children of the same tree. These are data members whose type is
 *  @c Edge<T> or @c EdgeList<T> (or anything else that inherits from @c EdgeBase). These types do not have default constructors, so
 *  the constructor in the containing class is expected to construct them by passing @c *this as the first argument to their
 *  constructors.
 *
 *  In addition to edges that are part of the tree, it is permissible for a vertex to point directly to other vertices inside or
 *  outside the tree from which they're pointed. These are called "cross tree" pointers and do not use the @p EdgeBase types. By the
 *  way, the difference between an "edge" and a "pointer" in this context is that an edge is bidirectional.
 *
 *  Every vertex also has a @c parent public data member that points to the parent vertex in the tree. The @p parent data member is
 *  updated automatically whenever a child is attached to or detached from a parent vertex. A vertex may have exactly zero or one
 *  parent. If vertex <em>v</em> has no parent, then there does not exist any vertex that has an edge pointing to <em>v</em>.  Vice
 *  versa, if there exists a vertex pointing to <em>v</em> then <em>v</em>->parent points to that vertex.
 *
 *  Two types of traversals are defined: forward and reverse. The traversals visit only vertices of the specified type (or
 *  derivatives thereof), they support both pre- and post-order depth-first traversals, and they can return user-defined types, and
 *  they can short-circuit by returning a certain class of values. For more information, see @ref traverse and @ref
 *  traverseReverse. All other traversals can be defined in terms of these two.
 *
 *  Example of a class that has two scalar children one of which is always allocated, one vector child, and one cross-tree pointer.
 *
 * @code
 *  class Test: public Base {  // Base ultimately inherits from Tree::Vertex<T>
 *  public:
 *      using Ptr = TestPtr;   // defined in BasicTypes.h as "using TestPtr = std::shared_ptr<Test>"
 *
 *  public:
 *      Edge<Test> left;       // tree edge to the "left" child
 *      Edge<Other> right;     // tree edge to the "right" child, always allocated. Other must also inherit from Base.
 *      EdgeList<Test> list;   // tree edges to a bunch of children
 *      TestPtr cross;         // non-tree pointer to some other vertex
 *
 *  protected:
 *      Test()                 // C++ constructor hidden from casual users
 *          : left(*this),
 *            right(*this, Other::instance()),
 *            list(*this) {}
 *
 *  public:
 *      static Ptr instance(const Ptr &a, const Ptr &b) {
 *          auto self = Ptr(new Test);
 *          self->left = a;
 *          self->list.push_back(b);
 *
 *          // Adjustments the parents happens automatically
 *          assert(self->left == a && a->parent == self);
 *          assert(self->list.front() == b && b->parent == self);
 *
 *          return self;
 *      }
 *  };
 * @endcode */
template<class B>
class Vertex: public std::enable_shared_from_this<Vertex<B>> {
public:

    /** User's base class. */
    using UserBase = B;

    /** Pointer to user's base class. */
    using UserBasePtr = std::shared_ptr<UserBase>;

    /** Alias for traversal events. */
    using TraversalEvent = Sawyer::Tree::TraversalEvent;

public:
    template<class T> class Edge;
    template<class T> class EdgeVector;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Errors and exceptions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Child-to-parent edges
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

        // The parent pointer is a raw pointer because it is safe to do so, and because we need to know the pointer before the
        // parent is fully constructed.
        //
        // It is safe (never dangling) because the pointer can only be changed by a forward edge, which is always a member of a
        // vertex, and the parent pointer is only set to point to that vertex. When the parent is deleted the edge is deleted and
        // its destructor changes the parent pointer back to null.
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Base class for parent-to-child edges
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    enum class Link { NO, YES };

    // Used internally as the non-template abstract base class for all parent-to-child edge types.
    class EdgeBase {
    public:
        // Previous edge in this object, including edges defined in base classes. Edges in this linked list are ordered so that
        // if edge A was initialized before edge B, then edge A will appear earlier in the list. The "prev" pointers in the list
        // point backward through the list.
        EdgeBase *prev;

    protected:
        UserBase &parent_;                              // required parent owning this child edge

    public:
        virtual ~EdgeBase() {}
        EdgeBase() = delete;
        EdgeBase(const EdgeBase&) = delete;
        EdgeBase& operator=(const EdgeBase&) = delete;
        explicit EdgeBase(EdgeBase *prev, UserBase &parent)
            : prev(prev), parent_(parent) {}

        // Number of child pointers in this edge. Edges are either 1:1 or 1:N. Some of the child pointers could be null.
        virtual size_t size() const = 0;

        // Return the i'th pointer. The argument is expected to be in the domain.
        virtual UserBasePtr pointer(size_t i) const = 0;

        // Traverse through all the non-null children of appropriate type pointed to by all edges in the order that the edges were
        // initialized. The visitor is invoked with two arguments: a non-null shared pointer to the child, and an indication of
        // whether this is a pre- or post-order visit. The first visitor call that returns a value that is true in a Boolean context
        // short circuits the traversal and that value becomes the return value of the traversal.
        template<class T, class Visitor>
        auto traverse(Visitor visitor) -> decltype(visitor(std::shared_ptr<T>(), TraversalEvent::ENTER)) {
            if (prev) {
                if (auto retval = prev->traverse<T>(visitor))
                    return retval;
            }
            for (size_t i = 0; i < size(); ++i) {
                if (auto child = pointer(i)) {
                    if (auto retval = child->template traverse<T>(visitor))
                        return retval;
                }
            }
            return decltype(visitor(std::shared_ptr<T>(), TraversalEvent::ENTER))();
        }

        // Return the list of child pointers for this edge. An `Edge` will have exactly one pointer, and an `EdgeVector` will
        // have any number of pointers. The pointers may be null.
        std::vector<UserBasePtr> immediateChildren() const {
            std::vector<UserBasePtr> retval;
            retval.reserve(size());
            for (size_t i = 0; i < size(); ++i)
                retval.push_back(pointer(i));
            return retval;
        }

    protected:
        // Return this edge and the previous edges in the order they were initialized. Each member is either a non-null pointer to
        // an `Edge` or a non-null pointer to an `EdgeVector`.
        std::vector<const EdgeBase*> baseEdgeList() const {
            std::vector<const EdgeBase*> retval;
            for (const EdgeBase *child = this; child; child = prev)
                retval.push_back(child);
            std::reverse(retval.begin(), retval.end());
            return retval;
        }

    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 1:1 parent-to-child edge
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    class Edge: public EdgeBase {
    public:
        /** Type of child being pointed to. */
        using Child = T;

        /** Type of pointer to the child. */
        using ChildPtr = std::shared_ptr<Child>;

    private:
        using ChangeSignal = boost::signals2::signal<void(ChildPtr, ChildPtr)>;

    private:
        ChildPtr child_;                                // optional child to which this edge points
        ChangeSignal beforeChange_;                     // signal emitted before the child pointer is changed
        ChangeSignal afterChange_;                      // signal emitted after the child pointer is changed

#ifdef SAWYER_HAVE_CEREAL
    private:
        friend class cereal::access;

        template<class Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive &archive) {
            archive(cereal::make_nvp("child", child_));
        }
#endif

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
    private:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &archive, const unsigned /*version*/) {
            archive & boost::serialization::make_nvp("child", child_);
        }
#endif

    public:
        /** Destructor clears child's parent. */
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

    private:
        template<class U> friend class EdgeVector;
        // Used internally to initialize an edge without linking it to prior edges
        Edge(Link, UserBase &parent, const ChildPtr &child);

    public:
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
        Edge& operator=(const ChildPtr &newChild) {
            if (newChild != child_) {
                checkChildInsertion(newChild);

                const ChildPtr oldChild = child_;
                beforeChange_(oldChild, newChild);

                // Unlink the child from the tree
                if (child_) {
                    child_->parent.reset();                     // child-to-parent edge
                    child_.reset();                             // parent-to-child edge
                }

                // Link new child into the tree
                if (newChild) {
                    newChild->parent.set(this->parent_);
                    child_ = newChild;
                }

                afterChange_(oldChild, newChild);
            }
            return *this;
        }

        Edge& operator=(const ReverseEdge &parent) {
            return (*this) = parent();
        }
        /** @} */

        /** True if child is not null. */
        explicit operator bool() const {
            return child_ != nullptr;
        }

        /** Functors to call before the child pointer is changed.
         *
         *  The functor is called before the child pointer is changed and may throw an exception to indicate that the change
         *  should not occur. The two arguments are the old and new child pointers, either of which might be null. The callbacks
         *  are not invoked during edge constructor or destructor calls. */
        boost::signals2::connection beforeChange(const typename ChangeSignal::slot_type &slot) {
            return beforeChange_.connect(slot);
        }

        /** Functors to call after the child pointer is changed.
         *
         *  The functor is called after the edge child pointer is changed. The two arguments are the old and new child pointers,
         *  either of which might be null.  The callbacks are not invoked during edge constructor or destructor calls. */
        boost::signals2::connection afterChange(const typename ChangeSignal::slot_type &slot) {
            return afterChange_.connect(slot);
        }

    private:
        void checkChildInsertion(const ChildPtr &child) const;

        size_t size() const override {
            return 1;
        }

        UserBasePtr pointer(size_t i) const override {
            ASSERT_always_require(0 == i);
            return child_;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 1:N parent-to-child edge
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** A 1:N tree edge from parent to children.
     *
     *  This edge type points to a variable number of children and acts like an @c std::vector. */
    template<class T>
    class EdgeVector: public EdgeBase {
    public:
        /** Type of children being pointed to. */
        using Child = T;

        /** Type of pointers to children. */
        using ChildPtr = std::shared_ptr<Child>;

    private:
        using Vector = std::vector<std::unique_ptr<Edge<Child>>>;
        using ResizeSignal = boost::signals2::signal<void(int, ChildPtr)>;

    public:
        using value_type = Edge<Child>;
        using size_type = typename Vector::size_type;
        using difference_type = typename Vector::difference_type;
        using reference = value_type&;
        using const_reference = const value_type&;

    public:
        template<class BaseIterator>
        class Iterator {
            BaseIterator baseIterator_;

        public:
            Iterator() = delete;
            Iterator(const BaseIterator &baseIterator)
                : baseIterator_(baseIterator) {}

            Iterator(const Iterator &other)
                : baseIterator_(other.baseIterator_) {}

            Iterator& operator=(const Iterator &other) {
                baseIterator_ = other.baseIterator_;
                return *this;
            }

            Iterator& operator++() {
                ++baseIterator_;
                return *this;
            }

            Iterator operator++(int) {
                Iterator temp = *this;
                ++baseIterator_;
                return temp;
            }

            Iterator& operator--() {
                --baseIterator_;
                return *this;
            }

            Iterator operator--(int) {
                Iterator temp = *this;
                --baseIterator_;
                return temp;
            }

            Iterator& operator+=(difference_type n) {
                baseIterator_ += n;
                return *this;
            }

            Iterator operator+(difference_type n) const {
                Iterator retval = *this;
                retval += n;
                return retval;
            }

            Iterator& operator-=(difference_type n) {
                baseIterator_ -= n;
                return *this;
            }

            Iterator operator-(difference_type n) const {
                Iterator retval = *this;
                retval -= n;
                return retval;
            }

            difference_type operator-(const Iterator &other) const {
                return other.baseIterator_ - baseIterator_;
            }

            auto& operator*() const {
                ASSERT_not_null(*baseIterator_);
                return **baseIterator_;
            }

            auto& operator->() const {
                ASSERT_not_null(*baseIterator_);
                return &**baseIterator_;
            }

            auto& operator[](difference_type i) const {
                ASSERT_not_null(baseIterator_[i]);
                return *baseIterator_[i];
            }

            bool operator==(const Iterator &other) const {
                return baseIterator_ == other.baseIterator_;
            }

            bool operator!=(const Iterator &other) const {
                return baseIterator_ != other.baseIterator_;
            }

            bool operator<(const Iterator &other) const {
                return baseIterator_ < other.baseIterator_;
            }

            bool operator<=(const Iterator &other) const {
                return baseIterator_ <= other.baseIterator_;
            }

            bool operator>(const Iterator &other) const {
                return baseIterator_ > other.baseIterator_;
            }

            bool operator>=(const Iterator &other) const {
                return baseIterator_ >= other.baseIterator_;
            }
        };

    public:
        using iterator = Iterator<typename Vector::iterator>;

    private:
        Vector edges_;
        ResizeSignal beforeResize_;                     // signal emitted before a resize operation
        ResizeSignal afterResize_;                      // signal emitted after a resize operation

#ifdef SAWYER_HAVE_CEREAL
    private:
        friend class cereal::access;

        template<class Archive>
        void CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {
            const size_t nEdges = edges_.size();
            archive(CEREAL_NVP(nEdges));
            for (size_t i = 0; i < nEdges; ++i) {
                ChildPtr child = at(i)();
                archive(CEREAL_NVP(child));
            }
        }

        template<class Archive>
        void CEREAL_LOAD_FUNCTION_NAME(Archive &archive) {
            size_t nEdges = 0;
            archive(CEREAL_NVP(nEdges));
            for (size_t i = 0; i < nEdges; ++i) {
                ChildPtr child;
                archive(CEREAL_NVP(child));
                push_back(child);
            }
        }
#endif

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
    private:
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive &archive, const unsigned /*version*/) const {
            const size_t nEdges = edges_.size();
            archive <<BOOST_SERIALIZATION_NVP(nEdges);
            for (size_t i = 0; i < nEdges; ++i) {
                ChildPtr child = at(i)();
                archive <<BOOST_SERIALIZATION_NVP(child);
            }
        }

        template<class Archive>
        void load(Archive &archive, const unsigned /*version*/) {
            size_t nEdges = 0;
            archive >>BOOST_SERIALIZATION_NVP(nEdges);
            for (size_t i = 0; i < nEdges; ++i) {
                ChildPtr child;
                archive >>BOOST_SERIALIZATION_NVP(child);
                push_back(child);
            }
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

    public:
        /** Destructor clears children's parents. */
        ~EdgeVector() {}

        /** Construct a child edge that belongs to the specified parent.
         *
         *  When constructing a class containing a data member of this type (i.e., a tree edge that points to a child of this
         *  vertex), the data member must be initialized by passing @c *this as the argument.  See the example in this class
         *  documentation. */
        explicit EdgeVector(UserBase &parent);

    public:
        /** Test whether vector is empty.
         *
         *  Returns true if this vertex contains no child edges, null or otherwise. */
        bool empty() const {
            return edges_.empty();
        }

        /** Number of child edges.
         *
         *  Returns the number of children edges, null or otherwise. */
        size_t size() const override {
            return edges_.size();
        }

        /** Reserve space so the child edge vector can grow without being reallocated. */
        void reserve(size_t n) {
            edges_.reserve(n);
        }

        /** Reserved capacity. */
        size_t capacity() const {
            return edges_.capacity();
        }

        /** Insert a child pointer at the end of this vertex.
         *
         *  If the new element is non-null, then it must satisfy all the requirements for inserting a vertex as a child of another
         *  vertex, and its parent pointer will be adjusted automatically. */
        void push_back(const ChildPtr& elmt) {
            beforeResize_(+1, elmt);
            auto edge = std::unique_ptr<Edge<Child>>(new Edge<Child>(Link::NO, this->parent_, elmt));
            edges_.push_back(std::move(edge));
            afterResize_(+1, elmt);
        }

        /** Erase a child edge from the end of this vertex.
         *
         *  If the edge being erased points to a child, then that child's parent pointer is reset. */
        void pop_back() {
            ASSERT_forbid(edges_.empty());
            ChildPtr removed = (*edges_.back())();
            beforeResize_(-1, removed);
            edges_.pop_back();
            afterResize_(-1, removed);
        }

        /** Return the i'th edge.
         *
         * @{ */
        const Edge<Child>& at(size_t i) const {
            ASSERT_require(i < edges_.size());
            return *edges_.at(i);
        }
        Edge<Child>& at(size_t i) {
            ASSERT_require(i < edges_.size());
            return *edges_.at(i);
        }
        const Edge<Child>& operator[](size_t i) const {
            return at(i);
        }
        Edge<Child>& operator[](size_t i) {
            return at(i);
        }
        /** @} */

        /** Return the first edge.
         *
         * @{ */
        const Edge<Child>& front() const {
            ASSERT_forbid(empty());
            return at(0);
        }
        Edge<Child>& front() {
            ASSERT_forbid(empty());
            return at(0);
        }
        /** @} */

        /** Return the last edge.
         *
         * @{ */
        const Edge<Child>& back() const {
            ASSERT_forbid(empty());
            return at(size() - 1);
        }
        Edge<Child>& back() {
            ASSERT_forbid(empty());
            return at(size() - 1);
        }
        /** @} */

        /** Return an iterator to the first edge. */
        iterator begin() {
            return iterator(edges_.begin());
        }

        /** Return an iterator to one past the last edge. */
        iterator end() {
            return iterator(edges_.end());
        }

        /** Functors to call before the vector size is changed.
         *
         *  The first argument is the change in size of the vector (1 for a @ref push_back call, or -1 for a @ref pop_back call),
         *  and the second argument is the pointer being pushed or popped. If any of the callbacks throw an exception then the
         *  push or pop operation is aborted without making any changes. */
        boost::signals2::connection beforeResize(const typename ResizeSignal::slot_type &slot) {
            return beforeResize_.connect(slot);
        }

        /** Functors to call after the vector size is changed.
         *
         *  The first argument is the change in size of the vector (1 for a @ref push_back call, or -1 for a @ref pop_back call),
         *  and the second argument is the pointer being pushed or popped. Since these callbacks occur after the push or pop
         *  operation, they do not affect its behavior. */
        boost::signals2::connection afterResize(const typename ResizeSignal::slot_type &slot) {
            return afterResize_.connect(slot);
        }

    protected:
        UserBasePtr pointer(size_t i) const override {
            return at(i)();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vertex
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Pointer to the parent in the tree.
     *
     *  A vertex's parent pointer is adjusted automatically when the vertex is inserted or removed as a child of another vertex. An
     *  invariant of this design is that whenever vertex A is a child of vertex B, then vertex B is a parent of vertex A. */
    ReverseEdge parent;

private:
    EdgeBase *treeEdges_ = nullptr;

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    // This base class is responsible for serializing the parts of the EdgeBase data members pointed to by `treeEdges_`, but not
    // their child pointers. The child pointers are serialized by the derived classes where those data members are defined.
    template<class Archive>
    void
    CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {
        const std::vector<EdgeBase*> baseEdgeList = [this]() {
            if (treeEdges_) {
                return treeEdges_->baseEdgeList();
            } else {
                return std::vector<EdgeBase*>();
            }
        }();

        const size_t nBases = baseEdgeList.size();
        archive(CEREAL_NVP(nBases));
        for (const EdgeBase *baseEdge: baseEdgeList) {
            const size_t baseOffset = (char*)baseEdge - (char*)this;
            archive(CEREAL_NVP(baseOffset));
        }
    }

    template<class Archive>
    void
    CEREAL_LOAD_FUNCTION_NAME(Archive &archive) const {
        const size_t nBases = 0;
        archive(CEREAL_NVP(nBases));
        for (size_t i = 0; i < nBases; ++i) {
            size_t baseOffset = 0;
            archive(CEREAL_NVP(baseOffset));
            EdgeBase *baseEdge = (EdgeBase*)((char*)this + baseOffset);
            baseEdge->reset(treeEdges_);
            treeEdges_ = baseEdge;
        }
    }
#endif

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    // This base class is responsible for serializing the parts of the EdgeBase data members pointed to by `treeEdges_`, but not
    // their child pointers. The child pointers are serialized by the derived classes where those data members are defined.
    template<class Archive>
    void
    save(Archive &archive, const unsigned /*version*/) const {
        const std::vector<EdgeBase*> baseEdgeList = [this]() {
            if (treeEdges_) {
                return treeEdges_->baseEdgeList();
            } else {
                return std::vector<EdgeBase*>();
            }
        }();

        const size_t nBases = baseEdgeList.size();
        archive <<BOOST_SERIALIZATION_NVP(nBases);
        for (const EdgeBase *baseEdge: baseEdgeList) {
            const size_t baseOffset = (char*)baseEdge - (char*)this;
            archive <<BOOST_SERIALIZATION_NVP(baseOffset);
        }
    }

    template<class Archive>
    void
    load(Archive &archive, const unsigned /*version*/) {
        const size_t nBases = 0;
        archive >>BOOST_SERIALIZATION_NVP(nBases);
        for (size_t i = 0; i < nBases; ++i) {
            size_t baseOffset = 0;
            archive >>BOOST_SERIALIZATION_NVP(baseOffset);
            EdgeBase *baseEdge = (EdgeBase*)((char*)this + baseOffset);
            baseEdge->reset(treeEdges_);
            treeEdges_ = baseEdge;
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

public:
    virtual ~Vertex() {}

protected:
    virtual void destructorHelper() {}
    Vertex();

public:
    /** Returns a shared pointer to this vertex. */
    UserBasePtr pointer();

    /** Tests whether this object is a certain type.
     *
     *  Returns a shared pointer to the object if the object is of dynamic type T, otherwise returns a null pointer. */
    template<class T>
    std::shared_ptr<T> isa();

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
     *  arguments: the vertex being visited and an enum indicating whether the visit is the first (@c Traverse::ENTER) or the
     *  second (@c Traverse::LEAVE) visitation. The traversal has the same return type as the functor. If the functor returns a
     *  value which evaluates to true in Boolean context, then the traversal immediately returns that value, otherwise it continues
     *  until the entire subtree is visited and returns a default-constructed value. */
    template<class T, class Visitor>
    auto traverse(const Visitor &visitor) {
        auto vertex = std::dynamic_pointer_cast<T>(this->pointer());
        if (vertex) {
            if (auto retval = visitor(vertex, TraversalEvent::ENTER))
                return retval;
        }
        if (treeEdges_) {
            if (auto retval = treeEdges_->template traverse<T>(visitor))
                return retval;
        }
        if (vertex) {
            return visitor(vertex, TraversalEvent::LEAVE);
        } else {
            return decltype(visitor(vertex, TraversalEvent::ENTER))();
        }
    }

    /** Pre-order forward traversal.
     *
     *  Perform a depth-first pre-order traversal. The functor is called once for each vertex before any of its children are
     *  traversed. This is equivalent to the @ref traverse traversal where the functor checks that the event is an @c ENTER event
     *  before doing anything. If the functor returns a value which evaluates to true in Boolean context, then the traversal
     *  immediately returns that value, otherwise it continues until the entire subtree is visited and returns a default-constructed
     *  value. */
    template<class T, class Visitor>
    auto traversePre(const Visitor &visitor) {
        return traverse<T>([&visitor] (const std::shared_ptr<T> &vertex, TraversalEvent event) {
            if (TraversalEvent::ENTER == event) {
                return visitor(vertex);
            } else {
                return decltype(visitor(vertex))();
            }
        });
    }

    /** Post-order forward traversal.
     *
     *  Perform a depth-first post-order traversal. The functor is called once for each vertex all of its children are
     *  traversed. This is equivalent to the @ref traverse traversal where the functor checks that the event is an @c LEAVE event
     *  before doing anything. If the functor returns a value which evaluates to true in Boolean context, then the traversal
     *  immediately returns that value, otherwise it continues until the entire subtree is visited and returns a default-constructed
     *  value. */
    template<class T, class Visitor>
    auto traversePost(const Visitor &visitor) {
        return traverse<T>([&visitor] (const std::shared_ptr<T> &vertex, TraversalEvent event) {
            if (TraversalEvent::LEAVE == event) {
                return visitor(vertex);
            } else {
                return decltype(visitor(vertex))();
            }
        });
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

    /** Returns the pointer for a child.
     *
     *  Returns the pointer for the child at index @p i. If @p i is out of range, then a null pointer is returned, which is
     *  indistinguishable from the case when a valid index is specified but that child is a null pointer. */
    UserBasePtr child(size_t i) const;

    /** Returns the number of children.
     *
     *  This is the number of children for this class and the base class, recursively. Some children may be null pointers. */
    size_t nChildren() const;
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
    : EdgeBase(parent.treeEdges_, parent) {
    this->parent_.treeEdges_ = this;
}

#ifndef DOCUMENTATION                                   // doxygen has problems with this
template<class B>
template<class T>
Vertex<B>::Edge<T>::Edge(UserBase &parent, const std::shared_ptr<T> &child)
    : EdgeBase(parent.treeEdges_, parent), child_(child) {
    checkChildInsertion(child);
    this->parent_.treeEdges_ = this;                    // must be after checkChildInsertin for exception safety
    if (child)
        child->parent.set(parent);
}
#endif

#ifndef DOCUMENTATION                                   // doxygen has problems with this
template<class B>
template<class T>
Vertex<B>::Edge<T>::Edge(Link link, UserBase &parent, const std::shared_ptr<T> &child)
    : EdgeBase(Link::YES == link ? parent.treeEdges_ : nullptr, parent), child_(child) {
    checkChildInsertion(child);
    if (Link::YES == link)
        this->parent_.treeEdges_ = this;                // must be after checkChildInsertin for exception safety
    if (child)
        child->parent.set(parent);
}
#endif

template<class B>
template<class T>
void
Vertex<B>::Edge<T>::checkChildInsertion(const std::shared_ptr<T> &child) const {
    if (child) {
        if (child->parent)
            throw InsertionError(child);
#ifndef NDEBUG
        for (const UserBase *p = &this->parent_; p; p = p->parent().get()) {
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
    return child_.get() == other().get();
}

template<class B>
template<class T>
template<class U>
bool
Vertex<B>::Edge<T>::operator!=(const Edge<U> &other) const {
    return child_.get() != other().get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations for EdgeVector<T>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class B>
template<class T>
Vertex<B>::EdgeVector<T>::EdgeVector(UserBase &parent)
    : EdgeBase(parent.treeEdges_, parent) {
    this->parent_.treeEdges_ = this;
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
template<class T>
std::shared_ptr<T>
Vertex<B>::isa() {
    return std::dynamic_pointer_cast<T>(this->shared_from_this());
}

template<class B>
typename Vertex<B>::UserBasePtr
Vertex<B>::child(size_t i) const {
    std::vector<EdgeBase*> edges;
    for (const EdgeBase *edge = treeEdges_; edge; edge = edge->prev)
        edges.push_back(edge);
    for (const EdgeBase *edge: boost::adaptors::reverse(edges)) {
        if (i < edge->size()) {
            return edge->pointer(i);
        } else {
            i -= edge->size();
        }
    }
    return {};
}

template<class B>
size_t
Vertex<B>::nChildren() const {
    size_t n = 0;
    for (const EdgeBase *edge = treeEdges_; edge; edge = edge->prev)
        n += edge->size();
    return n;
}

} // namespace
} // namespace
#endif
