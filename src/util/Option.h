#ifndef ROSE_Option_H
#define ROSE_Option_H

#include <exception>

/** Stores a value or nothing. Similar to Scala's Option type. This is particularly useful for std::map-like containers when
 *  one wants to obtain either a value stored in the map, or nothing if the value's key is not present in the map.  The STL
 *  approach requires one to first obtain an iterator, then compare the iterator to the end iterator, then conditionally
 *  dereference the iterator.  On the other hand, a map lookup could return an Option that contains either the found value or a
 *  user-supplied default value.  Thus, code like this, using the STL:
 *
 * @code
 *  typedef std::map<std::string, FileInfo*> Files;
 *  Files files = ...; // populate file info
 *  ...
 *  // Process a particular file
 *  Files::iterator found = files.find("README.txt");
 *  FileInfo *info = found == files.end() ? NULL : found->second;
 *  if (info)
 *      // do whatever
 * @endcode
 *
 * becomes like this using Option:
 *
 * @code
 *  typedef Map<std::string, FileInfo*> Files;
 *  Files files = ...; // populate file info
 *  ...
 *  // Process a particular file
 *  if (FileInfo *info = files.get("README.txt").getOrElse(NULL))
 *      // do whatever
 * @endcode
 *
 * In fact, since Map defines its own getOrElse(), this can be shortened even more:
 *
 * @code
 *  if (FileInfo *info = files.getOrElse("README.txt", NULL))
 * @endcode
 *
 * Note that an Option stores a copy of a value rather than a reference to a value.
 */
template<typename T>
class Option {
public:
    typedef T value_type;

    /** Construct an Option that stores nothing. */
    Option(): empty_(true) {}

    /** Construct an Option that stores a value. */
    Option(const T& value): value_(value), empty_(false) {}     // implicit

    /** Return true if this option contains no value. Scala uses isEmpty(), but empty() is more C++ STL-like. */
    bool empty() const { return empty_; }

    /** Return the value, assuming there is one. */
    const T& get() const {
        if (empty())
            throw std::domain_error("key not present in map");
        return value_;
    }

    /** Return a value or the specified default.  We use CamelCase like Scala, rather than underscores like C++ STL. */
    const T& getOrElse(const T& dflt) const { return empty() ? dflt : value_; }

private:
    T value_;                   // the stored value, but only if empty_ is false
    bool empty_;                // true if no value is stored.
};

#endif
