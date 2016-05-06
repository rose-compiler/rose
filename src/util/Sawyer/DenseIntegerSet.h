// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Set optimized to store densely-packed integers
#ifndef Sawyer_DenseIntegerSet_H
#define Sawyer_DenseIntegerSet_H

#include <Sawyer/Sawyer.h>
#include <Sawyer/Exception.h>
#include <Sawyer/Interval.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range.hpp>
#include <string>
#include <vector>

namespace Sawyer {
namespace Container {

/** Unordered set of densely-packed integers.
 *
 *  This set is optimized to store integers from a range whose size is close to the cardinality of the set. For instance, if
 *  the set is intended to store integers in the range [100,199] and contains nearly 100 values at least at some point in its
 *  lifetime, then this is an appropriate container class to use for the set.  Insert, erase, and existence testing are
 *  constant time, as are obtaining the beginning and end iterators and incrementing an iterator.
 *
 *  It is traditional to use an array or vector of Booleans to represent dense sets, but that approach results in poor iterator
 *  performance when the set happens to be sparse.  This container avoids the poor iterator performance by also storing the
 *  members as a linked list (although it does so in a way that doesn't actually store the member values explicitly). But the
 *  "cost" of constant-time iteration is that the members of the set are not traversed in any particular order.
 *
 *  The combination of constant-time insert/erase/lookup and constant-time iterator increment makes this an ideal container for
 *  tree search algorithms where the set initially contains all vertex IDs but for most of the search the set is sparse. */
template<typename T>
class DenseIntegerSet {
public:
    // Needed by iterators
    struct Member {
        Member *next, *prev;                            // circular list inc. head_; nulls imply member is not present in set
        Member(): next(NULL), prev(NULL) {}
        Member(Member *next, Member *prev): next(next), prev(prev) {}
    };

private:
    Interval<T> domain_;                                // domain of values that can be members of this set
    std::vector<Member> members_;                       // members forming a doubly-linked circular list inc. head_
    Member head_;                                       // head node of doubly-linked circular member list.
    size_t nMembers_;                                   // number of members contained in the set

public:
    typedef T Value;                                    /**< Type of values stored in this container. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Construction
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Construct an empty set that cannot store any members.
     *
     *  This object can only represent the empty set, but it's useful to have this default constructor in order to create a set
     *  that can be stored in a vector of sets, among other things. */
    DenseIntegerSet()
        : head_(&head_, &head_) {}

    /** Construct an empty set that can hold values from the specified domain.
     *
     *  Constructs a set whose members can be chosen from the specified domain. The domain can be specified as an interval,
     *  as a least and greated value, or as the number of values. If specified as the number of values, @em N, then the domain
     *  is zero through <em>N-1</em>, inclusive.  The set is initially empty.
     *
     * @{ */
    explicit DenseIntegerSet(const Interval<Value> &domain)
        : domain_(domain), head_(&head_, &head_), nMembers_(0) {
        size_t n = (size_t)domain.greatest() - (size_t)domain.least() + 1;
        ASSERT_require(n != 0 || !domain.isEmpty());
        members_.resize(n);
    }

    DenseIntegerSet(Value least, Value greatest)
        : domain_(Interval<Value>::hull(least, greatest)), head_(&head_, &head_), nMembers_(0) {
        size_t n = (size_t)greatest - (size_t)least + 1;
        ASSERT_require(n != 0 || !domain_.isEmpty());
        members_.resize(n);
    }

    explicit DenseIntegerSet(Value n)
        : domain_(Interval<Value>::baseSize(0, n)), members_(n), head_(&head_, &head_), nMembers_(n) {
        if (0 == n)
            domain_ = Interval<Value>();
    }
    /** @} */

    /** Copy constructor. */
    DenseIntegerSet(const DenseIntegerSet &other)
        : domain_(other.domain_), head_(&head_, &head_), nMembers_(0) {
        size_t n = (size_t)domain_.greatest() - (size_t)domain_.least() + 1;
        ASSERT_require(n != 0 || !domain_.isEmpty());
        members_.resize(n);
        BOOST_FOREACH (Value v, other.values())
            insert(v);
    }

