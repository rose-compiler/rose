#ifndef ROSE_WorkLists_H
#define ROSE_WorkLists_H

#include <boost/logic/tribool.hpp>
#include <cassert>
#include <list>
#include <map>

/** List of things to work on.  A WorkList is an ordered list of items of user-defined type.  Items are inserted (and copied)
 *  onto either the front or back of the list and are removed from either the front or back, allowing the WorkList to be used
 *  as a queue, stack, or combination.  The modifier method names (push, pop, unshift, shift) are the same as for lists in
 *  Perl, and the worklist operates in a similar manner: push/pop operate on one end of the list while unshift/shift operate on
 *  the other end of the list.
 *
 *  Methods that insert items can optionally check that the item to be inserted is not already in the list, and skip the
 *  insertion if it is.  Their behavior in this regard is controlled by an optional Boolean (actually, boost::tribool) argument
 *  that that enables or disables the uniqueness check.  If the argument is indeterminate (the default) then the uniqueness
 *  check depends on whether uniqueness checking was enabled or disabled when the list was constructed.  The optional argument
 *  allows a list that was constructed one way to be used a different way, although doing so may be less efficient.
 *
 *  Subclasses WorkListUnique and WorkListNonUnique are convenient ways to create a worklist and provide better documentation
 *  as to the default setting for uniqueness checking.
 *
 *  @section Complexity Time and space complexity
 *
 *  Lists that are constructed to perform uniqueness checks store two copies of their elements: the extra copy is an std::map
 *  that's used for its ability to do O(log(N)) lookups for the uniqueness checks.  Insertion is O(log N) regardless of whether
 *  uniqueness is checked at the time of insertion.  Removal and existance are also O(log N).
 *
 *  Lists that are constructed to not perform uniqueness checks store only one copy of each element. Insertion and removal are
 *  constant time operations, but existance is linear.
 *
 *  Size is always a constant time operation even if the list is implemented with std::list.
 *
 *  @section Ex1 Examples
 *
 *  The following code prints "7123456789"
 *
 *  @code
 *   static int tail[3] = {7, 8, 9}, head[3] = {3, 2, 1};
 *   WorkListUnique<int> worklist;
 *   worklist.push(5);
 *   worklist.unshift(4);
 *   worklist.push(5); //no-op for WorkListUnique
 *   worklist.push(6);
 *   worklist.push(tail+0, tail+3);
 *   worklist.unshift(head+0, head+3);
 *   worklist.unshift(6, true);  //no-op
 *   worklist.unshift(7, false); //disable uniqueness check
 *   while (!worklist.empty())
 *      std::cout <<worklist.shift();
 *  @endcode
 *
 * If we change the worklist to a WorkListNonUnique, then it would print "71234556789" instead (the only difference is that the
 * second push(5) defaulted to non-unique insertion).
 */
template<typename T, class Compare = std::less<T> >
class WorkList {
public:
    typedef T value_type;
    typedef Compare key_compare;

    explicit WorkList(bool check_uniqueness=false): nitems_(0), use_map_(check_uniqueness) {}

    /** Returns true if this work list is empty. This is a constant time operation. */
    bool empty() const { return 0 == nitems_; }

    /** Returns the number of items in the work list. This is a constant time operation. */ 
    size_t size() const { return nitems_; }

    /** Reset the list to an empty state. */
    void clear() { list_.clear(); map_.clear(); }

    /** Return true if the specified item is already on the work list. Time complexity is O(log(N)). */
    bool exists(const T &item) const {
        return use_map_ ? map_.find(item)!=map_.end() : std::find(list_.begin(), list_.end(), item)!=list_.end();
    }

    /** Add an item to the front of the work list.  If @p unique is true (the default) then the item is added to the list only
     *  if it's not already on the list. Returns true if the item was added to the list. Time complexity is O(log(N)). */
    bool unshift(const T&, boost::tribool check_uniqueness=boost::indeterminate);

    /** Adds multiple items to the front of the work list.  If @p unique is true (the default) then an item is added to the
     *  list only if it's not already on the list. Returns the number of items that were added to the list. Time complexity is
     *  O(M log(N+M)) where M is the number of items being inserted. */
    template<class Iterator>
    size_t unshift(const Iterator &begin, const Iterator &end, boost::tribool check_uniqueness=boost::indeterminate);

    /** Remove and return the item from the front of the work list.  The list must not be empty. Time complexity is O(log(N)). */
    T shift();

    /** Add an item to the back of the work list. If @p unique is true (the default) then the item is added to the list only
     *  if it's not already on the list. Returns true if the item was added to the list. Time complexity is O(log(N)). */
    bool push(const T&, boost::tribool check_uniqueness=boost::logic::indeterminate);
    
    /** Adds multiple items to the back of the work list.  If @p unique is true (the default) then an item is added to the
     *  list only if it's not already on the list. Returns the number of items that were added to the list. Time complexity is
     *  O(M log(N+M)) where M is the number of items being inserted. */
    template<class Iterator>
    size_t push(const Iterator &begin, const Iterator &end, boost::tribool check_uniqueness=boost::indeterminate);

    /** Remove an item from the back of the work list.  The list must not be empty. Time complexity is O(log(N)). */
    T pop();

