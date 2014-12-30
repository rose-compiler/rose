#ifndef Sawyer_DistinctList_H
#define Sawyer_DistinctList_H

#include <boost/foreach.hpp>
#include <list>
#include <sawyer/Map.h>
#include <sawyer/Sawyer.h>
#include <stdexcept>

namespace Sawyer {
namespace Container {

/** A doubly-linked list of distinct items.
 *
 *  Each item on the list is unequal to any other item on the list. Most operations take O(log N) time. */
template<class T, class Cmp = std::less<T> >
class DistinctList {
public:
    typedef T Item;
    typedef Cmp Comparator;
    typedef std::list<Item> Items;                      // iterators must be insert- and erase-stable

private:
    typedef Sawyer::Container::Map<Item, typename Items::iterator, Comparator> Map;
    Items items_;
    Map position_;

public:
    /** Construct an empty list. */
    DistinctList() {}

    /** Copy-construct a list. */
    template<class T2, class Cmp2>
    DistinctList(const DistinctList<T2, Cmp2> &other) {
        BOOST_FOREACH (const T2 &item, other.items())
            pushBack(item);
    }

    /** Assign one list to another. */
    template<class T2, class Cmp2>
    DistinctList& operator=(const DistinctList<T2, Cmp2> &other) {
        clear();
        BOOST_FOREACH (const T2 &item, other.items())
            pushBack(item);
        return *this;
    }

    /** Clear the list.
     *
     *  Erase all items from the list making the list empty. */
    void clear() {
        items_.clear();
        position_.clear();
    }
    
    /** Determines whether list is empty.
     *
     *  Returns true if the list is empty, false if it contains anything.  Time complexity is constant. */
    bool isEmpty() const {
        return position_.isEmpty();
    }

    /** Number of items in list.
     *
     *  Returns the total number of items in the list in constant time. */
    size_t size() const {
        return position_.size();
    }

    /** Determine if an item exists.
     *
     *  Returns true if the specified item exists in the list, false if not. */
    bool exists(const Item &item) const {
        return position_.exists(item);
    }
    
    /** Determine the position of an item.
     *
     *  Returns the position of an item from the beginning of the list.  This is an O(n) operation. */
    size_t position(const Item &item) const {
        if (!position_.exists(item))
            return size();
        size_t retval = 0;
        BOOST_FOREACH (const Item &x, items_) {
            if (x == item)
                return retval;
            ++retval;
        }
        return retval;
    }
    
    /** Reference to item at front of list.
     *
     *  Returns a const reference to the item at the front of the list, or throws an <code>std::runtime_error</code> if the
     *  list is empty. */
    const Item& front() const {
        if (isEmpty())
            throw std::runtime_error("front called on empty list");
        return items_.front();
    }
    
    /** Reference to item at back of list.
     *
     *  Returns a const reference to the item at the back of the list, or throws an <code>std::runtime_error</code> if the
     *  list is empty. */
    const Item& back() const {
        if (isEmpty())
            throw std::runtime_error("back called on empty list");
        return items_.back();
    }
    
    /** Insert item at front of list if distinct.
     *
     *  If @p item does not exist in the list then insert a copy at the front of the list. If the item exists then do nothing. */
    void pushFront(const Item &item) {
        typename Map::NodeIterator found = position_.find(item);
        if (found == position_.nodes().end()) {
            items_.push_front(item);
            position_.insert(item, items_.begin());
        }
    }

    /** Insert item at back of list if distinct.
     *
     *  If @p item does not exist in the list then insert a copy at the back of the list. If the item exists then do nothing. */
    void pushBack(const Item &item) {
        typename Map::NodeIterator found = position_.find(item);
        if (found == position_.nodes().end()) {
            items_.push_back(item);
            position_.insert(item, --items_.end());
        }
    }

    /** Return and erase item at front of list.
     *
     *  Returns a copy of the item at the front of the list and that item is removed from the list.  Throws an
     *  <code>std::runtime_error</code> if the list is empty. */
    Item popFront() {
        if (isEmpty())
            throw std::runtime_error("popFront called on empty list");
        Item item = items_.front();
        items_.pop_front();
        position_.erase(item);
        return item;
    }

    /** Return and erase item at back of list.
     *
     *  Returns a copy of the item at the back of the list and that item is removed from the list.  Throws an
     *  <code>std::runtime_error</code> if the list is empty. */
    Item popBack() {
        if (isEmpty())
            throw std::runtime_error("popBack called on empty list");
        Item item = items_.back();
        items_.pop_back();
        position_.erase(item);
        return item;
    }

    /** Erase an item from the list.
     *
     *  Erases the item equal to @p item from the list if it exists, does nothing otherwise. */
    void erase(const Item &item) {
        typename Map::NodeIterator found = position_.find(item);
        if (found != position_.nodes().end()) {
            items_.erase(found->value());
            position_.eraseAt(found);
        }
    }

    /** Return all items as a list.
     *
     *  Returns all items as a list. The list is const since it should only be modified through this API. */
    const Items& items() const {
        return items_;
    }
};

} // namespace
} // namespace

#endif
