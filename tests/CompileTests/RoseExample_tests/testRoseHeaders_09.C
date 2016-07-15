
// DQ (9/25/2007): Need to move this to here so that all of ROSE will see it.
#define Rose_STL_Container std::vector

// #include <sage3basic.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "rose_override.h"                              // defines ROSE_OVERRIDE as "override" if C++11 is present


#include <semaphore.h>
#include "fileoffsetbits.h"
#include "rosedll.h"
//tps (05/04/2010): Added compatibility
#ifdef _MSC_VER
# if _MSC_VER < 1900
  #define snprintf _snprintf
# endif
#endif

// George Vulov (Aug. 23, 2010): This macro is not available in OS X by default
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
    ({ \
        long int _result; \
        do _result = (long int) (expression); \
        while (_result == -1L && errno == EINTR); \
        _result; \
    })
#endif

// DQ (4/21/2009): Note that this header file will include the STL string header file 
// which will include sys/stat.h, so the _FILE_OFFSET_BITS macro must be already set 
// to avoid an ODR violation when using ROSE on 32-bit systems.
// DQ (11/10/2007): Added support for ROSE specific paths to be available. These are 
// useful for tools built using ROSE, they are not presently being used within ROSE.
// RPM (8/22/2008): Commented out because it contains info that prevents us from making
// optimal use of ccache. "rose_paths.h" contains info that changes every time we
// configure. Can it be included at a finer granularity than this?
// DQ (8/25/2008): Turn this back on since it breaks ROSE for everyone else.  We 
// are searching for a better solution.
#include "rose_paths.h"


// DQ (5/30/2004): Added to permit warnings to be placed in the source code so that
// issues can be addressed later but called out during development (and eliminated
// from the final released version of the source code).
#define PRINT_DEVELOPER_WARNINGS 0
// #define PRINT_DEVELOPER_WARNINGS 1

// Part of debuging use of SUN 6.1 compiler
#if defined(__WIN32__) || defined (__WIN16__)
#error "WIN macros should not be defined (test in sage3.h)"
#endif

// Part of debuging use of SUN 6.1 compiler
#if defined(__MSDOS__) && defined(_Windows)
#error "MSDOS macros should not be defined"
#endif

// DQ (4/21/2009): Added test to debug use of _FILE_OFFSET_BITS macro in controling size of "struct stat"
// #if defined(_FILE_OFFSET_BITS)
// #warning "The _FILE_OFFSET_BITS macro should not be set yet!"
// #endif

// DQ (4/21/2009): This macro is set too late!
// Force 64-bit file offsets in struct stat
// #define _FILE_OFFSET_BITS 64
#include <sys/stat.h>

//#include <cstdlib> // For abort()
#include <algorithm>
#include <fstream>


// DQ (8/25/2014): Added logic to isTemplateDeclaration(a_routine_ptr) to force isTemplateDeclaration 
// in ROSE/EDG connection to be false where the topScopeStack() is a template class instantaition scope.
#define ENFORCE_NO_FUNCTION_TEMPLATE_DECLARATIONS_IN_TEMPLATE_CLASS_INSTANTIATIONS 0

// DQ (9/24/2004): Try again to remove use of set parent side effect in EDG/Sage III connection! This works!!!
#define REMOVE_SET_PARENT_FUNCTION

// DQ (6/12/2007): Force checking for valid pointers to IR nodes being overwritten.
#define DEBUG_SAGE_ACCESS_FUNCTIONS 0
// DQ (6/12/2007): Force assertion test to fail such cases caught when DEBUG_SAGE_ACCESS_FUNCTIONS == 1, else just report error.
#define DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION 0

// DQ (10/12/2004): Remove the resetTemplateName() from use within the EDG/Sage connection
// because it will (where required) force calls to generate the qualified name which
// requires the parent pointer to have already been set.  Since we defer the 
// setting of the parent pointers until post processing of the Sage III AST.
// It is now called within the AstFixup.C.
#define USE_RESET_TEMPLATE_NAME false

// The ROSE_DEPRECATED marker unconditionally marks a function or variable as deprecated and will produce a warning if
// whenever a use of that function or variable occurs.  Do not disable this macro; see ROSE_DEPRECATED_FUNCTION instead.
// If you mark a function or variable as deprecated, then BE SURE TO FIX PLACES WHERE IT IS USED IN ROSE!!!  The WHY argument
// should be a string literal (unevaluated) describing why it's deprecated or what to use instead.
#if defined(__GNUC__)
#   define ROSE_DEPRECATED(WHY) __attribute__((deprecated))
#elif defined(_MSC_VER)
#   define ROSE_DEPRECATED(WHY) /*deprecated*/
#else
#   define ROSE_DEPRECATED(WHY) /*deprecated*/
#endif

