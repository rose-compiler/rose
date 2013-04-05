#ifndef ROSE_WorkList_H
#define ROSE_WorkList_H

#include <cassert>
#include <list>
#include <map>

/** List of things to work on.  A WorkList is an ordered list of items of user-defined type.  Items are pushed onto either the
 *  front or back of the list and are removed from either the front or back, allowing the WorkList to be used as a queue or
 *  stack.  The modifier method names (push, pop, unshift, shift) are the same as for lists in Perl, and the worklist operates
 *  in a similar manner.  Items can be added and removed in any order from either end of the data structure.
 *
 *  When adding items via unshift() or push(), a second Boolean argument, @p unique, determines whether duplicates are allowed
 *  in the list.  If true (the default) then the unshift() or push() is a no-op if the item is already in the list.  Insertion
 *  and removal are log(n) time complexity regardless of whether duplicates are allowed. If you itend to use the WorkList in a
 *  way that always allows duplicates then you're better off using an std::list to obtain constant-time insertion and removal
 *  operations. */
template<typename T, class Compare = std::less<T> >
class WorkList {
public:
    typedef T value_type;
    typedef Compare key_compare;

    WorkList(): nitems_(0) {}

    /** Returns true if this work list is empty. This is a constant time operation. */
    bool empty() const { return 0 == nitems_; }

    /** Returns the number of items in the work list. This is a constant time operation. */ 
    size_t size() const { return nitems_; }

    /** Return true if the specified item is already on the work list. Time complexity is order log(n). */
    bool exists(const T &item) const { return map_.find(item)!=map_.end(); }

    /** Add an item to the front of the work list.  If @p unique is true (the default) then the item is added to the list only
     *  if it's not already on the list. Returns true if the item was added to the list. */
    bool unshift(const T&, bool unique=true);

    /** Remove and return the item from the front of the work list.  The list must not be empty. */
    T shift();

    /** Add an item to the back of the work list. If @p unique is true (the default) then the item is added to the list only
     *  if it's not already on the list. Returns true if the item was added to the list. */
    bool push(const T&, bool unique=true);

    /** Remove an item from the back of the work list.  The list must not be empty. */
    T pop();

private:
    void removed(const T&);
    std::list<T> list_;
    std::map<T, size_t, key_compare> map_;
    size_t nitems_;
};

/*******************************************************************************************************************************
 * Template member functions
 *******************************************************************************************************************************/

template<typename T, class Compare>
bool
WorkList<T, Compare>::unshift(const T &item, bool unique)
{
    std::pair<typename std::map<T, size_t, Compare>::iterator, bool> found = map_.insert(std::pair<T, size_t>(item, 1));
    if (unique && !found.second)
        return false;
    if (!found.second)
        ++found.first->second;
    list_.push_front(item);
    ++nitems_;
    return true;
}

template<typename T, class Compare>
bool
WorkList<T, Compare>::push(const T &item, bool unique)
{
    std::pair<typename std::map<T, size_t, Compare>::iterator, bool> found = map_.insert(std::make_pair(item, 1));
    if (unique && !found.second)
        return false;
    if (!found.second)
        ++found.first->second;
    list_.push_back(item);
    ++nitems_;
    return true;
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
void
WorkList<T, Compare>::removed(const T &item)
{
    typename std::map<T, size_t>::iterator found = map_.find(item);
    assert(found!=map_.end());
    if (found->second > 1) {
        --found->second;
    } else {
        map_.erase(found);
    }
    --nitems_;
}

#endif
