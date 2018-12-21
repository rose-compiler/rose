// utility_functions.h -- header file for the ROSE Optimizing Preprocessor

#ifndef UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

#include "Cxx_Grammar.h"
class UnparseDelegate;

#define BACKEND_VERBOSE_LEVEL 2

// DQ (11/1/2009): replaced "version()" with separate "version_number()" and "version_message()" functions.
// returns a string containing the current version message (includes the version number).
std::string version_message();

// DQ (11/1/2009): replaced "version()" with separate "version_number()" and "version_message()" functions.
// returns a string containing the current version number
std::string version_number();

//! SCM version identifier for ROSE
std::string rose_scm_version_id();

//! SCM version date-identifier for ROSE (Unix timestamp)
time_t rose_scm_version_date();

//! Boost version identifier (numeric)
unsigned int rose_boost_version_id();

//! Boost version path
std::string rose_boost_version_path();

// DQ (7/3/2013): Added support to output pre-defined macro settings.
//! Output at least some of the predefined macro settings.
void outputPredefinedMacros();


// Simple interface for ROSE (error codes are in SgProject.frontendErrorCode(), backendErrorCode() )
// tps : Need to make this function (DLL) public 
ROSE_DLL_API SgProject* frontend ( int argc, char** argv, bool frontendConstantFolding = false );
ROSE_DLL_API SgProject* frontend ( const std::vector<std::string>& argv, bool frontendConstantFolding = false );

// DQ (4/17/2015): After discussion with Liao, Markus, and Pei-Hung, we have agreed that
// we want to support multiple SgProject IR nodes.  So in addition to the SgProject* frontend()
// function we will in the future support:
//   1) A SgProject member function "frontend()" that can be used to generate a new SgFile in
//      an existing SgProject.
//   2) A SgBuilder function for building an empty SgProject (without files).
//   3) We will remove the use of the SageInterface::getProject() (which asserts that there 
//      is only one SgProject).

// This builds a shell of a frontend SgProject with associated SgFile objects (but with empty 
// SgGlobal objects) supporting only commandline processing and requiring the frontend to be 
// called explicitly for each SgFile object.  See tutorial/selectedFileTranslation.C for example.
SgProject* frontendShell ( int argc, char** argv);
ROSE_DLL_API SgProject* frontendShell ( const std::vector<std::string>& argv);

// DQ (3/18/2006): Modified backend function interface to permit handling of user specified
// objects to control the formatting of code generation and the use of alternative code generation
// techniques (e.g. copy-based code generation).
// int backend ( SgProject* project );
//
// WARNING: If a non-null unparseFormatHelp is specified then backend will unconditionally delete it.  Therefore, the caller
// must have allocated it on the heap or else strange errors will result.
ROSE_DLL_API int backend ( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL, UnparseDelegate* unparseDelagate = NULL );

// DQ (8/24/2009): This backend calls the backend compiler using the original input source file list.
// This is useful as a test code for testing ROSE for use on projects that target Compass or any
// other analysis only tool using ROSE. Called in tests/nonsmoke/functional/testAnalysis.C for example.
ROSE_DLL_API int backendCompilesUsingOriginalInputFile ( SgProject* project, bool compile_with_USE_ROSE_macro = false );

// DQ (2/6/2010): This backend forces all code to be generated but still uses the backend vendor 
// compiler to compile the original code.  This is a step between backendUsingOriginalInputFile(),
// which does not generate code; and backend() which generated code and compiles it.  The use of
// this backend permits an intermediate test of robustness where the code that we generate might
// be generated incorrectly (usually with missing name qualification as required for a specific 
// backend (vendor) compiler).
ROSE_DLL_API int backendGeneratesSourceCodeButCompilesUsingOriginalInputFile ( SgProject* project );

//QY: new back end that performs only source-to-source translations 
// of the original file. Furthermore, statements are copied from 
// the original file if they are not changed
ROSE_DLL_API int copy_backend( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL );

// int globalBackendErrorCode  = 0;
// int backend ( const SgProject & project, int & errorCode = globalBackendErrorCode);
// SgProject & frontend ( int argc, char * argv[] );

ROSE_DLL_API void generatePDF ( const SgProject & project );

// DQ (12/20/2018): Added option to exclude template instantiations which make the graphs smaller and tractable t manage for larger C++ applications.
ROSE_DLL_API void generateDOT ( const SgProject & project, std::string filenamePostfix = "", bool excludeTemplateInstantiations = false );

// DQ (9/22/2017): Adding support that is can work with any IR node, so that we can generated DOT files on untyped ASTs.
// I prefer the API that takes a SgNode pointer.
// ROSE_DLL_API void generateDOT ( SgNode* node, std::string baseFilename, std::string filenamePostfix = "" );
ROSE_DLL_API void generateDOT ( SgNode* node, std::string filename );

