#ifndef STRINGMAP_HH
#define STRINGMAP_HH

#include "common.hh"
#include <ext/hash_map>

/*this class hashes strings */
struct HashString {
    size_t operator()( const string &s ) const
    {
        hash<const char*> o;
        return o( s.c_str() );
    }
};

/* Convenient definition for hash_maps on strings.
 * Unfortunately, one can't templatize typedef's hence the structs.
 */
template <class Data>
struct StringMap {
    typedef hash_map< string, Data, HashString  > Type;
};

template <class Data>
struct StringMultiMap {
    typedef hash_multimap< string, Data, HashString  > Type;
};

#endif
