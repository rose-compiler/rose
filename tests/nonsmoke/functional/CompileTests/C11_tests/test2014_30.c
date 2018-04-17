/* This C11 feature is not defined for the Intel v14 compiler (only later versions). */
#if (defined(__INTEL_COMPILER) && (__INTEL_COMPILER > 1400)) || !defined(__INTEL_COMPILER)
_Alignas(int) float x;
#endif