// The ROSE_DEPRECATED_FUNCTION and ROSE_DEPRECATED_VARIABLE conditionally mark a function or variable as deprecated.  At this
// time, ROSE itself contains hundreds of uses of deprecated functions and variables because the people that marked those
// functions and variables as deprecated did not also fix ROSE to avoid calling them.  The warnings can be suppressed by
// defining ROSE_SUPPRESS_DEPRECATION_WARNINGS on the compiler command-line during configuration.
//
// For the time being we explicitly define ROSE_SUPPRESS_DEPRECATION_WARNINGS because of the problem mentioned above. ROSE
// authors should only add ROSE_DEPRECATED markers and not use new ROSE_DEPRECATED_FUNCTION or ROSE_DEPRECATED_VARAIBLE since
// the latter two do nothing. AND BE SURE TO FIX PLACES IN ROSE WHERE THE DEPRECATED THING IS USED!!!!!
#undef ROSE_SUPPRESS_DEPRECATION_WARNINGS
#define ROSE_SUPPRESS_DEPRECATION_WARNINGS
#if !defined(ROSE_SUPPRESS_DEPRECATION_WARNINGS)
#   if !defined(ROSE_DEPRECATED_FUNCTION)
#       define ROSE_DEPRECATED_FUNCTION ROSE_DEPRECATED
#   endif
#   if !defined(ROSE_DEPRECATED_VARIABLE)
#       define ROSE_DEPRECATED_VARIABLE ROSE_DEPRECATED
#   endif
#else
#   if !defined(ROSE_DEPRECATED_FUNCTION)
#       define ROSE_DEPRECATED_FUNCTION /*deprecated*/
#   endif
#   if !defined(ROSE_DEPRECATED_VARIABLE)
#       define ROSE_DEPRECATED_VARIABLE /*deprecated*/
#   endif
#endif

// Used to mark deprecated functions and advertise that fact to developers and especially to end users.  This is sometimes
// turned off during development (because it's annoying) by defining ROSE_SUPPRESS_DEPRECATION_WARNINGS when configuring.
#if !defined(ROSE_SUPPRESS_DEPRECATION_WARNINGS)
#   if defined(__GNUC__)
        // Put ROSE_DEPRECATED_FUNCTION after the declaration, i.e.: int Foo::bar() const ROSE_DEPRECATED_FUNCTION;
#       define ROSE_DEPRECATED_FUNCTION __attribute__((deprecated))
#       define ROSE_DEPRECATED_VARIABLE __attribute__((deprecated))
#   elif defined(_MSC_VER)
        // Microsoft Visual C++ needs "__declspec(deprecated)" before the declaration. We don't really want to put
        // ROSE_DEPRECATED_FUNCTION both before and after functions, so we just don't worry about advertising deprecation when
        // using Microsoft compilers.  Use MinGW instead if you want a real C++ compiler on Windows.
#       define ROSE_DEPRECATED_FUNCTION /*deprecated*/
#       define ROSE_DEPRECATED_VARIABLE /*deprecated*/
#   else
        // No portable way to mark C++ functions as deprecated.
#       define ROSE_DEPRECATED_FUNCTION /*deprecated*/
#       define ROSE_DEPRECATED_VARIABLE /*deprecated*/
#   endif
#else
#   define ROSE_DEPRECATED_FUNCTION /*deprecated*/
#   define ROSE_DEPRECATED_VARIABLE /*deprecated*/
#endif

// DQ (12/22/2007): Name of implicit Fortran "main" when building the program function.
#define ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME "rose_fortran_main"

// DQ (10/6/2004): We have tracked down and noted all locations where a Sage III member function modifies its input parameters.
// The locations where this happens are marked with a print statement which this macro permits us to turn off when we want to
// either make an intermediate release of just not see the warning messages.  Many side-effects have been removed and some are 
// pending more details discussions internally.  I would like to goal to be a simple rule that input parameters to constructors
// are not modified by the constructor or any function called within the constructor body.  A stronger rule would be that the
// input parameters to any access function which gets and data member of sets a data member would not modified its input 
// parameters. Same idea but applied to all access functions, not just constructors.  It is not clear if we need go further.
// Clearly it might be important to have some function that modify their input parameters but a simple design would disallow it!
#define PRINT_SIDE_EFFECT_WARNINGS false


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


// AJ (10/21/2004) : the current version of g++ 3.2.3 has the "hash_map" deprecated - this
// deprecated hash_map is // in the global namespace and generates a warning every time
// the file gets included. The "ext/hash_map" is the newer version of the hash_map but
// it is no longer in the global namespace or std ( since the hash_map is not part of
// the standard STL) but in the compiler specific namespace (__gnu_cxx). Because of this,
// we have opted for using the newer version and explicitly using the namespace __gnu_cxx
// See below the using namespace section. If there is a need to change this to a more 
// standard include "hash_map", please make sure you have selected the right namespace for 
// using the hash_map and modify the section below
// for that.
// Liao, 7/10/2009 
//#if __GNUC__ > 4 ||
//  (__GNUC__ == 4 && (__GNUC_MINOR__ > 3 ||
//                   (__GNUC_MINOR__ == 3 &&
//                    __GNUC_PATCHLEVEL__ >= 0)))
//#include <unordered_map>
//#else   



//#endif
#if 1
#ifdef _MSC_VER
// DQ (11/4/2009): MS Visual Studio version of hash_multimap
//#include <cliext/hash_map>
#else
// DQ (11/4/2009): Use the GNU depricated stuff (what works in ROSE at the moment)
// tps (01/25/2010) : deprecated - does not work in setup.h
// CH (04/28/2010) : We don't need it anymore
//#include <ext/hash_map>
#endif
#endif

#if 1

// tps (01/22/2010) :refactored
#include "rosedefs.h"

#endif
