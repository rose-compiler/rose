/*
 * this includes the forward declarations of all the sage node classes
 * from the generated files (i.e. gives just the class names.)
 *
 */

#ifndef SAGE3_CLASSES_H
#define SAGE3_CLASSES_H

// DQ (3/30/2006): Currently we have TRUE and FALSE through out the ROSE source code.
// it might be better to change this to "true" and "false" instead, but until then
// we have to define these (early in the compilation).
// JJW (8/26/2008): Removing these
// #ifndef TRUE
//    #define TRUE true
// #endif
// #ifndef FALSE
//    #define FALSE false
// #endif

// DQ (3/12/2006): This is included here as specified in the Autoconf manual (using <> instead of "")
// We have also abandoned the ifdef HAVE_CONFIG_H cpp conditional use of rose_config.h as well.
// This is placed here in sage3.h instead of in rose.h because it needs to always be seen even 
// by internal ROSE files that only include sage3.h.
#include <rose_config.h>

// DQ (4/21/2009): Error checking to avoid difficult to debug ORD violations on 32-bit systems.
#if defined(_SYS_STAT_H)
#warning "sys/stat.h should not have been included before the _FILE_OFFSET_BITS macro is set! (use rose.h first...)"
#endif

// DQ (4/21/2009): This must be included before rose_paths.h since that 
// header includes the STL string header which will include sys/stat.h first.
// Force 64-bit file offsets in struct stat
#define _FILE_OFFSET_BITS 64

// DQ (4/21/2009): This must be set before sys/stat.h is included by any other header file.
// Use of _FILE_OFFSET_BITS macro is required on 32-bit systems to controling size of "struct stat"
#if !(defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64))
#error "The _FILE_OFFSET_BITS macro should be set before any sys/stat.h is included by any other header file!"
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

// This is a problem with the SUN CC version 6.0 compiler
#include <sstream>  // This (sstream) should eventually replace calls to strstream.h (Kyle)

// DQ (12/7/2003): g++ 3.x prefers to see <fstream> and use of <fstream> 
//                 or <fstream.h> is setup in config.h so use it here.
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // For abort()

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

#if 0
// DQ (5/22/2005):
// This is often turned of during development because it is annoying (until we can actually 
// remove numerous functions where they are declared).  Also not clear what versions of GNU 
// support it.

// Use the new GNU attribute mechanism to add more information to the source code
// By specifying funcations and variables as depreicated we can start the remove
// some functions and data members from Sage III.
#define ROSE_DEPRECATED_FUNCTION __attribute__ ((deprecated))
#define ROSE_DEPRECATED_VARIABLE __attribute__ ((deprecated))
#else
// DQ (9/8/2004): Allow these to get turned of to simplify debugging while we have not yet removed their use.
#define ROSE_DEPRECATED_FUNCTION 
#define ROSE_DEPRECATED_VARIABLE 
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
#define STRICT_ERROR_CHECKING false


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
//#if __GNUC__ > 4 || \
//  (__GNUC__ == 4 && (__GNUC_MINOR__ > 3 || \
//		     (__GNUC_MINOR__ == 3 && \
//		      __GNUC_PATCHLEVEL__ >= 0)))
//#include <unordered_map>
//#else   
#include <ext/hash_map>
//#endif
#if 0
// See above the <ext/hash_map> section.
// using namespace __gnu_cxx;
namespace rose_hash 
   {
  // The condition here is not needed for GCC 4.2, but is needed (and does not completely work) for earlier versions
// #ifdef _GLIBCXX_DEBUG
// using namespace __gnu_debug;
// using __gnu_cxx::hash;
// #else

     using namespace __gnu_cxx;

  // DQ (4/23/2009): These should be put into this namespace so that we don't contaminate the global scope.
  // DQ (8/19/2008): This is already defined in src/frontend/SageIII/astMerge/buildMangledNameMap.h
     struct eqstr_string
        {
          bool operator()(const std::string & s1, const std::string & s2) const;    
        };

/*! \brief Hash name support for symbol tables within ROSE.
 */
     struct hash_string
        {
          rose_hash::hash<const char*> hasher;

          public:
               size_t operator()(const std::string & name) const;
        };

// #endif
   }
#endif

// Support for preprocessors declarations and comments
#include "rose_attributes_list.h"

// DQ (9/6/2006): Modern C++ compiler always make these available
// so we don't need to allow for where they are unavailable.
#include <list>
#include <stack>
#include <vector>
#include <string>
#include <cstdlib>

// Include ROSE common utility function library
#include "string_functions.h"
#include "escape.h"
#include "processSupport.h"

// Include support for Brian Gunney's command line parser tool (nice work)
#include "sla.h"

