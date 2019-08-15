// This is reproducer ROSE-37

// #include "ROSE-37-a.h"
#include "test2018_107.h"

namespace namespace1 {
void func1 () {
  int local1 = namespace2::array2[1];
}
}

 
#if 0
// and this code (ROSE-37-a.h):

namespace namespace1 {
namespace namespace2 {

static const int array1[ 2 ] = {

// define is needed:
#define X -1
    0, X

#undef X
};

static const int array2[ 2 ] = {
    1, 0
};
}
} 
#endif
 
#if 0
get this error:
rose_ROSE-37.cxx(7): error: namespace "namespace1" has no member "array2" int local1 = namespace1::array2[1];
#endif