    /** Assignment operator.
     *
     *  Assignment does not change the domain of the destination. If one of the members of @p other is outside the domain of
     *  this container then an @c Exception::Domain error is thrown and this object is not modified. */
    DenseIntegerSet& operator=(const DenseIntegerSet &other) {
        DenseIntegerSet tmp(domain());
        BOOST_FOREACH (Value v, other.values())
            tmp.insert(v);
        std::swap(domain_, tmp.domain_);
        std::swap(members_, tmp.members_);
        std::swap(nMembers_, tmp.nMembers_);

        // heads require some fixups because the prev/next pointers of the list and the heads point to the address of the head
        // in their original object, not the new object.
        std::swap(head_, tmp.head_);
        if (head_.next == &tmp.head_) {
            head_.next = head_.prev = &head_;
        } else {
            head_.prev->next = &head_;
            head_.next->prev = &head_;
        }

        // Fix up tmp's list too for completeness, although the only thing we do is delete it.
        if (tmp.head_.next == &head_) {
            tmp.head_.next = tmp.head_.prev = &tmp.head_;
        } else {
            tmp.head_.prev->next = &tmp.head_;
            tmp.head_.next->prev = &tmp.head_;
        }
        
        return *this;
    }
        
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Iterators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Bidirectional iterates over members of a set.
     *
     *  This iterator iterates over the values that are currently members of a set. A set is able to return a begin and end
     *  iterator in constant time, and the iterator's increment and dereference operators are constant time.  These iterators
     *  return const references since the container does not support modifying existing members through an iterator. When
     *  compiled in debugging mode, these iterators also include various assertions about their validity.
     *
     *  DenseIntegerSet iterators are stable over insert and erase; inserting and erasing members of the iterated set does
     *  not invalidate existing iterators except in the case of iterators that are pointing to an erased member. However,
     *  although using an iterator as a pointer is safe, iterating over a set that's being modified at the same time is
     *  generally not supported--there is no guarantee that iterations will reach all members in these cases.  The
     *  erase-at-iterator method returns an iterator in order to guarantee that the iterator used for erasing does indeed reach
     *  all members. */
    class ConstIterator: public std::iterator<std::bidirectional_iterator_tag, const Value> {
        friend class DenseIntegerSet;
        const DenseIntegerSet *set_;
        const Member *member_;
        mutable Value value_;                           // so we can return a const ref

        ConstIterator(const DenseIntegerSet *s, const Member *m)
            : set_(s), member_(m) {}
        
    public:
        /** Iterators are comparable for equality.
         *
         *  Two iterators are equal if and only if they point to the same member of the same set or they are both end iterators
         *  of the same set. */
        bool operator==(const ConstIterator &other) const {
            return set_ == other.set_ && member_ == other.member_;
        }

        /** Iterators are comparable for inequality.
         *
         *  Two iterators are unequal if they do not satisfy the equality predicate. */
        bool operator!=(const ConstIterator &other) const {
            return set_ != other.set_ || member_ != other.member_;
        }

        /** Iterators are less-than comparable.
         *
         *  The exact meaning of less-than is undefined, but it satisfies the requirements of a less-than operator. Namely,
         *
         *  @li if two iterators are equal then neither is less than the other.
         *  @li if two iterators are unequal, then exactly one is less than the other.
         *  @li non-symmetric: if A is less than B then B is not less than A.
         *  @li transitive: if A is less than B and B is less than C then A is less than C.
         *
         *  This property allows iterators to be used as keys in containers that order their keys. */
        bool operator<(const ConstIterator &other) const {
            if (set_ != other.set_)
                return set_ < other.set_;
            return member_ < other.member_;
        }

        /** Increment.
         *
         *  Causes this iterator to point to the next member. The members of a set have no particular order. Incrementing the
         *  end iterator has undefined behavior.  Incrementing is a constant-time operation.
         *
         *  @{ */
        ConstIterator& operator++() {
            member_ = member_->next;
            return *this;
        }
        ConstIterator operator++(int) {
            ConstIterator retval = *this;
            ++*this;
            return retval;
        }
        /** @} */

        /** Decrement.
         *
         *  Causes this iterator to point to the previous member. The members of a set have no particular order. Decrementing
         *  the begin iterator has undefined behavior.  Decrementing is a constant-time operation.
         *
         * @{ */
        ConstIterator& operator--() {
            member_ = member_->prev;
            return *this;
        }

        ConstIterator& operator--(int) {
            ConstIterator retval = *this;
            --*this;
            return retval;
        }
        /** @} */

        /** Dereference.
         *
         *  Returns the value to which the iterator points. Dereferencing the end iterator has undefined behavior.
         *  Dereferencing is a constant-time operation. */
        const Value& operator*() const {
            value_ = set_->deref(*this);
            return value_;
        }
    };

