// utility_functions.h -- header file for the ROSE Optimizing Preprocessor

#ifndef UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

#define BACKEND_VERBOSE_LEVEL 2

// returns a string containing the current version number
std::string version();

// Simple interface for ROSE (error codes are in SgProject.frontendErrorCode(), backendErrorCode() )
SgProject* frontend ( int argc, char** argv);
SgProject* frontend ( const std::vector<std::string>& argv);

// This builds a shell of a frontend SgProject with associated SgFile objects (but with empty 
// SgGlobal objects) supporting only commandline processing and requiring the frontend to be 
// called explicitly for each SgFile object.  See tutorial/selectedFileTranslation.C for example.
SgProject* frontendShell ( int argc, char** argv);
SgProject* frontendShell ( const std::vector<std::string>& argv);

// DQ (3/18/2006): Modified backend function interface to permit handling of user specified
// objects to control the formatting of code generation and the use of alternative code generation
// techniques (e.g. copy-based code generation).
// int backend ( SgProject* project );
int backend ( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL, UnparseDelegate* unparseDelagate = NULL );

//QY: new back end that performs only source-to-source translations 
// of the original file. Furthermore, statements are copied from 
// the original file if they are not changed
int copy_backend( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL );

// int globalBackendErrorCode  = 0;
// int backend ( const SgProject & project, int & errorCode = globalBackendErrorCode);
// SgProject & frontend ( int argc, char * argv[] );

void generatePDF ( const SgProject & project );
void generateDOT ( const SgProject & project );

// DQ (6/14/2007): Support for whole AST graphs output with attributes (types, symbols, all edges, etc.)
// We define a default value for the maximum graph size (number of nodes).
void generateAstGraph ( const SgProject* project, int maxSize = 2000, std::string filenameSuffix = "" );

// output of EDG AST (useful for debugging connection to SAGE)
//void pdfPrintAbstractSyntaxTreeEDG ( SgFile *file );
// void generatePDFofEDG ( const SgProject & project );


#if 1
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

          std::string getFileNameByTraversalBackToFileNode ( const SgNode* astNode );
          std::string getFileName ( const SgFile* file );

       // DQ (5/25/2005): Removed from ROSE class (since they are redundant with other uses)
          std::string getFileName     ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          int   getLineNumber   ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          int   getColumnNumber ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          bool  isPartOfTransformation( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
#if 0
          static char* getWorkingDirectory (); //! get the current directory
          static char* getSourceDirectory  ( char* fileNameWithPath ); //! get the sourceDirectory directory
#else
          std::string getWorkingDirectory (); //! get the current directory
          std::string getSourceDirectory  ( std::string fileNameWithPath ); //! get the sourceDirectory directory
#endif
          std::string getFileNameWithoutPath ( SgStatement* statementPointer );
          std::string stripPathFromFileName ( const std::string& fileNameWithPath ); //! get the filename from the full filename
#if 0
          char* getPathFromFileName   ( const char* fileNameWithPath ); //! get the path from the full filename
#else
          std::string getPathFromFileName   ( std::string fileNameWithPath ); //! get the path from the full filename
#endif
          std::string stripFileSuffixFromFileName ( const std::string& fileNameWithSuffix ); //! get the name without the ".C"

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
#if 0
       // Moved to ROSE/src/midend/astDiagnostics/astPerformance.[hC].
       // DQ (12/11/2006): Added simpler function than ramust mechanism, used for computing the memory in use.
      //! Function that I got from Bill Henshaw (originally from PetSc), for computing current memory in use.
       // static double getCurrentMemoryUsage();
          static long int getCurrentMemoryUsage();
#endif
   };
#endif

#endif // ifndef UTILITY_FUNCTIONS_H







