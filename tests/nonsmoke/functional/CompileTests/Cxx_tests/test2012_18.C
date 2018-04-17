#if 0
#include<map>

using namespace std;

map<int,int> xmap;
#endif



// Simpler code demonstrating the same bug as the map<> example demonstrates.
template<typename T> class X;
template<typename T> class X;

// This works fine.
// class X;
// class X;


