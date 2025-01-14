#include "sage3basic.h"

#include "checkIsModifiedFlag.h"
#include <Rose/CommandLine.h>

#include "AstJSONGeneration.h"
#include "AstDOTGeneration.h"

#include "wholeAST_API.h"

#ifdef _MSC_VER
#include <direct.h>     // getcwd
#else
#include "plugin.h"  // dlopen() is not available on Windows
#endif

#include <thread>
#include <time.h>

// Headers required only to obtain version numbers
#include <boost/version.hpp>
#ifdef ROSE_HAVE_LIBREADLINE
#   include <readline/readline.h>
#endif
#ifdef ROSE_HAVE_LIBMAGIC
#   include <magic.h>
#endif
#ifdef ROSE_HAVE_LIBYICES
#   include <yices_c.h>
#endif
#ifdef ROSE_HAVE_Z3_VERSION_H
#   include <z3_version.h>
#endif
#ifdef ROSE_HAVE_CAPSTONE
#   include <capstone/capstone.h>
#endif
#ifdef ROSE_HAVE_DLIB
#   include <dlib/revision.h>
#endif
#ifdef ROSE_HAVE_LIBGCRYPT
#   include <gcrypt.h>
#endif
#ifdef ROSE_HAVE_LIBPQXX
#   include <pqxx/version.hxx>
#endif
#ifdef ROSE_HAVE_SQLITE3
#   include <sqlite3.h>
#endif

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// DQ (9/8/2017): Debugging ROSE_ASSERT. Call sighandler_t signal(int signum, sighandler_t handler);
#include<signal.h>

#include "Rose/AST/IO.h"
#include "Rose/AST/cmdline.h"

#include "AST_FILE_IO.h"
// Note that this is required to define the Sg_File_Info_XXX symbols (need for file I/O)
#include "Cxx_GrammarMemoryPoolSupport.h"

using namespace std;
using namespace Rose;
using namespace Rose::Diagnostics;

// global variable for turning on and off internal debugging.
// Consider using Rose::Diagnostics instead [Robb Matzke 2021-08-18]
int ROSE_DEBUG = 0;

Rose::Options Rose::global_options;
Rose::Options::Options()
   {
  // DQ (3/6/2017): Default option value to minimize the chattyness of ROSE based tools.
     frontend_notes    = false;
     frontend_warnings = false;
     backend_notes     = false;
     backend_warnings  = false;
   }

// DQ (3/6/2017): Adding ROSE options data structure to support frontend and backend options (see header file for details).
Rose::Options::Options(const Options & X)
   {
  // DQ (3/6/2017): Default option value to minimize the chattyness of ROSE based tools.
     frontend_notes    = X.frontend_notes;
     frontend_warnings = X.frontend_warnings;
     backend_notes     = X.backend_notes;
     backend_warnings  = X.backend_warnings;
   }

// DQ (3/6/2017): Adding ROSE options data structure to support frontend and backend options (see header file for details).
Options & Rose::Options::operator= (const Options & X)
   {
  // DQ (3/6/2017): Default option value to minimize the chattyness of ROSE based tools.
     frontend_notes    = X.frontend_notes;
     frontend_warnings = X.frontend_warnings;
     backend_notes     = X.backend_notes;
     backend_warnings  = X.backend_warnings;

     return *this;
   }

bool Rose::Options::get_frontend_notes()
   {
     return frontend_notes;
   }

void Rose::Options::set_frontend_notes(bool flag)
   {
     frontend_notes = flag;
   }

bool Rose::Options::get_frontend_warnings()
   {
     return frontend_warnings;
   }

void Rose::Options::set_frontend_warnings(bool flag)
   {
     frontend_warnings = flag;
   }

bool Rose::Options::get_backend_notes()
   {
     return backend_notes;
   }

void Rose::Options::set_backend_notes(bool flag)
   {
     backend_notes = flag;
   }

bool Rose::Options::get_backend_warnings()
   {
     return backend_warnings;
   }

void Rose::Options::set_backend_warnings(bool flag)
   {
     backend_warnings = flag;
   }


#define OUTPUT_TO_FILE true
#define DEBUG_COPY_EDIT false

// DQ (9/27/2018): We need to build multiple maps, one for each file (to support token based unparsing for multiple files,
// such as what is required when using the unparsing header files feature).
std::map<int,std::map<SgNode*,TokenStreamSequenceToNodeMapping*>* > Rose::tokenSubsequenceMapOfMaps;

// DQ (1/19/2021): This is part of moving to a new map that uses the SgSourceFile pointer instead of the file_id.
std::map<SgSourceFile*,std::map<SgNode*,TokenStreamSequenceToNodeMapping*>* > Rose::tokenSubsequenceMapOfMapsBySourceFile;

// DQ (5/27/2021): This is required for the token-based unparsing, specifically for knowing when to
// unparse the trailing whitespace at the end of the last statement in a scope to the end of the scope.
std::map<SgSourceFile*,std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> > > Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile;

// DQ (11/27/2013): Adding vector of nodes in the AST that defines the token unparsing AST frontier.
std::map<int,std::map<SgStatement*,FrontierNode*>*> Rose::frontierNodesMapOfMaps;

// DQ (11/27/2013): Adding adjacency information for the nodes in the token unparsing AST frontier.
std::map<int,std::map<SgNode*,PreviousAndNextNodeData*>*> Rose::previousAndNextNodeMapOfMaps;

// DQ (11/29/2013): Added to support access to multi-map of redundant mapping of frontier IR nodes to token subsequences.
std::map<int,std::multimap<int,SgStatement*>*> Rose::redundantlyMappedTokensToStatementMapOfMultimaps;
std::map<int,std::set<int>*> Rose::redundantTokenEndingsMapOfSets;

// DQ (11/20/2015): Provide a statement to use as a key in the token sequence map to get representative whitespace.
std::map<int,std::map<SgScopeStatement*,SgStatement*>*> Rose::representativeWhitespaceStatementMapOfMaps;

// DQ (11/30/2015): Provide a statement to use as a key in the macro expansion map to get info about macro expansions.
std::map<int,std::map<SgStatement*,MacroExpansion*>*> Rose::macroExpansionMapOfMaps;

// DQ (10/29/2018): Build a map for the unparser to use to locate SgIncludeFile IR nodes.
std::map<std::string, SgIncludeFile*> Rose::includeFileMapForUnparsing;

// DQ (11/25/2020): These are the boolean variables that are computed in the function compute_language_kind()
// and inlined via the SageInterface::is_<language kind>_language() functions.  See more details comment in
// the header file.
bool Rose::is_Ada_language        = false;
bool Rose::is_C_language          = false;
bool Rose::is_OpenMP_language     = false;
bool Rose::is_UPC_language        = false;
bool Rose::is_UPC_dynamic_threads = false;
bool Rose::is_C99_language        = false;
bool Rose::is_Cxx_language        = false;
bool Rose::is_Java_language       = false;
bool Rose::is_Jvm_language        = false;
bool Rose::is_Jovial_language     = false;
bool Rose::is_Fortran_language    = false;
bool Rose::is_CAF_language        = false;
bool Rose::is_PHP_language        = false;
bool Rose::is_Python_language     = false;
bool Rose::is_Cuda_language       = false;
bool Rose::is_OpenCL_language     = false;
bool Rose::is_binary_executable   = false;


// DQ (3/24/2016): Adding Robb's message logging mechanism to contrl output debug message from the EDG/ROSE connection code.
using namespace Rose::Diagnostics;

// DQ (3/5/2017): Added general IR node specific message stream to support debugging message from the ROSE IR nodes.
Sawyer::Message::Facility Rose::ir_node_mlog;

void Rose::initDiagnostics()
   {
     static bool initialized = false;
     if (!initialized)
        {
          initialized = true;
          ir_node_mlog = Sawyer::Message::Facility("rose_ir_node", Rose::Diagnostics::destination);
          ir_node_mlog.comment("operating on ROSE internal representation nodes");
          Rose::Diagnostics::mfacilities.insertAndAdjust(ir_node_mlog);
        }
   }

// DQ (4/17/2010): This function must be defined if C++ support in ROSE is disabled.
std::string edgVersionString()
   {
#ifdef ROSE_BUILD_CXX_LANGUAGE_SUPPORT
     string edg_version = string("edg-") + StringUtility::numberToString(ROSE_EDG_MAJOR_VERSION_NUMBER) + "." + StringUtility::numberToString(ROSE_EDG_MINOR_VERSION_NUMBER);
  // string edg_version = string("edg-") + StringUtility::numberToString(ROSE_EDG_MAJOR_VERSION_NUMBER) + "." + StringUtility::numberToString(ROSE_EDG_MINOR_VERSION_NUMBER) + " " + StringUtility::numberToString(__EDG_VERSION__);
#else
     string edg_version = "unknown (EDG is disabled)";
#endif
     return edg_version;
   }

// DQ (4/17/2010): Added OFP version number support.
// DQ (2/12/2010): When we have a mechanism to get the version number of OFP, put it here.
std::string ofpVersionString()
   {
  // Need to make sure that ROSE can get a version number independent of Fortran support
  // being installed or include information in the return string when OFP is not installed.
  // return "unknown";
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
     string ofp_version = string("ofp-") + StringUtility::numberToString(ROSE_OFP_MAJOR_VERSION_NUMBER) + "." + StringUtility::numberToString(ROSE_OFP_MINOR_VERSION_NUMBER) + "." + StringUtility::numberToString(ROSE_OFP_PATCH_VERSION_NUMBER);
#else
     string ofp_version = "unknown (OFP is disabled)";
#endif

     return ofp_version;
   }

