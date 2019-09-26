// Right angle bracket

// C++03's parser defines \u201c>>\u201d as the right shift operator in all cases. However, with nested 
// template declarations, there is a tendency for the programmer to neglect to place a space between the 
// two right angle brackets, thus causing a compiler syntax error.

#include<vector>

// C++11 improves the specification of the parser so that multiple right angle brackets will be 
// interpreted as closing the template argument list where it is reasonable. This can be overridden 
// by using parentheses:

template<bool Test> class SomeType {};

#if 0
std::vector<SomeType<1>2>> x1;  // Interpreted as a std::vector of SomeType<true> 2>,
// which is not legal syntax. 1 is true.
#endif

std::vector<SomeType<(1>2)>> x1;  // Interpreted as std::vector of SomeType<false>,
// which is legal C++11 syntax. (1>2) is false.

