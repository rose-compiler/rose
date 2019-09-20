// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_IndexedList_H
#define Sawyer_IndexedList_H

#include <Sawyer/Assert.h>
#include <Sawyer/DefaultAllocator.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Sawyer.h>

#include <boost/range/iterator_range.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <iterator>
#include <vector>

namespace Sawyer {
namespace Container {

/** Traits for indexed lists. */
template<class T>
struct IndexedListTraits {
    typedef typename T::NodeIterator NodeIterator;
    typedef typename T::ValueIterator ValueIterator;
};

template<class T>
struct IndexedListTraits<const T> {
    typedef typename T::ConstNodeIterator NodeIterator;
    typedef typename T::ConstValueIterator ValueIterator;
};

/** Doubly-linked list with constant-time indexing.
 *
 *  This container is a hybrid of a doubly-linked list and a vector, having these features:
 *
 *  @li A new element can be inserted at any position in the list in constant time.
 *
 *  @li Any element can be erased (removed from the list) in constant time.
 *
 *  @li Each element has a unique ID number.
 *
 *  @li Finding an element by its ID number is a contant-time operation.
 *
 *  Element ID numbers are consecutive unsigned integers between zero (inclusive) and the size of the list (exclusive), and are
 *  convenient for indexing into lookup tables that the user keeps separate from the list itself.  However, in order to meet
 *  the requirement the erasure is a constant-time operation, when an element is erased from the list, the element that had the
 *  largest ID number is given the ID of the element that was erased.  In other words, ID numbers are not stable across
 *  erasure.  This is actually not much different than using array indices as ID numbers and erasing an item from the middle of
 *  the array, except that the approach taken by IndexedList is contant rather than linear time.
 *
 *  Also, the order of ID numbers is not required to be identical to the order of the elements in the list.  One may iterate
 *  over the elements in list order by using iterators, or iterate over the elements in ID order by using a simple "for" loop.
 *
 * @code
 *  IndexedList<MyClass> list;
 *
 *  // Iteration in list order
 *  for (MyClass::Iterator i=list.begin(); i!=list.end(); ++i)
 *      std::cout <<"member #" <<i.id() <<" = " <<*i <<"\n";
 *
 *  // Iteration in ID order
 *  for (size_t i=0; i<list.size(); ++i)
 *      std::cout <<"member #" <<i <<" = " <<list[i] <<"\n";
 * @endcode
 *
 *  This container, as with the rest of the library, uses CamelCase names for types and methods, where types begin with an
 *  upper-case letter and methods begin with a lower-case letter.  This may be surprising for users accustomed to the STL
 *  naming scheme.  For instance, iterators are named <code>Iterator</code>, <code>ConstIterator</code>, etc., rather than
 *  <code>iterator</code>, <code>const_iterator</code>, etc. */
template<class T, class Alloc = DefaultAllocator>
class IndexedList {
public:
    typedef T Value;                                    /**< Type of values stored in this container. */
    typedef Alloc Allocator;                            /**< Allocator for the storage nodes. */
    class Node;                                         /**< Type of node holding values. */

private:

    static const size_t NO_ID = (size_t)(-1);


