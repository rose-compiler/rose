// Build a templated function
template<typename T> void foobar (T x){ }

// template<typename T> class X { public: static T y; };

// template <typename T> T min (T a, T b) { return a < b ? a : b; }

// using namespace std;

// Explicit instantiation of templated function
// This is unparsed as: "template void foobar < int > (int x,int x);"
template void foobar<int>(int x);
