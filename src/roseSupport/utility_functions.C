
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "checkIsModifiedFlag.h"

#if ROSE_WITH_LIBHARU
#include "AstPDFGeneration.h"
#endif

#include "AstDOTGeneration.h"

#include "wholeAST_API.h"

#ifdef _MSC_VER
#include <direct.h>     // getcwd
#else
#include "plugin.h"  // dlopen() is not available on Windows
#endif

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

// DQ (10/11/2007): This is commented out to avoid use of this mechanism.
// #include <copy_unparser.h>

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// DQ (9/8/2017): Debugging ROSE_ASSERT. Call sighandler_t signal(int signum, sighandler_t handler);
#include<signal.h>

#include "AST_FILE_IO.h"
#include "merge.h"
// Note that this is required to define the Sg_File_Info_XXX symbols (need for file I/O)
#include "Cxx_GrammarMemoryPoolSupport.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace Rose;

// global variable for turning on and off internal debugging
int ROSE_DEBUG = 0;

#if 1
// ArrayAssignmentUsingTransformationGrammar* globalArrayAssignmentUsingTransformationGrammar = NULL;

// CW: here we should definitly find a better way
// to specify the cache parameters
// Removed unused variables (next two declarations) [Rasmussen 2019.01.29]
// const int roseTargetCacheSize     = 8192;
// const int roseTargetCacheLineSize = 32;
// cacheInfo roseTargetCacheInfo(roseTargetCacheSize,roseTargetCacheLineSize);

// What is this and who put it here?
// unsigned int *uint_global_dbug_ptr;

#endif

// DQ (8/10/2004): This was moved to the SgFile a long time ago and should not be used any more)
// bool Rose::verbose                 = false;
// DQ (8/11/2004): build a global state here
// int Rose::roseVerbose = 0;

// DQ (3/6/2017): Adding ROSE options data structure to support frontend and backend options (see header file for details).
Rose::Options Rose::global_options;

// DQ (3/6/2017): Adding ROSE options data structure to support frontend and backend options (see header file for details).
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
// DQ (10/28/2013): Put the token sequence map here, it is set and accessed via member functions on the SgSourceFile IR node.
// std::map<SgNode*,TokenStreamSequenceToNodeMapping*> Rose::tokenSubsequenceMap;
// std::set<int,std::map<SgNode*,TokenStreamSequenceToNodeMapping*> > Rose::tokenSubsequenceMapSet;
std::map<int,std::map<SgNode*,TokenStreamSequenceToNodeMapping*>* > Rose::tokenSubsequenceMapOfMaps;

// DQ (11/27/2013): Adding vector of nodes in the AST that defines the token unparsing AST frontier.
// std::vector<FrontierNode*> Rose::frontierNodes;
// std::map<SgStatement*,FrontierNode*> Rose::frontierNodes;
std::map<int,std::map<SgStatement*,FrontierNode*>*> Rose::frontierNodesMapOfMaps;

// DQ (11/27/2013): Adding adjacency information for the nodes in the token unparsing AST frontier.
// std::map<SgNode*,PreviousAndNextNodeData*> Rose::previousAndNextNodeMap;
std::map<int,std::map<SgNode*,PreviousAndNextNodeData*>*> Rose::previousAndNextNodeMapOfMaps;

// DQ (11/29/2013): Added to support access to multi-map of redundant mapping of frontier IR nodes to token subsequences.
// std::multimap<int,SgStatement*> Rose::redundantlyMappedTokensToStatementMultimap;
// std::set<int> Rose::redundantTokenEndingsSet;
std::map<int,std::multimap<int,SgStatement*>*> Rose::redundantlyMappedTokensToStatementMapOfMultimaps;
std::map<int,std::set<int>*> Rose::redundantTokenEndingsMapOfSets;

// DQ (11/20/2015): Provide a statement to use as a key in the token sequence map to get representative whitespace.
// std::map<SgScopeStatement*,SgStatement*> Rose::representativeWhitespaceStatementMap;
std::map<int,std::map<SgScopeStatement*,SgStatement*>*> Rose::representativeWhitespaceStatementMapOfMaps;

// DQ (11/30/2015): Provide a statement to use as a key in the macro expansion map to get info about macro expansions.
// std::map<SgStatement*,MacroExpansion*> Rose::macroExpansionMap;
std::map<int,std::map<SgStatement*,MacroExpansion*>*> Rose::macroExpansionMapOfMaps;

// DQ (10/29/2018): Build a map for the unparser to use to locate SgIncludeFile IR nodes.
std::map<std::string, SgIncludeFile*> Rose::includeFileMapForUnparsing;


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
       // printf ("In Rose::initDiagnostics(): Calling Sawyer::Message::Facility() \n");
          ir_node_mlog = Sawyer::Message::Facility("rose_ir_node", Rose::Diagnostics::destination);
          ir_node_mlog.comment("operating on ROSE internal representation nodes");
          Rose::Diagnostics::mfacilities.insertAndAdjust(ir_node_mlog);
       // printf ("In Rose::initDiagnostics(): DONE Calling Sawyer::Message::Facility() \n");
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

static std::string
readlineVersionString() {
#ifdef ROSE_HAVE_LIBREADLINE
    return StringUtility::numberToString(RL_VERSION_MAJOR) + "." + StringUtility::numberToString(RL_VERSION_MINOR);
#else
    return "unknown (readline is disabled)";
#endif
}

static std::string
libmagicVersionString() {
#ifdef ROSE_HAVE_LIBMAGIC
#ifdef MAGIC_VERSION
    return StringUtility::numberToString(MAGIC_VERSION);
#else
    return "unknown (but enabled)";
#endif
#else
    return "unknown (libmagic is disabled)";
#endif
}

