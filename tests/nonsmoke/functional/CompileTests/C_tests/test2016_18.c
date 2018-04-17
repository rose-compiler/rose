/* See test2016_19.c and test2016_20.c for better examples of this test. */

struct point { const int x, y; };

// These have to be constants if they are in global scope.
const int xvalue = 1;
const int yvalue = 1;

#ifndef __GNUC__
// If this is in global scope then we can't use the extra {} that ROSE generates.
// But also this code is non standard and is an error with GNU 4.8 but only a waring with EDG and Intel.
const struct point p = { .y = yvalue, .x = xvalue };
#else
#ifdef __INTEL_COMPILER
const struct point p = { .y = yvalue, .x = xvalue };
#endif
#ifdef __EDG_VERSION__
const struct point p = { .y = yvalue, .x = xvalue };
#endif
#endif
