
int x;

#if 1
/* Review this issue later (problem with MSVC support for reference to compiler defined std namespace not yet seen). */
// The "std" namespace is implicitly defined in C++, so it 
// can be referenced even if it was not defined explictly.
using namespace std;
#endif


