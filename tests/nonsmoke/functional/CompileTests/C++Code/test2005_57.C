// Test code to test use of explicit template declarations (not sure what they mean but they cause 
// instantiations to be generated in g++).

// NOTE: Need to test variables and function declarations as well (anything else???)

#include <string>

// Build a templated function
template<typename T> void foobar (T x){ }

template<typename T> class X { public: static T y; };

template <typename T> T min (T a, T b) { return a < b ? a : b; }

using namespace std;

// Explicit instantiation of templated class
template class basic_string<char>;

// Explicit instantiation of templated static member (does not seem to work!)
// template int X<int>::y;

// Explicit instantiation of templated function
// This is unparsed as: "template void foobar < int > (int x,int x);"
template void foobar<int>(int x);

#if 1
// Explicit instantiation of templated function
template int min<int>(int x, int y);

// Explicit instantiation of templated member function
template basic_string<char> & basic_string<char>::append<string::iterator>(string::iterator __first, string::iterator __last);
#endif
