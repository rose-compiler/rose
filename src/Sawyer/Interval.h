// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_Interval_H
#define Sawyer_Interval_H

#include <Sawyer/Assert.h>
#include <Sawyer/Sawyer.h>
#include <boost/integer_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>

namespace Sawyer {
namespace Container {

/** Range of values delimited by endpoints.
 *
 *  This class represents a range of contiguous values by specifying the lower and upper end points, both of which are included
 *  in the range. Alternatively, a range may be empty; the default constructor creates empty ranges.  The value type, @p T, is
 *  intended to be an unsigned integer type.  Signed integers may be used, but the caller should be prepared to handle negative
 *  sizes due to overflow (see @ref size).  Non-integer types are not recommended since some methods (e.g., @ref size) assume
 *  that <em>n</em> and <em>n+1</em> are adjacent values, which is not the case for floating point.
 *
 *  Values of this type are immutable except for the assignment operator; operations like intersection return a new object
 *  rather than modifying an existing object. */
template<class T>
class Interval {
public:
    /** Types of values in the interval. */
    typedef T Value;
private:
    T lo_, hi_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(lo_);
        s & BOOST_SERIALIZATION_NVP(hi_);
    }
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive &archive) {
        archive(CEREAL_NVP(lo_));
        archive(CEREAL_NVP(hi_));
    }
#endif

public:
    /** Bidirectional forward iterator.
     *
     *  This iterator can be incremented, decremented, or dereferenced unless it is already at the end, as determined by its
     *  @ref atEnd predicate or by comparison with any other iterator whose @ref atEnd is true, including a default-constructed
     *  iterator.  An iterator is considered to be at its end if it has been incremented past the interval's greatest value or
     *  decremented past the interval's least value. Iterators will operate safely even when the least or greatest value of the
     *  range is the least or greatest possible value that can be represented in type T and incrementing or decrementing would
     *  cause an overflow. This is acheived by pretending that there's an extra value one less than the interval's least value
     *  and one greater than the intervals greatest value, regardless of whether such values exist for type T. Incrementing
     *  an iterator that's positioned one past the interval's greatest is a no-op, as is decrementing an iterator positioned
     *  one prior to the interval's least end.  On the other hand, decrementing an iterator positioned one past the interval's
     *  greatest end positions the iterator at greatest and clears its @ref atEnd property; similarly, incrementing an iterator
     *  positioned one prior to the interval's least end positions the iterator at the least value. */
    class ConstIterator: public boost::iterator_facade<ConstIterator, const Value, boost::bidirectional_traversal_tag,
                                                       Value> {
        friend class Interval;
        friend class boost::iterator_core_access;

        T first_, cur_, last_;
        bool atEnd_;

        ConstIterator(T first, T last, T cur): first_(first), cur_(cur), last_(last), atEnd_(false) {}
    public:
        /** Create an empty iterator.
         *
         *  All increment and decrement operators are no-ops and @ref atEnd always returns true. It is illegal to attempt to
         *  dereference such an iterator.  A default-constructed iterator compares equal to all other default-constructed
         *  iterators and equal to all iterators positioned at one of their ends. */
        ConstIterator(): first_(0), cur_(0), last_(0), atEnd_(true) {}

        /** Predicate to determine if an iterator is at one of its end positions.
         *
         *  Returns true if this iterator is positioned one prior to the interval's least end, or one after the interval's
         *  greatest end. These two positions are virtual and exist even if type T is unable to represent them. Since intervals
         *  are immutable, their iterators' end-points are defined when the iterator is created and are constant. */
        bool atEnd() const {
            return atEnd_;
        }

    private:
        Value dereference() const {
            ASSERT_forbid(atEnd());
            return cur_;
        }

        bool equal(const ConstIterator &other) const {
            if (atEnd() || other.atEnd())
                return atEnd() && other.atEnd();
            return cur_ == other.cur_;
        }

        // Incrementing an iterator associated with an empty interval is a no-op, and such an interval's @ref atEnd always
        // returns true. Otherwise, incrementing an iterator positioned one past the interval's greatest end is a
        // no-op. Otherwise, incrementing an iterator positioned one prior to the interval's least end returns the iterator to
        // the interval's least value. Otherwise the iterator derefences to a value one greater than before this call.
        void increment() {
            if (cur_ == last_) {                        // avoid overflow
                atEnd_ = true;
            } else if (atEnd_) {
                ASSERT_require(cur_ == first_);
                atEnd_ = false;
            } else {
                ++cur_;
            }
        }

        void decrement() {
            if (cur_ == first_) {                       // avoid overflow
                atEnd_ = true;
            } else if (atEnd_) {
                ASSERT_require(cur_ == last_);
                atEnd_ = false;
            } else {
                --cur_;
            }
        }
    };
    
public:
    /** Constructs an empty interval. */
    Interval(): lo_(1), hi_(0) {}

