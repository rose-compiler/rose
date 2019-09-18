// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_GraphIteratorSet_H
#define Sawyer_GraphIteratorSet_H

#include <Sawyer/Graph.h>

namespace Sawyer {
namespace Container {

/** Set of graph edge or vertex pointers (iterators).
 *
 *  This container holds a set of unique edge or vertex iterators for a single graph. Iterators form a total order by comparing their
 *  ID numbers, which is why all iterators in this container must belong to the same graph.  This container is able to store only
 *  valid iterators, not end iterators.  Attempting to insert an iterator with the same ID number as one that already exists is a
 *  no-op.
 *
 *  Since ID numbers are not stable over erasure, this container must be notified whenever edges or vertices (whichever are
 *  stored by this container), are removed from the graph, even if the items removed from the graph are not the same ones as
 *  what are stored in this container. */
template<class T>
class GraphIteratorSet {
public:
    typedef T Value;                                    /**< Type of values stored in this set. */

private:
    // These members are mutable so that we can delay the sorting until the last possible minute while still appearing to
    // have a const-correct interface.
    typedef std::vector<Value> StlVector;
    mutable StlVector items_;                           // the pointers to graph edges or vertices stored in this container
    mutable bool needsUpdate_;                          // true if the items_ are possibly not sorted

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Iterators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Iterates over values in this set. */
    typedef typename StlVector::const_iterator ConstIterator;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default construct an empty set. */
    GraphIteratorSet()
        : needsUpdate_(false) {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Iteration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Value iterator range.
     *
     *  Returns an iterator range that covers all values in the set. */
    boost::iterator_range<ConstIterator> values() const {
        update();
        return boost::iterator_range<ConstIterator>(items_.begin(), items_.end());
    }

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
    
    /** Insert the specified edge or vertex if its ID doesn't exist in this set. */
    void insert(const Value &item) {
        update();
        insertUnique(item);
    }

    /** Insert multiple edges or vertices. */
    void insert(const GraphIteratorSet &other) {
        update();
        BOOST_FOREACH (const Value &item, other.values()) {
            insert(item);
        }
    }
    
    /** Insert multiple edges or vertices. */
    template<class SrcIterator>
    void insert(const SrcIterator &begin, const SrcIterator &end) {
        update();
        for (SrcIterator i = begin; i != end; ++i)
            insertUnique(*i);
    }

    /** Remove the edge or vertex if it exists. */
    void erase(const Value &item) const {
        update();
        typename std::vector<Value>::iterator lb = std::lower_bound(items_.begin(), items_.end(), item, sortById);
        if (lb != items_.end() && (*lb)->id() == item->id())
            items_.erase(lb);
    }

    /** Removes and returns the least iterator. */
    Value popFront() {
        update();
        ASSERT_forbid(isEmpty());
        Value retval = items_[0];
        items_.erase(items_.begin());
        return retval;
    }
    
    /** Remove all edges or vertices from this set. */
    void clear() {
        items_.clear();
        needsUpdate_ = false;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Does the edge or vertex exist in this container? */
    bool exists(const Value &item) const {
        update();
        typename std::vector<Value>::iterator lb = std::lower_bound(items_.begin(), items_.end(), item, sortById);
        return lb != items_.end() && (*lb)->id() == item->id();
    }

    /** True if container has no edges or vertices. */
    bool isEmpty() const {
        return items_.empty();
    }
    bool empty() const { return isEmpty(); } // undocumented compatibility

    /** Number of items stored in this set. */
    size_t size() const {
        return items_.size();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Internal stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    static bool sortById(const Value &a, const Value &b) {
        return a->id() < b->id();
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
    
    void insertUnique(const Value &item) {
        typename std::vector<Value>::iterator lb = std::lower_bound(items_.begin(), items_.end(), item, sortById);
        if (lb == items_.end() || (*lb)->id() != item->id())
            items_.insert(lb, item);
    }
};

} // namespace
} // namespace

#endif