#ifdef ROSE_HAVE_LIBREADLINE
static std::string
readlineVersionString() {
    #if !defined(RL_VERSION_MAJOR) || !defined(RL_VERSION_MINOR)
        // It appears as though RL_READLINE_VERSION is a 16-bit number whose low 8 bits are the minor version and whose high 8
        // bits are the major version.
        #define RL_VERSION_MAJOR ((RL_READLINE_VERSION & 0xff00) >> 8)
        #define RL_VERSION_MINOR (RL_READLINE_VERSION & 0xff)
    #endif
    return StringUtility::numberToString(RL_VERSION_MAJOR) + "." + StringUtility::numberToString(RL_VERSION_MINOR);
}
#endif

// similar to rose_boost_version_id but intended for human consumption (i.e., "1.50.0" rather than 105000).
static std::string
boostVersionString() {
    return (StringUtility::numberToString(BOOST_VERSION / 100000) + "." +
            StringUtility::numberToString(BOOST_VERSION / 100 % 1000) + "." +
            StringUtility::numberToString(BOOST_VERSION % 100));
}

std::string version_message() {
    std::ostringstream ss;

    // NOTE: In the output below,
    //
    //   * a feature defined within ROSE, such as an analysis capability, is either "enabled" or "disabled".
    //
    //   * a library used by ROSE is either "used" (in which case we show the version number) or "unused".

    // Use the same version string as outut by the --version switch. This string is usually "ROSE 0.x.y.z" but can be changed
    // at runtime. Tools often change this to be a tool version number followed by the ROSE version number.
    ss <<Rose::CommandLine::versionString <<" (configured " <<ROSE_CONFIGURE_DATE <<")\n";

    //-----------------------------------------------------------------------
    // GLobal information regardless of what languages ROSE is configured to analyze
    //-----------------------------------------------------------------------

    // Show some indication of how optimized the ROSE library is. There's no portable way to determine what compiler
    // optimization flags are being used to compile ROSE or even if that set constitutes "full" optimization, whatever
    // that might mean.  But we do know that assertions can prevent certain types of optimization, not to mention that
    // there are enough assertions in ROSE that simply checking them at runtime takes measurably significant time.
#ifdef NDEBUG
    ss <<"  --- logic assertions:           disabled\n";
    // full optimizations *might* be enabled; we just don't know for sure
#else
    ss <<"  --- logic assertions:           enabled\n";
    ss <<"  --- full optimization:          disabled\n";
#endif

#ifdef _REENTRANT
    ss <<"  --- multi-threading:            enabled, "
       <<StringUtility::plural(std::thread::hardware_concurrency(), "hardware threads") <<"\n";
#else
    ss <<"  --- multi-threading:            disabled\n";
#endif

    ss <<"  --- boost library:              " <<boostVersionString() <<" (" <<rose_boost_version_path() <<")\n";
#ifdef ROSE_HAVE_LIBREADLINE
    ss <<"  --- readline library:           " <<readlineVersionString() <<"\n";
#else
    ss <<"  --- readline library:           unused\n";
#endif

    //-----------------------------------------------------------------------
    // Information related to any source language analysis (not binary analysis).
    //-----------------------------------------------------------------------

#ifdef ROSE_ENABLE_SOURCE_ANALYSIS
#ifdef USE_CMAKE
    string build_tree_path                   = "not available";
    string install_path                      = "not available";
#else
    string build_tree_path                   = ROSE_COMPILE_TREE_PATH;
    string install_path                      = ROSE_INSTALLATION_PATH;
#endif
    ss <<"  --- library build path:         " <<build_tree_path <<"\n";
    ss <<"  --- original installation path: " <<install_path <<"\n";
#endif

    //-----------------------------------------------------------------------
    // Information related to C/C++ analysis.
    //-----------------------------------------------------------------------

#if defined(ROSE_BUILD_CPP_LANGUAGE_SUPPORT) || defined(ROSE_BUILD_C_LANGUAGE_SUPPORT)
    ss <<"  --- C/C++ analysis:             enabled\n";
    extern string edgVersionString();
    ss <<"  ---   C/C++ front-end:          " <<edgVersionString() <<"\n";

    // This prints (originally and still now) the version of the C++ compiler instead of the C compiler. This is fine for the
    // normal case where both compilers come from the same compiler collection, but would be wrong, for instance, if the user
    // configured ROSE to use a C compiler from GCC and a C++ compiler from LLVM. [Robb Matzke 2021-08-18]
    ss <<"  ---   C back-end:               "
       <<BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER <<"." <<BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER
       <<" (" <<BACKEND_C_COMPILER_NAME_WITH_PATH <<")\n";

    ss <<"  ---   C++ back-end:             "
       <<BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER <<"." <<BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER
       <<" (" <<BACKEND_CXX_COMPILER_NAME_WITH_PATH <<")\n";
#else
    ss <<"  --- C/C++ analysis:             disabled\n";
#endif

    //-----------------------------------------------------------------------
    // Information related to Fortran analysis.
    //-----------------------------------------------------------------------

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
    ss <<"  --- Fortran analysis:           enabled\n";

    extern string ofpVersionString();
    ss <<"  ---   OFP Fortran parser:       " <<ofpVersionString() <<"\n";

    ss <<"  ---   Fortran back-end:         "
       <<BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER <<"." <<BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER
       <<" (" <<BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH <<")\n";
#else
    ss <<"  --- Fortran analysis:           disabled\n";
#endif

    //-----------------------------------------------------------------------
    // Information related to binary analysis.
    //-----------------------------------------------------------------------

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    ss <<"  --- binary analysis:            enabled\n";

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    ss <<"  ---   object serialization:     enabled\n";
#else
    ss <<"  ---   object serialization:     disabled\n";
#endif

#ifdef ROSE_ENABLE_ASM_AARCH32
    ss <<"  ---   ARM AArch32 (A32/T32):    enabled\n";
#else
    ss <<"  ---   ARM AArch32 (A32/T32):    disabled\n";
#if !defined(ROSE_HAVE_CAPSTONE)
    ss <<"          missing capstone library\n";
#endif
#endif

#ifdef ROSE_ENABLE_ASM_AARCH64
    ss <<"  ---   ARM AArch64 (A64):        enabled\n";
#else
    ss <<"  ---   ARM AArch64 (A64):        disabled\n";
#if !defined(ROSE_HAVE_CAPSTONE)
    ss <<"          missing capstone library\n";
#endif
#endif

    ss <<"  ---   MIPS (be and le):         enabled\n";
    ss <<"  ---   Motorola m68k (coldfire): enabled\n";
    ss <<"  ---   PowerPC (be and le):      enabled\n";
    ss <<"  ---   Intel x86 (i386):         enabled\n";
    ss <<"  ---   Intel x86-64 (amd64):     enabled\n";

#ifdef ROSE_ENABLE_CONCOLIC_TESTING
    ss <<"  ---   concolic testing:         enabled\n";
#else
    ss <<"  ---   concolic testing:         disabled\n";
#if !defined(__linux__)
    ss <<"          this is not Linux\n";
#endif
#if !defined(ROSE_HAVE_SQLITE3) && !defined(ROSE_HAVE_LIBPQXX)
    ss <<"          no SQL database driver\n";
#endif
#if BOOST_VERSION < 106400
    ss <<"          Boost library is too old\n";
#endif
#if !defined(ROSE_HAVE_BOOST_SERIALIZATION_LIB)
    ss <<"          Boost serialization is not available\n";
#endif
#if !defined(ROSE_ENABLE_DEBUGGER_LINXU)
    ss <<"          PTRACE facility is not available\n";
#endif
#endif

#ifdef ROSE_ENABLE_DEBUGGER_LINUX
    ss <<"  ---   Linux PTRACE debugging    enabled\n";
#else
    ss <<"  ---   Linux PTRACE debugging    disabled\n";
#endif

#ifdef ROSE_ENABLE_DEBUGGER_GDB
    ss <<"  ---   GDB debugger interface    enabled\n";
#else
    ss <<"  ---   GDB debugger interface    disabled\n";
    ss <<"          Boost version is too old\n";
#endif

#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION
    ss <<"  ---   fast lib identification   enabled\n";
#else
    ss <<"  ---   fast lib identification   disabled\n";
#if !defined(ROSE_HAVE_SQLITE3) && !defined(ROSE_HAVE_LIBPQXX)
    ss <<"          no SQL database driver\n";
#endif
#if defined(__APPLE__) || defined(__MACH__)
    ss <<"          not supported on macOS\n";
#endif
#endif

#ifdef ROSE_ENABLE_MODEL_CHECKER
    ss <<"  ---   model checking            enabled\n";
#else
    ss <<"  ---   model checking            disabled\n";
#if !defined(_REENTRANT)
    ss <<"          multi-threading is disabled\n";
#endif
#endif

#ifdef ROSE_HAVE_CAPSTONE
    ss <<"  ---   capstone library:         " <<CS_VERSION_MAJOR <<"." <<CS_VERSION_MINOR <<"." <<CS_VERSION_EXTRA <<"\n";
#else
    ss <<"  ---   capstone library:         unused\n";
#endif

#if !defined(ROSE_HAVE_DLIB)
    ss <<"  ---   dlib library:             unused\n";
#elif defined(DLIB_PATCH_VERSION)
    ss <<"  ---   dlib library:             " <<DLIB_MAJOR_VERSION <<"." <<DLIB_MINOR_VERSION <<"." <<DLIB_PATCH_VERSION <<"\n";
#elif defined(DLIB_MINOR_VERSION)
    ss <<"  ---   dlib library:             " <<DLIB_MAJOR_VERSION <<"." <<DLIB_MINOR_VERSION <<"\n";
#else
    ss <<"  ---   dlib library:             unknown version\n";
#endif

#ifdef ROSE_HAVE_LIBGCRYPT
    ss <<"  ---   gcrypt library:           " <<GCRYPT_VERSION <<"\n";
#else
    ss <<"  ---   gcrypt library:           unused\n";
#endif

#ifdef ROSE_HAVE_LIBMAGIC
    ss <<"  ---   magic numbers library:    " <<MAGIC_VERSION <<"\n";
#else
    ss <<"  ---   magic numbers library:    unused\n";
#endif

#ifdef ROSE_HAVE_LIBPQXX
    ss <<"  ---   pqxx library:             " <<PQXX_VERSION <<"\n";
#else
    ss <<"  ---   pqxx library:             unused\n";
#endif

#ifdef ROSE_HAVE_SQLITE3
    ss <<"  ---   sqlite library:           " <<SQLITE_VERSION <<"\n";
#else
    ss <<"  ---   sqlite library:           unused\n";
#endif

#ifdef ROSE_HAVE_YAMLCPP
    ss <<"  ---   yaml-cpp library:         unknown version\n"; // not provided by the library or headers
#else
    ss <<"  ---   yaml-cpp library:         unused\n";
#endif

#ifdef Z3_FULL_VERSION
    ss <<"  ---   z3 library:               " <<Z3_FULL_VERSION <<" (" <<ROSE_Z3 <<")\n";
#elif defined(ROSE_HAVE_Z3)
    ss <<"  ---   z3 library:               unknown version (" <<ROSE_Z3 <<")\n";
#else
    ss <<"  ---   z3 library:               unused\n";
#endif

#else
    ss <<"  --- binary analysis:            disabled\n";
#endif

    //-----------------------------------------------------------------------
    // Information related to other language analysis, alphabetically. These
    // CPP symbols with weird and inconsistent names come from ROSE's Autotools
    // configuration system. If you remove them from this list because they're
    // not supported anymore, then kindly also remove them from the rest of the
    // ROSE library source code and tests!
    //-----------------------------------------------------------------------

#if defined ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION || defined ROSE_EXPERIMENTAL_LIBADALANG_CONNECTION
    ss <<"  --- Ada analysis:               enabled\n";
#else
    ss <<"  --- Ada analysis:               disabled\n";
#endif

#ifdef ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION
    ss <<"  --- C# analysis:                enabled\n";
#else
    ss <<"  --- C# analysis:                disabled\n";
#endif

#ifdef ROSE_BUILD_CUDA_LANGUAGE_SUPPORT
    ss <<"  --- CUDA analysis:              enabled\n";
#else
    ss <<"  --- CUDA analysis:              disabled\n";
#endif

#ifdef ROSE_BUID_JAVA_LANGUAGE_SUPPORT
    ss <<"  --- Java analysis:              enabled\n";
#else
    ss <<"  --- Java analysis:              disabled\n";
#endif

#ifdef ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
    ss <<"  --- Jovial analysis:            enabled\n";
#else
    ss <<"  --- Jovial analysis:            disabled\n";
#endif

#ifdef ROSE_EXPERIMENTAL_MATLAB_ROSE_CONNECTION
    ss <<"  --- Matlab analysis:            enabled\n";
#else
    ss <<"  --- Matlab analysis:            disabled\n";
#endif

#ifdef ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT
    ss <<"  --- OpenCL analysis:            enabled\n";
#else
    ss <<"  --- OpenCL analysis:            disabled\n";
#endif

#ifdef ROSE_BUILD_PHP_LANGUAGE_SUPPORT
    ss <<"  --- PHP analysis:               enabled\n";
#else
    ss <<"  --- PHP analysis:               disabled\n";
#endif

#ifdef ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
    ss <<"  --- Python analysis:            enabled\n";
#else
    ss <<"  --- Python analysis:            disabled\n";
#endif

    return ss.str();
}

