#ifndef Sawyer_IntervalSet_H
#define Sawyer_IntervalSet_H

#include <sawyer/IntervalMap.h>
#include <sawyer/Optional.h>
#include <sawyer/Sawyer.h>

#include <boost/integer_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace Sawyer {
namespace Container {

/** A container holding a set of values.
 *
 *  This container is somewhat like the STL <code>std::set</code> container except it is optimized for the case when large
 *  numbers of values are contiguous.  It adds the ability to insert and erase intervals as well as scalars, and provides a
 *  mechanism to iterate over the storage nodes (intervals) rather than over the scalar values.
 *
 *  The @p Interval template parameter must implement the Sawyer::Container::Interval API, at least to some extent. */
template<class I>
class IntervalSet {
    // We use an IntervalMap to do all our work, always storing int(0) as the value.
    typedef IntervalMap<I, int> Map;
    Map map_;
public:
    typedef I Interval;
    typedef typename I::Value Scalar;                   /**< Type of scalar values stored in this set. */

    /** Node iterator.
     *
     *  Iterates over the storage nodes of the container, which are the Interval type provided as a class template
     *  parameter. Dereferencing the iterator will return a const reference to an interval (possibly a singlton interval). */
    class ConstNodeIterator: public boost::iterator_facade<ConstNodeIterator, const Interval,
                                                           boost::bidirectional_traversal_tag> {
        typedef typename IntervalMap<Interval, int>::ConstNodeIterator MapNodeIterator;
        MapNodeIterator iter_;
    public:
        ConstNodeIterator() {}
    private:
        friend class boost::iterator_core_access;
        friend class IntervalSet;
        explicit ConstNodeIterator(MapNodeIterator iter): iter_(iter) {}
        const Interval& dereference() const { return iter_->key(); }
        bool equal(const ConstNodeIterator &other) const { return iter_ == other.iter_; }
        void increment() { ++iter_; }
        void decrement() { --iter_; }
    };

