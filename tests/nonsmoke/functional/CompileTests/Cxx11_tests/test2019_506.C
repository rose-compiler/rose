// ROSE-2181

// In this test code: half of the namespace declaration is in a different file.

namespace namespace_1 {

}

// #include "ROSE-79a.h"
#include "test2019_506.h"

}

 
#if 0
And header file:

namespace namespace_1 {
#endif