// These are supported this way so that they can be redefined as required
#ifndef ROSE_ASSERT
#ifndef NDEBUG
#define ROSE_ASSERT assert
#else // We use assert(false) equivalents so often for "should not get here", but we don't want nontrivial side effects in asserts to be run when assert is disabled
#define ROSE_ASSERT(x) do {if (__builtin_constant_p(x)) {if (x) {} else (std::abort)();}} while (0)
#endif
#endif
#ifndef ROSE_ABORT
#define ROSE_ABORT  (std::abort)
#endif

// DQ (3/29/2006): I sure would like to remove this since it 
// has a potential to effect other files from other projects
// used with ROSE.
// #define INLINE

// DQ (9/21/2005): Support for memory pools.
// This allows for a total number of IR nodes (for each type of IR node) of
// (MAX_NUMBER_OF_MEMORY_BLOCKS * DEFAULT_CLASS_ALLOCATION_POOL_SIZE)
// It might be better to use an STL vector here since they we don't have
// an upper bound on the number of IR nodes of each type!!!
#define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 1000
#define MAX_NUMBER_OF_MEMORY_BLOCKS        1000


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

#if 0
// DQ (12/8/2008): Trying to move off of the old form of the binary executable file format.

// DQ (6/15/2008): This had to be moved here so that source files only including "sage3.h"
// would see the required declarations (now that "Cxx_Grammar.h" references declarations in
// the "Exec" namespace.
// Files in src/frontend/ExecFormats
// This is included in the "ExecELF.h" and "ExecPE.h" files already.
// #include "ExecGeneric.h"
   #include "ExecELF.h"
   #include "ExecPE.h"

// DQ (8/7/2008): Added newer support for NE (older windows) and LE (OS/2) formats.
  #include "ExecNE.h"
  #include "ExecLE.h"
#endif

// DQ (9/21/2005): This is the simplest way to include this here
// This is the definition of the Sage III IR classes (generated header).
#include <Cxx_Grammar.h>

// Disable CC++ extensions (we want to support only the C++ Standard)
#undef CCPP_EXTENSIONS_ALLOWED

// This should be a simple include (without dependence upon ROSE_META_PROGRAM
#include "utility_functions.h"

// Markus Schordan: temporary fixes for Ast flaws (modified by DQ)
#include <typeinfo>
#include "AstProcessing.h"

// Markus Kowarschik: Support for preprocessors declarations and comments
#include "attachPreprocessingInfo.h"

// Lingxiao's work to add comments from all header files to the AST.
#include "attach_all_info.h"

// DQ (8/20/2005): Changed name to make sure that we don't use the old 
// header file (which has been removed).
// #include "AstFixes.h"
#include "astPostProcessing.h"

// DQ (12/9/2004): The name of this file has been changed to be the new location
// of many future Sage III AST manipulation functions in the future.  A namespace
// (SageInterface) is defined in sageInterface.h.
// #include "sageSupport.h"
#include "sageInterface.h"

//Liao, 2/8/2008. SAGE III node building interface
#include "sageBuilder.h"
#include "sageBuilderAsm.h"

//Liao, 10/9/2008, support for abstract handles for language constructs
#include "abstract_handle.h"
#include "roseAdapter.h"

// Liao, 11/14/2008, support for annotation-based array optimization
// The headers bring some garbage which cannot be compiled
//#include "ArrayAnnot.h"
//#include "ArrayInterface.h"
//include "ArrayRewrite.h"

// DQ (5/27/2007): Removed all entries in this file (only had AST Merge API and 
// these were moved to merge.h).  One less header file make everything a little simpler!
// DQ (7/7/2005): This is now a file where temporary functions may be
// implemented before movign them into Sage III more formally.
// #include "sageSupport.h"

// DQ (3/29/2006): Moved Rich's support for better name mangling to a 
// separate file (out of the code generation via ROSETTA).
#include "manglingSupport.h"

// Markus Kowarschik: we use the new mechanism of handling preprocessing info;
// i.e., we output the preprocessing info attached to the AST nodes.
// See the detailed explanation of the mechanisms in the beginning of file
// attachPreprocessingInfo.C
#define USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO 0

// DQ (7/6/2005): Added to support performance analysis of ROSE.
// This is located in ROSE/src/midend/astDiagnostics
#include "AstPerformance.h"

// DQ (5/28/2007): Added new AST Merge API
#include "astMergeAPI.h"

// DQ (6/3/2007): added internal support for AST visualization
// #include "astVisualization/wholeAST_API.h"
#include "wholeAST_API.h"

// DQ (9/1/2006): It is currently an error to normalize the source file names stored 
// in the SgProject IR node to be absolute paths if they didn't originally appear 
// that way on the commandline.  We have partial support for this but it is a bug
// at the moment to use this.  However, we do now (work by Andreas) normalize the
// source file name when input to EDG so that all Sg_File_Info objects store an
// absolute path (unless modified using a #line directive, see test2004_60.C as an 
// example).  The current work is an incremental solution.
#define USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST 0

// JJW 10-23-2007
// Add possibility to include Valgrind header for memcheck
#if ROSE_USE_VALGRIND
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#endif

#endif