    /** Scalar value iterator.
     *
     *  Scalar value iterators iterate over each scalar value in the set. Two caveats to beware of:
     *
     *  @li The set can hold a very large number of values, even the entire value space, in which case iterating over values
     *      rather than storage nodes could take a very long time.
     *  @li Iterating over values for a non-integral type is most likely nonsensical. */
    class ConstScalarIterator: public boost::iterator_facade<ConstScalarIterator, const typename Interval::Value,
                                                             boost::bidirectional_traversal_tag> {
        ConstNodeIterator iter_;
        typename Interval::Value offset_;
        mutable typename Interval::Value value_;        // so dereference() can return a reference
    public:
        ConstScalarIterator(): offset_(0) {}
        ConstScalarIterator(ConstNodeIterator iter): iter_(iter), offset_(0) {}
    private:
        friend class boost::iterator_core_access;
        friend class IntervalSet;
        const typename Interval::Value& dereference() const {
            ASSERT_require2(iter_->least() <= iter_->greatest(), "stored interval cannot be empty");
            ASSERT_require(iter_->least() + offset_ <= iter_->greatest());
            value_ = iter_->least() + offset_;
            return value_;                              // must return a reference
        }
        bool equal(const ConstScalarIterator &other) const {
            return iter_ == other.iter_ && offset_ == other.offset_;
        }
        void increment() {
            ASSERT_require2(iter_->least() <= iter_->greatest(), "stored interval cannot be empty");
            if (iter_->least() + offset_ == iter_->greatest()) {
                ++iter_;
                offset_ = 0;
            } else {
                ++offset_;
            }
        }
        void decrement() {
            ASSERT_require2(iter_->least() <= iter_->greatest(), "stored interval cannot be empty");
            if (0==offset_) {
                --iter_;
                offset_ = width(*iter_);
            } else {
                --offset_;
            }
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Default constructor.
     *
     *  Creates an empty set. */
    IntervalSet() {}

    /** Copy constructor.
     *
     *  The newly constructed set will contain copies of the nodes from @p other. */
    template<class Interval2>
    IntervalSet(const IntervalSet<Interval2> &other) {
        typedef typename IntervalSet<Interval2>::ConstNodeIterator OtherNodeIterator;
        for (OtherNodeIterator otherIter=other.nodes().begin(); otherIter!=other.nodes().end(); ++otherIter)
            insert(*otherIter);
    }

    /** Construct from an IntervalMap.
     *
     *  The newly constructed set will contain copies of the intervals from the specified @ref IntervalMap.  The map's
     *  intervals must be convertible to the set's interval type. The map's values are not used. */
    template<class Interval2, class T, class Policy>
    explicit IntervalSet(const IntervalMap<Interval2, T, Policy> &other) {
        typedef typename IntervalMap<Interval2, T, Policy>::ConstNodeIterator OtherNodeIterator;
        for (OtherNodeIterator otherIter=other.nodes().begin(); otherIter!=other.nodes().end(); ++otherIter)
            insert(otherIter->key());
    }

    /** Assignment from another set.
     *
     *  Causes this set to contain the same intervals as the @p other set. The other set's intervals must be convertible to
     *  this set's interval type. */
    template<class Interval2>
    IntervalSet& operator=(const IntervalSet<Interval2> &other) {
        clear();
        typedef typename IntervalSet<Interval2>::ConstNodeIterator OtherNodeIterator;
        for (OtherNodeIterator otherIter=other.nodes().begin(); otherIter!=other.nodes().end(); ++otherIter)
            insert(*otherIter);
        return *this;
    }

    /** Assignment from an IntervalMap.
     *
     *  Causes this set to contain the same intervals as the specified map.  The map's intervals must be convertible to this
     *  set's interval type.  Since sets and maps have different requirements regarding merging of neighboring intervals, the
     *  returned container might not have node-to-node correspondence with the map, but both will contain the same logical
     *  intervals. */
    template<class Interval2, class T, class Policy>
    IntervalSet& operator=(const IntervalMap<Interval2, T, Policy> &other) {
        clear();
        typedef typename IntervalMap<Interval2, T, Policy>::ConstNodeIterator OtherNodeIterator;
        for (OtherNodeIterator otherIter=other.nodes().begin(); otherIter!=other.nodes().end(); ++otherIter)
            insert(otherIter->key());
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Iteration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Iterator range for all interval nodes actually stored by this set. */
    boost::iterator_range<ConstNodeIterator> nodes() const {
        return boost::iterator_range<ConstNodeIterator>(ConstNodeIterator(map_.nodes().begin()),
                                                        ConstNodeIterator(map_.nodes().end()));
    }

    /** Iterator range for all scalar values logically represented by this set. */
    boost::iterator_range<ConstScalarIterator> scalars() const {
        return boost::iterator_range<ConstScalarIterator>(ConstScalarIterator(nodes().begin()),
                                                          ConstScalarIterator(nodes().end()));
    }

    /** Find the first node whose interval ends at or above the specified scalar key.
     *
     *  Returns an iterator to the node, or the end iterator if no such node exists. */
    ConstNodeIterator lowerBound(const typename Interval::Value &scalar) const {
        return ConstNodeIterator(map_.lowerBound(scalar));
    }

    /** Find the last node whose interval starts at or below the specified scalar key.
     *
     *  Returns an iterator to the node, or the end iterator if no such node exists. */
    ConstNodeIterator findPrior(const typename Interval::Value &scalar) const {
        return ConstNodeIterator(map_.findPrior(scalar));
    }

    /** Find the node containing the specified scalar key.
     *
     *  Returns an iterator to the matching node, or the end iterator if no such node exists. */
    ConstNodeIterator find(const typename Interval::Value &scalar) const {
        return ConstNodeIterator(map_.find(scalar));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Size
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Determine whether the container is empty.
     *
     *  Returns true only if this set contains no elements. */
    bool isEmpty() const {
        return map_.isEmpty();
    }

    /** Number of scalar elements represented.
     *
     *  Returns the number of scalar elements (not intervals or storage nodes) contained in this set.  Since the return type is
     *  the same as the type used in the interval end points, this function can return overflowed values.  For instance, a set
     *  that contains all possible values in the value space is likely to return zero. */
    typename Interval::Value size() const {
        return map_.size();
    }

    /** Number of storage nodes.
     *
     *  Returns the number of nodes stored in this container, which for sets is always the number of maximally contiguous
     *  intervals.  Most algorithms employed by IntervalSet methods are either logarithmic or scalar in this number. */
    size_t nIntervals() const {
        return map_.nIntervals();
    }

    /** Returns the range of values in this map. */
    Interval hull() const {
        return map_.hull();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Predicates
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Determines whether this set overlaps with the argument.
     *
     *  Returns true if this set contains any values that are also present in the argument.
     *
     * @{ */
    template<class Interval2>
    bool isOverlapping(const Interval2 &interval) const {
        return map_.isOverlapping(interval);
    }

    template<class Interval2>
    bool isOverlapping(const IntervalSet<Interval2> &other) const {
        return map_.isOverlapping(other.map_);
    }

    template<class Interval2, class T2, class Policy2>
    bool isOverlapping(const IntervalMap<Interval2, T2, Policy2> &other) const {
        return map_.isOverlapping(other);
    }
    /** @} */

    /** Determines whether this set is distinct from the argument.
     *
     *  Returns true if none of the values of this set are equal to any value in the argument.
     *
     * @{ */
    template<class Interval2>
    bool isDistinct(const Interval2 &interval) const {
        return !isOverlapping();
    }

    template<class Interval2>
    bool isDistinct(const IntervalSet<Interval2> &other) const {
        return !isOverlapping(other);
    }

    template<class Interval2, class T2, class Policy2>
    bool isDistinct(const IntervalMap<Interval2, T2, Policy2> &other) const {
        return !isOverlapping(other);
    }
    /** @} */


    /** Determines whether this set fully contains the argument.
     *
     *  Returns true if this set contains all values represented by the argument.
     *
     * @{ */
    template<class Interval2>
    bool contains(const Interval2 &interval) const {
        return map_.contains(interval);
    }

    template<class Interval2>
    bool contains(const IntervalSet<Interval2> &other) const {
        return map_.contains(other.map_);
    }

    template<class Interval2, class T2, class Policy2>
    bool contains(const IntervalMap<Interval2, T2, Policy2> &other) const {
        return map_.contains(other);
    }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Searching
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /** Returns the minimum scalar contained in this set. */
    Scalar least() const {
        ASSERT_forbid(isEmpty());
        return map_.least();
    }

    /** Returns the maximum scalar contained in this set. */
    Scalar greatest() const {
        ASSERT_forbid(isEmpty());
        return map_.greatest();
    }

    /** Returns the limited-minimum scalar contained in this set.
     *
     *  Returns the minimum scalar that exists in this set and which is greater than or equal to @p lowerLimit.  If no such
     *  value exists then nothing is returned. */
    Optional<Scalar> least(Scalar lowerLimit) const {
        return map_.least(lowerLimit);
    }

    /** Returns the limited-maximum scalar contained in this set.
     *
     *  Returns the maximum scalar that exists in this set and which is less than or equal to @p upperLimit.  If no such
     *  value exists then nothing is returned. */
    Optional<Scalar> greatest(Scalar upperLimit) const {
        return map_.greatest(upperLimit);
    }

    /** Returns the limited-minimum scalar not contained in this set.
     *
     *  Returns the minimum scalar equal to or greater than the @p lowerLimit which is not in this set.  If no such value
     *  exists then nothing is returned. */
    Optional<Scalar> leastNonExistent(Scalar lowerLimit) const {
        return map_.leastUnmapped(lowerLimit);
    }

    /** Returns the limited-maximum scalar not contained in this set.
     *
     *  Returns the maximum scalar equal to or less than the @p upperLimit which is not in this set.  If no such value exists
     *  then nothing is returned. */
    Optional<Scalar> greatestNonExistent(Scalar upperLimit) const {
        return map_.greatestUnmapped(upperLimit);
    }

    /** Find the first fit node at or after a starting point.
     *
     *  Finds the first node of contiguous values beginning at or after the specified starting iterator, @p start, and which is
     *  at least as large as the desired @p size.  If there are no such nodes then the end iterator is returned.
     *
     *  Caveat emptor: The @p size argument has the name type as the interval end points. If the end points have a signed type,
     *  then it is entirely likely that the size will overflow.  In fact, it is also possible that unsigned sizes overflow
     *  since, for example, an 8-bit unsigned size cannot hold the size of an interval representing the entire 8-bit space.
     *  Therefore, use this method with care. */
    ConstNodeIterator firstFit(const typename Interval::Value &size, ConstNodeIterator start) const {
        return ConstNodeIterator(map_.firstFit(size, start.iter_));
    }

    /** Find the best fit node at or after a starting point.
     *
     *  Finds a node of contiguous values beginning at or after the specified starting iterator, @p start, and which is at
     *  least as large as the desired @p size.  If there is more than one such node, then the first smallest such node is
     *  returned.  If there are no such nodes then the end iterator is returned.
     *
     *  Caveat emptor: The @p size argument has the name type as the interval end points. If the end points have a signed type,
     *  then it is entirely likely that the size will overflow.  In fact, it is also possible that unsigned sizes overflow
     *  since, for example, an 8-bit unsigned size cannot hold the size of an interval representing the entire 8-bit space.
     *  Therefore, use this method with care. */
    ConstNodeIterator bestFit(const typename Interval::Value &size, ConstNodeIterator start) const {
        return ConstNodeIterator(map_.bestFit(size, start.iter_));
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Modifiers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Remove all values.
     *
     *  All values are removed from this set and the set becomes empty. */
    void clear() {
        map_.clear();
    }

    /** Invert and intersect.
     *
     *  Inverts this set and then intersects it with @p restricted. */
    void invert(const Interval &restricted) {
        IntervalSet inverted;
        if (!restricted.isEmpty()) {
            typename Interval::Value pending = restricted.least();
            bool insertTop = true;
            for (typename Map::ConstKeyIterator iter=map_.lowerBound(restricted.least()); iter!=map_.keys().end(); ++iter) {
                if (iter->least() > restricted.greatest())
                    break;
                if (pending < iter->least())
                    inverted.insert(Interval::hull(pending, iter->least()-1));
                if (iter->greatest() < restricted.greatest()) {
                    pending = iter->greatest() + 1;
                } else {
                    insertTop = false;
                    break;
                }
            }
            if (insertTop)
                inverted.insert(Interval::hull(pending, restricted.greatest()));
        }
        std::swap(map_, inverted.map_);
    }

    /** Insert specified values.
     *
     *  The values can be specified by a interval (or scalar if the interval has an implicit constructor), another set whose
     *  interval type is convertible to this set's interval type, or an IntervalMap whose intervals are convertible.
     *
     * @{ */
    template<class Interval2>
    void insert(const Interval2 &interval) {
        map_.insert(interval, 0);
    }

    template<class Interval2>
    void insertMultiple(const IntervalSet<Interval2> &other) {
        typedef typename IntervalSet<Interval2>::ConstNodeIterator OtherIterator;
        for (OtherIterator otherIter=other.nodes().begin(); otherIter!=other.nodes().end(); ++otherIter)
            map_.insert(*otherIter, 0);
    }

    template<class Interval2, class T, class Policy>
    void insertMultiple(const IntervalMap<Interval2, T, Policy> &other) {
        typedef typename IntervalMap<Interval2, T, Policy>::ConstKeyIterator OtherIterator;
        for (OtherIterator otherIter=other.keys().begin(); otherIter!=other.keys().end(); ++otherIter)
            map_.insert(*otherIter, 0);
    }
    /** @} */

    /** Remove specified values.
     *
     *  The values can be specified by an interval (or scalar if the interval has an implicit constructor), another set whose
     *  interval type is convertible to this set's interval type, or an IntervalMap whose intervals are convertible.
     *
     * @{ */
    template<class Interval2>
    void erase(const Interval2 &interval) {
        map_.erase(interval);
    }

    template<class Interval2>
    void eraseMultiple(const IntervalSet<Interval2> &other) {
        ASSERT_forbid2((void*)&other==(void*)this, "use IntervalSet::clear() instead");
        typedef typename IntervalSet<Interval2>::ConstNodeIterator OtherNodeIterator;
        for (OtherNodeIterator otherIter=other.nodes().begin(); otherIter!=other.nodes().end(); ++otherIter)
            map_.erase(*otherIter);
    }

    template<class Interval2, class T, class Policy>
    void eraseMultiple(const IntervalMap<Interval2, T, Policy> &other) {
        typedef typename IntervalMap<Interval2, T, Policy>::ConstNodeIterator OtherNodeIterator;
        for (OtherNodeIterator otherIter=other.nodes().begin(); otherIter!=other.nodes().end(); ++otherIter)
            map_.erase(otherIter->first);
    }
    /** @} */

    /** Interset with specified values.
     *
     *  Computes in place intersection of this container with the specified argument.  The argument may be an interval (or
     *  scalar if the interval has an implicit constructor), another set whose interval type is convertible to this set's
     *  interval type, or an IntervalMap whose intervals are convertible.
     *
     * @{ */
    template<class Interval2>
    void intersect(const Interval2 &interval);          // FIXME[Robb Matzke 2014-04-12]: not implemented yet

    template<class Interval2>
    void intersect(const IntervalSet<Interval2> &other);// FIXME[Robb Matzke 2014-04-12]: not implemented yet

    template<class Interval2, class T, class Policy>
    void intersect(const IntervalMap<Interval2, T, Policy> &other);// FIXME[Robb Matzke 2014-04-12]: not implemented yet
    /** @} */

};

} // namespace
} // namespace

#endif
