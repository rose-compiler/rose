
// This is included here as specified in the Autoconf manual (using <> instead of "")
#include <config.h>

// DQ (9/12/2009): This appears to be required to define "strdup()" 
// when using header files from GNU g++ version 4.3 or greater.
#include <string.h>

#if 0
// START: Evaluate the macros that drive the compilation
#if defined(COMPILE_APP)
#error "COMPILE_APP is defined"
#else
#error "COMPILE_APP is NOT defined"
#endif

#if defined(COMPILE_SERIAL_APP)
#error "COMPILE_SERIAL_APP is defined"
#else
#error "COMPILE_SERIAL_APP is NOT defined"
#endif

// END: Evaluate the macros that drive the compilation
#endif

// Use correct include file depending whether templates are used or not

#if defined(USE_TEMPLATES)
#   include "A++_templates.h"
#else
#   include "A++_notemplates.h"
#endif

