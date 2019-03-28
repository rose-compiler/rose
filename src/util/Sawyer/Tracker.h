// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Container_Tracker_H
#define Sawyer_Container_Tracker_H

#include <Sawyer/Set.h>
#include <Sawyer/Synchronization.h>

namespace Sawyer {
namespace Container {

/** Set-based index referenced by @ref TrackerTraits.
 *
 *  This is the default index and supports O(log N) lookups and insertions. The @p Key type must be copyable and have a less-than
 *  comparison. */
template<class Key>
class TrackerSetIndex {
    Set<Key> set;
public:
    void clear() {
        set.clear();
    }
    bool exists(const Key &key) const {
        return set.exists(key);
    }
    bool insert(const Key &key) {
        return set.insert(key);
    }
};

/** Vector-based index referenced by TrackerTraits.
 *
 *  This index type is suitable for trackers whose @p Key values are a dense set of low-valued, non-negative integers that can be used
 *  as indexes into a vector. This index supports O(1) lookups and amortized O(1) insertions. */
template<class Key>
class TrackerVectorIndex {
    std::vector<bool> bvec;
public:
    void clear() {
        bvec.clear();
    }
    bool exists(const Key &key) const {
        return key < bvec.size() && bvec[key];
    }
    bool insert(const Key &key) {
        bool retval = !exists(key);
        if (key >= bvec.size())
            bvec.resize(key+1, false);
        bvec[key] = true;
        return retval;
    }
};

/** Hash-based index referenced by TrackerTraits.
 *
 *  This index type supports O(1) lookups and amortized O(1) insertions. The @p Key type must be suitable for use by
 *  @c boost::unordered_set. */
template<class Key>
class TrackerUnorderedIndex {
    boost::unordered_set<Key> set;
public:
    void clear() {
        set.clear();
    }
    bool exists(const Key &key) const {
        return set.find(key) != set.end();
    }
    bool insert(const Key &key) {
        return set.insert(key).second;
    }
};

/** Traits for @ref Tracker. */
template<class Key>
struct TrackerTraits {
    /** Type of index for storing member keys.
     *
     *  This type should define three member functions: @c clear that takes no arguments and removes all keys from the index;
     *  @c exists that takes a key and returns true if and only if it is present in the index; and @c insert that takes a key
     *  and inserts it into the index returning true if and only if the key did not previously exist in the index. None of these
     *  functions need to be thread safe since they will be synchronized from the @ref Tracker class. */
    typedef TrackerSetIndex<Key> Index;
};

/** Tracks whether something has been seen before.
 *
 *  Template parameter @p T are the things that are being tracked, but the tracker doesn't necessarily store them. Instead, it
 *  stores a key, type @p K, that can be derived from @p T and which might be a lot smaller.
 *
 *  Many iterative algorithms need to operate on an object just once. They use an @c std::set to remember which objects they've
 *  encountered already. For example:
 *
 * @code
 *  // C++03
 *  std::set<Thing> seen;
 *  bool done = false;
 *  while (!done) {
 *      std::vector<Thing> things = findSomeThings(); // unsorted
 *      done = true; // unless we process something
 *      BOOST_FOREACH (const Thing &thing, things) {
 *          if (seen.insert(thing).second) {
 *              process(thing);
 *              done = false;
 *          }
 *      }
 *  }
 * @endcode
 *
 *  There are two problems with the above code:
 *
 *  @li If @c Thing is large, then storing it in an @c std::set is expensive. In fact, @c Thing might not even be copyable in
 *      which case storing it in the set is impossible. Authors work around this by storing a proxy for the @c Thing, such as
 *      an identification number.
 *  @li The @c for loop serializes the processing of the objects, but it might be more efficient to process all the @c things
 *      at once. Authors do this by filtering out from the vector those things that have already been processed in a previous
 *      pass.
 *
 *  So a more realistic implementation looks like this:
 *
 * @code
 *  // C++03
 *  std::set<Identifier> seen;
 *  bool done = false;
 *  while (!done) {
 *      std::vector<Thing> things = findSomeThings(); // unsorted
 *      size_t nSaved = 0;
 *      for (size_t i = 0; i < things.size(); ++i) {
 *          if (seen.insert(Identifier(things[i])).second)
 *              things[nSaved++] = things[i];
 *      }
 *      things.resize(nSaved);
 *      done = things.empty();
 *      process(things);
 *  }
 * @endcode
 *
 *  The main problem with the above implementation is there's a lot going on that isn't the main point of the algorithm:
 *
 *  @li The @p seen set stores values of type @p Identifier and there's nothing really obvious that says that @p Identifier is
 *      related somehow to @p Thing.  You'd have to go find the definition of @p Identifier to see that it can be implicitly
 *      constructed from @p Thing.
 *  @li There's a whole six lines (almost half the implementation) to remove things that have already been processed in a
 *      previous iteration. It would be nice to be able to use std::vector::erase together with std::remove_if, but in C++03
 *      that takes no fewer lines and they're moved farther away in their own class.
 *
 *  The purpose of the @p Tracker class is to reduce all this clutter:
 *
 * @code
 *  // C++03
 *  Tracker<Thing, Identifier> tracker;
 *  bool done = false;
 *  while (!done) {
 *      std::vector<Thing> things = findSomeThings(); // unsorted
 *      tracker.removeIfSeen(things);
 *      done = things.empty();
 *      process(things);
 *  }
 * @endcode */
template<class T, class K = T, class Traits = TrackerTraits<K> >
class Tracker {
public:
    /** Type of values represented by the tracker. */
    typedef T Value;

