#ifndef ROSE_Predef_H
#define ROSE_Predef_H
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file contains definitions needed by all translation units.
//
// Rules for this header file:
//
//    1. First and formost, it should be small and fast. This header is included by every translation unit for the ROSE library, all
//       translation units for ROSE code generaton, and all user tools employing the ROSE library.
//
//    2 If it's something that needs to be defined before any other header files are included, then place it in this header.
//
//    3. If the definiton is needed by all translation units, then place it in this file. Beware that the requirements for ROSE
//       configured for source analysis are very different than when ROSE is configured for binary analysis.
//
//    4 If the definition is needed by all translation units that are part of a major analysis capability, such as source analysis
//      or binary analysis, then it MUST be conditionally compiled. Users of binary analysis don't want to pay the price to compile
//      source analysis if they don't need it, and vice versa.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Much of ROSE's old binary support uses the intX_t and uintX_t types (where X is a bit width), so we need to have the stdc printf
// format macros defined for portability.  We do that here because it needs to be done before <inttypes.h> is included for the first
// time, and we know that most source files for the ROSE library include this header at or near the beginning.  We
// don't want to define __STDC_FORMAT_MACROS in user code that includes "rose.h" (the user may define it), and we need to define it
// in such a way that we won't get warning's if its already defined.  [RMP 2012-01-29]
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

// This is first because it quickly brings in all the configuration-related settings that are needed by #ifdef's in the rest of this
// header.
#include "featureTests.h"

// These symbols are defined here because they need to be defined early, before any other Cereal headers are included. And we need
// to define them in order to avoid conflicts with boost::serialization that uses the same names.
#ifdef ROSE_HAVE_CEREAL
    #ifdef CEREAL_SERIALIZE_FUNCTION_NAME
        #include <rose_pragma_message.h>
        ROSE_PRAGMA_MESSAGE("sage3basic.h must be included before Cereal header files")
    #endif
    #define CEREAL_SAVE_FUNCTION_NAME cerealSave
    #define CEREAL_LOAD_FUNCTION_NAME cerealLoad
    #define CEREAL_SERIALIZE_FUNCTION_NAME cerealSerialize
#endif

// DQ (9/25/2007): Need to move this to here so that all of ROSE will see it.
#define Rose_STL_Container std::vector

#endif