std::string version_number()
   {
#ifdef VERSION
  // returns a string containing the current version number
  // the VERSION macro is defined at compile time on the
  // compile command line by the Makefile generated by automake
     return VERSION;
#else
  ROSE_ASSERT(! "Expected CPP macro VERSION to be defined");
#endif
   }

unsigned int rose_boost_version_id()
  {
#ifdef ROSE_BOOST_VERSION
  // generated by automake
    return ROSE_BOOST_VERSION;
#else
  ROSE_ASSERT(! "Expected CPP macro ROSE_BOOST_VERSION to be defined");
  return 0; // Windows compiler requires return value
#endif
  }

std::string rose_boost_version_path()
  {
#ifdef ROSE_BOOST_PATH
  // generated by automake
    return ROSE_BOOST_PATH;
#else
  ROSE_ASSERT(! "Expected CPP macro ROSE_BOOST_PATH to be defined");
  return ""; // Windows compiler requires return value
#endif
  }

// DQ (4/17/2010): This function must be defined if C++ support in ROSE is disabled.
void
outputPredefinedMacros()
   {
     printf ("Output of relevant pre-defined macros in ROSE: \n");
#ifdef __GNUC__
      printf ("   macro: __GNUC__ = %d \n",__GNUC__);
#endif
#ifdef __GNUC_MINOR__
      printf ("   macro: __GNUC_MINOR__ = %d \n",__GNUC_MINOR__);
#endif
#ifdef BOOST_COMPILER
      printf ("   macro: BOOST_COMPILER = %s \n",BOOST_COMPILER);
#endif
#ifdef __VERSION__
      printf ("   macro: __VERSION__ = %s \n",__VERSION__);
#endif
#ifdef __GNUC_PATCHLEVEL__
      printf ("   macro: __GNUC_PATCHLEVEL__ = %d \n",__GNUC_PATCHLEVEL__);
#endif
#ifdef __GNUC__
      printf ("   macro: __GNUC__*10000 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__ = %d \n",__GNUC__*10000 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__);
#endif
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      printf ("   macro: __GXX_EXPERIMENTAL_CXX0X__ = %d \n",__GXX_EXPERIMENTAL_CXX0X__);
#endif

#ifdef __EDG_VERSION__
      printf ("   macro: __EDG_VERSION__ = %d \n",__EDG_VERSION__);
#endif
#ifdef __EDG__
      printf ("   macro: __EDG__ = %d \n",__EDG__);
#endif
#ifdef XXX
      printf ("   macro: XXX = %d \n",XXX);
#endif
   }

/*! \brief Call to frontend, processes commandline and generates a SgProject object.

    This function represents a simple interface to the use of ROSE as a library.
    The commandline is processed and the return parameter is the generate SgProject object.
 */

SgProject*
frontend (int argc, char** argv, bool frontendConstantFolding )
   {
     return frontend(std::vector<std::string>(argv, argv + argc),frontendConstantFolding);
   }

SgProject*
frontend (const std::vector<std::string>& argv, bool frontendConstantFolding )
   {
  // DQ (4/11/2017): Call this as early as possible (usually good enough if it is here).
  // Note that there are ROSE-based tools that are calling the SgProject constructor directly,
  // and these are not covered by placing ROSE_INITIALIZE here (examples are: loopProcessor).
  // But most are older examples of tools built using ROSE.
     ROSE_INITIALIZE;

     TimingPerformance timer ("ROSE frontend():");

  // Syncs C++ and C I/O subsystems!
     ios::sync_with_stdio();

  // make sure that there is some sort of commandline (at least a file specified)
     if (argv.size() == 1)
        {
          SgFile::usage(1);      // Print usage and exit with exit status == 1
        }

  // We parse plugin related command line options before calling project();
     std::vector<std::string> argv2 = argv; // workaround const argv
#ifdef _MSC_VER
    if ( SgProject::get_verbose() >= 1 )
        printf ("Note: Dynamic Loadable Plugins are not supported on Microsoft Windows yet. Skipping Rose::processPluginCommandLine () ...\n");
#else
     Rose::processPluginCommandLine(argv2);
#endif

  // Separate the creation of a new project with building the AST. The default constructor
  // initializes the object and then parse is used to build the AST (in a separate step).
  // The current constructors that conflate object creation with parsing should be deprecated.
  // [Rasmussen, 2024.04.03]
     SgProject* project = new SgProject;

     ASSERT_not_null(project);
     project->set_frontendConstantFolding(frontendConstantFolding);

  // Create the AST by setting command-line options and then parsing all files from the command line
     project->parse(argv);

     if (project->numberOfFiles() == 0) {
       mlog[WARN] << "SgProject::frontend: no source files were processed from the command line\n";
     }

  // DQ (1/27/2017): Comment this out so that we can generate the dot graph to debug symbol with null basis.
     unsetNodesMarkedAsModified(project);

     std::list<std::string> const & astfiles = project->get_astfiles_in();
     if (astfiles.size() > 0) {
       Rose::AST::IO::load(project, astfiles);
       ASSERT_require(project->get_ast_merge());
     }

     if (project->get_ast_merge()) {
       Rose::AST::IO::merge(project);
     }

  // Set the mode to be transformation, mostly for Fortran. Liao 8/1/2013
     if (SageBuilder::SourcePositionClassificationMode == SageBuilder::e_sourcePositionFrontendConstruction) {
       SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionTransformation);
     }

     Rose::AST::cmdline::graphviz.frontend.exec(project);
     Rose::AST::cmdline::checker.frontend.exec(project);

  // Connect to Ast Plugin Mechanism
