// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_GraphIteratorBiMap_H
#define Sawyer_GraphIteratorBiMap_H

#include <Sawyer/GraphIteratorMap.h>

namespace Sawyer {
namespace Container {

/** Bidirectional map of graph edge or vertex pointers.
 *
 *  A bi-directional 1:1 mapping between edges or vertices of one graph to edges or vertices of another graph. The edges or
 *  vertices are represented by iterators (pointers) and indexed internally by their ID numbers. Since ID numbers are not stable
 *  over erasure, whenever any of either graph's edges or vertices are erased (depending on whether this container holds edges or
 *  vertices) the @ref updateIdNumbers should be called. */
template<class LhsIterator, class RhsIterator>
class GraphIteratorBiMap {
public:
    typedef GraphIteratorMap<LhsIterator, RhsIterator> ForwardMap;
    typedef GraphIteratorMap<RhsIterator, LhsIterator> ReverseMap;

private:
    ForwardMap forward_;
    ReverseMap reverse_;

public:
    /** Default constructor. */
    GraphIteratorBiMap() {}
    
    /** Construct a new map by composition of two maps.
     *
     *  Given two BiMap objects where the range type of the first is the domain type of the second, construct a new BiMap
     *  from the domain of the first to the range of the second.  The new map will contain only those domain/range pairs that
     *  map across both input maps. */
    template<class U>
    GraphIteratorBiMap(const GraphIteratorBiMap<LhsIterator, U> &a, const GraphIteratorBiMap<U, RhsIterator> &b) {
        BOOST_FOREACH (const typename ForwardMap::Node &anode, a.forward_.nodes()) {
            if (b.forward_.exists(anode.value())) {
                const RhsIterator &target = b.forward_[anode.value()];
                insert(anode.key(), target);
            }
        }
    }

    /** Indicate that an update is necessary due to erasures.
     *
     *  If the graph whose iterators are stored in this container has any edges or vertices that are erased (whichever type
     *  are stored in this container), then this function should be called to tell the container that the ID numbers of its
     *  contained iterators have possibly changed.
     *
     *  The actual re-sorting of this container will be delayed as long as possible, but logically the user can assume that
     *  it occurs immediately. */
    void updateIdNumbers() {
        forward_.updateIdNumbers();
        reverse_.updateIdNumbers();
    }

    /** Insert a mapping from edge or vertex @p a to edge or vertex @p b. */
    void insert(const LhsIterator &a, const RhsIterator &b) {
        if (Sawyer::Optional<RhsIterator> found = forward_.find(a)) {
            if (*found == b)
                return;
            reverse_.erase(b);
        }
        forward_.insert(a, b);
        reverse_.insert(b, a);
    }

    /** Erase a pair based on the left hand side. */
    void eraseSource(const LhsIterator &a) {
        if (Sawyer::Optional<RhsIterator> found = forward_.find(a)) {
            reverse_.erase(*found);
            forward_.erase(a);
        }
    }

    /** Erase a pair based on the right hand side. */
    void eraseTarget(const RhsIterator &b) {
        if (Sawyer::Optional<LhsIterator> found = reverse_.find(b)) {
            forward_.erase(*found);
            reverse_.erase(b);
        }
    }
    
    /** Return the forward mapping. */
    const ForwardMap& forward() const {
        return forward_;
    }

    /** Return the reverse mapping. */
    const ReverseMap& reverse() const {
        return reverse_;
    }
    
    /** Remove all entries from this container. */
    void clear() {
        forward_.clear();
        reverse_.clear();
    }
};

} // namespace
} // namespace

#endif
