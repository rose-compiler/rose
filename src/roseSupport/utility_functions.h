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

// Simple interface for ROSE (error codes are in SgProject.frontendErrorCode(), backendErrorCode() )
// tps : Need to make this function (DLL) public 
ROSE_DLL_API SgProject* frontend ( int argc, char** argv);
ROSE_DLL_API SgProject* frontend ( const std::vector<std::string>& argv);

// This builds a shell of a frontend SgProject with associated SgFile objects (but with empty 
// SgGlobal objects) supporting only commandline processing and requiring the frontend to be 
// called explicitly for each SgFile object.  See tutorial/selectedFileTranslation.C for example.
SgProject* frontendShell ( int argc, char** argv);
SgProject* frontendShell ( const std::vector<std::string>& argv);

// DQ (3/18/2006): Modified backend function interface to permit handling of user specified
// objects to control the formatting of code generation and the use of alternative code generation
// techniques (e.g. copy-based code generation).
// int backend ( SgProject* project );
ROSE_DLL_API int backend ( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL, UnparseDelegate* unparseDelagate = NULL );

// DQ (8/24/2009): This backend calls the backend compiler using the original input source file list.
// This is useful as a test code for testing ROSE for use on projects that target Compass or any
// other analysis only tool using ROSE. Called in tests/testAnalysis.C for example.
int backendCompilesUsingOriginalInputFile ( SgProject* project );

// DQ (2/6/2010): This backend forces all code to be generated but still uses the beakend vendor 
// compiler to compile the original code.  This is a step between backendUsingOriginalInputFile(),
// which does not generate code; and backend() which generated code and compiles it.  The use of
// this backend permits an intermediate test of robustness where the code that we generate might
// be generated incorrectly (usually with missing name qualification as required for a specific 
// backend (vendor) compiler).
int backendGeneratesSourceCodeButCompilesUsingOriginalInputFile ( SgProject* project );

//QY: new back end that performs only source-to-source translations 
// of the original file. Furthermore, statements are copied from 
// the original file if they are not changed
int copy_backend( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL );

// int globalBackendErrorCode  = 0;
// int backend ( const SgProject & project, int & errorCode = globalBackendErrorCode);
// SgProject & frontend ( int argc, char * argv[] );

void generatePDF ( const SgProject & project );
void generateDOT ( const SgProject & project, std::string filenamePostfix = "" );

// DQ (9/1/2008): Added function to generate the compete AST when specificed with multiple files 
// on the command line.  This is the older default behavior of generateDOT (from last year, or so).
void generateDOT_withIncludes   ( const SgProject & project, std::string filenamePostfix = "" );
void generateDOTforMultipleFile ( const SgProject & project, std::string filenamePostfix = "" );

// DQ (6/14/2007): Support for whole AST graphs output with attributes (types, symbols, all edges, etc.)
// We define a default value for the maximum graph size (number of nodes).
void generateAstGraph ( const SgProject* project, int maxSize = 2000, std::string filenameSuffix = "" );

// output of EDG AST (useful for debugging connection to SAGE)
//void pdfPrintAbstractSyntaxTreeEDG ( SgFile *file );
// void generatePDFofEDG ( const SgProject & project );

#include "rose.h"
//#include "../midend/astProcessing/DOTGeneration.h"
//#include "roseInternal.h"
//#include "../midend/astProcessing/AstDotGeneration.h"
//using namespace AstDOTGenerationExtended_Defaults;
#include "../midend/astProcessing/AstDOTGeneration.h"

template <typename ExtraNodeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo, typename ExtraNodeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions, typename ExtraEdgeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo, typename ExtraEdgeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions>
struct generateDOTExtended ;
//void
//generateDOTExtended ( const SgProject & project, std::string filenamePostfix = "", ExtraNodeInfo_t eni = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo(), ExtraNodeOptions_t eno = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions(), ExtraEdgeInfo_t eei = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), ExtraEdgeOptions_t eeo = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() );

template <typename ExtraNodeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo, typename ExtraNodeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions, typename ExtraEdgeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo, typename ExtraEdgeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions>
struct generateDOTExtended_withIncludes ;

template <typename ExtraNodeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo, typename ExtraNodeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions, typename ExtraEdgeInfo_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo, typename ExtraEdgeOptions_t = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions>
struct generateDOTExtendedforMultipleFile ;

#include "utility_functionsImpl.C"

namespace ROSE
   {
  // This class serves as a catch all location for functions of general use within ROSE
  // we have added variables that are set using command line parameters to avoid the use of
  // global variables.  

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

          std::string getWorkingDirectory (); //! get the current directory
          std::string getSourceDirectory  ( std::string fileNameWithPath ); //! get the sourceDirectory directory

          std::string getFileNameWithoutPath ( SgStatement* statementPointer );
          std::string stripPathFromFileName ( const std::string& fileNameWithPath ); //! get the filename from the full filename

          std::string getPathFromFileName   ( std::string fileNameWithPath ); //! get the path from the full filename

       // DQ (9/8/2008): This is removed since it is redundant with the version in StringUtility.
       // std::string stripFileSuffixFromFileName ( const std::string& fileNameWithSuffix ); //! get the name without the ".C"

       // std::string getPragmaString ( SgStatement  *stmt );
       // std::string getPragmaString ( SgExpression *expr );

       // SgPragma* getPragma ( SgExpression *expr );
       // SgPragma* getPragma ( SgStatement  *stmt );
       // SgPragma* getPragma ( SgBinaryOp   *binaryOperator );

       // std::string identifyVariant ( int Code );

          SgName concatenate ( const SgName & X, const SgName & Y );

          void usage (int status);

          void filterInputFile ( const std::string inputFileName, const std::string outputFileName );

      //! Functions to move to SgStatement object in SAGE III later
          SgStatement* getPreviousStatement ( SgStatement *targetStatement );
          SgStatement* getNextStatement     ( SgStatement *targetStatement );

   };

#endif // ifndef UTILITY_FUNCTIONS_H







