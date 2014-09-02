#ifdef USE_ROSE

/* The restrict keyword is different for GNU-based compilers (like on Chaos4)
 * than for non-GNU-based compilers (like on AIX) */
#if defined(__GNUC__) || defined(__PGIC__)
// #error "Define RESTRICT_KEYWORD as __restrict__"
#define XXX_RESTRICT_KEYWORD __restrict__ /* not aliased */
#else
// #error "Define RESTRICT_KEYWORD as restrict"
#define XXX_RESTRICT_KEYWORD restrict /* not aliased */
#endif

#else

/* Define macro as blank to not compile using the restrict keyword */
#define XXX_RESTRICT_KEYWORD

#endif

// With mpiicpc XXX_RESTRICT_KEYWORD expands to restrict (fails), but 
// with gnu g++ this expands to __restrict__ (works).
void foobar( double * XXX_RESTRICT_KEYWORD y )
   {
     y[1] = 0;
   }