// DQ (9/1/2008): Added function to generate the compete AST when specificed with multiple files 
// on the command line.  This is the older default behavior of generateDOT (from last year, or so).
ROSE_DLL_API void generateDOT_withIncludes   ( const SgProject & project, std::string filenamePostfix = "" );
ROSE_DLL_API void generateDOTforMultipleFile ( const SgProject & project, std::string filenamePostfix = "" );

// DQ (6/14/2007): Support for whole AST graphs output with attributes (types, symbols, all edges, etc.)
// We define a default value for the maximum graph size (number of nodes).
ROSE_DLL_API void generateAstGraph ( const SgProject* project, int maxSize = 2000, std::string filenameSuffix = "" );
// void generateAstGraph ( const SgProject* project, int maxSize, std::string filenameSuffix = "", CustomMemoryPoolDOTGeneration::s_Filter_Flags* filter_flags = NULL)

// output of EDG AST (useful for debugging connection to SAGE)
//void pdfPrintAbstractSyntaxTreeEDG ( SgFile *file );
// void generatePDFofEDG ( const SgProject & project );

#ifndef SWIG

//#include "rose.h"
//#include "../midend/astProcessing/DOTGeneration.h"
//#include "roseInternal.h"
//#include "../midend/astProcessing/AstDotGeneration.h"
//using namespace AstDOTGenerationExtended_Defaults;
//#include "../midend/astProcessing/AstDOTGeneration.h"
#include "AstDOTGeneration.h"

template <typename ExtraNodeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo, typename ExtraNodeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions, typename ExtraEdgeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo, typename ExtraEdgeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions>
struct generateDOTExtended ;
//void
//generateDOTExtended ( const SgProject & project, std::string filenamePostfix = "", ExtraNodeInfo_t eni = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo(), ExtraNodeOptions_t eno = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions(), ExtraEdgeInfo_t eei = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), ExtraEdgeOptions_t eeo = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() );

template <typename ExtraNodeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo, typename ExtraNodeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions, typename ExtraEdgeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo, typename ExtraEdgeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions>
struct generateDOTExtended_withIncludes ;

template <typename ExtraNodeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo, typename ExtraNodeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions, typename ExtraEdgeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo, typename ExtraEdgeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions>
struct generateDOTExtendedforMultipleFile ;

// endif for ifndef SWIG
#endif

#ifndef SWIG
// DQ (3/10/2013): Swig has a problem with:
// src/roseSupport/utility_functions.h:138: Error: 'stripPathFromFileName' is multiply defined in the generated target language module in scope 'roseJNI'.
// src/util/stringSupport/string_functions.h:221: Error: Previous declaration of 'stripPathFromFileName'
// src/util/stringSupport/string_functions.h:223: Error: Previous declaration of 'getPathFromFileName'
// So exclude this code below.

