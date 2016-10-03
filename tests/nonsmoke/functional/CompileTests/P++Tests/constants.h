/* Constants used in the Machine Dependent Interface */

#ifdef HAVE_CONFIG_H
/* This is included here as specified in the Autoconf manual (using <> instead of "") */
/* #include <config.h> */
#endif

#include <assert.h>

/* Prototypes for functions defined in common_code.c */
void MDI_ABORT();
void MDI_Assertion_Support ( char* Source_File_With_Error, unsigned Line_Number_In_File );

/* We need a special C code version of assert for the MDI layer within A++/P++. */
#if defined(MDI_NDEBUG) || defined(NDEBUG)
/* Define a version of assert for MDI code in A++/P++ (case of NDEBUG defined) */
#define MDI_ASSERT(f) NULL
/* #define MDI_ASSERT(f) assert(f) */
#else
/* Define a version of assert for MDI code in A++/P++ (case of NDEBUG NOT defined) */
#define MDI_ASSERT(f) \
     if(f)            \
          (void)NULL; \
     else             \
          MDI_Assertion_Support (__FILE__,__LINE__)
#endif

/* GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
 */
#ifdef __GNUC__
#pragma interface
#endif

/* ... MAX_ARRAY_DIMENSION might be different for A++ and P++
  because P++ can have a maximum of 4 dimensions right now ... */
#if defined(COMPILE_APP)
#include "../../../A++/include/max_array_dim.h"
/* error "Test: Compiling A++ MDI" */
#elif defined(COMPILE_PPP)
#include "../../../P++/include/max_array_dim.h"
/* error "Test: Compiling P++ MDI" */
#endif

/* Use the Max dimension size used in the A++ or P++ code */
#define MAXDIMS MAX_ARRAY_DIMENSION

#define TRUE  1
#define FALSE 0

/* Skip index compression - untill later */
#define NODIMCOMPRESS

/* Turn on MDI debugging information only if A++ internal debugging is turned on */
#if INTERNALDEBUG
#define MDI_DEBUG TRUE
#endif

/* (MDI Performance fix: 1/3/94)
   Because seperate compilation forces the values of the constants to be unknow at
   compile time for 99% of the MDI interface these have been changed to constants
   so that the compiler can do optimizations that were previously not possible!
   This change makes no difference in the readability of the code!
*/

/* Access index values into A++ descriptor objects for relevant values */
/* This allows the C or FORTRAN code to READ the C++ decriptor objects */

/* include "int_array.h" */
#include "array_domain.h"

#define MDI_DECIMAL_DISPLAY_FORMAT 0
#define MDI_EXPONENTIAL_DISPLAY_FORMAT 1

















