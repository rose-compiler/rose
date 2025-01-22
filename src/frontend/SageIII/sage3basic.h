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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This part of this header contains things that *MUST* be done very early due to designs of non-ROSE header files.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <RoseFirst.h>

// The boost::filesystem::path class has no serialization function, and boost::serialization doesn't provide a non-intrusive
// implementation. Therefore ROSE needs to define one. This code must occur before including any headers that serialize
// boost::filesystem::path, and specifically before defining AST node types.
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/filesystem.hpp>
#include <boost/serialization/nvp.hpp>
namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, boost::filesystem::path &path, const unsigned /*version*/) {
            if (Archive::is_saving::value) {
                std::string nativePath = path.string();
                ar & BOOST_SERIALIZATION_NVP(nativePath);
            } else {
                std::string nativePath;
                ar & BOOST_SERIALIZATION_NVP(nativePath);
                path = nativePath;
            }
        }
    }
}
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This part of the file contains things that must be done early in nearly every ROSE implementation file for nearly every ROSE
// configuration. This section should be very small and should not include any headers that are even marginally expensive to
// parse.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The rest of this file contains optional things that are not needed by every ROSE implementation file (*.C) or by every ROSE
// configuration. Do not add more to this -- we're trying to get rid of this section. Instead, move things to other header
// files (if necessary) and include them into only those source files that depend on them.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Rose/Constants.h"                             // defines things like Rose::UNLIMITED, Rose::INVALID_INDEX, etc.

// DQ (11/12/2011): This is support to reduce the size of ROSE so that I can manage development on my laptop.
// This option defines a subset of ROSE as required to support wotk on the new EDG front-end.
// This is defined here becasuse it is not enough to define it in the rose_config.h
// because that can't be read early enough to effect what header files are included.
// #define ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT

#include <inttypes.h>

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
// [Robb Matzke 2024-03-14]: This is only used in once place, and it conditionally compiles a comment.
#define ENFORCE_NO_FUNCTION_TEMPLATE_DECLARATIONS_IN_TEMPLATE_CLASS_INSTANTIATIONS 0

// DQ (10/12/2004): Remove the resetTemplateName() from use within the EDG/Sage connection
// because it will (where required) force calls to generate the qualified name which
// requires the parent pointer to have already been set.  Since we defer the 
// setting of the parent pointers until post processing of the Sage III AST.
// It is now called within the AstFixup.C.
// [Robb Matzke 2024-03-14]: apparently unused
#define USE_RESET_TEMPLATE_NAME false

#include <ROSE_DEPRECATED.h>

// DQ (10/6/2004): We have tracked down and noted all locations where a Sage III member function modifies its input parameters.
// The locations where this happens are marked with a print statement which this macro permits us to turn off when we want to
// either make an intermediate release of just not see the warning messages.  Many side-effects have been removed and some are 
// pending more details discussions internally.  I would like to goal to be a simple rule that input parameters to constructors
// are not modified by the constructor or any function called within the constructor body.  A stronger rule would be that the
// input parameters to any access function which gets and data member of sets a data member would not modified its input 
// parameters. Same idea but applied to all access functions, not just constructors.  It is not clear if we need go further.
// Clearly it might be important to have some function that modify their input parameters but a simple design would disallow it!
//
// [Robb Matzke 2024-03-14]: Used only in one place to conditionally compile a printf statement.
#define PRINT_SIDE_EFFECT_WARNINGS false

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
#include <Rose/StringUtility.h>
#include "FileUtility.h"
#include "escape.h"

// Include support for Brian Gunney's command line parser tool (nice work)
#include "sla.h"

// [Robb Matzke 2024-03-14]: apparently unused
#define SKIP_MANGLED_NAME_CACHING 0

// [Robb Matzke 2024-03-14]: apparently unused
#define USE_OLD_BINARY_EXECUTABLE_IR_NODES 0

#define USING_OLD_EXECUTABLE_FORMAT_SUPPORT 0
#if USING_OLD_EXECUTABLE_FORMAT_SUPPORT
// DQ (12/8/2008): Build a forward declaration for the input parameter type for the 
// SgAsmFileHeader class constructor.
namespace Exec { namespace ELF { class ElfFileHeader; }; };
#endif

// DQ (12/28/2009): Moved from Cxx_Grammar.h to simplify splitting large files generated by ROSETTA.
#include "AstAttributeMechanism.h"

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

// DQ (7/6/2005): Added to support performance analysis of ROSE.
// This is located in ROSE/src/midend/astDiagnostics
#include "AstPerformance.h"


// DQ (4/10/2010): Moved Dwarf and Intel Pin headers to here from rose.h.
// DQ (11/7/2008): Added Dwarf support to ROSE AST (applies only to binary executables generated with dwarf debugging information).
#ifndef _MSC_VER
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

#include <Rose/Initialize.h>

// Liao, 2018/6/25, define the actual version value for OpenMP 4.5
#define OMPVERSION 201511

#endif


