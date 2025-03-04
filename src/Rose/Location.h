#ifndef ROSE_Location_H
#define ROSE_Location_H
#include <RoseFirst.h>
#include <ostream>
#include <string>

namespace Rose {

/** Base class for location types.
 *
 *  A location is something that specifies a location for something else. For instance, a location might be part of a
 *  line in a source code file, or a memory address in a binary file.
 *
 *  All locations implement a common base API consisting of:
 *
 *  @li A @ref toString method that returns a string representation of the location.
 *  @li A @ref print method that sends a string representation of the location to an output stream.
 *  @li A @ref printableName method that returns a string that can be printed safely to a terminal.
 *  @li A static @c parse method in subclasses that parses the string representation returned by @ref printableName.
 *  @li An @ref isEmpty method that returns true if the object is default constructed.
 *  @li Comparison operators (all six).
 *  @li An explicit conversion to @c bool returning false for default-constructed locations. */
class Location {
public:
    // NOTICE: Subclasses must implemented a static "parse" method


    virtual ~Location() = default;

    /** Convert location to string.
     *
     *  Returns a string representation of the location. This representation is not necessarily parsable.
     *
     *  See also, @ref print. */
    virtual std::string toString() const = 0;

    /** Output location to a stream.
     *
     *  The format is the same as the @ref toString method and is not necessarily parsable.
     *
     *  See also, @ref toString. */
    virtual void print(std::ostream&) const = 0;

    /** Convert location to escaped, parsable string.
     *
     *  Returns a string that can be safely emitted to a terminal. The format should also be parsable so the static @c parse method
     *  in subclasses can create an equal object from the string. */
    virtual std::string printableName() const = 0;

    /** Equality and inequality.
     *
     *  Objects are equal if they are the same type and they point to the same location. Otherwise they are unequal.
     *
     *  The operator== is implemented in the base class only. For the expression `a == b` it calls both `a.isEqual(b)` and
     *  `b.isEqual(a)`, returning true only if both tests return true. Subclasses should implement `x.isEqual(y)` by dynamic casting
     *  @c y to the type of @c x and returning true if and only if the cast succeeds and @c x and @c y point to the same location.
     *
     *  The operator!= is implemented in the base class as the complement of operator==.
     *
     *  @{ */
    virtual bool isEqual(const Location &other) const = 0;
    virtual bool operator==(const Location &other) const final {
        return isEqual(other) && other.isEqual(*this);
    }
    virtual bool operator!=(const Location &other) const final {
        return !(*this == other);
    }
    /** @} */


    virtual bool operator<(const Location&) const = 0;
    virtual bool operator<=(const Location&) const = 0;
    virtual bool operator>(const Location&) const = 0;
    virtual bool operator>=(const Location&) const = 0;

    /** Test whether this object is valid.
     *
     *  @{ */
#ifdef _MSC_VER
    // buggy compiler unable to compile this declaration; error is "C2216: 'explicit' cannot be used with 'sealed'". So we'll
    // rely on the non-Microsoft compilers to enforce the "final".
    virtual explicit operator bool() const {
        return isValid();
    }
#else
    virtual explicit operator bool() const final {
        return isValid();
    }
#endif
    virtual bool isValid() const = 0;
    /** @} */

    /** Test whether this object is empty.
     *
     *  A default constructed location is empty. Other objects are valid.
     *
     * @{ */
    virtual bool operator!() const final {
        return isEmpty();
    }
    virtual bool isEmpty() const final {
        return *this ? false : true;
    }
    /** @} */
};

inline std::ostream& operator<<(std::ostream &out, const Location &x) {
    x.print(out);
    return out;
}


} // namespace
#endif