    // Forms a circular list. ProtoNode is the first part of a Node so that we can static_cast from ProtoNode to Node.
    class ProtoNode {
    public:
        size_t id;                                      // Unique, small, contiguous ID numbers
        ProtoNode *next, *prev;                         // Linkage to neighoring nodes
        explicit ProtoNode(size_t id=NO_ID): id(id), next(this), prev(this) {}
        bool isHead() const { return id==NO_ID; }       // implies no Node memory is attached; don't static_cast to Node!
        void insert(ProtoNode &newNode) {               // insert newNode before this node
            ASSERT_forbid(newNode.isHead());
            ASSERT_require(newNode.next==&newNode);
            ASSERT_require(newNode.prev==&newNode);
            prev->next = &newNode;
            newNode.prev = prev;
            prev = &newNode;
            newNode.next = this;
        }
        void remove() {                                 // remove this node from the list
            ASSERT_forbid(isHead());
            prev->next = next;
            next->prev = prev;
            next = prev = this;
        }
        Node& dereference() {
            ASSERT_forbid(isHead());
            Node *node = (Node*)this;                   // ProtoNode must be first data member of Node
            ASSERT_require(&node->linkage_ == this);    // it wasn't first
            return *node;
        }
        const Node& dereference() const {
            ASSERT_forbid(isHead());
            const Node *node = (const Node*)this;       // ProtoNode must be first data member of Node
            ASSERT_require(&node->linkage_ == this);    // it wasn't first
            return *node;
        }
    };

public:
    /** Combination user-defined value and ID number.
     *
     *  This class represents the user-defined value and an ID number and serves as the type of object stored by the underlying
     *  list. */
    class Node {
        ProtoNode linkage_;                             // This member MUST BE FIRST so ProtoNode::dereference works
        Value value_;                                   // User-supplied data for each node
    private:
        friend class IndexedList;
        Node(size_t id, const Value &value): linkage_(id), value_(value) { ASSERT_forbid(linkage_.isHead()); }
    public:
        /** Unique identification number.
         *
         *  Obtains the unique (within this container) identification number for a storage node.  Identification numbers are
         *  zero through the size of the list (exclusive) but not necessarily in the same order as the nodes of the list.  IDs
         *  are stable across insertion but not erasure. When a node is erased from the list, the node that had the highest ID
         *  number (if not the one being erased) is renumbered to fill the gap left by the one that was erased. */
        const size_t& id() const { return linkage_.id; }

        /** Accessor for user-defined value.
         *
         *  The user defined value can be accessed by this method, which returns either a reference to a const value or a
         *  reference to a mutable value depending on whether this container is const or mutable.
         *
         *  @{ */
        Value& value() { return value_; }
        const Value& value() const { return value_; }
        Value& operator*() { return value_; }
        const Value& operator*() const { return value_; }
        Value* operator->() { return &value_; }
        const Value* operator->() const { return &value_; }
        /** @} */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Iterators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    template<class Derived, class Value, class BaseIterator>
    class IteratorBase: public std::iterator<std::bidirectional_iterator_tag, Value> {
    protected:
        BaseIterator base_;
        IteratorBase() { base_ = NULL; }
        IteratorBase(const IteratorBase &other): base_(other.base_) {}
        IteratorBase(const BaseIterator &base): base_(base) {}
    public:
        bool isAtEnd() const { return base_->isHead(); }
        Derived& operator++() { base_ = base_->next; return *derived(); }
        Derived operator++(int) { Derived old(this->base_); base_ = base_->next; return old; }
        Derived& operator--() { base_ = base_->prev; return *derived(); }
        Derived operator--(int) { Derived old(this->base_); base_ = base_->prev; return old; }
        template<class OtherIter> bool operator==(const OtherIter &other) const { return base_ == other.base(); }
        template<class OtherIter> bool operator!=(const OtherIter &other) const { return base_ != other.base(); }
    protected:
        Derived* derived() { return static_cast<Derived*>(this); }
        const Derived* derived() const { return static_cast<const Derived*>(this); }
    public:
        const BaseIterator& base() const { return base_; }
    };

public:
    /** List node bidirectional iterator.
     *
     *  This iterator traverse the elements of the list in the order they are stored in the list and returns a reference to a
     *  storage node (@ref Node, a user-defined value and associated ID number) when dereferenced.  If one wants to traverse
     *  elements in the order of the ID numbers instead, just use a "for" loop to iterate from zero to the number of items in
     *  the list and make use of the constant-time lookup-by-ID feature.
     *
     *  Iterators are stable across insertion and erasure.  In other words, an iterator is guaranteed to not become invalid
     *  when other elements are added to or removed from the container.  Added elements will become part of any existing
     *  iterator traversals when they are inserted between that iterator's current and ending position.
     *
     *  Node ID numbers are accessed from the iterator by invoking its <code>id</code> method.  ID numbers are stable across
     *  insertion, but not erasure; erasing an element from the list will cause zero or one other element to receive a new ID
     *  number, and this new ID will be immediately reflected in all existing iterators that point to the affected node. */
    class NodeIterator: public IteratorBase<NodeIterator, Node, ProtoNode*> {
        typedef                IteratorBase<NodeIterator, Node, ProtoNode*> Super;
    public:
        NodeIterator() {}
        NodeIterator(const NodeIterator &other): Super(other) {}
        Node& operator*() const { return this->base_->dereference(); }
        Node* operator->() const { return &this->base_->dereference(); }
    private:
        friend class IndexedList;
        NodeIterator(ProtoNode *base): Super(base) {}
        NodeIterator(Node *node): Super(&node->linkage_) {}
    };