#ifdef _MSC_VER
    if ( SgProject::get_verbose() >= 1 )
        printf ("Note: Dynamic Loadable Plugins are not supported on Microsoft Windows yet. Skipping Rose::obtainAndExecuteActions ()\n");
#else
     Rose::obtainAndExecuteActions(project);
#endif
     return project;
   }

/*! \brief Call to build SgProject with empty SgFiles.

    This function represents a simple interface to build a SgProject with all the
    SgFiles in place any initialized properly but containing a SgGlobal with no declarations.
    The purpose of this function is to build a SgProject with SgFile object for each file
    on the command line, and allow for the frontend to be called separately for each file.
    The commandline is completely processed so that all information is in each SgFile of the
    SgProject.
    This way specific files can be processed conditionally.  This mechanism can be used to
    build support for exclusion of specific files or for exclusion of files in a specific
    subdirectory.  Or to support specialized handling of files with a specific extension, etc.

 */

SgProject*
frontendShell (int argc, char** argv)
   {
     return frontendShell(std::vector<std::string>(argv, argv + argc));
   }

SgProject*
frontendShell (const std::vector<std::string> &argv)
   {
  // Convert this to a list of strings to simplify editing (adding new option)
     Rose_STL_Container<string> commandLineList = argv;
     printf ("frontendShell (top): argv = \n%s \n",StringUtility::listToString(commandLineList).c_str());

  // Invoke ROSE commandline option to skip internal frontend processing (we will
  // call the fronend explicitly for selected files, after construction of SgProject).
     commandLineList.push_back("-rose:skip_rose");

  // Build the SgProject, but if the above option was used this will only build empty SgFile nodes
     SgProject* project = frontend(commandLineList);
     ASSERT_not_null(project);

     project->display("In frontendShell(), after frontend()");

     for (auto file : project->get_fileList())
        {
       // Get the local command line so that we can remove the "-rose:skip_rose" option
          vector<string> local_argv = file->get_originalCommandLineArgumentList();

       // Note that we have to remove the "-rose:skip_rose" option that was saved
          CommandlineProcessing::removeArgs(local_argv, "-rose:skip_rose");

       // Set the new commandline (without the "-rose:skip_rose" option)
          file->set_originalCommandLineArgumentList(local_argv);

       // Things set by "-rose:skip_rose" option, which must be unset (reset to default valees)!
          file->set_skip_transformation(false);
          file->set_disable_edg_backend(false);

          file->set_useBackendOnly(false);
          file->set_skipfinalCompileStep(false);

       // Skip all processing of comments
          file->set_skip_commentsAndDirectives(false);
        }

     return project;
   }

/*! \brief Call to backend, generates either object file or executable.

    This function operates in two modes:
        1) If source files were specified on the command line, then it calls
           unparser and compiles generated file.
        2) If no source files are present then it operates as a linker processing
           all specified object files.
    If no source files or object files are specified then we return a error.

    This function represents a simple interface to the use of ROSE as a library.

     At this point in the control flow we have returned from the processing via the
     EDG frontend (or skipped it if that option was specified).
     The following has been done or explicitly skipped if such options were specified
     on the commandline:
        1) The application program has been parsed
        2) All AST's have been build (one for each grammar)
        3) The transformations have been edited into the C++ AST
        4) The C++ AST has been unparsed to form the final output file (all code has
           been generated into a different filename "rose_<original file name>.C")

    \internal The error code is returned, but it might be appropriate to make
              it more similar to the frontend() function and its handling of
              the error code.
 */
int
backend ( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate )
   {
     TimingPerformance timer ("AST Object Code Generation (backend):");
     int finalCombinedExitStatus = 0;

     if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
        {
          printf ("Inside of backend(SgProject*) \n");
        }

#ifdef ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
     signal(SIGABRT,SIG_DFL);
#endif
     Rose::AST::cmdline::graphviz.backend.exec(project);
     Rose::AST::cmdline::checker.backend.exec(project);

     std::string const & astfile_out = project->get_astfile_out();
     if (astfile_out != "") {
       std::list<std::string> empty_file_list;
       project->set_astfiles_in(empty_file_list);
       project->get_astfile_out() = "";
       AST_FILE_IO::reset();
       AST_FILE_IO::startUp(project);
       AST_FILE_IO::writeASTToFile(astfile_out);
       AST_FILE_IO::resetValidAstAfterWriting();
     }

     if (project->get_binary_only()) {
          project->skipfinalCompileStep(true);
     }

     if (project->get_useBackendOnly() == false)
        {
       // Add forward references for instantiated template functions and member functions
       // (which are by default defined at the bottom of the file (but should be declared
       // at the top once we know what instantiations should be built)).  They must be
       // defined at the bottom since they could call other functions not yet declared in
       // the file.  Note that this fixup is required since we have skipped the class template
       // definitions which would contain the declarations that we are generating.  We might
       // need that as a solution at some point if this fails to be sufficently robust.

       // generate C++ source code
          if (SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL) {
              cout <<  "Calling project->unparse()\n";
          }

          project->unparse(unparseFormatHelp, unparseDelegate);

          if (SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL) {
              cout << "source file(s) generated. (from AST)\n" << endl;
          }
        }

     if (!project->get_skipfinalCompileStep() && (project->numberOfFiles() > 0 || project->numberOfDirectories() > 0))
        {
       // Compile generated C++ source code with vendor compiler.
       // Generate object file (required for further template processing if templates exist).
          if (SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL) {
              cout << "Calling project->compileOutput()\n";
          }
          finalCombinedExitStatus = project->compileOutput();
        }
       else
        {
          if (SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL) {
              printf ("   project->get_compileOnly() = %s \n",project->get_compileOnly() ? "true" : "false");
          }

       // DQ (5/20/2005): If we have not permitted templates to be instantiated during initial
       // compilation then we have to do the prelink step (this is however still new and somewhat
       // problematic (buggy?)).  It relies upon the EDG mechansisms which are not well understood.
          bool callTemplateInstantiation = (project->get_template_instantiation_mode() == SgProject::e_none);
          ASSERT_require(!callTemplateInstantiation);

          if (SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL) {
              printf ("Calling project->link() \n");
          }

       // DQ (10/15/2005): Trap out case of C programs where we want to make sure that we don't use the C++ compiler to do our linking!
       // This could be done in the
          if (project->get_C_only() == true)
             {
               printf ("Link using the C language linker (when handling C programs) = %s \n",BACKEND_C_COMPILER_NAME_WITH_PATH);
               finalCombinedExitStatus = project->link(BACKEND_C_COMPILER_NAME_WITH_PATH);
             }
          else if (project->get_Fortran_only() == true)
             {
               printf ("Link using the Fortran language linker (when handling Fortran programs) = %s \n",BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH);
               finalCombinedExitStatus = project->link(BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH);
             }
          else if (project->get_Jovial_only() || project->get_Jvm_only())
             {
            // Skip linking (this could apply to other language families also [Rasmussen 2024.05.16]
               finalCombinedExitStatus = 0;
             }
          else
             {
            // Use the default name for C++ compiler (defined at configure time)
               if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
                    printf ("Link using the default linker (when handling non-C programs) = %s \n",BACKEND_CXX_COMPILER_NAME_WITH_PATH);
               finalCombinedExitStatus = project->link(BACKEND_CXX_COMPILER_NAME_WITH_PATH);
             }
        }

     if ( SgProject::get_verbose() >= 1 ) {
        cout << "source file(s) compiled with vendor compiler. (exit status = " << finalCombinedExitStatus << ").\n" << endl;
     }

  // Set the final error code to be returned to the user.
     project->set_backendErrorCode(finalCombinedExitStatus);

     return project->get_backendErrorCode();
   }