static std::string
yamlcppVersionString() {
#ifdef ROSE_HAVE_LIBYAML
    return "unknown (but enabled)";                     // not sure how to get a version number for this library
#else
    return "unknown (yaml-cpp is disabled)";
#endif
}

static std::string
yicesVersionString() {
#ifdef ROSE_HAVE_LIBYICES
    if (const char *s = yices_version())
        return s;
    return "unknown (but enabled)";
#else
    return "unknown (libyices is disabled)";
#endif
}

// similar to rose_boost_version_id but intended for human consumption (i.e., "1.50.0" rather than 105000).
static std::string
boostVersionString() {
    return (StringUtility::numberToString(BOOST_VERSION / 100000) + "." +
            StringUtility::numberToString(BOOST_VERSION / 100 % 1000) + "." +
            StringUtility::numberToString(BOOST_VERSION % 100));
}

// DQ (11/1/2009): replaced "version()" with separate "version_number()" and "version_message()" functions.
std::string version_message()
   {
     extern string edgVersionString();
     extern string ofpVersionString();

  // DQ (7/3/2013): Added output of pre-defined macros.  This output should actually be limited to use 
  // with -h and --version, similar to GNU compilers, as I recall.
  // outputPredefinedMacros();

#if 0
  // DQ (12/13/2016): Adding backend compiler and version info.
     printf ("Using backend C++ compiler (without path): %s version: %d.%d \n",BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH,BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER,BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER);
     printf ("Using backend C/C++ compiler (with path):  %s \n",BACKEND_CXX_COMPILER_NAME_WITH_PATH);
     printf ("Using backend  C  compiler (with path):    %s version: %d.%d \n",BACKEND_C_COMPILER_NAME_WITH_PATH,BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER,BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER);

     printf ("ROSE_COMPILE_TREE_PATH = %s \n",ROSE_COMPILE_TREE_PATH);
     printf ("ROSE_INSTALLATION_PATH = %s \n",ROSE_INSTALLATION_PATH);
#endif

     string backend_Cxx_compiler_without_path = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
     string backend_Cxx_compiler_with_path    = BACKEND_CXX_COMPILER_NAME_WITH_PATH;
     string backend_C_compiler_without_path   = BACKEND_C_COMPILER_NAME_WITH_PATH;
     string backend_C_compiler_with_path      = BACKEND_C_COMPILER_NAME_WITH_PATH;
     string backend_Cxx_compiler_version      = StringUtility::numberToString(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER) + "." + StringUtility::numberToString(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER);

#ifdef USE_CMAKE
     string build_tree_path                   = "CMake does not set: ROSE_COMPILE_TREE_PATH";
     string install_path                      = "CMake does not set: ROSE_INSTALLATION_PATH";
#else
     string build_tree_path                   = ROSE_COMPILE_TREE_PATH;
     string install_path                      = ROSE_INSTALLATION_PATH;
#endif

     return
       // "ROSE (pre-release beta version: " + version_number() + ")" +
          "ROSE (version: " + version_number() + ")" +
          "\n  --- using EDG C/C++ front-end version: " + edgVersionString() +
          "\n  --- using OFP Fortran parser version: " + ofpVersionString() +
          "\n  --- using Boost version: " + boostVersionString() + " (" + rose_boost_version_path() + ")" +
          "\n  --- using backend C compiler: " + backend_C_compiler_without_path + " version: " + backend_Cxx_compiler_version +
          "\n  --- using backend C compiler path (as specified at configure time): " + backend_C_compiler_with_path +
          "\n  --- using backend C++ compiler: " + backend_Cxx_compiler_without_path + " version: " + backend_Cxx_compiler_version +
          "\n  --- using backend C++ compiler path (as specified at configure time): " + backend_Cxx_compiler_with_path +
          "\n  --- using original build tree path: " + build_tree_path +
          "\n  --- using instalation path: " + install_path +
          "\n  --- using GNU readline version: " + readlineVersionString() +
          "\n  --- using libmagic version: " + libmagicVersionString() +
          "\n  --- using yaml-cpp version: " + yamlcppVersionString() +
          "\n  --- using lib-yices version: " + yicesVersionString();
  }

// DQ (11/1/2009): replaced "version()" with separate "version_number()" and "version_message()" functions.
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

