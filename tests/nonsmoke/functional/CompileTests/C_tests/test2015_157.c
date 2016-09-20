#if 1
#include "test2015_157.h"
#else
extern __thread int tls__current_cpu;
#endif

// Original code:
// __thread int tls__current_cpu;
// Unparsed code:
// int tls__current_cpu;
__thread int tls__current_cpu;
