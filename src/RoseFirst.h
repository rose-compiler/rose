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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// These things were moved here from sage3basic.h before other ROSE header files were included.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DQ (5/30/2004): Added to permit warnings to be placed in the source code so that
// issues can be addressed later but called out during development (and eliminated
// from the final released version of the source code).
#define PRINT_DEVELOPER_WARNINGS 0

// DQ (9/24/2004): Try again to remove use of set parent side effect in EDG/Sage III connection! This works!!!
#define REMOVE_SET_PARENT_FUNCTION

// DQ (6/12/2007): Force checking for valid pointers to IR nodes being overwritten.
#define DEBUG_SAGE_ACCESS_FUNCTIONS 0

// DQ (6/12/2007): Force assertion test to fail such cases caught when DEBUG_SAGE_ACCESS_FUNCTIONS == 1, else just report error.
#define DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION 0

// DQ (12/22/2007): Name of implicit Fortran "main" when building the program function.
#define ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME "rose_fortran_main"

// DQ (10/21/2004): We require a relaxed level of internal error checking for manually generated AST fragments!
// This is required for get through the current regression tests associated with the loop processing code which
// does not follwo the new rules for what qualifies as a valid AST.  Time is needed for the AST Interface code
// to be adapted to the new rules.  Not clear how this will effect the unparser!!!
// In the future we want to make this value "TRUE" this is a work around until then.
#ifdef _MSC_VER
#define STRICT_ERROR_CHECKING 0
#else
#define STRICT_ERROR_CHECKING false
#endif

// DQ (11/7/2007): Reimplementation of "fixup" support for the AST copy mechanism.
// This version separates the fixup into three phases:
// Use three files to organize the separate functions (parent/scope setup, symbol table setup, and symbol references).
// Order of operations:
//    1) Setup scopes on all declaration (e.g. SgInitializedName objects).
//    2) Setup the symbol table.
//        template instantiations must be added to to the symbol tables as defined by their scope
//        because they may be located outside of their scope (indicated by their template declaration).
//        We might need a test and set policy.
//        Use the help map to support error checking in the symbol table construction.  Check that
//        scopes are not in the original AST (not keys in the help map).
//    3) Setup the references (SgVarRefExp objects pointers to SgVariableSymbol objects)
#define ALT_FIXUP_COPY 1

// DQ (9/21/2005): Support for memory pools.
// This allows for a total number of IR nodes (for each type of IR node) of
// (MAX_NUMBER_OF_MEMORY_BLOCKS * DEFAULT_CLASS_ALLOCATION_POOL_SIZE)
// It might be better to use an STL vector here since they we don't have
// an upper bound on the number of IR nodes of each type!!!
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1000
//
// Typical values used to testing the AST File I/O are 1 or 2, but larger values
// are required for better performance.  At some point the value should be evaluated
// as even a value of 1000 is likely a bit small for larger whole applications.
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1000
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 2
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 3
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1000
//
// DQ (11/3/2016): This size causes the AST File I/O to fail.  It is likely that
// since the INITIAL_SIZE_OF_MEMORY_BLOCKS is set to 10000, the DEFAULT_CLASS_ALLOCATION_POOL_SIZE
// should apparently be significantly less that the INITIAL_SIZE_OF_MEMORY_BLOCKS.
// It is not clear what the rule should be for this.
// When it fails the error is:
//      static const string& Sg_File_Info::getFilenameFromID(int): Assertion `failure == false' failed.
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 10000 (fails)
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 2000 (passes)
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 4000 (fails)
//
// The value must be positive.
#define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 2000

// DQ (9/231/2005): Map these to the C library memory alloction/deallocation functions.
// These could use alternative allocators which allocate on page boundaries in the future.
// This is part of the support for memory pools for the Sage III IR nodes.
// #define ROSE_MALLOC malloc
// #define ROSE_FREE free
// DQ (9/9/2008): Don't let this be confused by a member function called "free" in Robb's work.
#define ROSE_MALLOC ::malloc
#define ROSE_FREE ::free

// DQ (10/6/2006): Allow us to skip the support for caching so that we can measure the effects.
#define SKIP_BLOCK_NUMBER_CACHING 0

// Markus Kowarschik: we use the new mechanism of handling preprocessing info;
// i.e., we output the preprocessing info attached to the AST nodes.
// See the detailed explanation of the mechanisms in the beginning of file
// attachPreprocessingInfo.C
#define USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO 0

// DQ (9/1/2006): It is currently an error to normalize the source file names stored
// in the SgProject IR node to be absolute paths if they didn't originally appear
// that way on the commandline.  We have partial support for this but it is a bug
// at the moment to use this.  However, we do now (work by Andreas) normalize the
// source file name when input to EDG so that all Sg_File_Info objects store an
// absolute path (unless modified using a #line directive, see test2004_60.C as an
// example).  The current work is an incremental solution.
#define USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST 0

// DQ (10/26/2016): Adding mechanism to suppress use of delete in SgType IR nodes, so
// that the memory pool will not be changing while we are traversing it.  I think this
// is perhaps a fundamental problem in the memory pool traversal if operations are done
// that modify the memory pools during the traversal.
#define ALLOW_DELETE_OF_EXPLORATORY_NODE 1




#endif