    /** Copy-constructs an interval. */
    Interval(const Interval &other): lo_(other.lo_), hi_(other.hi_) {}

    /** Constructs a singleton interval. */
    Interval(T value): lo_(value), hi_(value) {}

#if 0 // [Robb Matzke 2014-05-14]: too much confusion with "hi" vs. "size". Use either baseSize or hull instead.
    /** Constructs an interval from endpoints.
     *
     *  The first end point must be less than or equal to the second end point.  If both endpoints are equal then a singleton
     *  interval is constructed. */
    Interval(T lo, T hi): lo_(lo), hi_(hi) {
        ASSERT_require(lo <= hi);
    }
#endif

    /** Construct an interval from two endpoints.
     *
     *  Returns the smallest interal that contains both points. */
    static Interval hull(T v1, T v2) {
        Interval retval;
        retval.lo_ = std::min(v1, v2);
        retval.hi_ = std::max(v1, v2);
        return retval;
    }

    /** Construct an interval from one endpoint and a size.
     *
     *  Returns the smallest interval that contains @p lo (inclusive) through @p lo + @p size (exclusive).  If @p size is zero
     *  then an empty interval is created, in which case @p lo is irrelevant. */
    static Interval baseSize(T lo, T size) {
        ASSERT_forbid(baseSizeOverflows(lo, size));
        return 0==size ? Interval() : Interval::hull(lo, lo+size-1);
    }

    /** Construct an interval from one endpoint and size, and clip overflows.
     *
     *  Returns the smallest interval that contains @p lo (inclusive) through @p lo + @p size (exclusvie). If @p lo + @p size
     *  doesn't fit in an instance of @ref Value then the greatest possible value is used. */
    static Interval baseSizeSat(T lo, T size) {
        if (baseSizeOverflows(lo, size)) {
            return hull(lo, boost::integer_traits<T>::const_max);
        } else {
            return baseSize(lo, size);
        }
    }

    /** Construct an interval that covers the entire domain. */
    static Interval whole() {
        return hull(boost::integer_traits<T>::const_min, boost::integer_traits<T>::const_max);
    }

    /** Assignment from an interval. */
    Interval& operator=(const Interval &other) {
        lo_ = other.lo_;
        hi_ = other.hi_;
        return *this;
    }

    /** Assignment from a scalar. */
    Interval& operator=(T value) {
        lo_ = hi_ = value;
        return *this;
    }

    /** Tests whether a base and size overflows.
     *
     *  If the base (least value) plus the size would be larger than the maximum possible value, then returns true, otherwise
     *  returns false. */
    static bool baseSizeOverflows(T base, T size) {
        // Warning: This only works when T is unsigned since signed integer overflow is undefined behavior in C++.
        return base + size < base;
    }

    /** Returns lower limit. */
    T least() const {
        ASSERT_forbid(isEmpty());
        return lo_;
    }

    /** Returns upper limit. */
    T greatest() const {
        ASSERT_forbid(isEmpty());
        return hi_;
    }

    /** True if interval is empty. */
    bool isEmpty() const { return 1==lo_ && 0==hi_; }

    /** True if interval is a singleton. */
    bool isSingleton() const { return lo_ == hi_; }

    /** True if interval covers entire space. */
    bool isWhole() const { return lo_==boost::integer_traits<T>::const_min && hi_==boost::integer_traits<T>::const_max; }

