#ifndef ROSE_Map_H
#define ROSE_Map_H

#include <boost/optional.hpp>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#endif

#include <stdexcept>
#include <map>

/** Extends std::map with methods that return optional values. */
template<class Key, class T, class Compare=std::less<Key>, class Alloc=std::allocator<std::pair<const Key, T> > >
class Map: public std::map<Key, T, Compare, Alloc> {
public:
    typedef std::map<Key, T, Compare, Alloc> map_type;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(map_type);
    }
#endif

public:
    // Constructors are the same as for std::map
    Map() {}

    explicit Map(const Compare& comp, const Alloc& alloc = Alloc())
        : map_type(comp, alloc) {}

    template <class InputIterator>
    Map(InputIterator first, InputIterator last, const Compare& comp = Compare(), const Alloc& alloc = Alloc())
        : map_type(first, last, comp, alloc) {};

    Map(const Map& other)
        : map_type(other) {}

    Map& operator=(const Map&) = default;

    /** Lookup and return a value or nothing.  This is similar to the operator[] defined for std::map, but does not add a new
     *  pair to the map when the key does not exist.  It returns a copy of the value that was found (if found). Here's an
     *  example of the usual way to use this:
     *
     * @code
     *  Map<std::string, FileInfo> files;
     *  ...
     *  std::string filename = "foo.txt";
     *  FileInfo info = files.get(filename).get_value_or(FileInfo());
     * @endcode
     */
    boost::optional<T> get(const Key &key) const {
        typename map_type::const_iterator found = this->find(key);
        return found==this->end() ? boost::optional<T>() : boost::optional<T>(found->second);
    }

    /** Look up one value or throw an exception.  This is like get() except it can't handle the case where the map does not
     * contain the requested key.  It is unlike operator[] in that it doesn't add a new default-constructed value. The return
     * value is a reference to the value stored in the map.
     * @{ */
    const T& get_one(const Key &key) const {
        typename map_type::const_iterator found = this->find(key);
        if (found==this->end())
            throw std::domain_error("key not present in map");
        return found->second;
    }
    T& get_one(const Key &key) {
        typename map_type::iterator found = this->find(key);
        if (found==this->end())
            throw std::domain_error("key not present in map");
        return found->second;
    }
    /** @} */
    
    /** Convenience for getting a value from an Option.  Since the "map.get(key).get_value_or(dflt)" idiom is so frequenct, a
     *  get_value_or() method is defined directly on the map, combining both arguments into a single method call.
     * @{ */
    const T& get_value_or(const Key& key, const T& dflt) const {
        typename map_type::const_iterator found = this->find(key);
        return found==this->end() ? dflt : found->second;
    }
    T& get_value_or(const Key& key, T& dflt) {
        typename map_type::iterator found = this->find(key);
        return found==this->end() ? dflt : found->second;
    }
    /** @} */

    /** Convenience for determining if a key exists in this map.  The STL map requires one to do a find() and compare the
     *  result to end().  Although that can be more efficient when the value for that key will be used (because one just
     *  dereferences the already-obtained iterator rather than searching again), it requires more code when all one wants to do
     *  is check for existence. */
    bool exists(const Key &key) const { return this->find(key)!=this->end(); }
};

#endif