    /** List const node bidirectional iterator.
     *
     *  This iterator traverse the elements of the list in the order they are stored in the list and returns a reference to a
     *  const storage node (@ref Node, a user-defined value and associated ID number) when dereferenced.  If one wants to
     *  traverse elements in the order of the ID numbers instead, just use a "for" loop to iterate from zero to the number of
     *  items in the list and make use of the constant-time lookup-by-ID feature.
     *
     *  Iterators are stable across insertion and erasure.  In other words, an iterator is guaranteed to not become invalid
     *  when other elements are added to or removed from the container.  Added elements will become part of any existing
     *  iterator traversals when they are inserted between that iterator's current and ending position.
     *
     *  Node ID numbers are accessed from the iterator by invoking its <code>id</code> method.  ID numbers are stable across
     *  insertion, but not erasure; erasing an element from the list will cause zero or one other element to receive a new ID
     *  number, and this new ID will be immediately reflected in all existing iterators that point to the affected node. */
    class ConstNodeIterator: public IteratorBase<ConstNodeIterator, const Node, const ProtoNode*> {
        typedef                     IteratorBase<ConstNodeIterator, const Node, const ProtoNode*> Super;
    public:
        ConstNodeIterator() {}
        ConstNodeIterator(const ConstNodeIterator &other): Super(other) {}
        ConstNodeIterator(const NodeIterator &other): Super(other.base()) {}
        const Node& operator*() const { return this->base_->dereference(); }
        const Node* operator->() const { return &this->base_->dereference(); }
    private:
        friend class IndexedList;
        ConstNodeIterator(const ProtoNode *base): Super(base) {}
        ConstNodeIterator(const Node *node): Super(&node->linkage_) {}
    };

    /** List value bidirectional iterator.
     *
     *  This iterator traverse the elements of the list in the order they are stored in the list and returns a reference to the
     *  user-defined value when dereferenced.  If one wants to traverse elements in the order of the ID numbers instead, just
     *  use a "for" loop to iterate from zero to the number of items in the list and make use of the constant-time lookup-by-ID
     *  feature.
     *
     *  Iterators are stable across insertion and erasure.  In other words, an iterator is guaranteed to not become invalid
     *  when other elements are added to or removed from the container.  Added elements will become part of any existing
     *  iterator traversals when they are inserted between that iterator's current and ending position. */
    class ValueIterator: public IteratorBase<ValueIterator, Value, ProtoNode*> {
        typedef                 IteratorBase<ValueIterator, Value, ProtoNode*> Super;
    public:
        ValueIterator() {}
        ValueIterator(const ValueIterator &other): Super(other) {}
        ValueIterator(const NodeIterator &other): Super(other.base()) {}
        Value& operator*() const { return this->base()->dereference().value(); }
        Value* operator->() const { return &this->base()->dereference().value(); }
    private:
        friend class IndexedList;
        ValueIterator(ProtoNode *base): Super(base) {}
        ValueIterator(Node *node): Super(&node->linkage_) {}
    };

