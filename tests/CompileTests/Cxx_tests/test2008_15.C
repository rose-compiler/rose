
// The simplest reproduction case is just including an SSE intrinsic header file, e.g.:
// This gives parse errors using both the GCC 4.3.0 and ICC 10.0 versions of the intrinsic header files, for the reasons mentioned earlier.
// Thanks,
// Justin Holewinski

#include <xmmintrin.h>
/* Empty source file */