namespace Rose
   {
  // This class serves as a catch all location for functions of general use within ROSE
  // we have added variables that are set using command line parameters to avoid the use of
  // global variables.  

  // DQ (3/6/2017): Adding ROSE options data structure to support frontend and backend options such as:
  //    1) output of warnings from the EDG (or other) frontend.
  //    2) output of notes from the EDG (or other) frontend.
  //    3) output of warnings from the backend compiler.
  // The reason for this options clas is to support tools that would want to suppress warnings from ROSE,
  // and still also permit compilers that would be implemented using ROSE to have the full range of output
  // from EDG (or other frontends) to control notes and warnings.
     class Options
        {
          private:
               bool frontend_notes;
               bool frontend_warnings;
               bool backend_notes;
               bool backend_warnings;

          public:
               ROSE_DLL_API Options();
               ROSE_DLL_API Options(const Options & X);
               ROSE_DLL_API Options & operator= (const Options & X);

            // Access functions for options API.
               ROSE_DLL_API bool get_frontend_notes();
               ROSE_DLL_API void set_frontend_notes(bool flag);
               ROSE_DLL_API bool get_frontend_warnings();
               ROSE_DLL_API void set_frontend_warnings(bool flag);
               ROSE_DLL_API bool get_backend_notes();
               ROSE_DLL_API void set_backend_notes(bool flag);
               ROSE_DLL_API bool get_backend_warnings();
               ROSE_DLL_API void set_backend_warnings(bool flag);
        };

  // Global variable (in this rose namespace) to permit multiple parts of ROSE to access consistant information on options.
     ROSE_DLL_API extern Options global_options;


       // DQ (8/10/2004): This was moved to the SgFile a long time ago and should not be used any more)
       // DQ (8/11/2004): Need to put this back so that there is a global concept of verbosity for all of ROSE.
       // static int roseVerboseLevel;

       // These functions trim the header files from the unparsed output.
       // static int isCutStart ( SgStatement *st );
       // static int isCutEnd ( SgStatement *st );
       // void ROSE_Unparse ( SgFile *f , std::ostream *of );

       // This function helps isolate the details of the UNIX strcmp function
       // static int isSameName ( const std::string& s1, const std::string& s2 );
          int containsString ( const std::string& masterString, const std::string& targetString );

        // DQ (9/5/2008): Try to remove these functions...
          std::string getFileNameByTraversalBackToFileNode ( const SgNode* astNode );
       // std::string getFileName ( const SgFile* file );

       // DQ (5/25/2005): Removed from ROSE class (since they are redundant with other uses)
          std::string getFileName     ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          int   getLineNumber   ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          int   getColumnNumber ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          bool  isPartOfTransformation( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;

          ROSE_DLL_API std::string getWorkingDirectory (); //! get the current directory
          ROSE_DLL_API std::string getSourceDirectory  ( std::string fileNameWithPath ); //! get the sourceDirectory directory

          std::string getFileNameWithoutPath ( SgStatement* statementPointer );
          ROSE_DLL_API std::string utility_stripPathFromFileName ( const std::string& fileNameWithPath ); //! get the filename from the full filename

          ROSE_DLL_API std::string getPathFromFileName   ( std::string fileNameWithPath ); //! get the path from the full filename

       // DQ (9/8/2008): This is removed since it is redundant with the version in StringUtility.
       // std::string stripFileSuffixFromFileName ( const std::string& fileNameWithSuffix ); //! get the name without the ".C"

       // std::string getPragmaString ( SgStatement  *stmt );
       // std::string getPragmaString ( SgExpression *expr );

       // SgPragma* getPragma ( SgExpression *expr );
       // SgPragma* getPragma ( SgStatement  *stmt );
       // SgPragma* getPragma ( SgBinaryOp   *binaryOperator );

       // std::string identifyVariant ( int Code );

          SgName concatenate ( const SgName & X, const SgName & Y );

          ROSE_DLL_API void usage (int status);

          void filterInputFile ( const std::string inputFileName, const std::string outputFileName );

      //! Functions to move to SgStatement object in SAGE III later
          SgStatement* getPreviousStatement ( SgStatement *targetStatement , bool climbOutScope = true);
          SgStatement* getNextStatement     ( SgStatement *targetStatement );

       // DQ (9/27/2018): We need to build multiple maps, one for each file (to support token based unparsing for multiple files,
       // such as what is required when using the unparsing header files feature).
       // DQ (10/28/2013): Put the token sequence map here, it is set and accessed via member functions on the SgSourceFile IR node.
       // extern std::map<SgNode*,TokenStreamSequenceToNodeMapping*> tokenSubsequenceMap;
          extern std::map<int,std::map<SgNode*,TokenStreamSequenceToNodeMapping*>* > tokenSubsequenceMapOfMaps;

       // DQ (11/27/2013): Adding vector of nodes in the AST that defines the token unparsing AST frontier.
       // extern std::vector<FrontierNode*> frontierNodes;
       // extern std::map<SgStatement*,FrontierNode*> frontierNodes;
          extern std::map<int,std::map<SgStatement*,FrontierNode*>*> frontierNodesMapOfMaps;

       // DQ (11/27/2013): Adding adjacency information for the nodes in the token unparsing AST frontier.
       // extern std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap;
          extern std::map<int,std::map<SgNode*,PreviousAndNextNodeData*>*> previousAndNextNodeMapOfMaps;

       // DQ (11/29/2013): Added to support access to multi-map of redundant mapping of frontier IR nodes to token subsequences.
       // extern std::multimap<int,SgStatement*> redundantlyMappedTokensToStatementMultimap;
       // extern std::set<int> redundantTokenEndingsSet;
          extern std::map<int,std::multimap<int,SgStatement*>*> redundantlyMappedTokensToStatementMapOfMultimaps;
          extern std::map<int,std::set<int>*> redundantTokenEndingsMapOfSets;

       // DQ (11/20/2015): Provide a statement to use as a key in the token sequence map to get representative whitespace.
       // extern std::map<SgScopeStatement*,SgStatement*> representativeWhitespaceStatementMap;
          extern std::map<int,std::map<SgScopeStatement*,SgStatement*>*> representativeWhitespaceStatementMapOfMaps;

       // DQ (11/30/2015): Provide a statement to use as a key in the macro expansion map to get info about macro expansions.
       // extern std::map<SgStatement*,MacroExpansion*> macroExpansionMap;
          extern std::map<int,std::map<SgStatement*,MacroExpansion*>*> macroExpansionMapOfMaps;

       // DQ (10/29/2018): Build a map for the unparser to use to locate SgIncludeFile IR nodes.
          extern std::map<std::string, SgIncludeFile*> includeFileMapForUnparsing;

  // DQ (3/5/2017): Added general IR node specific message stream to support debugging message from the ROSE IR nodes.
     extern Sawyer::Message::Facility ir_node_mlog;

     void initDiagnostics();
   };


// endif for ifndef SWIG
#endif

#endif // ifndef UTILITY_FUNCTIONS_H