    /** Key type for the values represented by the tracker.
     *
     *  The tracker stores keys, not values. A key should be able to be constructed from a value. */
    typedef K Key;

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members.
    typename Traits::Index index_;

public:
    /** Make this tracker forget everything it has seen.
     *
     *  Thread safety: This method is thread safe. */
    void clear() {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        index_.clear();
    }

    /** Test and then insert the value.
     *
     *  Causes the specified value to be seen by this tracker and then returns whether it had been seen already.
     *
     *  Thread safety: This method is thread safe. */
    bool testAndSet(const Value &value) {
        Key key(value);
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return !index_.insert(key);
    }

    /** Unary operator is the same as @ref testAndSet. */
    bool operator()(const Value &value) {
        return testAndSet(value);
    }

    /** Test whether a value has been encountered previously.
     *
     *  Returns true if the value has been seen before, false if not. Does not cause the value to be added to the tracker.
     *
     *  Thread safety: This method is thread safe. */
    bool wasSeen(const Value &value) const {
        Key key(value);
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return index_.exists(key);
    }

    /** Cause this tracker to see a value.
     *
     *  Insert the value into the tracker and return true only if it was not already present.  This function is identical to
     *  @ref testAndSet and the unary function operator except it returns the opposite Boolean.
     *
     *  Thread safety: This method is thread safe. */
    bool insert(const Value &value) {
        return !testAndSet(value);
    }

    /** Remove and track items from a vector.
     *
     *  Modifies @p vector by removing those items that are present in the @p tracker, and adds the other items to the @p
     *  tracker.  The relative order of the items in the vector is unchanged. The vector is processed from low to high indexes
     *  as the tracker is updated.
     *
     *  One use of this is similar to @c std::unique in that it can remove duplicates from a vector. However, unlike @c
     *  std::unique, this function can operate on unsorted vectors. For example:
     *
     *  @code
     *    std::vector<int> va{1, 2, 1, 8, 2, 3};
     *    std::vector<int> vb{4, 3, 2, 5, 8, 6};
     *    Tracker<int> tracker;
     *    tracker.removeIfSeen(va); // va = {1, 2, 8, 3} tracker = {1, 2, 3, 8}
     *    tracker.removeIfSeen(vb); // vb = {4, 5, 6}    tracker = {1, 2, 3, 4, 5, 6, 8}
     *  @endcode
     *
     *  Thread safety: Althrough the operations on the tracker are thread-safe, the operations on the vector are not. Therefore
     *  it's permissible to be using the specified tracker in two different threads as long as this thread is the only one
     *  accessing the vector. */
    void removeIfSeen(std::vector<Value> &vector) {
        size_t nSaved = 0;
        for (size_t i = 0; i < vector.size(); ++i) {
            if (insert(vector[i]))
                vector[nSaved++] = vector[i];
        }
        vector.resize(nSaved);
    }
};

} // namespace
} // namespace

#endif