    /** Returns the object at the front/back of the list without removing it.  The list must not be empty. Items are returned
     * by const reference because changing their value could change their sort order. If you need to change the value of the
     * front or back element then do so by a shift-modify-unshift or pop-modify-push sequence. Time complexity is O(1).
     * @{ */
    const T& front() const;
    const T& back() const;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // These are here for compatibility with another WorkList API
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Adds an item(s) to the end of the queue.  The item is not added if the list was constructed to check for duplicates and
     * the item being inserted is already in the list.
     * @{ */
    void add(const T& item) { push(item); }
    void add(const std::vector<T> &items) { push(items.begin(), items.end()); }
    void add(const std::set<T> &items) { push(items.begin(), items.end()); }
    /** @} */

    /** Remove and return an item from the front of the work list.  The list must not be empty. */
    T take() { return shift(); }

private:
    void removed(const T&);
    std::list<T> list_;
    std::map<T, size_t, key_compare> map_;
    size_t nitems_;
    bool use_map_;
};

/** A version of WorkList that checks for uniqueness by default. The insertion methods check for item uniqueness and do not
 *  insert the item if it currently exists in the list. The uniqueness checking can be disabled per insert operation by
 *  specifying a false value for the check_uniqueness argument. Insertions and existence are O(log N) regardless of whether
 *  uniqueness checks are performed. Such a worklist internally stores two copies of every element. */
template<typename T, class Compare = std::less<T> >
class WorkListUnique: public WorkList<T, Compare> {
public:
    WorkListUnique(): WorkList<T, Compare>(true) {}
};

/** A version of WorkList that does not check for uniqueness by default.  The insertion methods do not check for uniqueness and
 *  always insert the specified item into the list.  The uniqueness checking can be enabled per insert operation by specifying
 *  a true value for the check_uniqueness argument.  Insertions are O(1) by default, but O(N) if a uniqueness check is
 *  performed.  Existence checks are always O(N). */
template<typename T, class Compare = std::less<T> >
class WorkListNonUnique: public WorkList<T, Compare> {
public:
    WorkListNonUnique(): WorkList<T, Compare>(false) {}
};

/*******************************************************************************************************************************
 * Template member functions
 *******************************************************************************************************************************/

template<typename T, class Compare>
bool
WorkList<T, Compare>::unshift(const T &item, boost::tribool check_uniqueness)
{
    if (indeterminate(check_uniqueness))
        check_uniqueness = use_map_;
    if (use_map_) {
        std::pair<typename std::map<T, size_t, Compare>::iterator, bool> found = map_.insert(std::pair<T, size_t>(item, 1));
        if (check_uniqueness && !found.second)
            return false;
        if (!found.second)
            ++found.first->second;
    } else if (check_uniqueness) {
        if (std::find(list_.begin(), list_.end(), item)!=list_.end())
            return false;
    }
    list_.push_front(item);
    ++nitems_;
    return true;
}

template<typename T, class Compare>
template<class Iterator>
size_t
WorkList<T, Compare>::unshift(const Iterator &begin, const Iterator &end, boost::tribool check_uniqueness)
{
    size_t retval = 0;
    for (Iterator i=begin; i!=end; ++i) {
        if (unshift(*i, check_uniqueness))
            ++retval;
    }
    return retval;
}

template<typename T, class Compare>
bool
WorkList<T, Compare>::push(const T &item, boost::tribool check_uniqueness)
{
    if (indeterminate(check_uniqueness))
        check_uniqueness = use_map_;
    if (use_map_) {
        std::pair<typename std::map<T, size_t, Compare>::iterator, bool> found = map_.insert(std::make_pair(item, 1));
        if (check_uniqueness && !found.second)
            return false;
        if (!found.second)
            ++found.first->second;
    } else if (check_uniqueness) {
        if (std::find(list_.begin(), list_.end(), item)!=list_.end())
            return false;
    }
    list_.push_back(item);
    ++nitems_;
    return true;
}

template<typename T, class Compare>
template<class Iterator>
size_t
WorkList<T, Compare>::push(const Iterator &begin, const Iterator &end, boost::tribool check_uniqueness)
{
    size_t retval = 0;
    for (Iterator i=begin; i!=end; ++i) {
        if (push(*i, check_uniqueness))
            ++retval;
    }
    return retval;
}

template<typename T, class Compare>
T
WorkList<T, Compare>::shift()
{
    assert(!empty());
    T item = list_.front();
    list_.pop_front();
    removed(item);
    return item;
}

template<typename T, class Compare>
T
WorkList<T, Compare>::pop()
{
    assert(!empty());
    T item = list_.back();
    list_.pop_back();
    removed(item);
    return item;
}

template<typename T, class Compare>
const T&
WorkList<T, Compare>::front() const
{
    assert(!empty());
    return list_.front();
}

template<typename T, class Compare>
const T&
WorkList<T, Compare>::back() const
{
    assert(!empty());
    return list_.back();
}

template<typename T, class Compare>
void
WorkList<T, Compare>::removed(const T &item)
{
    if (use_map_) {
        typename std::map<T, size_t>::iterator found = map_.find(item);
        assert(found!=map_.end());
        if (found->second > 1) {
            --found->second;
        } else {
            map_.erase(found);
        }
    }
    --nitems_;
}

#endif