    /** List const value bidirectional iterator.
     *
     *  This iterator traverse the elements of the list in the order they are stored in the list and returns a reference to the
     *  user-defined const value when dereferenced.  If one wants to traverse elements in the order of the ID numbers instead,
     *  just use a "for" loop to iterate from zero to the number of items in the list and make use of the constant-time
     *  lookup-by-ID feature.
     *
     *  Iterators are stable across insertion and erasure.  In other words, an iterator is guaranteed to not become invalid
     *  when other elements are added to or removed from the container.  Added elements will become part of any existing
     *  iterator traversals when they are inserted between that iterator's current and ending position. */
    class ConstValueIterator: public IteratorBase<ConstValueIterator, const Value, const ProtoNode*> {
        typedef                      IteratorBase<ConstValueIterator, const Value, const ProtoNode*> Super;
    public:
        ConstValueIterator() {}
        ConstValueIterator(const ConstValueIterator &other): Super(other) {}
        ConstValueIterator(const NodeIterator &other): Super(other.base()) {}
        ConstValueIterator(const ConstNodeIterator &other): Super(other.base()) {}
        ConstValueIterator(const ValueIterator &other): Super(other.base()) {}
        const Value& operator*() const { return this->base()->dereference().value(); }
        const Value* operator->() const { return &this->base()->dereference().value(); }
    private:
        friend class IndexedList;
        ConstValueIterator(const ProtoNode *base): Super(base) {}
        ConstValueIterator(const Node *node): Super(&node->linkage_) {}
    };
    /** @} */

private:
    Allocator allocator_;                               // provided allocator for list nodes
    ProtoNode *head_;                                   // always point to a list head, not a true node (normal allocator)
    typedef std::vector<Node*> Index;                   // allocated with provided allocator
    Index index_;                                       // allocated with normal allocator

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        size_t n = size();
        s <<BOOST_SERIALIZATION_NVP(n);
        for (const ProtoNode *pnode = head_->next; pnode != head_; pnode = pnode->next) {
            ASSERT_require(n-- > 0);
            size_t id = pnode->dereference().id();
            const Value &value = pnode->dereference().value();
            s <<BOOST_SERIALIZATION_NVP(id);
            s <<BOOST_SERIALIZATION_NVP(value);
        }
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        clear();
        size_t n = 0;
        s >>BOOST_SERIALIZATION_NVP(n);
        ASSERT_require(index_.empty());
        index_.resize(n, NULL);
        for (size_t i=0; i<n; ++i) {
            size_t id = 0;
            s >>BOOST_SERIALIZATION_NVP(id);
            Node *node = new (allocator_.allocate(sizeof(Node))) Node(id, Value());
            s >>boost::serialization::make_nvp("value", node->value());

            ASSERT_require(id < index_.size());
            ASSERT_require(index_[id] == NULL);
            index_[id] = node;

            head_->insert(node->linkage_);              // append to end of node list
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

        
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Initialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Default constructor.
     *
     *  Create a new list which is empty. */
    explicit IndexedList(const Allocator &allocator = Allocator())
        : allocator_(allocator), head_(new ProtoNode) {}

    /** Copy constructor.
     *
     *  The newly constructed list's allocator is copy-constructed from the source list's allocator.  For allocators that
     *  contain state (like pool allocators), this copies the settings but not the pools of allocated data. */
    IndexedList(const IndexedList &other): allocator_(other.allocator_), head_(new ProtoNode) {
        for (ConstValueIterator otherIter=other.values().begin(); otherIter!=other.values().end(); ++otherIter)
            pushBack(*otherIter);
    }

    /** Copy constructor. */
    template<class T2, class Alloc2>
    IndexedList(const IndexedList<T2, Alloc2> &other, const Allocator &/*allocator*/ = Allocator())
        : allocator_(Allocator()), head_(new ProtoNode) {
        typedef typename IndexedList<T2>::ConstValueIterator OtherIter;
        for (OtherIter otherIter=other.values().begin(); otherIter!=other.values().end(); ++otherIter)
            pushBack(Value(*otherIter));
    }

    /** Filling constructor.
     *
     *  Constructs the list by inserting @p nElmts copies of @p val. */
    explicit IndexedList(size_t nElmts, const Value &val = Value(), const Allocator &allocator = Allocator())
        : allocator_(allocator), head_(new ProtoNode) {
        index_.reserve(nElmts);
        for (size_t i=0; i<nElmts; ++i)
            pushBack(val);
    }

    /** Assignment from another list.
     *
     *  Causes this list to look like the @p other list in that this list has copies of the nodes of the other list. However,
     *  the ID numbers in this list may be different than the ID numbers of the other list. */
    IndexedList& operator=(const IndexedList &other) {
        clear();
        insertMultiple(nodes().begin(), other.values());
        return *this;
    }

    /** Assignment from another list.
     *
     *  Causes this list to look like the @p other list in that this list has copies of the nodes of the other list. However,
     *  the ID numbers in this list may be different than the ID numbers of the other list. */
    template<class T2>
    IndexedList& operator=(const IndexedList<T2> &other) {
        clear();
        insert(nodes().begin(), other.values());
        return *this;
    }

    ~IndexedList() {
        clear();
        delete head_;
    }

    /** Allocator.
     *
     *  Returns a reference to the allocator that's being used for elements of this list. */
    const Allocator& allocator() const {
        return allocator_;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Iteration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** All elements.
     *
     *  Returns an iterator range that references all nodes in the container in their list order.
     *
     * @{ */
    boost::iterator_range<NodeIterator> nodes() {
        return boost::iterator_range<NodeIterator>(NodeIterator(head_->next), NodeIterator(head_));
    }
    boost::iterator_range<ConstNodeIterator> nodes() const {
        return boost::iterator_range<ConstNodeIterator>(ConstNodeIterator(head_->next), ConstNodeIterator(head_));
    }
    boost::iterator_range<ValueIterator> values() {
        return boost::iterator_range<ValueIterator>(ValueIterator(head_->next), ValueIterator(head_));
    }
    boost::iterator_range<ConstValueIterator> values() const {
        return boost::iterator_range<ConstValueIterator>(ConstValueIterator(head_->next), ConstValueIterator(head_));
    }
    /** @} */

private:
    bool isLocalIterator(const ConstValueIterator &iter) const {
        const ProtoNode *pn = iter.base();
        ASSERT_not_null(pn);
        if (pn == head_)
            return true;                                // local end iterator
        const Node *node = &pn->dereference();
        size_t id = node->id();
        return id<index_.size() && node==index_[id];
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Size and capacity
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Determines if this list is empty.
     *
     *  Returns true only if this list contains no elements. */
    bool isEmpty() const {
        return index_.empty();
    }

    /** Number of elements in list.
     *
     *  Returns the number of elements stored in the list in constant time. */
    size_t size() const {
        return index_.size();
    }

    /** Allocated capacity of the index.
     *
     *  Each container maintains an index in order to achieve constant time lookup by node ID, and this method queries or
     *  changes the number of elements that the index is prepared to handle.  If more elements are added to the index than it
     *  is prepared to handle, then a new index is created in linear time.
     *
     * @{ */
    size_t capacity() const {
        return index_.capacity();
    }
    void capacity(size_t n) {
        index_.reserve(n);
    }
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Searching
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Lookup by ID.
     *
     *  Returns a list iterator for the node with the specified ID number.  The ID number must exist; i.e., it must be less
     *  than the number of elements in the list.  This method executes in constant time.
     *
     * @{ */
    NodeIterator find(size_t id) {
        ASSERT_require(id < index_.size());
        ASSERT_not_null(index_[id]);
        return NodeIterator(index_[id]);
    }
    ConstNodeIterator find(size_t id) const {
        ASSERT_require(id < index_.size());
        ASSERT_not_null(index_[id]);
        return ConstNodeIterator(index_[id]);
    }
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Accessors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** First element of list.
     *
     *  Returns a reference to the first element stored in this list.  The list must not be empty.
     *
     * @{ */
    Node& frontNode() {
        ASSERT_forbid(isEmpty());
        return head_->next->dereference();
    }
    const Node& frontNode() const {
        ASSERT_forbid(isEmpty());
        return head_->next->dereference();
    }
    Value& frontValue() {
        return frontNode().value();
    }
    const Value& frontValue() const {
        return frontNode().value();
    }
    /** @} */

    /** Last element of list.
     *
     *  Returns a reference to the last element stored in this list.  The list must not be empty.
     *
     * @{ */
    Node& backNode() {
        ASSERT_forbid(isEmpty());
        return head_->prev->dereference();
    }
    const Node& backNode() const {
        ASSERT_forbid(isEmpty());
        return head_->prev->dereference();
    }
    Value& backValue() {
        return backNode().value();
    }
    const Value& backValue() const {
        return backNode().value();
    }
    /** @} */

    /** Element reference by ID.
     *
     *  Returns a reference to the element with the specified ID number.  The ID number must exist in this list.  ID numbers
     *  are consecutive, beginning at zero.
     *
     * @{ */
    Node& indexedNode(size_t id) {
        ASSERT_require(id < size());
        ASSERT_not_null(index_[id]);
        return *index_[id];
    }
    const Node& indexedNode(size_t id) const {
        ASSERT_require(id < size());
        ASSERT_not_null(index_[id]);
        return *index_[id];
    }
    Value& indexedValue(size_t id) {
        return indexedNode(id).value();
    }
    const Value& indexedValue(size_t id) const {
        return indexedNode(id).value();
    }
    Value& operator[](size_t id) {
        return indexedValue(id);
    }
    const Value& operator[](size_t id) const {
        return indexedValue(id);
    }

    Optional<Value> getOptional(size_t id) const {
        return id < size() ? Optional<Value>(indexedValue(id)) : Optional<Value>();
    }

    Value& getOrElse(size_t id, Value &dflt) {
        return id < size() ? indexedValue(id) : dflt;
    }
    const Value& getOrElse(size_t id, const Value &dflt) const {
        return id < size() ? indexedValue(id) : dflt;
    }

    const Value& getOrDefault(size_t id) const {
        static const Value dflt;
        return id < size() ? indexedValue(id) : dflt;
    }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Mutators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Insert at the front of the list.
     *
     *  Inserts a copy of the @p value at the beginning of the list.  The new copy is given an ID number which one larger than
     *  the previously largest ID number in this list.  No other element ID numbers are changed by this operation. */
    IndexedList& pushFront(const Value &value) {
        insert(nodes().begin(), value);
        return *this;
    }

    /** Insert at the back of the list.
     *
     *  Inserts a copy of the @p value at the end of the list.  The new copy is given an ID number which one larger than the
     *  previously largest ID number in this list.  No other element ID numbers are changed by this operation. */
    IndexedList& pushBack(const Value &value) {
        insert(nodes().end(), value);
        return *this;
    }

    /** Insert element at position.
     *
     *  Inserts a copy of @p value at the indicated position in the list.  The new copy is given an ID number which one larger
     *  than the previously largest ID number in this list.  No other element ID numbers are changed by this operation. */
    NodeIterator insert(const ValueIterator &position, const Value &value) {
        ProtoNode *pos = position.base();
        Node *node = new (allocator_.allocate(sizeof(Node))) Node(index_.size(), value);
        index_.push_back(node);
        pos->insert(node->linkage_);
        return NodeIterator(node);
    }

    /** Insert multiple copies at position.
     *
     *  Inserts @p nElmts copies of @p value at the indicated position.  The new copies are given sequential ID numbers in the
     *  order they are inserted, with the first inserted element having an ID number which is one larger than the previously
     *  largest ID number in this list.  No other element ID numbers are changed by this operation. */
    IndexedList& insert(const ValueIterator &position, size_t nElmts, const Value &value) {
        for (size_t i=0; i<nElmts; ++i)
            insert(position, value);
        return *this;
    }

    /** Insert elements at position.
     *
     *  Inserts, at the indicated position, copies of the elements specified by the iterator range.  The new copies are given
     *  sequential ID numbers in the order they are inserted, with the first inserted element having an ID number which is one
     *  larger than the previously largest ID number in this list.  No other element ID numbers are changed by this
     *  operation. */
    template<class OtherValueIterator>
    IndexedList& insertMultiple(const ValueIterator &position, const boost::iterator_range<OtherValueIterator> &range) {
        for (OtherValueIterator otherIter=range.begin(); otherIter!=range.end(); ++otherIter)
            insert(position, Value(*otherIter));
        return *this;
    }


    /** Empties the list.
     *
     *  Clears the list by removing all elements from it. This operation is linear time. */
    void clear() {
        for (size_t i=0; i<index_.size(); ++i) {
            index_[i]->~Node();
            allocator_.deallocate((void*)index_[i], sizeof(Node));
        }
        index_.clear();
        head_->next = head_->prev = head_;
    }

    /**  Erase one element.
     *
     *   Erases the element having the specified ID. The ID number must exist in the list. */
    NodeIterator erase(size_t id) {
        ASSERT_require(id < size());
        return eraseAt(find(id));
    }

    /** Remove one element.
     *
     *  Removes the element at the indicated position.  The ID number of one other element (the one with the now highest ID)
     *  will be changed to fill the gap left by the erasure. */
    NodeIterator eraseAt(const ValueIterator &position) {
        ASSERT_require(isLocalIterator(position));
        ProtoNode *pos = position.base();
        ProtoNode *next = pos->next;                    // to build the return value
        pos->remove();                                  // remove node from the doubly linked list w/out deleting
        size_t id = pos->id;
        if (id + 1 < index_.size()) {
            std::swap(index_.back(), index_[id]);
            index_[id]->linkage_.id = id;
        }
        index_.back()->~Node();
        allocator_.deallocate(index_.back(), sizeof(Node));
        index_.pop_back();
        return NodeIterator(next);
    }

    // range must be a range within this container
    NodeIterator eraseAtMultiple(const boost::iterator_range<NodeIterator> &range) {
        boost::iterator_range<ValueIterator> valueRange(range.begin(), range.end());
        return eraseAtMultiple(valueRange);
    }
    NodeIterator eraseAtMultiple(const boost::iterator_range<ValueIterator> &range) {
        ValueIterator otherIter = range.begin();
        while (otherIter!=range.end())
            otherIter = eraseAt(otherIter);
        return NodeIterator(range.end().base());
    }

    // debugging
    void dump(std::ostream &o) const {
        o <<"list contents:\n"
          <<"  index:\n";
        for (size_t i=0; i<index_.size(); ++i)
            o <<"    [" <<i <<"]=" <<index_[i] <<"\n";
        o <<"  list:\n";
        ProtoNode *pn = head_;
        for (size_t i=0; i<index_.size()+1; ++i, pn=pn->next) {
            o <<"    " <<pn <<"\t" <<pn->next <<"\t" <<pn->prev <<"\t" <<pn->id <<"\n";
            ASSERT_require(pn->isHead() || index_[pn->id]==&pn->dereference());
            ASSERT_require(pn->next->prev == pn);
            ASSERT_require(pn->prev->next == pn);
        }
    }
};

} // namespace
} // namespace

#endif
