#if 0
// This is not legal C++ code.
void foo( int N, int a[N])
{
    // whatever
}
#endif


namespace X
   {
     int a;
   }

namespace X
   {
     int b;
   }

void foo()
   {
     using namespace X;
   }