int
backendCompilesUsingOriginalInputFile ( SgProject* project, bool compile_with_USE_ROSE_macro )
   {
  // DQ (8/24/2009):
  // To work with existing makefile systems, we want to force an object file to be generated.
  // So we want to call the backend compiler on the original input file (instead of generating
  // an new file from the AST and running it through the backend).  The whole point is to
  // gnerate a object file.  The effect is that this does a less agressive test of ROSE
  // but test only the parts required for analysis tools instead of transformation tools.
  // This avoids some programs in the name qualification support that is called by the backend
  // and permits testing of the parts of ROSE relevant for analysis tools (e.g. Compass).
  // Of course we eventually want everything to work, but I want to test the compilation of
  // ROSE using ROSE as part of test to get Compass running regularly on ROSE.

  // Note that the command generated may have to be fixup later to include more subtle details
  // required to link libraries, etc.  At present this function only handles the support required
  // to build an object file.
     SgStringList commandLineToGenerateObjectFile;

     enum language_enum
        {
          e_none    = 0,
          e_c       = 1,
          e_cxx     = 2,
          e_fortran = 3,
          e_last_language
        };

     language_enum language = e_none;
     language = project->get_C_only()       ? e_c       : language;
     language = project->get_Cxx_only()     ? e_cxx     : language;
     language = project->get_Fortran_only() ? e_fortran : language;

     if (language == e_none)
        {
       // DQ (4/7/2010): Set the default language for ROSE to be C++
       // This will mean that linking fortran object files will not be possible with ROSE.
       // but at least configure will work propoerly since it will have a valid default.
       // If we add state to SgProject, then we could set the default, but also allow it
       // to be overriden using -rose:C or -rose:Cxx or -rose:Fortran options.
          language = e_cxx;
        }

     switch (language)
        {
        case e_c       : commandLineToGenerateObjectFile.push_back(BACKEND_C_COMPILER_NAME_WITH_PATH);       break;
        case e_cxx     : commandLineToGenerateObjectFile.push_back(BACKEND_CXX_COMPILER_NAME_WITH_PATH);     break;
        case e_fortran : commandLineToGenerateObjectFile.push_back(BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH); break;

          default:
             {
                 // Note that the default is C++, and that if there are no SgFile objects then there is no place to hold the default
                 // since the SgProject does not have any such state to hold this information.  A better idea might be to give the
                 // SgProject state so that it can hold if it is used with -rose:C or -rose:Cxx or -rose:Fortran on the command line.

               printf ("Default reached in switch in backendCompilesUsingOriginalInputFile() \n");
               printf ("   Note use options: -rose:C or -rose:Cxx or -rose:Fortran to specify which language backend compiler to link object files. \n");
               ROSE_ABORT();
             }
        }

     if (compile_with_USE_ROSE_macro == true)
        {
       // DQ (11/3/2011): Mark this as being called from ROSE (even though the backend compiler is being used).
       // This will help us detect where strings handed in using -D options may have lost some outer quotes.
       // There may also be a better fix to detect quoted strings and requote them, so this should be considered also.
            commandLineToGenerateObjectFile.push_back("-DUSE_ROSE");
        }

     int finalCombinedExitStatus = 0;
     if (project->numberOfFiles() > 0)
        {
          SgStringList originalCommandLineArgumentList = project->get_originalCommandLineArgumentList();

       // DQ (2/20/2010): Added filtering of options that should not be passed to the vendor compiler.
          SgFile::stripRoseCommandLineOptions(originalCommandLineArgumentList);
          SgFile::stripEdgCommandLineOptions(originalCommandLineArgumentList);

          SgStringList::iterator it = originalCommandLineArgumentList.begin();

       // Iterate past the name of the compiler being called (arg[0]).
          if (it != originalCommandLineArgumentList.end())
               it++;

       // JL (03/15/2018) Changed system to systemFromVector so that
       // command line arguments will be handled correctly ROSE-813
          for (SgStringList::iterator i = it; i != originalCommandLineArgumentList.end(); i++)
          {
              commandLineToGenerateObjectFile.push_back(*i);
          }

          if ( SgProject::get_verbose() >= 1 )
          {
              printf("Compile Line: ");
              for (SgStringList::iterator i = it; i != commandLineToGenerateObjectFile.end(); i++)
              {
                  printf("%s ", (*i).c_str());
              }
              printf("\n");
          }


       // DQ (12/28/2010): If we specified to NOT compile the input code then don't do so even when it is the
       // original code. This is important for Fortran 2003 test codes that will not compile with gfortran and
       // for which the tests/nonsmoke/functional/testTokenGeneration.C translator uses this function to generate object files.
          if (project->get_skipfinalCompileStep() == false)
          {
              finalCombinedExitStatus = systemFromVector (commandLineToGenerateObjectFile);
          }
        }
       else
        {
       // Note that in general it is not possible to tell whether to use gcc, g++, or gfortran to do the linking.
       // When we just have a list of object files then we can't assume anything (and project->get_C_only() will be false).
       // Note that commandLineToGenerateObjectFile is just the name of the backend compiler to use!
       // JL (03/15/2018) Put in ROSE_ASSERT to verify command line is just the linker
       //Thats all link is supposed to take
            ASSERT_require(commandLineToGenerateObjectFile.size() == 1);
            finalCombinedExitStatus = project->link(commandLineToGenerateObjectFile[0]);
        }

     return finalCombinedExitStatus;
   }



int
backendGeneratesSourceCodeButCompilesUsingOriginalInputFile ( SgProject* project )
   {
  // DQ (2/6/2010): This function is a step between calling the backend()
  // and calling backendCompilesUsingOriginalInputFile().  It it used
  // the test the generation of the source code, but not the compilation of
  // it using the backend (vendor) compiler.  This is used to test ROSE.

  // Users are likely to either want to use backend() to generate the source
  // code for there project and it compiled (e.g. for optimization) or call
  // backendCompilesUsingOriginalInputFile() to process the input code and
  // then generate object files or executables from the original code
  // (e.g for analysis).

  // This instance of complexity is why this needs to be a separate backend function.
  // Note that file->get_skip_unparse() will be false when the "-E" option, and
  // the unparse() function will properly assert that it should be true.
     if (project->get_skip_unparse() == false)
        {
          project->unparse();
        }

     return backendCompilesUsingOriginalInputFile(project);
   }


int
copy_backend(SgProject* /*project*/, UnparseFormatHelp* /*unparseFormatHelp*/)
   {
  // This is part of the copy-based unparser (from Qing).
  // This function calls the unparseFile function with a
  // CopyUnparser object (derived from UnparseDelegate)
  // to control the unparsing and substitute text based
  // copying for code generation from the AST.

  // This function does not presently have the same semantics as the "backend()".
  // The code above could be refactored to permit both backend function to more
  // readily have the same semantics later.

     printf ("Error: Inside of copy_backend(), the copy backend has been disabled in favor of a token based mechanism for unparsing. \n");
     ROSE_ABORT();

     return 0;
   }

void
generatePDF ( const SgProject & project )
   {
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  // DQ (6/14/2007): Added support for timing of the generatePDF() function.
     TimingPerformance timer ("ROSE generatePDF():");

     if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
          printf ("Inside of generatePDF \n");

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     AstJSONGeneration jsontest;
     SgProject & nonconstProject = (SgProject &) project;
     jsontest.generateInputFiles(&nonconstProject);
#endif
   }

void
generateDOT ( const SgProject & project, std::string filenamePostfix, bool excludeTemplateInstantiations )
   {
  // DQ (7/4/2008): Added default parameter to support the filenamePostfix
  // mechanism in AstDOTGeneration
     TimingPerformance timer ("ROSE generateDOT():");
     AstDOTGeneration astdotgen;
     SgProject & nonconstProject = (SgProject &) project;

  // DQ (12/14/2018): The number of nodes is computed globally, but the graph is generated only for the input file.
  // So this can suppress the generation of the graph when there are a large number of IR nodes from header files.
  // Multiplied the previous value by 10 to support building the smaller graph of the input file.
     int maxSize = 10000000;

     int numberOfASTnodes = numberOfNodes();

     if ( SgProject::get_verbose() >= 1 )
          printf ("In generateDOT(): numberOfASTnodes = %d maxSize = %d \n",numberOfASTnodes,maxSize);

  // DQ (2/18/2013): Compute the number of IR nodes for the AST and limit the size of these graphs (take too long to generate and the graphs are not useful).
     if (numberOfASTnodes < maxSize)
        {
       // Note that the use of generateInputFiles causes the graph to be generated
       // for only the input source file and not any included header files. The
       // result is a much smaller file (and generally a more useful one).
          astdotgen.generateInputFiles(&nonconstProject,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix,excludeTemplateInstantiations);
        }
       else
        {
          if ( SgProject::get_verbose() >= 0 )
               printf ("In generateDOT(): AST graph too large to generate. (numberOfASTnodes=%d) > (maxSize=%d) \n",numberOfASTnodes,maxSize);
        }
   }


void
generateDOT ( SgNode* node, std::string filename )
   {
  // DQ (9/22/2017): This function is being provided to support the generation of a dot file from any subtree.
  // The more imediate use for this function is to support generation of dot files from trees built using the ROSE Untyped nodes.

     TimingPerformance timer ("ROSE generateDOT():");

     AstDOTGeneration astdotgen;
     std::string filenamePostfix = "";
     astdotgen.generate(node, filename, DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP, filenamePostfix);
   }


void
generateDOT_withIncludes ( const SgProject & project, std::string filenamePostfix )
   {
     TimingPerformance timer ("ROSE generateDOT_withIncludes():");

     AstDOTGeneration astdotgen;
     SgProject & nonconstProject = (SgProject &) project;

  // Note that the use of generateInputFiles causes the graph to be generated
  // for only the input source file and not any included header files. The
  // result is a much smaller file (and generally a more useful one).

     AstDOTGeneration::traversalType tt = AstDOTGeneration::TOPDOWNBOTTOMUP;
     astdotgen.generate(&nonconstProject,tt,filenamePostfix);
   }

void
generateDOTforMultipleFile ( const SgProject & project, std::string filenamePostfix )
   {
     TimingPerformance timer ("ROSE generateDOTforMultipleFile():");

  // This is the best way to handle generation of DOT files where multiple files
  // are specified on the command line.  Later we may be able to filter out the
  // include files (but this is a bit difficult until generateInputFiles() can be
  // implemented to call the evaluation of inherited and synthesized attributes.
     generateDOT_withIncludes(project,filenamePostfix);
   }

