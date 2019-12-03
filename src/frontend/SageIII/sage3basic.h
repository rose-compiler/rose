/*
 * This header (or its precompiled version) includes the forward declarations of all the Sage IR node classes ("Sg*")
 * from the ROSETTA-generated files (i.e., gives just the class names).
 *
 * Every source file (.C) that becomes part of librose should include "sage3basic.h" as the first included file before any C++
 * token is processed by the compiler, thus allowing a precompiled version of this header to be used.  This applies to pretty
 * much every .C file under the $ROSE/src directory.  Such source files should not include "rose.h".
 *
 * No librose header file (those under $ROSE/src) should include sage3basic.h, rose_config.h, or rose.h.  If a header file
 * needs something that's declared in sage3basic.h then include sage3basic.h in the .C file first (GCC cannot use the
 * precompiled version if it is included from inside another header).  If a header file needs a configuration macro (like
 * HAVE_WHATEVER) from rose_config.h, then it should include "rosePublicConfig.h" instead (and use ROSE_HAVE_WHATEVER).
 */

#ifndef SAGE3_CLASSES_BASIC__H
#define SAGE3_CLASSES_BASIC__H

// DQ (11/12/2011): This is support to reduce the size of ROSE so that I can manage development on my laptop.
// This option defines a subset of ROSE as required to support wotk on the new EDG front-end.
// This is defined here becasuse it is not enough to define it in the rose_config.h
// because that can't be read early enough to effect what header files are included.
// #define ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT

// Much of ROSE's binary support uses the intX_t and uintX_t types (where X is a bit width), so we need to have the stdc printf
// format macros defined for portability.  We do that here because it needs to be done before <inttypes.h> is included for the
// first time, and we know that most source files for the ROSE library include this file (sage3basic.h) at or near the
// beginning.  We don't want to define __STDC_FORMAT_MACROS in user code that includes "rose.h" (the user may define it), and
// we need to define it in such a way that we won't get warning's if its already defined.  [RMP 2012-01-29]
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
#if defined(__clang__)
# define ROSE_DEPRECATED(WHY) __attribute__((deprecated(WHY)))
#elif defined(__GNUC__)
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

// tps (01/22/2010) :refactored
#include "rosedefs.h"
// Support for preprocessors declarations and comments
#include "rose_attributes_list.h"

// Include ROSE common utility function library
#include "StringUtility.h"
#include "FileUtility.h"
#include "escape.h"

// Include support for Brian Gunney's command line parser tool (nice work)
#include "sla.h"


// DQ (3/29/2006): I sure would like to remove this since it 
// has a potential to effect other files from other projects
// used with ROSE.
// #define INLINE

// DQ (9/21/2005): Support for memory pools.
// This allows for a total number of IR nodes (for each type of IR node) of
// (MAX_NUMBER_OF_MEMORY_BLOCKS * DEFAULT_CLASS_ALLOCATION_POOL_SIZE)
// It might be better to use an STL vector here since they we don't have
// an upper bound on the number of IR nodes of each type!!!
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1000

// Typical values used to testing the AST File I/O are 1 or 2, but larger values
// are required for better performance.  At some point the value should be evaluated
// as even a value of 1000 is likely a bit small for larger whole applications.
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1000
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 2
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 3
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1000

// DQ (11/3/2016): This size causes the AST File I/O to fail.  It is likely that
// since the INITIAL_SIZE_OF_MEMORY_BLOCKS is set to 10000, the DEFAULT_CLASS_ALLOCATION_POOL_SIZE
// should apparently be significantly less that the INITIAL_SIZE_OF_MEMORY_BLOCKS.
// It is not clear what the rule should be for this.
// When it fails the error is: 
//      static const string& Sg_File_Info::getFilenameFromID(int): Assertion `failure == false' failed.
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 10000 (fails)
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 2000 (passes)
#define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 2000
// #define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 4000 (fails)

// DQ (3/7/2010):Added error checking.
#if DEFAULT_CLASS_ALLOCATION_POOL_SIZE < 1
   #error "DEFAULT_CLASS_ALLOCATION_POOL_SIZE must be greater than zero!"
#endif

// DQ (3/7/2010): This is no longer used (for several years) and we use an STL based implementation.
// #define MAX_NUMBER_OF_MEMORY_BLOCKS        1000


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
#define SKIP_MANGLED_NAME_CACHING 0

#define USE_OLD_BINARY_EXECUTABLE_IR_NODES 0

#define USING_OLD_EXECUTABLE_FORMAT_SUPPORT 0
#if USING_OLD_EXECUTABLE_FORMAT_SUPPORT
// DQ (12/8/2008): Build a forward declaration for the input parameter type for the 
// SgAsmFileHeader class constructor.
namespace Exec { namespace ELF { class ElfFileHeader; }; };
#endif


// DQ (12/28/2009): Moved from Cxx_Grammar.h to simplify splitting large files generated by ROSETTA.
#include "AstAttributeMechanism.h"

// DQ (12/29/2009): This permits the optional use of smaller generated header files for the IR.
// Without this option the generated header file for the ROSE IR is nearly 300K in size, and
// this can be a problem for some compilers (MSVC).  This is part of work to optionally reduce
// the sizes of some of the larger ROSETTA generated files in ROSE. Later this will be controled
// using a configure command line option to ROSE (at configure time).
// #define ROSE_USING_SMALL_GENERATED_HEADER_FILES 1

