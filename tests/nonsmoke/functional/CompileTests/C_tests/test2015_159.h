#define always_inline __attribute__ (( always_inline )) __inline__

// The best fix for this is the modify the header file to avoid this next line.
// Removed the dependence upon optimization (specification of -O2 on command line).
// #ifdef __OPTIMIZE__
#define inline always_inline
// #endif

