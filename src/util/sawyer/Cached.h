// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          github.com:matzke1/sawyer.




#ifndef Sawyer_Cached_H
#define Sawyer_Cached_H

#include <sawyer/Sawyer.h>
#include <sawyer/Optional.h>

namespace Sawyer {

/** Implements cache data members.  A cache data member either stores a value or stores no value. When its value is
 *  present, it can be obtained, and when absent it has no value (not even a default constructed value).  The value of a
 *  cache data member can be modified even when its object is const.  For instance, an object that caches an
 *  expensive-to-compute value could be declared thusly:
 *
 * @code
 *  struct Foo {
 *      Cached<Bar> bar_;
 *  };
 * @endcode
 *
 *  When a Foo object is constructed the @c bar_ member will contain nothing (not even a default-constructed
 *  object). Here's a sample implementation that returns the value, performing the expensive calculation and caching the
 *  result if necessary.  The method is declared to take a const @c this pointer since it is conceptually a data member
 *  accessor:
 *
 * @code
 *  const Bar& Foo::bar() const { //typical accessor declaration
 *      if (!bar_.isCached())
 *          bar_ = expensiveComputation();
 *      return bar_.get();
 *  }
 * @endcode */
template<typename T>
class Cached {
public:
    /** Type of stored value. */
    typedef T Value;
private:
    mutable Sawyer::Optional<Value> value_;
public:
    /** Cached state.
     *
     *  Returns true when a value is cached, or false when nothing is cached. */
    bool isCached() const {
        return bool(value_);
    }

    /** Remove cached value.
     *
     *  Any value that might be cached is removed from the cache.  If the cache is already empty then nothing happens. */
    void clear() const {                                // const is intended
        value_ = Sawyer::Nothing();
    }

    /** Return the cached value.
     *
     *  If a value is cached then it is returned.  This should not be called unless a cached value is present (see @ref
     *  isCached).  An alternative is to use @ref getOptional.
     *
     *  @{ */
    const Value& get() const {
        return value_.get();
    }
    Value& get() {
        return value_.get();
    }
    /** @} */

    /** Return cached value or nothing.
     *
     *  If a value is cached then it is returned, otherwise nothing is returned. */
    const Sawyer::Optional<Value>& getOptional() const {
        return value_;
    }

    /** Assign a new value.
     *
     *  The specified value is cached.  Note that if the cache is holding a pointer to a value then caching a null pointer is
     *  different than caching nothing.  Setting the value to an instance of @ref Nothing has the same effect as calling @ref
     *  clear.
     *
     *  @{ */
    void set(const Value &x) const {                    // const is intentional
        value_ = x;
    }
    const Cached& operator=(const Value &x) const {     // const is intentional
        value_ = x;
        return *this;
    }
    /** @} */
};

} // namespace

#endif