// This is an Autoconf conditional macro (so it should not be defined).
#ifdef ROSE_USE_SMALLER_GENERATED_FILES
// #warning "ROSE_USE_SMALLER_GENERATED_FILES is defined"
   #define ROSE_USING_SMALL_GENERATED_HEADER_FILES 1
#endif

// DQ (10/4/2014): Added to support USE_ROSE_ATERM_SUPPORT macro.
// Including rose_config.h is a problem, and is caught in any files that
// also include rose.h.  Since ROSE source files in /src should not be 
// including rose.h I have fixed many of these, but at least one fails
// without rose.h (/src/midend/programAnalysis/genericDataflow/cfgUtils/CFGRewrite.C)
// so maybe we should have it be an enforced policy instead of the kind of
// error that is is if rose_config.h is included below.  The better solution 
// for users is to include rosePublicConfig.h below (no in place).
// Note also that some macros from ROSE may need to be added to the generated
// rosePublicConfig.h file (in the script scripts/publicConfiguration.pl).
// #include "rose_config.h"
#include "rosePublicConfig.h"

// DQ (10/4/2014): Not clear if this is the best way to control use of ATerm.
// I think we need a specific macro to be defined for when ATerms are being used.
// Also I want to initially seperate this from Windows support.
#ifndef _MSC_VER
// Rasmussen (04/17/2019): Support for ATerms has been deprecated for to and from
// Sage node support but is likely needed here for using ATerms to construct
// Sage nodes from ATerms (e.g. Jovial)
  #ifdef ROSE_USE_ROSE_ATERM_SUPPORT
 // DQ (9/27/2013): This is required to be defined for the 64bit ATerm support.
    #if (__x86_64__ == 1)
   // 64-bit machines are required to set this before including the ATerm header files.
      #define SIZEOF_LONG 8
      #define SIZEOF_VOID_P 8
    #else
   // 32-bit machines need not have the values defined (but it is required for this program).
      #define SIZEOF_LONG 4
      #define SIZEOF_VOID_P 4
    #endif

    #include "aterm1.h"
    #include "aterm2.h"
  #else
 // Define this away so that we can trivially compile without ATerm support.
    typedef int ATerm;
  #endif
#else
// Define this away so that we can trivially compile without ATerm support.
   typedef int ATerm;
#endif

// DQ (3/7/2013): I think that we need to use "" instead of <> and this may make a difference for SWIG.
// DQ (9/21/2005): This is the simplest way to include this here
// This is the definition of the Sage III IR classes (generated header).
// #include <Cxx_Grammar.h>
#include "Cxx_Grammar.h"

// DQ (10/4/2014): Not clear if this is the best way to control use of ATerm.
// I think we need a specific macro to be defined for when ATerms are being used.
// Also I want to initially seperate this from Windows support.
// Rasmussen (04/17/2019): Support for ATerms has been deprecated.
#ifndef _MSC_VER
//  #include "atermSupport.h"
#endif

// Disable CC++ extensions (we want to support only the C++ Standard)
#undef CCPP_EXTENSIONS_ALLOWED

// This should be a simple include (without dependence upon ROSE_META_PROGRAM
#include "utility_functions.h"

// DQ (3/6/2013): Adding support to restrict visability to SWIG.
// #ifndef ROSE_USE_SWIG_SUPPORT

// Markus Schordan: temporary fixes for Ast flaws (modified by DQ)
#include <typeinfo>

// DQ (12/9/2004): The name of this file has been changed to be the new location
// of many future Sage III AST manipulation functions in the future.  A namespace
// (SageInterface) is defined in sageInterface.h.
#include "sageInterface.h"


// DQ (3/29/2006): Moved Rich's support for better name mangling to a 
// separate file (out of the code generation via ROSETTA).
#include "manglingSupport.h"

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

// DQ (7/6/2005): Added to support performance analysis of ROSE.
// This is located in ROSE/src/midend/astDiagnostics
#include "AstPerformance.h"


// DQ (4/10/2010): Moved Dwarf and Intel Pin headers to here from rose.h.
// DQ (11/7/2008): Added Dwarf support to ROSE AST (applies only to binary executables generated with dwarf debugging information).
#ifndef _MSC_VER
// tps (11/23/2009) : Commented out right now to make progress in Windows
   #ifdef ROSE_HAVE_LIBDWARF
      #include "dwarfSupport.h"
   #endif

// DQ (3/8/2009): Added support for Intel Pin (Dynamic binary Instrumentation)
// tps (11/23/2009) : Commented out right now to make progress in Windows
   #ifdef USE_ROSE_INTEL_PIN_SUPPORT
// Note that pin.H (in it's header files) will define "TRUE" and "FALSE" as macros.
      #include "IntelPinSupport.h"
   #endif
#endif

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

// DQ (10/26/2016): Adding mechanism to suppress use of delete in SgType IR nodes, so 
// that the memory pool will not be changing while we are traversing it.  I think this
// is perhaps a fundamental problem in the memory pool traversal if operations are done
// that modify the memory pools during the traversal.
#define ALLOW_DELETE_OF_EXPLORATORY_NODE 1

// endif for ifndef ROSE_USE_SWIG_SUPPORT
// #endif

#include <initialize.h>                                 // defines Rose::initialize

// Liao, 2018/6/25, define the actual version value for OpenMP 4.5
#define OMPVERSION 201511

#endif


