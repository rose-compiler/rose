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