void generateAstGraph ( const SgProject* project, int maxSize, std::string filenameSuffix )
   {
     TimingPerformance timer ("ROSE generateAstGraph():");

  // Generate a name from all the files on the command line
     string filename = SageInterface::generateProjectName(project, /* supressSuffix = */ true );
     filename += "_WholeAST";
     filename += filenameSuffix;

     int numberOfASTnodes = numberOfNodes();

     if ( SgProject::get_verbose() >= 1 )
          printf ("In generateAstGraph(): numberOfASTnodes = %d maxSize = %d filename = %s \n",numberOfASTnodes,maxSize,filename.c_str());

  // Compute the number of IR nodes for the AST
     if (numberOfASTnodes < maxSize)
        {
       // Added support to handle options to control filtering of Whole AST graphs.
          std::vector<std::string>  argvList = project->get_originalCommandLineArgumentList();
          CustomMemoryPoolDOTGeneration::s_Filter_Flags* filter_flags = new CustomMemoryPoolDOTGeneration::s_Filter_Flags(argvList);

          generateWholeGraphOfAST(filename,filter_flags);
        }
       else
        {
          if ( SgProject::get_verbose() >= 1 )
               printf ("In generateAstGraph(): WHOLE AST graph too large to generate. (numberOfASTnodes=%d) > (maxSize=%d) \n",numberOfASTnodes,maxSize);
        }
   }





#if 0

// Include tree traversal for EDG abstract program tree
// #include "../EDG/src/displayTree.h"
#include "/frontend/EDG/EDG_3.3/src/displayTree.h"

//! Generate PDF file representing EDG's Abstract Syntax Tree
void
pdfPrintAbstractSyntaxTreeEDG ( SgFile *file )
   {
     char filename[256];

     printf ("## Dumping the EDG program tree to a PDF file ## \n");

  // Use the PDF file declared in the EDG/src/displayTree.C
     extern PDF* pdfGlobalFile;

     printf ("Rose::getWorkingDirectory() = %s \n",Rose::getWorkingDirectory());
     sprintf(filename,"%s/%s.edg.pdf",Rose::getWorkingDirectory(),Rose::utility_stripPathFromFileName(Rose::getFileName(file)));
     printf ("filename = %s \n",filename);

     ifstream sourceFile (Rose::getFileName(file));
     if (!sourceFile)
          cerr << "ERROR opening sourceFile" << endl;
     ROSE_ASSERT (sourceFile);

     fprintf(stderr, "Creating PDFlib file '%s'!\n", filename);

#if 0
     #define MAX_BUFFER_LENGTH 10000

     char buffer[100][MAX_BUFFER_LENGTH];
     int i = 0;

     while (sourceFile.getline(buffer[i],100,'\n') && i < MAX_BUFFER_LENGTH)
        {
          printf ("string = %s \n",buffer[i]);
          i++;
        }

  // printf ("Exting after printing source file \n");
  // ROSE_ABORT();
#endif

  // Initialize the PDFLib library
     PDF_boot();

  // Build a PDF file
     PDF* pdfFile = PDF_new();
     ROSE_ASSERT (pdfFile != NULL);

  // Open the pdf file for writing
     if (PDF_open_file(pdfFile, filename) == -1)
        {
          printf("Couldn't open PDF file '%s'!\n", filename);
          ROSE_ABORT();
        }

  // Initialize properties stored in PDF file
     PDF_set_info(pdfFile, "Keywords", "Abstract Syntax Tree (AST) EDG");
     PDF_set_info(pdfFile, "Subject", "Display of AST for EDG");
     PDF_set_info(pdfFile, "Title", "AST for Program Code");
     PDF_set_info(pdfFile, "Creator", "ROSE");
     PDF_set_info(pdfFile, "Author", "Daniel Quinlan");

//  unsigned char buf[64], tmp[64];
//  int c, pos;
//  int level1, level2=0, level3=0;
    int font;

    font = PDF_findfont(pdfFile, "Helvetica", "host", 0);

 // Specify the page size (can be much larger than letter size)
 // PDF_begin_page(pdfFile, a4_width, a4_height);
    PDF_begin_page(pdfFile, letter_width, letter_height);

 // Specify a 10 pt font
    int fontSize = 10;

 // setup the font to use in this file
    PDF_setfont(pdfFile, font, fontSize);

 // start at a reasonable position on the page
    int topMargin  = fontSize * 4;
    int leftMargin = fontSize * 4;

 // Translate the orgin of the mapping to the top left corner of the page
 // with a little room for margins
    PDF_translate(pdfFile,0+leftMargin,letter_height-topMargin);

#if 0
    strcpy (buffer[0], "abcdefg");
    PDF_show(pdfFile, buffer[0]);
    for (i=0; i < 20; i++)
         PDF_continue_text(pdfFile, buffer[0]);
#endif

#if 0
     while (sourceFile.getline(buffer[i],100,'\n') && i < MAX_BUFFER_LENGTH)
        {
          PDF_continue_text(pdfFile, buffer[i]);
       // printf ("string = %s \n",buffer[i]);
          i++;
        }
#endif

  /* private Unicode info entry */
     PDF_set_info(pdfFile, "Revision", "initial version 0.2");

  // initialize the global PDF file
     pdfGlobalFile = pdfFile;

  // Print out the EDG AST
     pdfOutput_complete_il();

  // Need this to avoid a warning from PDFLIB
     PDF_end_page(pdfFile);

  // Now close the PDF file
     PDF_close(pdfFile);
     PDF_delete(pdfFile);

  // finalize the last use of the PDF library
     PDF_shutdown();

     fprintf(stderr, "\nPDFlib EDG AST file '%s' finished!\n", filename);
   }
#endif

#if 0
void
generatePDFofEDG ( const SgProject & project )
   {
#if 0
  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     int i = 0;
     for (i=0; i < project.numberOfFiles(); i++)
        {
          SgFile & file = project.get_file(i);
          pdfPrintAbstractSyntaxTreeEDG(&file);
        }
#endif
   }
#endif

#if 1
int
Rose::getLineNumber ( SgLocatedNode* locatedNodePointer )
   {
  // Get the line number from the Sage II statement object
     ASSERT_not_null(locatedNodePointer);
     int lineNumber = -1;
  // Sometimes the locatedNode doesn't have a SgFile object
  // (likely because it is part of a parent statement object)
     if (locatedNodePointer->get_file_info() != nullptr)
        {
          ASSERT_not_null(locatedNodePointer->get_file_info());
          ASSERT_not_null(locatedNodePointer->get_file_info()->get_filename());
          Sg_File_Info* fileInfo = locatedNodePointer->get_file_info();
          lineNumber = fileInfo->get_line();
        }

     return lineNumber;
   }
#endif
#if 1
int
Rose::getColumnNumber ( SgLocatedNode* locatedNodePointer )
   {
  // Get the line number from the Sage II statement object
     ASSERT_not_null(locatedNodePointer);
     int columnNumber = -1;
  // Sometimes the locatedNode doesn't have a SgFile object
  // (likely because it is part of a parent statement object)
     if (locatedNodePointer->get_file_info() != nullptr)
        {
          ASSERT_not_null(locatedNodePointer->get_file_info());
          ASSERT_not_null(locatedNodePointer->get_file_info()->get_filename());
          Sg_File_Info* fileInfo = locatedNodePointer->get_file_info();
          columnNumber = fileInfo->get_col();
        }

     return columnNumber;
   }
#endif
#if 1
std::string
Rose::getFileName ( SgLocatedNode* locatedNodePointer )
   {
  // Get the filename from the Sage II statement object
     ASSERT_not_null(locatedNodePointer);
     std::string fileName = "NO NAME FILE";
  // Sometimes the locatedNode doesn't have a SgFile object
  // (likely because it is part of a parent statement object)
     if (locatedNodePointer->get_file_info() != nullptr)
        {
          ASSERT_not_null(locatedNodePointer->get_file_info());
          Sg_File_Info* fileInfo = locatedNodePointer->get_file_info();
          fileName = fileInfo->get_filenameString();
        }

     return fileName;
   }
#endif
#if 1
bool Rose:: isPartOfTransformation( SgLocatedNode* locatedNodePointer )
{
  bool result = false;
  Sg_File_Info *fileInfo = locatedNodePointer->get_file_info();
  if (fileInfo != 0)
     result = fileInfo->get_isPartOfTransformation();
  return result;
}
#endif

std::string
Rose::getFileNameWithoutPath ( SgStatement* statementPointer )
   {
  // Get the filename from the Sage III statement object
     ASSERT_not_null(statementPointer);
     ASSERT_not_null(statementPointer->get_file_info());

     std::string fileName = statementPointer->get_file_info()->get_filenameString();

     return utility_stripPathFromFileName(fileName);
   }

#if 1
std::string
Rose::utility_stripPathFromFileName ( const std::string& fileNameWithPath )
   {
     size_t pos = fileNameWithPath.rfind('/');
     if (pos == std::string::npos || pos == fileNameWithPath.size() - 1) {
       return fileNameWithPath;
     } else {
       return fileNameWithPath.substr(pos + 1);
   }
   }
#endif

#if 0
std::string
Rose::stripFileSuffixFromFileName ( const std::string& fileNameWithSuffix )
   {
  // This function is not sophisticated enough to handle binaries with paths such as:
  //    ROSE/ROSE_CompileTree/svn-LINUX-64bit-4.2.2/tutorial/inputCode_binaryAST_1
#if 1
     size_t lastDotPos = fileNameWithSuffix.rfind('.');
     return fileNameWithSuffix.substr(0, lastDotPos);
#else
  // Handle the case of files where the filename does not have a suffix
     size_t lastSlashPos = fileNameWithSuffix.rfind('/');
     size_t lastDotPos   = fileNameWithSuffix.rfind('.');

     string returnString;
     if (lastDotPos < lastSlashPos)
          returnString = fileNameWithSuffix;
       else
          returnString = fileNameWithSuffix.substr(0, lastDotPos);

     return returnString;
#endif
   }