namespace Rose {
namespace AST {
namespace IO {

typedef std::map<int, std::string> f2n_t;
typedef std::map<std::string, int> n2f_t;

void mergeFileIDs(f2n_t const & f2n, n2f_t const & n2f, f2n_t & gf2n, n2f_t & gn2f, size_t start_node) {
  std::map<int, int> idxmap;

  for (f2n_t::const_iterator i = f2n.begin(); i != f2n.end(); i++) {
    if (gn2f.count(i->second) == 0) {
      int idx = (int)gn2f.size();

      gn2f[i->second] = idx;
      gf2n[idx] = i->second;
      idxmap[i->first] = idx;
    }
  }

  unsigned num_nodes = Sg_File_Info::numberOfNodes();
  for (unsigned long i = start_node; i < num_nodes; i++) {
    // Compute the postion of the indexed Sg_File_Info object in the memory pool.
    unsigned long positionInPool = i % Sg_File_Info_CLASS_ALLOCATION_POOL_SIZE ;
    unsigned long memoryBlock    = (i - positionInPool) / Sg_File_Info_CLASS_ALLOCATION_POOL_SIZE;

    Sg_File_Info * fileInfo = &(((Sg_File_Info*)(Sg_File_Info_Memory_Block_List[memoryBlock]))[positionInPool]);
    ROSE_ASSERT(fileInfo != NULL);

    int oldFileId = fileInfo->get_file_id();
    int newFileId = idxmap[oldFileId];

    if (oldFileId >= 0 && oldFileId != newFileId) {
      fileInfo->set_file_id(newFileId);
    }
  }
}

void mergeSymbolTable(SgSymbolTable * gst, SgSymbolTable * st) {
  SgSymbolTable::BaseHashType* iht = st->get_table();
  ROSE_ASSERT(iht != NULL);

  SgSymbolTable::hash_iterator i = iht->begin();
  while (i != iht->end()) {
    SgSymbol * symbol = isSgSymbol((*i).second);
    ROSE_ASSERT(symbol != NULL);

    if (!gst->exists(i->first)) {
      // This function in the local function type table is not in the global function type table, so add it.
      gst->insert(i->first,i->second);
    } else {
      // These are redundant symbols, but likely something in the AST points to them so be careful.
      // This function type is already in the global function type table, so there is nothing to do (later we can delete it to save space)
    }
    i++;
  }
}

void mergeTypeSymbolTable(SgTypeTable * gtt, SgTypeTable * tt) {
  SgSymbolTable * st  = tt->get_type_table();
  SgSymbolTable * gst = gtt->get_type_table();

  mergeSymbolTable(gst, st);
}

void mergeFunctionTypeSymbolTable(SgFunctionTypeTable * gftt, SgFunctionTypeTable * ftt) {
  SgSymbolTable * fst  = ftt->get_function_type_table();
  SgSymbolTable * gfst = gftt->get_function_type_table();

  mergeSymbolTable(gfst, fst);
}

void append(SgProject * project, std::list<std::string> const & astfiles) {
  size_t num_nodes = Sg_File_Info::numberOfNodes();

  AST_FILE_IO::startUp(project);
  AST_FILE_IO::resetValidAstAfterWriting();

  // Save shared (static) fields, TODO:
  //   - global scope accross project
  //   - name mangling caches?
  SgTypeTable *         gtt = SgNode::get_globalTypeTable();
  SgFunctionTypeTable * gftt = SgNode::get_globalFunctionTypeTable();
  f2n_t gf2n = Sg_File_Info::get_fileidtoname_map();
  n2f_t gn2f = Sg_File_Info::get_nametofileid_map();

//printf("project = %p\n", project);
//printf("gtt     = %p\n", gtt);
//printf("gftt    = %p\n", gftt);

//generateWholeGraphOfAST("init", NULL);

  std::list<std::string>::const_iterator astfile = astfiles.begin();
  size_t cnt = 1;
  while (astfile != astfiles.end()) {
    // Note the postfix increment in the following two lines
    std::string astfile_ = *(astfile++);

    AST_FILE_IO::readASTFromFile(astfile_);
    AstData * ast = AST_FILE_IO::getAst(cnt++);

    // Check that the root of the read AST is valid
    SgProject * lproject = ast->getRootOfAst();
//  printf("lproject = %p\n", lproject);
    ROSE_ASSERT(lproject->get_freepointer() == AST_FileIO::IS_VALID_POINTER());

    // Insert all files into main project
    std::vector<SgFile *> const & files = lproject->get_files();
    for (std::vector<SgFile *>::const_iterator it = files.begin(); it != files.end(); ++it) {
      project->get_fileList().push_back(*it);
      (*it)->set_parent(project->get_fileList_ptr());
    }
    lproject->get_fileList_ptr()->get_listOfFiles().clear();

    // Load shared (static) fields from the AST being read
    AST_FILE_IO::setStaticDataOfAst(ast);

    // Merge static fields

    SgTypeTable *         lgtt = SgNode::get_globalTypeTable();
//  printf("lgtt     = %p\n", lgtt);
    mergeTypeSymbolTable(gtt, lgtt);

    SgFunctionTypeTable * lgftt = SgNode::get_globalFunctionTypeTable();
//  printf("lgftt    = %p\n", lgftt);
    mergeFunctionTypeSymbolTable(gftt, lgftt);

    mergeFileIDs(Sg_File_Info::get_fileidtoname_map(), Sg_File_Info::get_nametofileid_map(), gf2n, gn2f, num_nodes);

    // Restore shared (static) fields

    SgNode::set_globalTypeTable(gtt);
    SgNode::set_globalFunctionTypeTable(gftt);
    Sg_File_Info::set_fileidtoname_map(gf2n);
    Sg_File_Info::set_nametofileid_map(gn2f);

    num_nodes = Sg_File_Info::numberOfNodes();

//  generateWholeGraphOfAST(astfile_, NULL);
  }

//generateWholeGraphOfAST("loaded", NULL);

  mergeAST(project, /* skipFrontendSpecificIRnodes = */false);

  AST_FILE_IO::reset();

//generateWholeGraphOfAST("merged", NULL);
}

} // IO
} // AST
} // ROSE

/*! \brief Call to frontend, processes commandline and generates a SgProject object.

    This function represents a simple interface to the use of ROSE as a library.
    The commandline is processed and the return parameter is the generate SgProject object.
 */

