// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_GraphIteratorMap_H
#define Sawyer_GraphIteratorMap_H

#include <Sawyer/Graph.h>
#include <Sawyer/Optional.h>
#include <boost/range/iterator_range.hpp>

namespace Sawyer {
namespace Container {

/** Map of graph edge or vertex pointers to some other value.
 *
 *  This container maps edge or vertex pointers (iterators) to values of some other type, @p T.  Iterators form a total order
 *  by comparing their ID numbers, which is why all iterators in this container must belong to the same graph.  This container
 *  is able to store only valid iterators, not end iterators.  Attempting to insert an iterator with the same ID number as one
 *  that already exists is a no-op.
 *
 *  Since ID numbers are not stable over erasure, this container must be notified whenever edges or vertices (whichever are
 *  stored by this container), are removed from the graph, even if the items removed from the graph are not the same ones as
 *  what are stored in this container. */
template<class K, class V>
class GraphIteratorMap {
public:
    typedef K Key;                                      /**< Graph edge or vertex iterator used as keys. */
    typedef V Value;                                    /**< Type of value associated with each key. */

    /** The data stored at each node of the map. */
    class Node {
        Key key_;                                       /**< Key is the graph edge or vertex iterator. */
        Value value_;                                   /**< User defined value associated with each key. */
    public:
        /** Constructor. */
        Node(const Key &key, const Value &value)
            : key_(key), value_(value) {}

        /** Access the key of this node.
         *
         *  Keys are read-only since they're used to sort the container for efficient lookups. */
        const Key& key() const { return key_; }

        /** Access the value of this node.
         *
         * @{ */
        Value& value() { return value_; }
        const Value& value() const { return value_; }
        /** @} */
    };

private:
    // These members are mutable so that we can delay the sorting until the last possible minute while still appearing to
    // have a const-correct interface.
    typedef std::vector<Node> StlVector;
    mutable StlVector items_;                           // the pointers to graph edges or vertices stored in this container
    mutable bool needsUpdate_;                          // true if the items_ are possibly not sorted

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Iterators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    template<class Derived, class Value, class BaseIterator>
    class BidirectionalIterator: public std::iterator<std::bidirectional_iterator_tag, Value> {
    protected:
        BaseIterator base_;
        BidirectionalIterator() {}
        BidirectionalIterator(const BaseIterator &base): base_(base) {}
    public:
        /** Assignment. */
        Derived& operator=(const Derived &other) {
            base_ = other.base_;
            return *derived();
        }

        /** Pre-increment to next iterator position. */
        Derived& operator++() {
            ++base_;
            return *derived();
        }

        /** Post-increment to next iterator position. */
        Derived operator++(int) {
            Derived old = *derived();
            ++*this;
            return old;
        }

        /** Pre-decrement to previous iterator position. */
        Derived& operator--() {
            --base_;
            return *derived();
        }

        /** Post-decrement to previous iterator position. */
        Derived operator--(int) {
            Derived old = *derived();
            ++*this;
            return old;
        }

        /** Equality predicate.
         *
         *  Two iterators are equal if they point to the same item in the same container, or if they both point to the end
         *  iterator of the same container. */
        template<class OtherIter>
        bool operator==(const OtherIter &other) const {
            return base_ == other.base();
        }

        /** Inequality predicate. */
        template<class OtherIter>
        bool operator!=(const OtherIter &other) const {
            return base_ != other.base();
        }

        const BaseIterator& base() const {
            return base_;
        }
    protected:
        Derived* derived() {
            return static_cast<Derived*>(this);
        }
        const Derived* derived() const {
            return static_cast<const Derived*>(this);
        }
    };

public:
    /** Bidirectional iterator over key/value nodes.
     *
     *  Dereferencing this iterator will return a Node from which both the key and the value can be obtained. Node iterators
     *  are implicitly convertible to both key and value iterators. */
    class NodeIterator: public BidirectionalIterator<NodeIterator, Node, typename StlVector::iterator> {
        typedef                BidirectionalIterator<NodeIterator, Node, typename StlVector::iterator> Super;
    public:
        NodeIterator() {}

