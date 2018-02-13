#if 0
This is one of the bugs found in ROSE-1110 In Development .

The error:

error: no suitable user-defined conversion ... exists

happens when a vector instance in a .cc file gets unparsed into a different vector instance in the rose_....cc file and it no longer agrees with the vector instance in the corresponfing .hxx file.

The changed vector instance is demonstrated with this code:
#endif

#include <algorithm>
#include <vector>
std::vector<unsigned> local1;

#if 0
gets unparsed into:

// #include <algorithm>
// #include <vector>
// std::vector< std::char_traits< unsigned int > ::int_type , class std::allocator< std::char_traits< unsigned int > ::int_type > > local1;
#endif