// #include "sageCommonSourceHeader.h"
// extern an_il_header il_header;
//
SgProject*
frontend (int argc, char** argv, bool frontendConstantFolding )
   {
  // printf ("In frontend(int argc,char** argv): frontendConstantFolding = %s \n",frontendConstantFolding == true ? "true" : "false");

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

  // DQ (6/14/2007): Added support for timing of high level frontend function.
     TimingPerformance timer ("ROSE frontend():");

  // Syncs C++ and C I/O subsystems!
     ios::sync_with_stdio();

  // make sure that there is some sort of commandline (at least a file specified)
     if (argv.size() == 1)
        {
       // Rose::usage(1);      // Print usage and exit with exit status == 1
          SgFile::usage(1);      // Print usage and exit with exit status == 1
        }

  // printf ("In frontend(const std::vector<std::string>& argv): frontendConstantFolding = %s \n",frontendConstantFolding == true ? "true" : "false");

  // We parse plugin related command line options before calling project();
     std::vector<std::string> argv2= argv;  // workaround const argv
#ifdef _MSC_VER
    if ( SgProject::get_verbose() >= 1 )
        printf ("Note: Dynamic Loadable Plugins are not supported on Microsoft Windows yet. Skipping Rose::processPluginCommandLine () ...\n");
#else
     Rose::processPluginCommandLine(argv2);
#endif

  // Error code checks and reporting are done in SgProject constructor
  // return new SgProject (argc,argv);
     SgProject* project = new SgProject (argv2,frontendConstantFolding);
     ROSE_ASSERT (project != NULL);

  // DQ (9/6/2005): I have abandoned this form or prelinking (AT&T C Front style).
  // To be honest I find this level of technology within ROSE to be embarassing...
  // We not handle prelinking by generating all required template instantiations
  // as static functions.  A more global based prelinker will be built at some
  // point and will likely utilize the SGLite database or some other auxiliary file
  // mechansism.
  // DQ (3/31/2004): If there are templates used then we need to modify the *.ti file build by EDG.
  // buildTemplateInstantiationSupportFile ( project );

  // DQ (4/16/2015): This is replaced with a better implementation.
  // Make sure the isModified boolean is clear for all newly-parsed nodes.
  // checkIsModifiedFlag(project);

  // DQ (1/27/2017): Comment this out so that we can generate the dot graph to debug symbol with null basis.
     unsetNodesMarkedAsModified(project);
  // printf ("ERROR: In frontend(const std::vector<std::string>& argv): commented out unsetNodesMarkedAsModified() \n");

     std::list<std::string> const & astfiles = project->get_astfiles_in();
     if (astfiles.size() > 0) {
       Rose::AST::IO::append(project, astfiles);
     }
   
  // Set the mode to be transformation, mostly for Fortran. Liao 8/1/2013
  // Removed semicolon at end of if conditional to allow it to have a body [Rasmussen 2019.01.29]
     if (SageBuilder::SourcePositionClassificationMode == SageBuilder::e_sourcePositionFrontendConstruction)
       SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionTransformation);

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
frontendShell (const std::vector<std::string>& argv)
   {
  // Convert this to a list of strings to simplify editing (adding new option)
     Rose_STL_Container<string> commandLineList = argv;
     printf ("frontendShell (top): argv = \n%s \n",StringUtility::listToString(commandLineList).c_str());

  // Invoke ROSE commandline option to skip internal frontend processing (we will 
  // call the fronend explicitly for selected files, after construction of SgProject).
     commandLineList.push_back("-rose:skip_rose");

  // Build the SgProject, but if the above option was used this will only build empty SgFile nodes 
     SgProject* project = frontend(commandLineList);
     ROSE_ASSERT(project != NULL);

     project->display("In frontendShell(), after frontend()");

     SgFilePtrList::const_iterator i = project->get_fileList().begin();
     while (i != project->get_fileList().end())
        {
       // Get the local command line so that we can remove the "-rose:skip_rose" option
          vector<string> local_argv = (*i)->get_originalCommandLineArgumentList();

       // Note that we have to remove the "-rose:skip_rose" option that was saved
          CommandlineProcessing::removeArgs (local_argv,"-rose:skip_rose");
       // printf ("Remove -rose:skip_rose: argv = \n%s \n",StringUtility::listToString(CommandlineProcessing::generateArgListFromArgcArgv (local_argc,local_argv)).c_str());

          printf ("frontendShell (after): argv = \n%s \n",StringUtility::listToString(commandLineList).c_str());

       // Set the new commandline (without the "-rose:skip_rose" option)
          (*i)->set_originalCommandLineArgumentList(local_argv);

       // Things set by "-rose:skip_rose" option, which must be unset (reset to default valees)!
          (*i)->set_skip_transformation(false);
          (*i)->set_disable_edg_backend(false);

       // Leave this set to true so that the frontend can set it if the frontend is called for this SgFile
       // (*i)->set_skip_unparse(false);

          (*i)->set_useBackendOnly(false);
          (*i)->set_skipfinalCompileStep(false);

       // Skip all processing of comments
          (*i)->set_skip_commentsAndDirectives(false);
       // (*i)->set_collectAllCommentsAndDirectives(false);

       // file->display("After Remove -rose:skip_rose");

       // increment the file list iterator
          i++;
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
backend ( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelagate )
   {
  // DQ (7/12/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Object Code Generation (backend):");

     int finalCombinedExitStatus = 0;

#if 0
     printf ("Inside of backend(SgProject*) (from utility_functions.C) \n");
#endif

     if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
        {
          printf ("Inside of backend(SgProject*) \n");
        }

#ifdef ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
  // DQ (9/8/2017): Debugging ROSE_ASSERT. Call sighandler_t signal(int signum, sighandler_t handler);
  // signal(SIG_DFL,NULL);
     signal(SIGABRT,SIG_DFL);
#endif

     std::string const & astfile_out = project->get_astfile_out();
     if (astfile_out != "") {
       std::list<std::string> empty_file_list;
       project->set_astfiles_in(empty_file_list);
       project->get_astfile_out() == "";
       AST_FILE_IO::reset();
       AST_FILE_IO::startUp(project);
       AST_FILE_IO::writeASTToFile(astfile_out);
       AST_FILE_IO::resetValidAstAfterWriting();
     }

#if 0
  // DQ (9/8/2017): Debugging ROSE_ASSERT.
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     if (project->get_binary_only() == true)
        {
          ROSE_ASSERT(project != NULL);

       // DQ (8/21/2008): Only output a message when we we use verbose option.
          if ( SgProject::get_verbose() >= 1 )
               printf ("Note: Binary executables are unparsed, but not passed to gcc as assembly source code \n");

          project->skipfinalCompileStep(true);
        }

  // printf ("   project->get_useBackendOnly() = %s \n",project->get_useBackendOnly() ? "true" : "false");
     if (project->get_useBackendOnly() == false)
        {
       // Add forward references for instantiated template functions and member functions 
       // (which are by default defined at the bottom of the file (but should be declared 
       // at the top once we know what instantiations should be built)).  They must be 
       // defined at the bottom since they could call other functions not yet declared in 
       // the file.  Note that this fixup is required since we have skipped the class template 
       // definitions which would contain the declarations that we are generating.  We might 
       // need that as a solution at some point if this fails to be sufficently robust.
       // if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL-2 )
       //      printf ("Calling fixupInstantiatedTemplates() \n");
       // DQ (9/6/2005): I think this is handled separately within post processing
       // (either that or they are just marked for output n the post processing)
       // fixupInstantiatedTemplates(project);

       // generate C++ source code
          if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
               printf ("Calling project->unparse() \n");

#if 0
          printf ("Calling project->unparse() \n");
#endif

          project->unparse(unparseFormatHelp,unparseDelagate);

#if 0
          printf ("DONE: Calling project->unparse() \n");
#endif

          if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
               cout << "source file(s) generated. (from AST)" << endl;
        }

#if 0
     printf ("Inside of backend(SgProject*): SgProject::get_verbose()       = %d \n",SgProject::get_verbose());
     printf ("Inside of backend(SgProject*): project->numberOfFiles()       = %d \n",project->numberOfFiles());
     printf ("Inside of backend(SgProject*): project->numberOfDirectories() = %d \n",project->numberOfDirectories());
#endif

  // DQ (1/25/2010): We have to now test for both numberOfFiles() and numberOfDirectories(),
  // or perhaps define a more simple function to use more directly.
  // if (project->numberOfFiles() > 0)
     if (project->numberOfFiles() > 0 || project->numberOfDirectories() > 0)
        {
       // Compile generated C++ source code with vendor compiler.
       // Generate object file (required for further template processing 
       // if templates exist).
          if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
               printf ("Calling project->compileOutput() \n");

          finalCombinedExitStatus = project->compileOutput();
        }
       else
  // if (project->get_compileOnly() == false)
        {
          if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
               printf ("   project->get_compileOnly() = %s \n",project->get_compileOnly() ? "true" : "false");

       // DQ (5/20/2005): If we have not permitted templates to be instantiated during initial 
       // compilation then we have to do the prelink step (this is however still new and somewhat 
       // problematic (buggy?)).  It relies upon the EDG mechansisms which are not well understood.
          bool callTemplateInstantation = (project->get_template_instantiation_mode() == SgProject::e_none);

          if (callTemplateInstantation == true)
             {
            // DQ (9/6/2005): I think that this is no longer needed
               printf ("I don't think we need to call instantiateTemplates() any more! \n");
               ROSE_ASSERT(false);

            // The instantiation of templates can cause new projects (sets of source files) 
            // to be generated, but since the object files are already processed this is 
            // not an issue here.  A project might, additionally, keep track of the ASTs
            // associated with the different phases of instantions of templates.
               if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
                    printf ("Calling instantiateTemplates() \n");

               printf ("Skipping template support in backend(SgProject*) \n");
            // instantiateTemplates (project);
             }

          if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
               printf ("Calling project->link() \n");

       // DQ (10/15/2005): Trap out case of C programs where we want to make sure that we don't use the C++ compiler to do our linking!
       // This could be done in the 
          if (project->get_C_only() == true)
             {
               printf ("Link using the C language linker (when handling C programs) = %s \n",BACKEND_C_COMPILER_NAME_WITH_PATH);
            // finalCombinedExitStatus = project->link("gcc");
               finalCombinedExitStatus = project->link(BACKEND_C_COMPILER_NAME_WITH_PATH);
             }
            else
             {
            // Use the default name for C++ compiler (defined at configure time)
               if ( SgProject::get_verbose() >= BACKEND_VERBOSE_LEVEL )
                    printf ("Link using the default linker (when handling non-C programs) = %s \n",BACKEND_CXX_COMPILER_NAME_WITH_PATH);
               finalCombinedExitStatus = project->link(BACKEND_CXX_COMPILER_NAME_WITH_PATH);
             }

       // printf ("DONE with link! \n");
        }

  // Message from backend to user.
  // Avoid all I/O to stdout if useBackendOnly == true.
  // if (project->get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= 1 )
          cout << "source file(s) compiled with vendor compiler. (exit status = " << finalCombinedExitStatus << ").\n" << endl;

  // Set the final error code to be returned to the user.
     project->set_backendErrorCode(finalCombinedExitStatus);

#if 0
     printf ("Leaving backend(SgProject*) (from utility_functions.C) \n");
#endif

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

  // ROSE_ASSERT(language != e_none);
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
               ROSE_ASSERT(false);
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
       // finalCombinedExitStatus = system (commandLineToGenerateObjectFile.c_str());
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
            ROSE_ASSERT(commandLineToGenerateObjectFile.size() == 1);
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
copy_backend( SgProject* project, UnparseFormatHelp *unparseFormatHelp )
   {
  // This is part of the copy-based unparser (from Qing).
  // This function calls the unparseFile function with a 
  // CopyUnparser object (derived from UnparseDelegate)
  // to control the unparsing and substitute text based 
  // copying for code generation from the AST.

  // This function does not presently have the same semantics as the "backend()".
  // The code above could be refactored to permit both backend function to more
  // readily have the same semantics later.

#if 0
  // DQ (10/11/2007): This mechanism has been replaced by a token based mechanism to support exact code generation.

     for (int i=0; i < project->numberOfFiles(); ++i)
        {
          SgFile & file = project->get_file(i);

       // Build this on the stack for each SgFile
          CopyUnparser repl(file);

       // DQ (3/18/2006): Modified to handle formating options
          unparseFile(&file,unparseFormatHelp,&repl);
        }
#else
     printf ("Error: Inside of copy_backend(), the copy backend has been disabled in favor of a token based mechanism for unparsing. \n");
     ROSE_ASSERT(false);
#endif

  // DQ (5/19/2005): I had to make up a return value since one was not previously specified
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
#if ROSE_WITH_LIBHARU
     AstPDFGeneration pdftest;
     SgProject & nonconstProject = (SgProject &) project;
     pdftest.generateInputFiles(&nonconstProject);
#else
     printf("Warning: libharu support is not enabled\n");
#endif

#endif
   }

void
generateDOT ( const SgProject & project, std::string filenamePostfix, bool excludeTemplateInstantiations )
   {
  // DQ (7/4/2008): Added default parameter to support the filenamePostfix 
  // mechanism in AstDOTGeneration

  // DQ (6/14/2007): Added support for timing of the generateDOT() function.
     TimingPerformance timer ("ROSE generateDOT():");

     AstDOTGeneration astdotgen;
     SgProject & nonconstProject = (SgProject &) project;

  // DQ (12/14/2018): The number of nodes is computed globally, but the graph is genereated only for the input file.
  // So this can suppress the generation of the graph when there are a large number of IR nodes from header files.
  // Multiplied the previous value by 10 to support building the smaller graph of the input file.
  // DQ (2/18/2013): Generating a DOT file of over a million IR nodes is too much.
  // int maxSize = 1000000;
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
#if 0
       // This used to be the default, but it would output too much data (from include files).
          astdotgen.generate(&nonconstProject);
#else
       // DQ (9/1/2008): This is the default for the last long while, but the SgProject IR nodes 
       // is not being processed (which appears to be a bug). This is because in the implementation
       // of the generateInputFiles the function traverseInputFiles is called.
       // astdotgen.generateInputFiles(&nonconstProject,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix);
          astdotgen.generateInputFiles(&nonconstProject,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix,excludeTemplateInstantiations);
#endif
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

  // DQ (6/14/2007): Added support for timing of the generateDOT() function.
     TimingPerformance timer ("ROSE generateDOT():");

     AstDOTGeneration astdotgen;

  // This used to be the default, but it would output too much data (from include files).
  // std::string filenamePostfix = ".dot";
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
#if 1
  // This used to be the default, but it would output too much data (from include files).
  // It is particularly useful when handling multiple files on the command line and 
  // traversing the files included from each file.
  // astdotgen.generate(&nonconstProject);
  // DOTGeneration::traversalType tt = TOPDOWNBOTTOMUP;
     AstDOTGeneration::traversalType tt = AstDOTGeneration::TOPDOWNBOTTOMUP;
     astdotgen.generate(&nonconstProject,tt,filenamePostfix);
#else
  // DQ (9/1/2008): This is the default for the last long while, but the SgProject IR nodes 
  // is not being processed (which appears to be a bug). This is because in the implementation
  // of the generateInputFiles the function traverseInputFiles is called.
     astdotgen.generateInputFiles(&nonconstProject,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix);
#endif
   }

void
generateDOTforMultipleFile ( const SgProject & project, std::string filenamePostfix )
   {
     TimingPerformance timer ("ROSE generateDOTforMultipleFile():");

  // This is the best way to handle generation of DOT files where multiple files
  // are specified on the command line.  Later we may be able to filter out the
  // include files (but this is a bit difficult until generateInputFiles() can be
  // implemetned to call the evaluation of inherited and synthesized attributes.
     generateDOT_withIncludes(project,filenamePostfix);
   }

void generateAstGraph ( const SgProject* project, int maxSize, std::string filenameSuffix )
// void generateAstGraph ( const SgProject* project, int maxSize, std::string filenameSuffix, CustomMemoryPoolDOTGeneration::s_Filter_Flags* filter_flags)
   {
  // DQ (6/14/2007): Added support for timing of the generateAstGraph() function.
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
       // generateWholeGraphOfAST(filename);

       // Added support to handle options to control filtering of Whole AST graphs.
       // std::vector<std::string>  argvList (argv, argv+ argc);
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
     ROSE_ASSERT (locatedNodePointer != NULL);
     int lineNumber = -1;
  // Sometimes the locatedNode doesn't have a SgFile object 
  // (likely because it is part of a parent statement object)
     if (locatedNodePointer->get_file_info() != NULL)
        {
          ROSE_ASSERT (locatedNodePointer->get_file_info() != NULL);
          ROSE_ASSERT (locatedNodePointer->get_file_info()->get_filename() != NULL);
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
     ROSE_ASSERT (locatedNodePointer != NULL);
     int columnNumber = -1;
  // Sometimes the locatedNode doesn't have a SgFile object 
  // (likely because it is part of a parent statement object)
     if (locatedNodePointer->get_file_info() != NULL)
        {
          ROSE_ASSERT (locatedNodePointer->get_file_info() != NULL);
          ROSE_ASSERT (locatedNodePointer->get_file_info()->get_filename() != NULL);
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
     ROSE_ASSERT (locatedNodePointer != NULL);
     std::string fileName = "NO NAME FILE";
  // Sometimes the locatedNode doesn't have a SgFile object 
  // (likely because it is part of a parent statement object)
     if (locatedNodePointer->get_file_info() != NULL)
        {
          ROSE_ASSERT (locatedNodePointer->get_file_info() != NULL);
       // printf ("In Rose::getFileName(): locatedNodePointer->get_file_info() = %p \n",locatedNodePointer->get_file_info());
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
     ROSE_ASSERT (statementPointer != NULL);
     ROSE_ASSERT (statementPointer->get_file_info() != NULL);

  // char* fileName = getFileName(statementPointer);
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
  // DQ (9/5/2006): Increase the buffer size
  // const int maxPathNameLength = 1024;
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
     currentDirectory = NULL;
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

     ROSE_ASSERT (astNode != NULL);

  // Make sure this is not a project node (since the SgFile exists below 
  // the project and could not be found by a traversal of the parent list)
     if (isSgProject(astNode) == NULL)
        {
          const SgNode* parent = astNode;
          while ( (parent != NULL) && (isSgFile(parent) == NULL) )
             {
            // printf ("In getFileNameByTraversalBackToFileNode(): parent = %p = %s \n",parent,parent->class_name().c_str());
               parent = parent->get_parent();
             }

          if (!parent) {
            const SgLocatedNode* ln = isSgLocatedNode(astNode);
            ROSE_ASSERT (ln);
            return ln->get_file_info()->get_filenameString();
          }
          // ROSE_ASSERT (parent != NULL);
          const SgFile* file = isSgFile(parent);
          ROSE_ASSERT (file != NULL);
          if (file != NULL)
             {
            // returnString = Rose::getFileName(file);
               returnString = file->getFileName();
             }

       // ROSE_ASSERT (returnString.length() > 0);
          ROSE_ASSERT (returnString.empty() == false);
        }

     return returnString;
   }
#endif

void
Rose::usage (int status)
   {
     SgFile::usage(status);
  // exit(status);
   }

int 
Rose::containsString ( const std::string& masterString, const std::string& targetString )
   {
     return masterString.find(targetString) != string::npos;
   }

void
Rose::filterInputFile ( const string inputFileName, const string outputFileName )
   {
  // This function filters the input file to remove ^M characters and expand tabs etc.
  // Any possible processing of the input file, before being compiled, should be done
  // by this function.

  // This function is implemented in the ROSE/dqDevelopmentDirectory directory.
   }

SgStatement*
Rose::getNextStatement ( SgStatement *currentStatement )
   {
     ROSE_ASSERT (currentStatement  != NULL);
  // CI (1/3/2007): This used to be not implemented ,,, here is my try
  //! get next statement will return the next statement in a function or method. if at the end or outside, it WILL return NULL
     
     SgStatement      *nextStatement = NULL;
     SgScopeStatement *scope         = currentStatement->get_scope();
     ROSE_ASSERT (scope != NULL);

  // DQ (9/18/2010): If we try to get the next statement from SgGlobal, then return NULL.
     if (isSgGlobal(currentStatement) != NULL)
          return NULL;

  // Make sure that we didn't get ourselves back from the get_scope() 
  // function (previous bug fixed, but tested here).
     ROSE_ASSERT (scope != currentStatement);

#if 0
     printf ("In ROSE::getNextStatement(): currentStatement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
     printf ("   --- scope = %p = %s \n",scope,scope->class_name().c_str());
#endif

     switch (currentStatement->variantT())
        {
          case V_SgForInitStatement:
          // case V_SgBasicBlock: // Liao 10/20/2010, We should allow users to get a statement immediately AFTER a block.
          case V_SgClassDefinition:
          case V_SgFunctionDefinition:
          case V_SgStatement:
          case V_SgFunctionParameterList:
             {
               ROSE_ASSERT(false);
            // not specified
               break;
             }

       // DQ (11/8/2015): Added support for SgLabelStatement (see testcode tests/nonsmoke/functional/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_134.C)
          case V_SgLabelStatement:
            {
              SgLabelStatement* lableStatement = isSgLabelStatement(currentStatement);
              nextStatement = lableStatement->get_statement();
              ROSE_ASSERT(nextStatement != NULL);
#if 1
              printf ("In getNextStatement(): case V_SgLabelStatement: nextStatement = %p = %s \n",nextStatement,nextStatement->class_name().c_str());
#endif
              break;
            }

          default:
             {
            // We have to handle the cases of a SgStatementPtrList and a 
            // SgDeclarationStatementPtrList separately
               if (scope->containsOnlyDeclarations() == true)
                  {
                 // Usually a global scope or class declaration scope
                    SgDeclarationStatementPtrList & declarationList = scope->getDeclarationList();
                    Rose_STL_Container<SgDeclarationStatement*>::iterator i;
                 // for (i = declarationList.begin(); (*i) != currentStatement; i++) {}
                    for (i = declarationList.begin(); (i != declarationList.end() && (*i) != currentStatement); i++) {}
                 // now i == currentStatement

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
                 // for (i = statementList.begin();(*i)!=currentStatement;i++) 
                    for (i = statementList.begin(); (*i) != currentStatement && i != statementList.end(); i++) 
                       {
                      //  SgStatement* cur_stmt = *i;
                      //  cout<<"Skipping current statement: "<<cur_stmt->class_name()<<endl;
                      //  cout<<cur_stmt->get_file_info()->displayString()<<endl;
                       }

                 // currentStatement is not found in the list
                    if (i ==  statementList.end()) 
                       {
                         cerr<<"fatal error: ROSE::getNextStatement(): current statement is not found within its scope's statement list"<<endl;
                         cerr<<"current statement is "<<currentStatement->class_name()<<endl;
                         cerr<<currentStatement->get_file_info()->displayString()<<endl;
                         cerr<<"Its scope is "<<scope->class_name()<<endl;
                         cerr<<scope->get_file_info()->displayString()<<endl;
                         ROSE_ASSERT (false);
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

         
SgStatement*
Rose::getPreviousStatement ( SgStatement *targetStatement , bool climbOutScope /*= true*/)
   {
     ROSE_ASSERT (targetStatement  != NULL);

     SgStatement      *previousStatement = NULL;
     SgScopeStatement *scope             = targetStatement->get_scope();
     ROSE_ASSERT (scope != NULL);

  // DQ (9/18/2010): If we try to get the previous statement from SgGlobal, then return NULL.
     if (isSgGlobal(targetStatement) != NULL)
          return NULL;

  // Make sure that we didn't get ourselves back from the get_scope() 
  // function (previous bug fixed, but tested here).
     if (scope == targetStatement)
        {
          printf ("Error: targetStatement = %p = %s \n",targetStatement,targetStatement->class_name().c_str());
        }
     ROSE_ASSERT (scope != targetStatement);

#if 0
     printf ("@@@@@ In Rose::getPreviousStatement(): targetStatement = %s \n",targetStatement->sage_class_name());
     printf ("@@@@@ In Rose::getPreviousStatement(): targetStatement->unparseToString() = %s \n",targetStatement->unparseToString().c_str());
     printf ("@@@@@ In Rose::getPreviousStatement(): scope = %s \n",scope->sage_class_name());
     printf ("@@@@@ In Rose::getPreviousStatement(): scope->unparseToString() = %s \n",scope->unparseToString().c_str());
#endif

     switch (targetStatement->variantT())
        {
          case V_SgFunctionParameterList:
             {
               if (climbOutScope)
               {
                 // We define the previous statement in this case to be the function declaration
                 previousStatement = isSgStatement(targetStatement->get_parent());
                 ROSE_ASSERT (isSgFunctionDeclaration(previousStatement) != NULL);
               }
               break;
             }

          case V_SgForInitStatement:
             {
               if (climbOutScope)
               {
                 previousStatement = isSgStatement(targetStatement->get_parent());
                 ROSE_ASSERT (isSgForStatement(previousStatement) != NULL);
               }
               break;
             }

          case V_SgBasicBlock:
             {
               if (climbOutScope)
               {
                 previousStatement = isSgStatement(targetStatement->get_parent());
                 ROSE_ASSERT (previousStatement != NULL);
               }
               break;
             }

          case V_SgClassDefinition:
          case V_SgFunctionDefinition:
             {
            // In the case of a definition we define the previous statement
            // to be to one appearing before the associated declaration.
               previousStatement = isSgStatement(targetStatement->get_parent());
               ROSE_ASSERT (previousStatement != NULL);
               break;
             }
          default:
             {
            // We have to handle the cases of a SgStatementPtrList and a 
            // SgDeclarationStatementPtrList separately
               if (scope->containsOnlyDeclarations() == true)
                  {
                 // Usually a global scope or class declaration scope
                    SgDeclarationStatementPtrList & declarationList = scope->getDeclarationList();
                    Rose_STL_Container<SgDeclarationStatement*>::iterator i = declarationList.begin();
                    Rose_STL_Container<SgDeclarationStatement*>::iterator previousStatementIterator =  declarationList.end();
                    while ( ( i != declarationList.end() ) && ( (*i) != targetStatement ) )
                       {
                         previousStatementIterator = i++;
                       }

                    if ( previousStatementIterator != declarationList.end() )
                       {
                         previousStatement = *previousStatementIterator;
                       }
                  }
                  // Liao 5/10/2010, special case when a true/false body of a if statement is not a basic block
                  // since getStatementList() is not defined for a if statement. 
                  // We define the previous statement of the true/false body to be the if statement
                  // This is consistent with the later handling that when a statement is the first in a parent, 
                  // treat the parent as the previous statement
                 else if (isSgIfStmt(scope))
                  {
                    if (climbOutScope)
                    {
                      previousStatement = isSgStatement(targetStatement->get_parent());
                      ROSE_ASSERT (isSgIfStmt(previousStatement) != NULL);
                    }
                  }
                 else
                  {
                    SgStatementPtrList & statementList = scope->getStatementList();
                    Rose_STL_Container<SgStatement*>::iterator i = statementList.begin();
                    Rose_STL_Container<SgStatement*>::iterator previousStatementIterator =  statementList.end();
                    while ( ( i != statementList.end() ) && ( (*i) != targetStatement ) )
                       {
                         previousStatementIterator = i++;
                       }

                    if ( previousStatementIterator != statementList.end() )
                       {
                         previousStatement = *previousStatementIterator;
                       }
                  }

            // If the target statement was the first statement in a scope then 
               if ((previousStatement == NULL)&& climbOutScope)
                  {
                 // Then set the previous statement to be the scope containing the current statement
                 // printf ("previousStatement == NULL: previous scope = %s \n",scope->unparseToString().c_str());
                    switch (scope->variantT())
                       {
                         case V_SgBasicBlock:
                            {
                              previousStatement = getPreviousStatement(scope);
                              break;
                            }

                         default:
                            {
                              previousStatement = scope;
                              break;
                            }
                       }
                  }
               break;
             }
        }

#if 0
     printf ("@@@@@ previousStatement = %p \n",previousStatement);
     if (previousStatement != NULL)
        printf ("@@@@@ previousStatement->unparseToString() = %s \n",previousStatement->unparseToString().c_str());
#endif

     if (climbOutScope)
       ROSE_ASSERT (isSgGlobal(targetStatement) != NULL || previousStatement != NULL);

     return previousStatement;
   }