        /** Copy constructor. */
        NodeIterator(const NodeIterator &other): Super(other) {} // implicit

        /** Dereference iterator to return a storage node. */
        Node& operator*() const {
            return *this->base();
        }

        /** Pointer to storage node. */
        Node* operator->() const {
            return &*this->base();
        }
    private:
        friend class GraphIteratorMap;
        NodeIterator(const typename StlVector::iterator &base)
            : Super(base) {}
    };

    /** Bidirectional iterator over constant key/value nodes.
     *
     *  Dereferencing this iterator will return a const Node from which both the key and the value can be obtained. Node
     *  iterators are implicitly convertible to both key and value iterators. */
    class ConstNodeIterator: public BidirectionalIterator<ConstNodeIterator, const Node, typename StlVector::const_iterator> {
        typedef                     BidirectionalIterator<ConstNodeIterator, const Node, typename StlVector::const_iterator> Super;
    public:
        ConstNodeIterator() {}

        /** Copy constructor. */
        ConstNodeIterator(const ConstNodeIterator &other) // implicit
            : Super(other) {}

        /** Copy constructor. */
        ConstNodeIterator(const NodeIterator &other)    // implicit
            : Super(typename StlVector::const_iterator(other.base())) {}

        /** Dereference iterator to return a storage node. */
        const Node& operator*() const {
            return *this->base();
        }

        /** Returns a pointer to a storage node. */
        const Node* operator->() const {
            return &*this->base();
        }
    private:
        friend class GraphIteratorMap;
        ConstNodeIterator(const typename StlVector::const_iterator &base)
            : Super(base) {}
        ConstNodeIterator(const typename StlVector::iterator &base)
            : Super(typename StlVector::const_iterator(base)) {}
    };

    /** Bidirectional iterator over keys.
     *
     *  Dereferencing this iterator will return a reference to a const key. Keys cannot be altered while they are a member of
     *  this container. */
    class ConstKeyIterator: public BidirectionalIterator<ConstKeyIterator, const Key, typename StlVector::const_iterator> {
        typedef BidirectionalIterator<ConstKeyIterator, const Key, typename StlVector::const_iterator> Super;
    public:
        ConstKeyIterator() {}

        /** Copy constructor. */
        ConstKeyIterator(const ConstKeyIterator &other) // implicit
            : Super(other) {}

        /** Copy constructor. */
        ConstKeyIterator(const NodeIterator &other)     // implicit
            : Super(typename StlVector::const_iterator(other.base())) {}

        /** Copy constructor. */
        ConstKeyIterator(const ConstNodeIterator &other) // implicit
            : Super(other.base()) {}

        /** Returns the key for the current iterator. */
        const Key& operator*() const {
            return this->base()->key();
        }

        /** Returns a pointer to the key. */
        const Key* operator->() const {
            return &this->base()->key();
        }
    };

    /** Bidirectional iterator over values.
     *
     *  Dereferencing this iterator will return a reference to the user-defined value of the node. Values may be altered in
     *  place while they are members of a container. */
    class ValueIterator: public BidirectionalIterator<ValueIterator, Value, typename StlVector::iterator> {
        typedef                 BidirectionalIterator<ValueIterator, Value, typename StlVector::iterator> Super;
    public:
        ValueIterator() {}

        /** Copy constructor. */
        ValueIterator(const ValueIterator &other)
            : Super(other) {}

        /** Copy constructor. */
        ValueIterator(const NodeIterator &other) // implicit
            : Super(other.base()) {}

        /** Dereference iterator to return the user-defined value. */
        Value& operator*() const {
            return this->base()->value();
        }

        /** Dereference iterator to return address of user-defined value. */
        Value* operator->() const {
            return &this->base()->value();
        }
    };
    