    /** True if two intervals overlap.
     *
     *  An empty interval never overlaps with any other interval, empty or not.
     *
     * @{ */
    bool overlaps(const Interval &other) const {
        return !intersection(other).isEmpty();
    }
    bool isOverlapping(const Interval &other) const {
        return overlaps(other);
    }
    /** @} */

    /** Containment predicate.
     *
     *  Returns true if this interval contains all of the @p other interval.  An empty interval is always contained in any
     *  other interval, even another empty interval.
     *
     * @{ */
    bool contains(const Interval &other) const {
        return (other.isEmpty() ||
                (!isEmpty() && least()<=other.least() && greatest()>=other.greatest()));
    }
    bool isContaining(const Interval &other) const {
        return contains(other);
    }
    /** @} */

    /** Adjacency predicate.
     *
     *  Returns true if the two intervals are adjacent.  An empty interval is adjacent to all other intervals, including
     *  another empty interval.
     *
     *  @{ */
    bool isLeftAdjacent(const Interval &right) const {
        return isEmpty() || right.isEmpty() || (!isWhole() && greatest()+1 == right.least());
    }
    bool isRightAdjacent(const Interval &left) const {
        return isEmpty() || left.isEmpty() || (!left.isWhole() && left.greatest()+1 == least());
    }
    bool isAdjacent(const Interval &other) const {
        return (isEmpty() || other.isEmpty() ||
                (!isWhole() && greatest()+1 == other.least()) ||
                (!other.isWhole() && other.greatest()+1 == least()));
    }
    /** @} */

    /** Relative position predicate.
     *
     *  Returns true if the intervals do not overlap and one is positioned left or right of the other.  Empty intervals are
     *  considered to be both left and right of the other.
     *
     *  @{ */
    bool isLeftOf(const Interval &right) const {
        return isEmpty() || right.isEmpty() || greatest() < right.least();
    }
    bool isRightOf(const Interval &left) const {
        return isEmpty() || left.isEmpty() || left.greatest() < least();
    }
    /** @} */

    /** Size of interval.
     *
     *  If the interval is the whole space then the return value is zero due to overflow. */
    Value size() const {
        return isEmpty() ? 0 : hi_ - lo_ + 1;
    }

    /** Equality test.
     *
     *  Two intervals are equal if they have the same lower and upper bound, and unequal if either bound differs. Two empty
     *  ranges are considered to be equal.
     *
     *  @{ */
    bool operator==(const Interval &other) const {
        return lo_==other.lo_ && hi_==other.hi_;
    }
    bool operator!=(const Interval &other) const {
        return lo_!=other.lo_ || hi_!=other.hi_;
    }
    /** @} */

    /** Intersection.
     *
     *  Returns an interval which is the intersection of this interval with another.
     *
     * @{ */
    Interval intersection(const Interval &other) const {
        if (isEmpty() || other.isEmpty() || greatest()<other.least() || least()>other.greatest())
            return Interval();
        return Interval::hull(std::max(least(), other.least()), std::min(greatest(), other.greatest()));
    }
    Interval operator&(const Interval &other) const {
        return intersection(other);
    }
    /** @} */

    /** Hull.
     *
     *  Returns the smallest interval that contains both this interval and the @p other interval.
     *
     *  @sa join */
    Interval hull(const Interval &other) const {
        if (isEmpty()) {
            return other;
        } else if (other.isEmpty()) {
            return *this;
        } else {
            return Interval::hull(std::min(least(), other.least()), std::max(greatest(), other.greatest()));
        }
    }

    /** Hull.
     *
     *  Returns the smallest interval that contains both this interval and another value. */
    Interval hull(T value) const {
        if (isEmpty()) {
            return Interval(value);
        } else {
            return Interval::hull(std::min(least(), value), std::max(greatest(), value));
        }
    }