#endif

#if 1
// DQ (3/15/2005): New, simpler and better implementation suggested function from Tom, thanks Tom!
string
Rose::getPathFromFileName ( const string fileName )
   {
     size_t pos = fileName.rfind('/');
     if (pos == std::string::npos) {
       return ".";
     } else {
       return fileName.substr(0, pos);
     }
   }
#endif

// Later I expect we will move these functions to be SgFile member functions

#if 0
 //! get the source directory (requires an input string currently)
char*
Rose::getSourceDirectory ( char* fileNameWithPath )
   {
     return getPathFromFileName (fileNameWithPath);
   }
#else
 //! get the source directory (requires an input string currently)
string
Rose::getSourceDirectory ( string fileNameWithPath )
   {
     return getPathFromFileName (fileNameWithPath);
   }
#endif

#if 0
 //! get the current directory
char*
Rose::getWorkingDirectory ()
   {
     int i = 0;  // index variable declaration

  // DQ (9/5/2006): Increase the buffer size
  // const int maxPathNameLength = 1024;
     const int maxPathNameLength = 10000;
     char* currentDirectory = new char [maxPathNameLength];
     for (i=0; i < maxPathNameLength; i++)
          currentDirectory[i] = '\0';  // set to NULL string

     char* returnString = getcwd(currentDirectory,maxPathNameLength);
     ROSE_ASSERT (returnString != NULL);

  // The semantics of the getcwd is that these should be the same (see if they are)
  // printf ("In Rose::getWorkingDirectory: Current directory = %s \n",currentDirectory);
  // printf ("In Rose::getWorkingDirectory: Current directory = %s \n",returnString);

  // live with the possible memory leak for now
  // delete currentDirectory;
     currentDirectory = NULL;

     return returnString;
   }
#else
 //! get the current directory
string
Rose::getWorkingDirectory ()
   {
     const unsigned int maxPathNameLength = 10000;
     char* currentDirectory = new char [maxPathNameLength+1];

  // CH (4/7/2010): In MSVC, the header file "direct.h" contains function 'getcwd'
     const char* getcwdResult = getcwd(currentDirectory,maxPathNameLength);

     if (!getcwdResult) {
       perror("getcwd: ");
       ROSE_ABORT();
     }
     string returnString = getcwdResult;
     delete [] currentDirectory;
     currentDirectory = nullptr;
     return returnString;
   }
#endif

SgName
Rose::concatenate ( const SgName & X, const SgName & Y )
   {
     return X + Y;
   }

#if 0
// DQ (9/5/2008): Try to remove this function!
std::string
Rose::getFileName ( const SgFile* file )
   {
  // Get the filename from the Sage II file object
     ROSE_ASSERT (file != NULL);
  // SgScopeStatement *globalScope = (SgScopeStatement *)(&(file->root()));
     const SgScopeStatement *globalScope = file->get_globalScope();
     ROSE_ASSERT (globalScope != NULL);
     Sg_File_Info* fileInfo = globalScope->get_file_info();
     ROSE_ASSERT (fileInfo != NULL);
     return fileInfo->get_filenameString();
   }
#endif

#if 1
// DQ (9/5/2008): Try to remove this function!
string
Rose::getFileNameByTraversalBackToFileNode ( const SgNode* astNode )
   {
     string returnString;
     ASSERT_not_null(astNode);

  // Make sure this is not a project node (since the SgFile exists below
  // the project and could not be found by a traversal of the parent list)
     if (isSgProject(astNode) == nullptr)
        {
          const SgNode* parent = astNode;
          while ( (parent != nullptr) && (isSgFile(parent) == nullptr) )
             {
               parent = parent->get_parent();
             }

          if (!parent) {
            const SgLocatedNode* ln = isSgLocatedNode(astNode);
            ASSERT_not_null(ln);
            return ln->get_file_info()->get_filenameString();
          }

          const SgFile* file = isSgFile(parent);
          ASSERT_not_null(file);
          if (file != nullptr)
             {
               returnString = file->getFileName();
             }
          ASSERT_require(returnString.empty() == false);
        }

     return returnString;
   }
#endif

void
Rose::usage (int status)
   {
     SgFile::usage(status);
   }

int
Rose::containsString ( const std::string& masterString, const std::string& targetString )
   {
     return masterString.find(targetString) != string::npos;
   }

void
Rose::filterInputFile(const string /*inputFileName*/, const string /*outputFileName*/) // blame quinlan1
   {
  // This function filters the input file to remove ^M characters and expand tabs etc.
  // Any possible processing of the input file, before being compiled, should be done
  // by this function.

  // This function is implemented in the ROSE/dqDevelopmentDirectory directory.
   }

SgStatement*
Rose::getNextStatement ( SgStatement *currentStatement )
   {
     ASSERT_not_null(currentStatement);
  // CI (1/3/2007): This used to be not implemented ,,, here is my try
  //! get next statement will return the next statement in a function or method. if at the end or outside, it WILL return NULL

     SgStatement      *nextStatement = nullptr;
     SgScopeStatement *scope         = currentStatement->get_scope();
     ASSERT_not_null(scope);

  // DQ (9/18/2010): If we try to get the next statement from SgGlobal, then return NULL.
     if (isSgGlobal(currentStatement) != nullptr)
          return nullptr;

  // Make sure that we didn't get ourselves back from the get_scope()
  // function (previous bug fixed, but tested here).
     ASSERT_require(scope != currentStatement);

     switch (currentStatement->variantT())
        {
          case V_SgForInitStatement:
          // case V_SgBasicBlock: // Liao 10/20/2010, We should allow users to get a statement immediately AFTER a block.
          case V_SgClassDefinition:
          case V_SgFunctionDefinition:
          case V_SgStatement:
          case V_SgFunctionParameterList:
             {
               ROSE_ABORT();
            // not specified
             }

       // DQ (11/8/2015): Added support for SgLabelStatement (see testcode tests/nonsmoke/functional/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_134.C)
          case V_SgLabelStatement:
            {
              SgLabelStatement* lableStatement = isSgLabelStatement(currentStatement);
              nextStatement = lableStatement->get_statement();
              ASSERT_not_null(nextStatement);
              break;
            }

          default:
             {
            // We have to handle the cases of a SgStatementPtrList and a
            // SgDeclarationStatementPtrList separately
               if (isSgDeclarationScope(scope)) return nullptr;
               if (scope->containsOnlyDeclarations() == true || (isSgDeclarationScope(scope)))
                  {
                 // Usually a global scope or class declaration scope
                    SgDeclarationStatementPtrList& declarationList = scope->getDeclarationList();
                    Rose_STL_Container<SgDeclarationStatement*>::iterator i;
                    for (i = declarationList.begin(); (i != declarationList.end() && (*i) != currentStatement); i++) {}

                 // DQ (7/19/2015): Needed to add support for template instatiations that might not be
                 // located in there scope (because they would be name qualified).
                    if (i == declarationList.end())
                       {
#if 0
                         printf ("Note: statement was not found in it's scope (happens for some template instantiations) \n");
#endif
                         nextStatement = NULL;
                       }
                      else
                       {
                         i++;
                         if (declarationList.end() == i)
                              nextStatement = NULL;
                           else
                              nextStatement=*i;
                       }
                  }
                 else
                  {
                    SgStatementPtrList & statementList = scope->getStatementList();
                    Rose_STL_Container<SgStatement*>::iterator i;
                 // Liao, 11/18/2009, Handle the rare case that current statement is not found
                 // in its scope's statement list
                    for (i = statementList.begin(); (*i) != currentStatement && i != statementList.end(); i++)
                       {
                      //  SgStatement* cur_stmt = *i;
                      //  cout<<"Skipping current statement: "<<cur_stmt->class_name()<<endl;
                      //  cout<<cur_stmt->get_file_info()->displayString()<<endl;
                       }

                 // currentStatement is not found in the list
                    if (i ==  statementList.end())
                       {
                         mlog[FATAL]<<"fatal error: ROSE::getNextStatement(): current statement is not found within its scope's statement list"<<endl;
                         mlog[FATAL]<<"current statement is "<<currentStatement->class_name()<<endl;
                         //~ mlog[FATAL]<<"code: " << currentStatement->unparseToString()<<endl;
                         mlog[FATAL]<<currentStatement->get_file_info()->displayString()<<endl;
                         mlog[FATAL]<<"Its scope is "<<scope->class_name()<<endl;
                         mlog[FATAL]<<scope->get_file_info()->displayString()<<endl;
#if 0
                         currentStatement->get_file_info()->display("fatal error: ROSE::getNextStatement(): current statement is not found within its scope's statement list: debug");
#endif
                         ROSE_ASSERT("!ROSE::getNextStatement not found");
                       }

                 // now i == currentStatement
                    ROSE_ASSERT (*i == currentStatement);

                 // DQ (7/19/2015): Added assertion that should be true, else i++ is not defined.
                    ROSE_ASSERT(i != statementList.end());

                    i++;
                    if (statementList.end() == i)
                          nextStatement = NULL;
                       else
                          nextStatement = *i;
                  }

            // If the target statement was the last statement in a scope then
               if (nextStatement == NULL)
                  {
                 // Someone might think of a better answer than NULL
                  }

               break;
             }
        }

  // This assertion does not make sense.
  // Since a trailing statement within a scope can have null next statement,
  // and  the statement can be not global scope statement, Liao 3/12/2009
  // ROSE_ASSERT (isSgGlobal(currentStatement) != NULL || nextStatement != NULL);

     return nextStatement;
   }