    /** Bidirectional iterator over values.
     *
     *  Dereferencing this iterator will return a reference to the user-defined value of the node. */
    class ConstValueIterator: public BidirectionalIterator<ConstValueIterator, const Value, typename StlVector::const_iterator> {
        typedef BidirectionalIterator<ConstValueIterator, const Value, typename StlVector::const_iterator> Super;
    public:
        ConstValueIterator() {}

        /** Copy constructor. */
        ConstValueIterator(const ConstValueIterator &other)
            : Super(other) {}

        /** Copy constructor. */
        ConstValueIterator(const ValueIterator &other)  // implicit
            : Super(typename StlVector::const_iterator(other.base())) {}

        /** Copy constructor. */
        ConstValueIterator(const ConstNodeIterator &other) // implicit
            : Super(other.base()) {}

        /** Copy constructor. */
        ConstValueIterator(const NodeIterator &other)   // implicit
            : Super(typename StlVector::const_iterator(other.base())) {}

        /** Dereference iterator to return the value of the user-defined data. */
        const Value& operator*() const {
            return this->base()->value();
        }

        /** Dereference iterator to return address of the user-defined data. */
        const Value* operator->() const {
            return &this->base()->value();
        }
    };
    
public:
    // Standard types needed by C++ containers with random access iterators for the C++-style iterating functions like begin()
    // and end().
    typedef typename StlVector::value_type value_type;
    typedef typename StlVector::allocator_type allocator_type;
    typedef typename StlVector::reference reference;
    typedef typename StlVector::pointer pointer;
    typedef typename StlVector::const_pointer const_pointer;
    typedef typename StlVector::iterator iterator;
    typedef typename StlVector::const_iterator const_iterator;
    typedef typename StlVector::reverse_iterator reverse_iterator;
    typedef typename StlVector::const_reverse_iterator const_reverse_iterator;
    typedef typename StlVector::difference_type difference_type;
    typedef typename StlVector::size_type size_type;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default construct an empty map. */
    GraphIteratorMap()
        : needsUpdate_(false) {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mutators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Indicate that an update is necessary due to erasures.
     *
     *  If the graph whose iterators are stored in this container has any edges or vertices that are erased (whichever type
     *  are stored in this container), then this function should be called to tell the container that the ID numbers of its
     *  contained iterators have possibly changed.
     *
     *  The actual re-sorting of this container will be delayed as long as possible, but logically the user can assume that
     *  it occurs immediately. */
    void updateIdNumbers() {
        needsUpdate_ = true;
    }

    /** Insert the specified edge or vertex associated with a value.
     *
     *  If the edge or vertex already exists, then its value is changed, otherwise a new edge or vertex with the associated
     *  value is inserted into the map. Note that this is different behavior than std::map where no new value is inserted if
     *  the key already exists. */
    void insert(const Key &item, const Value &value) {
        update();
        Node node(item, value);
        typename std::vector<Node>::iterator lb = std::lower_bound(items_.begin(), items_.end(), node, sortById);
        if (items_.end() == lb || lb->key()->id() != item->id()) {
            items_.insert(lb, node);
        } else {
            lb->value() = value;
        }
    }

    /** Insert a value only if its key doesn't already exist.
     *
     *  Returns a reference to the value, which should be used immediately. The reference is valid until the next modifying
     *  operation on this object. */
    Value& insertMaybe(const Key &item, const Value &value) {
        update();
        Node node(item, value);
        typename std::vector<Node>::iterator lb = std::lower_bound(items_.begin(), items_.end(), node, sortById);
        if (items_.end() == lb || lb->key()->id() != item->id())
            lb = items_.insert(lb, node);
        return lb->value();
    }

    /** Insert a default value if its key doesn't already exist.
     *
     *  Returns a reference to the value, which should be used immediately. The reference is value until the next modifying
     *  operation on this object. */
    Value& insertMaybeDefault(const Key &item) {
        return insertMaybe(item, Value());
    }

    /** Erase the specified key if it exists. */
    void erase(const Key &item) {
        update();
        Node node(item, Value());
        typename std::vector<Node>::iterator lb = std::lower_bound(items_.begin(), items_.end(), node, sortById);
        if (lb != items_.end() && lb->key()->id() == item->id())
            items_.erase(lb);
    }

    /** Remove all entries from this container. */
    void clear() {
        items_.clear();
        needsUpdate_ = false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Does the key exist in the map? */
    bool exists(const Key &item) const {
        update();
        Node node(item, Value());
        typename std::vector<Node>::const_iterator lb = std::lower_bound(items_.begin(), items_.end(), node, sortById);
        return lb != items_.end() && lb->key()->id() == item->id();
    }
    
    /** Find the value associated with a particular key. */
    Sawyer::Optional<Value> find(const Key &item) const {
        update();
        Node node(item, Value());
        typename std::vector<Node>::const_iterator lb = std::lower_bound(items_.begin(), items_.end(), node, sortById);
        if (lb != items_.end() && lb->key()->id() == item->id()) {
            return lb->value();
        } else {
            return Sawyer::Nothing();
        }
    }

    /** Return the value associated with an existing key. */
    Value operator[](const Key &item) const {
        update();
        return *find(item);
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Iteration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Iterators for container nodes.
     *
     *  This returns a range of node-iterators that will traverse all nodes (key/value pairs) of this container.
     *
     * @{ */
    boost::iterator_range<NodeIterator> nodes() {
        update();
        return boost::iterator_range<NodeIterator>(NodeIterator(items_.begin()), NodeIterator(items_.end()));
    }
    boost::iterator_range<ConstNodeIterator> nodes() const {
        update();
        return boost::iterator_range<ConstNodeIterator>(ConstNodeIterator(items_.begin()), ConstNodeIterator(items_.end()));
    }
    /** @} */

    /** Iterators for container keys.
     *
     *  Returns a range of key-iterators that will traverse the keys of this container.
     *
     * @{ */
    boost::iterator_range<ConstKeyIterator> keys() {
        update();
        return boost::iterator_range<ConstKeyIterator>(NodeIterator(items_.begin()), NodeIterator(items_.end()));
    }
    boost::iterator_range<ConstKeyIterator> keys() const {
        update();
        return boost::iterator_range<ConstKeyIterator>(ConstNodeIterator(items_.begin()), ConstNodeIterator(items_.end()));
    }
    /** @} */

    /** Iterators for container values.
     *
     *  Returns a range of iterators that will traverse the user-defined values of this container.  The values are iterated in
     *  key order, although the keys are not directly available via these iterators.
     *
     * @{ */
    boost::iterator_range<ValueIterator> values() {
        update();
        return boost::iterator_range<ValueIterator>(NodeIterator(items_.begin()), NodeIterator(items_.end()));
    }
    boost::iterator_range<ConstValueIterator> values() const {
        update();
        return boost::iterator_range<ConstValueIterator>(ConstNodeIterator(items_.begin()), ConstNodeIterator(items_.end()));
    }
    /** @} */

    // Undocumented C++-style iterators iterator over the key+value nodes.
    NodeIterator begin() { return NodeIterator(items_.begin()); }
    ConstNodeIterator begin() const { return NodeIterator(items_.begin()); }
    NodeIterator end() { return NodeIterator(items_.end()); }
    ConstNodeIterator end() const { return NodeIterator(items_.end()); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Internal functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    static bool sortById(const Node &a, const Node &b) {
        return a.key()->id() < b.key()->id();
    }

    void update() const {
        if (needsUpdate_) {
            std::sort(items_.begin(), items_.end(), sortById);
            needsUpdate_ = false;
        } else {
            check();
        }
    }

    void check() const {
        for (size_t i = 1; i < items_.size(); ++i)
            ASSERT_require(sortById(items_[i-1], items_[i]));
    }
};

} // namespace
} // namespace

#endif