    /** Split interval in two.
     *
     *  Returns two interval by splitting this interval so that @p splitPoint is in the left returned interval.  If the split
     *  is not a member of this interval then one of the two returned intervals will be empty, depending on whether the split
     *  point is less than or greater than this interval.  If this interval is empty then both returned intervals will be empty
     *  regardless of the split point. */
    std::pair<Interval, Interval> split(T splitPoint) const {
        if (isEmpty()) {
            return std::make_pair(Interval(), Interval());
        } else if (splitPoint < least()) {
            return std::make_pair(Interval(), *this);
        } else if (splitPoint < greatest()) {
            return std::make_pair(Interval::hull(least(), splitPoint), Interval::hull(splitPoint+1, greatest()));
        } else {
            return std::make_pair(*this, Interval());
        }
    }

    /** Creates an interval by joining two adjacent intervals.
     *
     *  Concatenates this interval with the @p right interval and returns the result.  This is similar to @ref hull except
     *  when neither interval is empty then the greatest value of this interval must be one less than the least value of the @p
     *  right interval.
     *
     *  @sa hull */
    Interval join(const Interval &right) const {
        if (isEmpty()) {
            return right;
        } else if (right.isEmpty()) {
            return *this;
        } else {
            ASSERT_require(greatest()+1 == right.least() && right.least() > greatest());
            return hull(right);
        }
    }

    /** Shift interval upward.
     *
     *  Adds @p n to all values of the interval to return a new interval.  An empty interval is returned if this interval
     *  is empty or adding @p n to its least value would overflow.  A smaller interval is returned if adding @p n to the
     *  greatest value would overflow. */
    Interval shiftRightSat(Value n) const {
        if (isEmpty() || baseSizeOverflows(least(), n)) {
            return Interval();
        } else if (baseSizeOverflows(greatest(), n)) {
            return hull(least() + n, boost::integer_traits<T>::const_max);
        } else {
            return hull(least() + n, greatest() + n);
        }
    }

    // These types are needed for BOOST_FOREACH but are not advertised as part of this interface.
    typedef ConstIterator const_iterator;
    typedef ConstIterator iterator;

    /** Iterator positioned at the least value.
     *
     *  Returns an iterator positioned at this interval's least value. If this interval is empty then the returned iterator's
     *  @ref ConstIterator::atEnd "atEnd" predicate will always return true.  Iterators are useful for accessing the values of
     *  an interval because they have special logic to avoid arithmetic overflows which can happen if the interval's least
     *  and/or greatest value happens to also be the least or greatest value representable by type T. See @ref ConstIterator
     *  for details. */
    ConstIterator begin() const {
        return isEmpty() ? ConstIterator() : ConstIterator(least(), greatest(), least());
    }

    /** Iterator positioned one past the greatest value.
     *
     *  Returns an iterator positioned one past this interval's least value even if such a value cannot be represented by type
     *  T. If this interval is empty then the returned iterator's @ref ConstIterator::atEnd "atEnd" predicate will always
     *  return true.  Iterators are useful for accessing the values of an interval because they have special logic to avoid
     *  arithmetic overflows which can happen if the interval's least and/or greatest value happens to also be the least or
     *  greatest value representable by type T. See @ref ConstIterator for details. */
    ConstIterator end() const {
        return isEmpty() ? ConstIterator() : ++ConstIterator(least(), greatest(), greatest());
    }
    
    /** Iterator range for values. */
    boost::iterator_range<ConstIterator> values() const {
        return boost::iterator_range<ConstIterator>(begin(), end());
    }
    
    // The following trickery is to allow things like "if (x)" to work but without having an implicit
    // conversion to bool which would cause no end of other problems.  This is fixed in C++11
private:
    typedef void(Interval::*unspecified_bool)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    /** Type for Boolean context.
     *
     *  Implicit conversion to a type that can be used in a boolean context such as an <code>if</code> or <code>while</code>
     *  statement.  For instance:
     *
     * @code
     *  if (Interval<unsigned> x = doSomething(...)) {
     *     // this is reached only if x is non-empty
     *  }
     * @endcode
     *
     *  The inteval evaluates to true if it is non-empty, and false if it is empty. */
    operator unspecified_bool() const {
        return isEmpty() ? 0 : &Interval::this_type_does_not_support_comparisons;
    }
};

} // namespace
} // namespace

#endif