#define DEBUG_PREVIOUS_STATEMENT 0

static SgStatement*
getPreviousStatement_support_for_declaration_list ( SgScopeStatement* parent_scope, SgStatement *targetStatement , bool climbOutScope /*= true*/)
   {
  // This supports scopes that contain declaration statement lists (SgGlobal, SgClassDefinition, etc.)

     ROSE_ASSERT(parent_scope    != NULL);
     ROSE_ASSERT(targetStatement != NULL);

     ROSE_ASSERT(parent_scope->containsOnlyDeclarations() == true);

     SgStatement* previousStatement = NULL;

#if DEBUG_PREVIOUS_STATEMENT
     printf ("In getPreviousStatement_support_for_declaration_list(): targetStatement = %p = %s \n",targetStatement,targetStatement->class_name().c_str());
#endif

  // Usually a global scope or class declaration scope
     SgDeclarationStatementPtrList & declarationList = parent_scope->getDeclarationList();

     Rose_STL_Container<SgDeclarationStatement*>::iterator targetIterator = find(declarationList.begin(),declarationList.end(),targetStatement);

     ROSE_ASSERT(targetStatement == *targetIterator);

     ROSE_ASSERT(targetIterator != declarationList.end());

     if (targetIterator == declarationList.begin())
        {
          if (climbOutScope)
             {
               previousStatement = isSgStatement(targetStatement->get_parent());
               ROSE_ASSERT (previousStatement != NULL);
             }
        }
       else
        {
          Rose_STL_Container<SgDeclarationStatement*>::iterator previousStatementIterator = --targetIterator;
          previousStatement = *previousStatementIterator;

       // DQ (3/12/2024): This should always be true.
          ROSE_ASSERT(previousStatement != targetStatement);
        }

#if DEBUG_PREVIOUS_STATEMENT
     printf ("@@@@@ previousStatement = %p \n",previousStatement);
     if (previousStatement != NULL)
        {
          printf ("@@@@@ previousStatement = %p = %s \n",previousStatement,previousStatement->class_name().c_str());
       // printf ("@@@@@ previousStatement->unparseToString() = %s \n",previousStatement->unparseToString().c_str());
        }
#endif

     if (climbOutScope)
        {
          ROSE_ASSERT (isSgGlobal(targetStatement) != NULL || previousStatement != NULL);
        }

     return previousStatement;
   }

static SgStatement*
getPreviousStatement_support_for_statement_list ( SgScopeStatement* parent_scope, SgStatement *targetStatement , bool climbOutScope /*= true*/)
   {
  // This supports scopes that contain statement lists (SgBasicBlock, etc.)

     ROSE_ASSERT(parent_scope    != NULL);
     ROSE_ASSERT(targetStatement != NULL);

     ROSE_ASSERT(parent_scope->containsOnlyDeclarations() == false);

     SgStatement* previousStatement = NULL;

#if DEBUG_PREVIOUS_STATEMENT
     printf ("In getPreviousStatement_support_for_statement_list(): targetStatement = %p = %s \n",targetStatement,targetStatement->class_name().c_str());
#endif

  // Usually a SgBasicBlock scope
     SgStatementPtrList & statementList = parent_scope->getStatementList();

     Rose_STL_Container<SgStatement*>::iterator targetIterator = find(statementList.begin(),statementList.end(),targetStatement);

     ROSE_ASSERT(targetStatement == *targetIterator);

     ROSE_ASSERT(targetIterator != statementList.end());

     if (targetIterator == statementList.begin())
        {
          if (climbOutScope)
             {
               previousStatement = isSgStatement(targetStatement->get_parent());
               ROSE_ASSERT (previousStatement != NULL);
             }
        }
       else
        {
          Rose_STL_Container<SgStatement*>::iterator previousStatementIterator = --targetIterator;
          previousStatement = *previousStatementIterator;

       // DQ (3/12/2024): This should always be true.
          ROSE_ASSERT(previousStatement != targetStatement);
        }

#if DEBUG_PREVIOUS_STATEMENT
     printf ("@@@@@ previousStatement = %p \n",previousStatement);
     if (previousStatement != NULL)
        {
          printf ("@@@@@ previousStatement = %p = %s \n",previousStatement,previousStatement->class_name().c_str());
       // printf ("@@@@@ previousStatement->unparseToString() = %s \n",previousStatement->unparseToString().c_str());
        }
#endif

     if (climbOutScope)
        {
          ROSE_ASSERT (isSgGlobal(targetStatement) != NULL || previousStatement != NULL);
        }

     return previousStatement;
   }

SgStatement*
Rose::getPreviousStatement ( SgStatement *targetStatement , bool climbOutScope /*= true*/)
   {
  // Note that the option to specify climbOutScope is only used in Liao's arithmeticIntensity
  // tool (specifically in midend/programAnalysis/arithmeticIntensity/ai_measurement.cpp).

     ROSE_ASSERT (targetStatement  != NULL);

     SgStatement      *previousStatement = NULL;

  // DQ (3/15/2024): We don't need this variable now.
  // SgScopeStatement *scope             = targetStatement->get_scope();
  // ROSE_ASSERT (scope != NULL);

  // DQ (9/18/2010): If we try to get the previous statement from SgGlobal, then return NULL.
     if (isSgGlobal(targetStatement) != NULL)
        {
          return NULL;
        }
#if 0
  // DQ (3/15/2024): We don't need this test now.
  // Make sure that we didn't get ourselves back from the get_scope()
  // function (previous bug fixed, but tested here).
     if (scope == targetStatement)
        {
          printf ("Error: targetStatement = %p = %s \n",targetStatement,targetStatement->class_name().c_str());
        }
     ROSE_ASSERT (scope != targetStatement);
#endif

#if DEBUG_PREVIOUS_STATEMENT
     printf ("@@@@@ In Rose::getPreviousStatement(): targetStatement = %p = %s climbOutScope = %s \n",targetStatement,targetStatement->class_name().c_str(),climbOutScope ? "true" : "false");
  // printf ("@@@@@ In Rose::getPreviousStatement(): targetStatement = %s \n",targetStatement->class_name().c_str());
     printf ("@@@@@ In Rose::getPreviousStatement(): targetStatement->unparseToString() = %s \n",targetStatement->unparseToString().c_str());
     printf ("@@@@@ In Rose::getPreviousStatement(): scope = %s \n",scope->class_name().c_str());
  // printf ("@@@@@ In Rose::getPreviousStatement(): scope->unparseToString() = %s \n",scope->unparseToString().c_str());
#endif

     SgStatement* const parentStatement = isSgStatement(targetStatement->get_parent());
     ROSE_ASSERT (parentStatement != NULL);

    // Liao 5/10/2010, special case when a true/false body of a if statement is not a basic block
    // since getStatementList() is not defined for a if statement.
    // We define the previous statement of the true/false body to be the if statement
    // This is consistent with the later handling that when a statement is the first in a parent,
    // treat the parent as the previous statement
    // PP 5/22/2024, generalize for a number of scope statements with similar property

     const bool isSpecialScopeStatement = (  isSgIfStmt(parentStatement)
                                          || isSgWhileStmt(parentStatement)
                                          || isSgForStatement(parentStatement)
                                          || isSgDoWhileStmt(parentStatement)
                                          || isSgSwitchStatement(parentStatement)
                                          );

     if (isSpecialScopeStatement)
        {
          // the target statement is a child of a special statement
          //   => previousStatement = parentStatement
          //   unless climbOutScope is provided, in which case there is none.
          if (climbOutScope)
            previousStatement = parentStatement;
          else
            ASSERT_require(previousStatement == nullptr);
        }
     else if (SgScopeStatement* parent_scope = isSgScopeStatement(parentStatement))
        {
          if (parent_scope->containsOnlyDeclarations() == true)
             {
            // Examples of this case would be a SgGlobal, SgClassDefinition, and some other scopes.
               previousStatement = getPreviousStatement_support_for_declaration_list(parent_scope,targetStatement,climbOutScope);
             }
            else
             {
            // Examples of this case would be a SgBasicBlock, and some other scopes.
               ROSE_ASSERT(parent_scope->containsOnlyDeclarations() == false);
               previousStatement = getPreviousStatement_support_for_statement_list(parent_scope,targetStatement,climbOutScope);
             }
        }
       else
        {
       // DQ (3/15/2024): Case of a classDefinition with a parent that is a declaration (SgJovialTableStatement, for Jovial, but could be classDeclaration for C++).
          if (climbOutScope)
             {
               previousStatement = parentStatement;
               ROSE_ASSERT (previousStatement != NULL);
             }
        }

#if DEBUG_PREVIOUS_STATEMENT
     printf ("@@@@@ previousStatement = %p \n",previousStatement);
     if (previousStatement != NULL)
        {
          printf ("@@@@@ previousStatement = %p = %s \n",previousStatement,previousStatement->class_name().c_str());
        }
#endif

     if (climbOutScope)
        {
          ROSE_ASSERT (isSgGlobal(targetStatement) != NULL || previousStatement != NULL);
        }

     return previousStatement;
   }
