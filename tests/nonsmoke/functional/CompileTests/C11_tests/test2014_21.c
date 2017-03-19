/* This C11 feature is not defined for the Intel v14 compiler (only later versions). */
#if defined(__INTEL_COMPILER) && (__INTEL_COMPILER > 1400)
_Alignas(int) float x;
#endif
__attribute__((aligned(8))) float z;


// float __attribute__((aligned(8))) y;
// __attribute__((aligned(8))) float z;
// float z __attribute__((aligned(8)));

// float x[3] __attribute__((align(0x100))) = { 1.0, 2.0, 3.0 };
// float b[4] __attribute__((aligned(0x1000))) = { 1.0, 2.0, 3.0, 4.0 };  EDG reports error: invalid alignment value specified by attribute
// float b[4] __attribute__((aligned(8))) = { 1.0, 2.0, 3.0, 4.0 };
