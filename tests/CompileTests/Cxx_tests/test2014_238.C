// This demonstrates an error in ROSE within the interaction of two boost header files.

#include "boost/function.hpp"

// #include "boost/unordered_map.hpp"
namespace boost
{
    template <class Key, class T, class Hash, class Pred, class Alloc>
    class unordered_map
    {
#if 1
        friend bool operator==<Key, T, Hash, Pred, Alloc>( unordered_map const&, unordered_map const& );
#endif
    };
}