    /** Iterator range for set members.
     *
     *  Returns an iterator range consiting of the begin and end iterators, in constant time. */
    boost::iterator_range<ConstIterator> values() const {
        return boost::iterator_range<ConstIterator>(ConstIterator(this, head_.next), ConstIterator(this, &head_));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Predicates and queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Whether the set is empty.
     *
     *  Returns true if the set is empty, false if not empty.  This is a constant-time operation. */
    bool isEmpty() const {
        return head_.next == &head_;
    }

    /** Number of members present.
     *
     *  Returns the number of values currently contained in this set. This is a constant-time operation. */
    size_t size() const {
        return nMembers_;
    }

    /** Domain of storable values.
     *
     *  Returns the set's domain, which is an interval describing which values can be possible members of this set. */
    Interval<Value> domain() const {
        return domain_;
    }

    /** Determines if a value is storable.
     *
     *  Returns true if the specified value can be stored in this set, and false otherwise.  A storable value is a value that
     *  falls within this set's domain. */
    bool isStorable(Value v) const {
        return domain().isContaining(v);
    }

    /** Determines whether a value is stored.
     *
     *  Returns true if the specified value is a member of this set, false if the value is not stored in this set.  This method
     *  returns false if the value is outside this set's domain. */
    bool exists(const Value &value) const {
        if (isEmpty() || !isStorable(value))
            return false;
        const Member &m = members_[value - domain_.least()];
        return head_.next == &m || m.prev != NULL;
    }

    /** Whether any value exists.
     *
     *  Returns true if any of the specified values exist in this set.  This operation takes time that is linearly
     *  proportional to the number of items in the @p other container. */
    template<class SawyerContainer>
    bool existsAny(const SawyerContainer &other) const {
        BOOST_FOREACH (const typename SawyerContainer::Value &otherValue, other.values()) {
            if (exists(otherValue))
                return true;
        }
        return false;
    }

    /** Whether all values exist.
     *
     *  Returns true if all specified values exist in this set. This operation takes time that is linearly proportaional to teh
     *  number of items in the @p other container. */
    template<class SawyerContainer>
    bool existsAll(const SawyerContainer &other) const {
        BOOST_FOREACH (const typename SawyerContainer::Value &otherValue, other.values()) {
            if (!exists(otherValue))
                return false;
        }
        return true;
    }

    /** Whether two sets contain the same members.
     *
     *  Returns true if this set and @p other contain exactly the same members. */
    template<class SawyerContainer>
    bool operator==(const SawyerContainer &other) const {
        return size() == other.size() && existsAll(other);
    }

    /** Whether two sets do not contain the same members.
     *
     *  Returns true if this set and the @p other set are not equal. */
    template<class SawyerContainer>
    bool operator!=(const SawyerContainer &other) const {
        return size() != other.size() || !existsAll(other);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Modifiers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Remove all members from this set.
     *
     *  This operation is linear in the number of members currently present in the set. */
    void clear() {
        Member *next = NULL;
        for (Member *m = head_.next; m != &head_; m = next) {
            next = m->next;
            m->prev = m->next = NULL;
        }
        head_.next = head_.prev = &head_;
        nMembers_ = 0;
    }

    /** Insert a value.
     *
     *  Inserts the specified value in constant time. Returns true if the value was inserted and false if the value already
     *  existed. If the value is outside the domain then an @ref Exception::DomainError is thrown. */
    bool insert(Value value) {
        if (!isStorable(value)) {
            std::string mesg;
            if (domain_.isEmpty()) {
                mesg = "cannot insert a value into a destination set with an empty domain";
            } else {
                mesg = "cannot insert " + boost::lexical_cast<std::string>(value) + " into a destination set whose domain is [" +
                       boost::lexical_cast<std::string>(domain_.least()) + ", " +
                       boost::lexical_cast<std::string>(domain_.greatest()) + "]";
            }
            throw Exception::DomainError(mesg);
        }
        Member &m = members_[value - domain_.least()];
        if (m.next)
            return false;                               // already a member
        m.prev = head_.prev;
        m.next = &head_;
        head_.prev->next = &m;
        head_.prev = &m;
        ++nMembers_;
        return true;
    }

    /** Insert all possible members.
     *
     *  Causes the set to contain all elements that are part of its domain. */
    void insertAll() {
        if (!members_.empty()) {
            for (size_t i=0; i<members_.size(); ++i) {
                if (0 == i) {
                    members_[0].prev = &head_;
                    head_.next = &members_[0];
                } else {
                    members_[i].prev = &members_[i-1];
                }
                if (i+1 < members_.size()) {
                    members_[i].next = &members_[i+1];
                } else {
                    members_[i].next = &head_;
                    head_.prev = &members_[i];
                }
            }
            nMembers_ = members_.size();
        }
    }

    /** Insert many values from another set.
     *
     *  Inserts all values of the @p other container into this set.
     *
     * @{ */
    template<class SawyerContainer>
    void insertMany(const SawyerContainer &other) {
        BOOST_FOREACH (const typename SawyerContainer::Value &otherValue, other.values())
            insert(otherValue);
    }

    template<class SawyerContainer>
    DenseIntegerSet& operator|=(const SawyerContainer &other) {
        insertMany(other);
        return *this;
    }
    /** @} */

    /** Erase a value.
     *
     *  If a value is specified, then the value is erased and this method returns true if the value existed and false if it
     *  didn't exist (in which case this is a no-op).  If a non-end iterator is specified, then the pointed to value is erased
     *  and the next iterator is returned.
     *
     *  Erasing is a constant-time operation.
     *
     * @{ */
    bool erase(Value value) {
        if (!exists(value))
            return false;
        Member &m = members_[value - domain_.least()];
        m.prev->next = m.next;
        m.next->prev = m.prev;
        m.next = m.prev = NULL;
        ASSERT_require(nMembers_ > 0);
        --nMembers_;
        return true;
    }

    ConstIterator erase(const ConstIterator &iter) {
        ASSERT_require2(iter.set_ != this, "iterator does not belong to this set");
        ASSERT_require2(iter.member_ != &head_, "cannot erase the end iterator");
        ASSERT_require2(iter.member_->next != NULL, "iterator no longer points to a member of this set");
        ConstIterator retval = iter;
        ++retval;
        Member &m = iter->member_;
        m.prev->next = m.next;
        m.next->prev = m.prev;
        m.next = m.prev = NULL;
        ASSERT_require(nMembers_ > 0);
        --nMembers_;
        return retval;
    }
    /** @} */

    /** Erase many values.
     *
     *  Erase those values from this set that are members of the @p other container.
     *
     * @{ */
    template<class SawyerContainer>
    void eraseMany(const SawyerContainer &other) {
        BOOST_FOREACH (const typename SawyerContainer::Value &otherValue, other.values())
            erase(otherValue);
    }

    template<class SawyerContainer>
    DenseIntegerSet& operator-=(const SawyerContainer &other) {
        eraseMany(other);
        return *this;
    }
    /** @} */

    /** Intersect this set with another.
     *
     *  Replaces this set with members that are only in this set and the @p other set.
     *
     * @{ */
    template<class SawyerContainer>
    void intersect(const SawyerContainer &other) {
        DenseIntegerSet tmp(*this);
        clear();
        BOOST_FOREACH (const typename SawyerContainer::Value &otherValue, other.values()) {
            if (tmp.exists(otherValue))
                insert(otherValue);
        }
    }

    template<class SawyerContainer>
    DenseIntegerSet& operator&=(const SawyerContainer &other) {
        intersect(other);
        return *this;
    }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Set-theoretic operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Compute the intersection of this set with another.
     *
     *  Returns a new set which has only those members that are common to this set and the @p other set. */
    template<class SawyerContainer>
    DenseIntegerSet operator&(const SawyerContainer &other) const {
        DenseIntegerSet retval = *this;
        retval &= other;
        return retval;
    }

    /** Compute the union of this set with another.
     *
     *  Returns a new set containing the union of all members of this set and the @p other set. */
    template<class SawyerContainer>
    DenseIntegerSet operator|(const SawyerContainer &other) const {
        DenseIntegerSet retval = *this;
        retval |= other;
        return retval;
    }

    /** Compute the difference of this set with another.
     *
     *  Returns a new set containing those elements of @p this set that are not members of the @p other set. */
    template<class SawyerContainer>
    DenseIntegerSet operator-(const SawyerContainer &other) const {
        DenseIntegerSet retval = *this;
        retval -= other;
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    // Dereference an iterator to get a value.
    Value deref(const ConstIterator &iter) const {
        const Member *m = iter.member_;
        ASSERT_require2(m != &head_, "dereferencing an end iterator");
        ASSERT_require2(m->next != NULL, "dereferencing erased member");
        return domain_.least() + (m - &members_[0]);
    }
};

} // namespace
} // namespace

#endif
