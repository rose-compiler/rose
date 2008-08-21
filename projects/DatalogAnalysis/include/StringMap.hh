#ifndef STRINGMAP_HH
#define STRINGMAP_HH

#include "common.hh"
#include <ext/hash_map>
#include <string>

/*this class hashes strings */
struct HashString {
    size_t operator()( const std::string &s ) const
    {
        __gnu_cxx::hash<const char*> o;
        return o( s.c_str() );
    }
};

/* Convenient definition for hash_maps on strings.
 * Unfortunately, one can't templatize typedef's hence the structs.
 */
template <class Data>
struct StringMap {
    typedef __gnu_cxx::hash_map< std::string, Data, HashString  > Type;
};

template <class Data>
struct StringMultiMap {
    typedef __gnu_cxx::hash_multimap< std::string, Data, HashString  > Type;
};

#endif
