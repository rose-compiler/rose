// This bug is fixed enough to now output the __thread keyword (GNU extension).
// However, the typeof opertor is not output and this might be work investigating 
// further at a later date.

typedef struct CPUState CPUState;

struct CPUState 
   {
     int nr_cores;
   };

#if 1
#include "test2015_156.h"
#else
extern __thread __typeof__(CPUState *) tls__current_cpu;
#endif

// Original code:
// __thread __typeof__(CPUState *) tls__current_cpu;
// Unparsed code:
// CPUState *tls__current_cpu;
__thread __typeof__(CPUState *) tls__current_cpu;
